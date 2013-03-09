/*****************************************************************************
 * Copyright (C) 2011-2013 Michael Krufky
 *
 * Author: Michael Krufky <mkrufky@linuxtv.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "feed.h"
#ifdef USE_HDHOMERUN
#undef USE_LINUXTV_TUNER
#else
#define USE_LINUXTV_TUNER
#endif
#ifdef USE_LINUXTV_TUNER
#include "linuxtv_tuner.h"
#else
#include "hdhr_tuner.h"
#endif
#include "serve.h"

#include "atsctext.h"

typedef std::map<uint8_t, tune> map_tuners;

struct dvbtee_context
{
	feed _file_feeder;
#ifdef USE_LINUXTV_TUNER
	linuxtv_tuner tuner;
#else
	hdhr_tuner tuner;
#endif
	serve *server;
};
typedef std::map<pid_t, struct dvbtee_context*> map_pid_to_context;

map_pid_to_context context_map;


void stop_server(struct dvbtee_context* context);

void cleanup(struct dvbtee_context* context, bool quick = false)
{
	if (context->server)
		stop_server(context);

	if (quick) {
		context->_file_feeder.stop_without_wait();

		context->tuner.feeder.stop_without_wait();
		context->tuner.feeder.close_file();
	} else {
		context->_file_feeder.stop();

		context->tuner.stop_feed();
	}

	context->_file_feeder.close_file();

	context->tuner.close_fe();
#if 1 /* FIXME */
	ATSCMultipleStringsDeInit();
#endif
}


void signal_callback_handler(int signum)
{
	struct dvbtee_context* context = context_map[getpid()];
	bool signal_dbg = true;

	const char *signal_desc;
	switch (signum) {
	case SIGINT:  /* Program interrupt. (ctrl-c) */
		signal_desc = "SIGINT";
		signal_dbg = false;
		break;
	case SIGABRT: /* Process detects error and reports by calling abort */
		signal_desc = "SIGABRT";
		break;
	case SIGFPE:  /* Floating-Point arithmetic Exception */
		signal_desc = "SIGFPE";
		break;
	case SIGILL:  /* Illegal Instruction */
		signal_desc = "SIGILL";
		break;
	case SIGSEGV: /* Segmentation Violation */
		signal_desc = "SIGSEGV";
		break;
	case SIGTERM: /* Termination */
		signal_desc = "SIGTERM";
		break;
	case SIGHUP:  /* Hangup */
		signal_desc = "SIGHUP";
		break;
	default:
		signal_desc = "UNKNOWN";
		break;
	}
	if (signal_dbg)
		fprintf(stderr, "%s: caught signal %d: %s\n", __func__, signum, signal_desc);

	cleanup(context, true);

	context->_file_feeder.parser.cleanup();
	context->tuner.feeder.parser.cleanup();

	exit(signum);
}


static void bitrate_stats(void *priv, stats_map &bitrates, stats_map &discontinuities, uint64_t tei_count, bool per_sec)
{
	/* display the bitrates for each pid, followed by special pid 0x2000 for the full TS */
	for (stats_map::const_iterator iter = bitrates.begin(); iter != bitrates.end(); ++iter) {
		char a[16];
		char b[16];
		fprintf(stderr, "pid %04x %5lu p%s  %sb%s  %sbit\n",
			iter->first, iter->second / 188, (per_sec) ? "/s" : "",
			stats_scale_unit(a, sizeof(a), iter->second), (per_sec) ? "/s" : "",
			stats_scale_unit(b, sizeof(b), iter->second * 8));
	}
	for (stats_map::const_iterator iter = discontinuities.begin(); iter != discontinuities.end(); ++iter)
		fprintf(stderr, "pid %04x\t%lu discontinuities (%lu%%)\n", iter->first, iter->second, ((!iter->second) || (!bitrates[iter->first])) ? 0 : (!bitrates.count(iter->first)) ? 0 : (100 * iter->second / (bitrates[iter->first] / 188)));

	if (tei_count)
		fprintf(stderr, "tei count: %lu (%lu%%)\n", tei_count, (!bitrates[0x2000]) ? 0 : (18800 * tei_count / bitrates[0x2000]));

	fprintf(stderr,"\n");
}


void stop_server(struct dvbtee_context* context)
{
	if (!context->server)
		return;

	context->server->stop();

	delete context->server;
	context->server = NULL;

	return;
}

int start_server(struct dvbtee_context* context, int num_tuners, unsigned int flags)
{
	context->server = new serve;
#if 0
	if (num_tuners < 0) {
#endif
		context->server->add_tuner(&context->tuner);
		if (flags & 2)
			context->tuner.feeder.parser.out.add_http_server(SERVE_DEFAULT_PORT+1);
#if 0
	} else {
		tune tuners[num_tuners];
		for (int i = 0; i < num_tuners; i++) {
			int dvb_adap = i; /* ID X, /dev/dvb/adapterX/ */
			int demux_id = 0; /* ID Y, /dev/dvb/adapterX/demuxY */
			int dvr_id   = 0; /* ID Y, /dev/dvb/adapterX/dvrY */
			int fe_id    = 0; /* ID Y, /dev/dvb/adapterX/frontendY */

			tuners[i].set_device_ids(dvb_adap, fe_id, demux_id, dvr_id);
			context->server->add_tuner(&tuners[i]);
			if (flags & 2)
				tuners[i].feeder.parser.out.add_http_server(SERVE_DEFAULT_PORT+1+i);
		}
	}
#endif
	context->server->set_scan_flags(0, flags >> 2);

	return context->server->start();
}

#if 0
void listen_for_data(struct dvbtee_context* context, char* tcpipfeedurl)
{
	// FIXME: right now we just pass a port number, in the future, we'll pass protocol://ip_address:port
	if (0 <= context->_file_feeder.start_socket(tcpipfeedurl)) {
		context->_file_feeder.wait_for_streaming_or_timeout(timeout);
		context->_file_feeder.stop();
		context->_file_feeder.close_file();
	}
}
#endif

void multiscan(struct dvbtee_context* context, int num_tuners, unsigned int scan_method,
	       unsigned int scan_flags, unsigned int scan_min, unsigned int scan_max, bool scan_epg, int eit_limit)
{
	int count = 0;
	int partial_redundancy = 0;
#ifdef USE_LINUXTV_TUNER
	linuxtv_tuner tuners[num_tuners];
#else
	hdhr_tuner tuners[num_tuners];
#endif
	int channels_to_scan = scan_max - scan_min + 1;

	for (int i = 0; i < num_tuners; i++) {
		int dvb_adap = i; /* ID X, /dev/dvb/adapterX/ */
		int demux_id = 0; /* ID Y, /dev/dvb/adapterX/demuxY */
		int dvr_id   = 0; /* ID Y, /dev/dvb/adapterX/dvrY */
		int fe_id    = 0; /* ID Y, /dev/dvb/adapterX/frontendY */

#ifdef USE_LINUXTV_TUNER
		tuners[i].set_device_ids(dvb_adap, fe_id, demux_id, dvr_id);
#else
		tuners[i].set_hdhr_id(0, 0, 0);
#endif
		tuners[i].feeder.parser.limit_eit(eit_limit);
	}
	switch (scan_method) {
	case 1: /* speed */
		for (int i = 0; i < num_tuners; i++) {
			int scan_start = scan_min + ((0 + i) * (unsigned int)channels_to_scan/num_tuners);
			int scan_end   = scan_min + ((1 + i) * (unsigned int)channels_to_scan/num_tuners);
			fprintf(stderr, "speed scan: tuner %d scanning from %d to %d\n", i, scan_start, scan_end);
			tuners[i].tune::start_scan(scan_flags, scan_start, scan_end, scan_epg);
			sleep(1); // FIXME
		}
		break;
	case 2: /* redundancy */
		for (int i = 0; i < num_tuners; i++) {
			fprintf(stderr, "redundancy scan: tuner %d scanning from %d to %d\n", i, scan_min, scan_max);
			tuners[i].tune::start_scan(scan_flags, scan_min, scan_max, scan_epg);
			sleep(5); // FIXME
		}
		break;
	case 4: /* speed AND partial redundancy */
		partial_redundancy = (num_tuners > 2) ? (num_tuners - 2) : 0;
		/* FALL-THRU */
	default:
	case 3: /* speed AND redundancy */
		for (int j = 0; j < num_tuners - partial_redundancy; j++) {
			for (int i = 0; i < num_tuners; i++) {
				if (j) {
					tuners[i].wait_for_scan_complete();
				}
				int scan_start, scan_end;
				if (i + j < num_tuners) {
					scan_start = scan_min + ((0 + (i + j)) * (unsigned int)channels_to_scan/num_tuners);
					scan_end   = scan_min + ((1 + (i + j)) * (unsigned int)channels_to_scan/num_tuners);
				} else {
					scan_start = scan_min + ((0 + ((i + j) - num_tuners)) * (unsigned int)channels_to_scan/num_tuners);
					scan_end   = scan_min + ((1 + ((i + j) - num_tuners)) * (unsigned int)channels_to_scan/num_tuners);
				}
				fprintf(stderr, "speed & %sredundancy scan: pass %d of %d, tuner %d scanning from %d to %d\n",
					(partial_redundancy) ? "partial " : "",
					j + 1, num_tuners - partial_redundancy,
					i, scan_start, scan_end);
				tuners[i].tune::start_scan(scan_flags, scan_start, scan_end, scan_epg);
				sleep(1); // FIXME
			}
		}
		break;
	}
	for (int i = 0; i < num_tuners; i++)
		tuners[i].wait_for_scan_complete();
#if 1
	for (int i = 0; i < num_tuners; i++) {
		int _count = tuners[i].feeder.parser.xine_dump();
		count += _count;
		fprintf(stderr, "tuner %d found %d services\n", i, _count);
	}
#else
	count += tuners[0].feeder.parser.xine_dump();
#endif
	fprintf(stderr, "found %d services in total\n", count);
}

void usage(bool help, char *myname)
{
	fprintf(stderr, "  "
		"-a\tadapter id\n  "
		"-A\t(1 for ATSC, 2 for ClearQAM)\n  "
		"-b\tdisplay bitrates & statistics\n  "
		"-c\tchannel to tune /\n\tcomma (,) separated list of channels to scan /\n\tscan minimum channel\n  "
		"-C\tchannel to tune /\n\tcomma (,) separated list of channels to scan /\n\tscan maximum channel\n  "
		"-f\tfrontend id\n  "
		"-F\tfilename to use as input\n  "
		"-t\ttimeout\n  "
		"-T\tnumber of tuners (dvb adapters) allowed to use, 0 for all\n  "
		"-s\tscan, optional arg when using multiple tuners: \n\t1 for speed, 2 for redundancy, \n\t3 for speed AND redundancy, \n\t4 for optimized speed / partial redundancy\n  "
		"-S\tserver mode, optional arg 1 for command server, \n\t2 for http stream server, 3 for both\n  "
		"-i\tpull local/remote tcp/udp port for data\n  "
		"-I\trequest a service and its associated PES streams by its service id\n  "
		"-E\tenable EPG scan, optional arg to limit the number of EITs to parse\n  "
		"-o\toutput filtered data, optional arg is a filename / URI, ie udp://127.0.0.1:1234\n  "
		"-O\toutput options: (or-able) 1 = PAT/PMT, 2 = PES, 4 = PSIP\n  "
		"-d\tdebug level\n  "
		"-h\tdisplay additional help\n\n");
	if (help)
		fprintf(stderr,
		"To tune to service id 1 of physical channel 33 and stream it to a udp port:\n  "
		"%s -c33 -I1 -oudp://192.168.1.100:1234\n\n"
		"To tune the second frontend of adapter 1 and stream the full TS of physical channel 44 to a tcp listener:\n  "
		"%s -c44 -otcp://192.168.1.200:5555\n\n"
		"To listen to a TCP port and stream to a UDP port:\n  "
		"%s -itcp://5555 -oudp://192.168.1.100:1234\n\n"
		"To parse a captured file and filter out the PSIP data, saving the PAT/PMT and PES streams to a file:\n  "
		"%s -Finput.ts -O3 -ofile://output.ts\n\n"
		"To parse a UDP stream for ten seconds:\n  "
		"%s -iudp://127.0.0.1:1234 -t10\n\n"
		"To scan for ClearQAM services using 5 tuners optimized for speed and partial redundancy:\n  "
		"%s -A2 -T5 -s4\n\n"
		"To start a server using adapter 0:\n  "
		"%s -a0 -S\n\n"
		, myname, myname, myname, myname, myname, myname, myname
	);
}

int main(int argc, char **argv)
{
	dvbtee_context context;
	int opt, channel = 0;
	bool b_read_dvr = false;
	bool b_scan     = false;
	bool scan_epg   = false;
	bool b_output_file   = false;
	bool b_output_stdout = false;
	bool b_serve    = false;
	bool b_kernel_pid_filters = false;
	bool b_help     = false;
	bool b_bitrate_stats = false;

	context.server = NULL;

	/* LinuxDVB context: */
	int dvb_adap = -1; /* ID X, /dev/dvb/adapterX/ */
	int fe_id    = -1; /* ID Y, /dev/dvb/adapterX/frontendY */
	int demux_id = 0; /* ID Y, /dev/dvb/adapterX/demuxY */
	int dvr_id   = 0; /* ID Y, /dev/dvb/adapterX/dvrY */

	unsigned int serv_flags  = 0;
	unsigned int scan_flags  = 0;
	unsigned int scan_min    = 0;
	unsigned int scan_max    = 0;
	unsigned int scan_method = 0;

	int num_tuners           = -1;
	unsigned int timeout     = 0;

	unsigned int wait_event  = 0;
	int eit_limit            = -1;

	enum output_options out_opt = (enum output_options)-1;

	char filename[256];
	memset(&filename, 0, sizeof(filename));

	char outfilename[256];
	memset(&outfilename, 0, sizeof(outfilename));

	char tcpipfeedurl[32];
	memset(&tcpipfeedurl, 0, sizeof(tcpipfeedurl));

	char service_ids[64];
	memset(&service_ids, 0, sizeof(service_ids));

	char channel_list[256];
	memset(&channel_list, 0, sizeof(channel_list));

        while ((opt = getopt(argc, argv, "a:A:bc:C:f:F:t:T:i:I:s::S::E::o::O:d::hH?")) != -1) {
		switch (opt) {
		case 'a': /* adapter */
			dvb_adap = strtoul(optarg, NULL, 0);
			b_read_dvr = true;
			break;
		case 'A': /* ATSC / QAM */
			scan_flags = strtoul(optarg, NULL, 0);
			b_read_dvr = true;
			break;
		case 'b': /* bitrates & statistics */
			b_bitrate_stats = true;
			break;
		case 'c': /* channel list | channel / scan min */
			if (strstr(optarg, ","))
				strcpy(channel_list, optarg);

			/* if a list was provided, use the first item */
			channel = strtoul(optarg, NULL, 0);
			scan_min = strtoul(optarg, NULL, 0);

			b_read_dvr = true;
			break;
		case 'C': /* channel list | channel / scan max */
			if (strstr(optarg, ","))
				strcpy(channel_list, optarg);

			/* if a list was provided, use the first item */
			scan_max = strtoul(optarg, NULL, 0);

			b_read_dvr = true;
			break;
		case 'f': /* frontend */
			fe_id = strtoul(optarg, NULL, 0);
			b_read_dvr = true;
			break;
		case 'F': /* Filename */
			strcpy(filename, optarg);
			break;
		case 't': /* timeout */
			timeout = strtoul(optarg, NULL, 0);
			break;
		case 'T': /* number of tuners (dvb adapters) allowed to use, 0 for all */
			num_tuners = strtoul(optarg, NULL, 0);
			b_read_dvr = true;
			break;
		case 's': /* scan, optional arg when using multiple tuners: 1 for speed, 2 for redundancy, 3 for speed AND redundancy, 4 for optimized speed / partial redundancy */
			b_scan = true;
			scan_method = (optarg) ? strtoul(optarg, NULL, 0) : 0;
			if (scan_method)
				fprintf(stderr, "MULTISCAN: %d...\n", scan_method);
			break;
		case 'S': /* server mode, optional arg 1 for command server, 2 for http stream server, 3 for both */
			b_serve = true;
			serv_flags = (optarg) ? strtoul(optarg, NULL, 0) : 0;
			break;
		case 'i': /* pull local/remote tcp/udp port for data */
			strcpy(tcpipfeedurl, optarg);
			break;
		case 'I': /* request a service by its service id */
			strcpy(service_ids, optarg);
			break;
		case 'E': /* enable EPG scan, optional arg to limit the number of EITs to parse */
#if 0
			b_scan = true; // FIXME
#endif
			scan_epg = true;
			wait_event = FEED_EVENT_EPG;
			eit_limit = (optarg) ? strtoul(optarg, NULL, 0) : -1;
			if (eit_limit >= 0)
				fprintf(stderr, "EIT LIMIT: %d...\n", eit_limit);
			break;
		case 'o': /* output filtered data, optional arg is a filename */
			if (optarg) {
				strcpy(outfilename, optarg);
				b_output_file = true;
			} else
				b_output_stdout = true;
			break;
		case 'O': /* output options */
			out_opt = (enum output_options)strtoul(optarg, NULL, 0);
			break;
		case 'd':
			if (optarg)
				libdvbtee_set_debug_level(strtoul(optarg, NULL, 0));
			else
				libdvbtee_set_debug_level(255);
			break;
		case 'h':
		case 'H':
			b_help = true;
			/* fall - thru  */
		default:
			usage(b_help, argv[0]);
			return -1;
		}
	}

	context_map[getpid()] = &context;

	signal(SIGINT,  signal_callback_handler); /* Program interrupt. (ctrl-c) */
	signal(SIGABRT, signal_callback_handler); /* Process detects error and reports by calling abort */
	signal(SIGFPE,  signal_callback_handler); /* Floating-Point arithmetic Exception */
	signal(SIGILL,  signal_callback_handler); /* Illegal Instruction */
	signal(SIGSEGV, signal_callback_handler); /* Segmentation Violation */
	signal(SIGTERM, signal_callback_handler); /* Termination */
	signal(SIGHUP,  signal_callback_handler); /* Hangup */
#if 1 /* FIXME */
	ATSCMultipleStringsInit();
#endif
	b_kernel_pid_filters = (strlen(service_ids) > 0) ? true : false;

	if (out_opt > 0)
		context.tuner.feeder.parser.out.set_options(out_opt);

	if (b_bitrate_stats) {
		if (b_read_dvr)
			context.tuner.feeder.parser.statistics.set_statistics_callback(bitrate_stats, &context);
		else
			context._file_feeder.parser.statistics.set_statistics_callback(bitrate_stats, &context);
	}
	if (b_output_file) {
		if (b_read_dvr)
			context.tuner.feeder.parser.add_output(outfilename);
		else
			context._file_feeder.parser.add_output(outfilename);
	}
	if (((b_scan) && (num_tuners == -1)) || (b_read_dvr)) {
#ifdef USE_LINUXTV_TUNER
		if ((dvb_adap >= 0) || (fe_id >= 0)) {
			if (dvb_adap < 0)
				dvb_adap = 0;
			if (fe_id < 0)
				fe_id = 0;
			context.tuner.set_device_ids(dvb_adap, fe_id, demux_id, dvr_id, b_kernel_pid_filters);
		}
#endif
		context.tuner.feeder.parser.limit_eit(eit_limit);
	}
	if (b_serve)
		start_server(&context, num_tuners, serv_flags | (scan_flags << 2));

	if ((scan_min) && (!scan_max))
		scan_max = scan_min;
	else
	if ((scan_max) && (!scan_min))
		channel = scan_min = scan_max;

	if ((b_scan) && ((b_read_dvr) || (num_tuners >= 0))) {
		if (num_tuners >= 0)
			multiscan(&context, num_tuners, scan_method, scan_flags, scan_min, scan_max, scan_epg, eit_limit); // FIXME: channel_list
		else {
			if (strlen(channel_list))
				context.tuner.scan_for_services(scan_flags, channel_list, scan_epg);
			else
				context.tuner.scan_for_services(scan_flags, scan_min, scan_max, scan_epg);
		}
		goto exit;
	}

	if (b_serve)
		goto exit;

	if (strlen(filename)) {
		if (0 <= context._file_feeder.open_file(filename)) {
			if (0 == context._file_feeder.start()) {
				context._file_feeder.wait_for_streaming_or_timeout(timeout);
				context._file_feeder.stop();
			}
			context._file_feeder.close_file();
		}
		goto exit;
	}

	if (strlen(tcpipfeedurl)) {
#if 0
		listen_for_data(&context, tcpipfeedurl);
#else
		// FIXME: right now we just pass a port number, in the future, we'll pass protocol://ip_address:port
		if (0 <= context._file_feeder.start_socket(tcpipfeedurl)) {
			context._file_feeder.wait_for_streaming_or_timeout(timeout);
			context._file_feeder.stop();
			context._file_feeder.close_file();
		}
#endif
		goto exit;
	}

	if (channel) {
		fprintf(stderr, "TUNE to channel %d...\n", channel);
		int fe_fd = context.tuner.open_fe();
		if (fe_fd < 0)
			return fe_fd;

		if (!scan_flags)
			scan_flags = SCAN_VSB;

		if (strlen(service_ids) > 0) {
			context.tuner.feeder.parser.set_service_ids(service_ids);
			if (out_opt < 0)
				context.tuner.feeder.parser.out.set_options((enum output_options)OUTPUT_AV);
		}

		if (context.tuner.tune_channel(
				(scan_flags == SCAN_VSB) ? VSB_8 : QAM_256, channel)) {
			if (!context.tuner.wait_for_lock_or_timeout(2000)) {
				context.tuner.close_fe();
				goto exit; /* NO LOCK! */
			}
			context.tuner.feeder.parser.set_channel_info(channel,
				(scan_flags == SCAN_VSB) ? atsc_vsb_chan_to_freq(channel) : atsc_qam_chan_to_freq(channel),
				(scan_flags == SCAN_VSB) ? "8VSB" : "QAM_256");
		}
	}

	if (b_read_dvr) {
		/* assume frontend is already streaming,
		   all we have to do is read from the DVR device */
		if (b_serve) /* if we're running in server mode, we dont wait, stop or close */
			context.tuner.start_feed();
		else {
			if (0 == context.tuner.start_feed()) {
				context.tuner.feeder.wait_for_event_or_timeout(timeout, wait_event);
				context.tuner.stop_feed();
			}
			if (channel) /* if we tuned the frontend ourselves then close it */
				context.tuner.close_fe();
		}
	}
	else
	if (0 == context._file_feeder.parser.get_fed_pkt_count()) {
		fprintf(stderr, "reading from STDIN\n");
		if (0 == context._file_feeder.start_stdin()) {
			context._file_feeder.wait_for_streaming_or_timeout(timeout);
			context._file_feeder.stop();
		}
	}
	if (b_scan) // scan channel mode, normal scan would have goto'd to exit
		context.tuner.feeder.parser.xine_dump();
	if (scan_epg)
		context.tuner.feeder.parser.epg_dump();
exit:
	if (context.server) {
		while (context.server->is_running()) sleep(1);
		stop_server(&context);
	}
//	cleanup(&context);
#if 1 /* FIXME */
	ATSCMultipleStringsDeInit();
#endif
	return 0;
}
