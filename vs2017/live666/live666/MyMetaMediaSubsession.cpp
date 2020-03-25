
#include "MyMetaMediaSubsession.hh"
#include <liveMedia.hh>
#include <InputFile.hh>
#include "BasicUDPSource.hh"

#ifndef _RTSP_SERVER_SUPPORTING_HTTP_STREAMING_HH
#include "RTSPServerSupportingHTTPStreaming.hh"
#endif

#include "MyMetaMediaSource.hh"

MyMetaMediaSubsession* MyMetaMediaSubsession::createNew(UsageEnvironment& env, const char* streamName, Boolean reuseFirstSource) 
{
    return new MyMetaMediaSubsession(env, streamName, reuseFirstSource);
}

MyMetaMediaSubsession::MyMetaMediaSubsession(UsageEnvironment& env, const char* streamName, Boolean reuseFirstSource)
: OnDemandServerMediaSubsession(env, reuseFirstSource)
{
    _streamName = streamName;
}

MyMetaMediaSubsession::~MyMetaMediaSubsession() {
}

void MyMetaMediaSubsession::startStream(unsigned clientSessionId, void* streamToken,
                                    TaskFunc* rtcpRRHandler,
                                    void* rtcpRRHandlerClientData,
                                    unsigned short& rtpSeqNum,
                                    unsigned& rtpTimestamp,
                                    ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
                                    void* serverRequestAlternativeByteHandlerClientData)
{
    // Call the original, default version of this routine:
    OnDemandServerMediaSubsession::startStream(clientSessionId, streamToken,
    				     rtcpRRHandler, rtcpRRHandlerClientData,
    				     rtpSeqNum, rtpTimestamp,
    				     serverRequestAlternativeByteHandler, serverRequestAlternativeByteHandlerClientData);
}

void MyMetaMediaSubsession::deleteStream(unsigned clientSessionId,
						 void*& streamToken) {
						 
    // Call the original, default version of this routine:
    OnDemandServerMediaSubsession::deleteStream(clientSessionId, streamToken);
}

//char const* MyMetaMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
//    return rtpSink == NULL ? NULL : rtpSink->auxSDPLine();
//}

FramedSource * MyMetaMediaSubsession::createNewStreamSource(unsigned clientSessionId,
                                              unsigned &estBitrate)
{
	MyMetaMediaSource *source = MyMetaMediaSource::createNew(envir(), _streamName.c_str());
    return source;
}                                

RTPSink *MyMetaMediaSubsession::createNewRTPSink(Groupsock *rtpGroupsock,
                                                         unsigned char rtpPayloadTypeIfDynamic,
                                                         FramedSource *inputSource) {
  SimpleRTPSink *sink = SimpleRTPSink::createNew(envir(), rtpGroupsock, 112,
                                                 90000, "video", "LIVE555", 1, False, True);
  //sink->setOnSendErrorFunc(onSendError, this);
  return sink;
}

char const *MyMetaMediaSubsession::sdpLines() {

  if (fSDPLines == NULL) {
    // We need to construct a set of SDP lines that describe this
    // subsession (as a unicast stream).  To do so, we first create
    // dummy (unused) source and "RTPSink" objects,
    // whose parameters we use for the SDP lines:
    unsigned estBitrate;
    FramedSource *inputSource = createNewStreamSource(0, estBitrate);
    if (inputSource == NULL) {
      return NULL;
    } // file not found

    struct in_addr dummyAddr;
    dummyAddr.s_addr = 0;
    Groupsock *dummyGroupsock = createGroupsock(dummyAddr, 0);
    unsigned char rtpPayloadType = 96 + trackNumber() - 1; // if dynamic
    RTPSink *dummyRTPSink = createNewRTPSink(dummyGroupsock, rtpPayloadType, inputSource);
    if (dummyRTPSink != NULL && dummyRTPSink->estimatedBitrate() > 0) {
      estBitrate = dummyRTPSink->estimatedBitrate();
    }

    setSDPLinesFromRTPSink(dummyRTPSink, inputSource, estBitrate);
    Medium::close(dummyRTPSink);
    delete dummyGroupsock;
    closeStreamSource(inputSource);
  }

  envir() << fSDPLines << "\n";

  return fSDPLines;
}

void MyMetaMediaSubsession::setSDPLinesFromRTPSink(RTPSink *rtpSink,
                                                           FramedSource *inputSource,
                                                           unsigned estBitrate) {
  if (rtpSink == NULL) {
    return;
  }

  char const *mediaType = rtpSink->sdpMediaType();
  unsigned char rtpPayloadType = rtpSink->rtpPayloadType();
  AddressString ipAddressStr(fServerAddressForSDP);
  char *rtpmapLine = rtpSink->rtpmapLine();
  char const *rangeLine = rangeSDPLine();

  char const *const sdpFmt =
    "m=%s %u RTP/AVP %d\r\n"
    "c=IN IP4 %s\r\n"
    "b=AS:%u\r\n"
    "%s"
    "%s"
    "a=control:%s\r\n";
  unsigned sdpFmtSize = strlen(sdpFmt)
    + strlen(mediaType) + 5 /* max short len */ + 3 /* max char len */
    + strlen(ipAddressStr.val())
    + 20 /* max int len */
    + strlen(rtpmapLine)
    + strlen(rangeLine)
    + strlen(trackId());
  char *sdpLines = new char[sdpFmtSize];
  sprintf(sdpLines, sdpFmt,
          mediaType, // m= <media>
          fPortNumForSDP, // m= <port>
          rtpPayloadType, // m= <fmt list>
          ipAddressStr.val(), // c= address
          estBitrate, // b=AS:<bandwidth>
          rtpmapLine, // a=rtpmap:... (if present)
          rangeLine, // a=range:... (if present)
          trackId()); // a=control:<track-id>
  delete[] (char *) rangeLine;
  delete[] rtpmapLine;

  fSDPLines = strDup(sdpLines);
  delete[] sdpLines;
}