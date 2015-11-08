#ifndef __VALUEOBJ_IGLOO_LISTENER_H__
#define __VALUEOBJ_IGLOO_LISTENER_H__

#include <igloo/igloo_alt.h>

class ValueObjTestListener : public igloo::TestListener
{
  public:
    virtual void TestRunStarting();
    virtual void TestRunEnded(const igloo::TestResults&);
    virtual void ContextRunStarting(const igloo::ContextBase&);
    virtual void ContextRunEnded(const igloo::ContextBase&);
    virtual void SpecRunStarting(const igloo::ContextBase&, const std::string&);
    virtual void SpecSucceeded(const igloo::ContextBase&, const std::string&);
    virtual void SpecFailed(const igloo::ContextBase&, const std::string&);
};

#endif // __VALUEOBJ_IGLOO_LISTENER_H__
