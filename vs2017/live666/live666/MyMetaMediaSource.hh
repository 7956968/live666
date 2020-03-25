#ifndef MY_META_MEIDA_SOURCE_H_
#define MY_META_MEIDA_SOURCE_H_

#ifndef _FRAMED_SOURCE_HH
#include "FramedSource.hh"
#endif


class MyMetaMediaSource: public FramedSource {
public:
    static MyMetaMediaSource* createNew(UsageEnvironment& env, const char * path);

protected:
    MyMetaMediaSource(UsageEnvironment& env, const char * path);
    virtual ~MyMetaMediaSource();
public: // redefined virtual functions:
    virtual void doGetNextFrame();

    char* _p = NULL;
};

#endif
