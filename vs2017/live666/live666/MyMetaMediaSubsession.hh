
#ifndef _MY_META_DATA_SERVER_MEDIA_SUBSESSION_HH
#define _MY_META_DATA_SERVER_MEDIA_SUBSESSION_HH

#include "OnDemandServerMediaSubsession.hh"

#include <string>

class MyMetaMediaSubsession: public OnDemandServerMediaSubsession {
public:
    static MyMetaMediaSubsession* createNew(UsageEnvironment& env, const char* streamName, Boolean reuseFirstSource);

    virtual void startStream(unsigned clientSessionId, void* streamToken,
                            TaskFunc* rtcpRRHandler,
                            void* rtcpRRHandlerClientData,
                            unsigned short& rtpSeqNum,
                            unsigned& rtpTimestamp,
                            ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
                            void* serverRequestAlternativeByteHandlerClientData);

    virtual void deleteStream(unsigned clientSessionId, void*& streamToken);
private:
    MyMetaMediaSubsession(UsageEnvironment& env, const char* streamName, Boolean reuseFirstSource);

    virtual ~MyMetaMediaSubsession();

protected: // redefined virtual functions
  virtual FramedSource *createNewStreamSource(unsigned clientSessionId,
                                              unsigned &estBitrate);
  // "estBitrate" is the stream's estimated bitrate, in kbps
  virtual RTPSink *createNewRTPSink(Groupsock *rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
                                    FramedSource *inputSource);

  virtual char const *sdpLines();

 private:
  void setSDPLinesFromRTPSink(RTPSink *rtpSink, FramedSource *inputSource,
                              unsigned estBitrate);
  std::string _streamName;
};


#endif
