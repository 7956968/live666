#include "MyMetaMediaSource.hh"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>

static void sendNextFrame(void* clientData) {
  MyMetaMediaSource* source = (MyMetaMediaSource*)clientData;
  source->doGetNextFrame();
}

MyMetaMediaSource* MyMetaMediaSource::createNew(UsageEnvironment& env, const char * path) 
{
    return new MyMetaMediaSource(env, path);
}

MyMetaMediaSource::MyMetaMediaSource(UsageEnvironment& env, const char * path)
    : FramedSource(env)
{
    _p = new char[2000];
}

MyMetaMediaSource::~MyMetaMediaSource()
{
	delete[]_p;
	printf("OUT\n");
}

void MyMetaMediaSource::doGetNextFrame() 
{
    memcpy(fTo, _p, 2000);
    fFrameSize = 2000;
    int uSecsToDelay = 20000;
    fDurationInMicroseconds = 20000;
    afterGetting(this);
}
