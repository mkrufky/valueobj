#include <igloo-listener.h>
#include <stdio.h>

using namespace igloo;

void ValueObjTestListener::TestRunStarting()
{
  printf("\n%s\n", __func__);
}

void ValueObjTestListener::TestRunEnded(const TestResults& results)
{
  printf("\n%s\n\n Succeeded: %d\n Failed: %d\n Total: %d\n\n", __func__, results.NumberOfSucceededTests(), results.NumberOfFailedTests(), results.NumberOfTestsRun());
}

void ValueObjTestListener::ContextRunStarting(const ContextBase& context)
{
  printf("\n%s: %s\n", context.Name().c_str(), __func__);
}

void ValueObjTestListener::ContextRunEnded(const ContextBase& context)
{
  printf("\n%s: %s\n", context.Name().c_str(), __func__);
}

void ValueObjTestListener::SpecRunStarting(const ContextBase& context, const std::string& specName)
{
  printf("\n%s: %s: %s\n", context.Name().c_str(), specName.c_str(), __func__);
}

void ValueObjTestListener::SpecSucceeded(const ContextBase& context, const std::string& specName)
{
  printf("%s: %s: %s\n", context.Name().c_str(), specName.c_str(), __func__);
}

void ValueObjTestListener::SpecFailed(const ContextBase& context, const std::string& specName)
{
  printf("%s: %s: %s\n", context.Name().c_str(), specName.c_str(), __func__);
}
