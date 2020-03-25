/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2014, Live Networks, Inc.  All rights reserved
// A subclass of "RTSPServer" that creates "ServerMediaSession"s on demand,
// based on whether or not the specified stream name exists as a file
// Implementation

#include "MyRTSPServer.hh"
#include <liveMedia.hh>
#include <string.h>

#include "MyMetaMediaSubsession.hh"

MyRTSPServer*
MyRTSPServer::createNew(UsageEnvironment& env, 
	UserAuthenticationDatabase* authDatabase,
	unsigned reclamationTestSeconds) {
	return new MyRTSPServer(env, authDatabase, reclamationTestSeconds);
}


MyRTSPServer::MyRTSPServer(UsageEnvironment& env, 
				     UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds)
  : RTSPServerSupportingHTTPStreaming(env, authDatabase, reclamationTestSeconds) {
  //product_init();
}

MyRTSPServer::~MyRTSPServer() {
}

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
					char const* fileName, FILE* fid); // forward

ServerMediaSession* MyRTSPServer::lookupServerMediaSession(char const* streamName, Boolean isFirstLookupInSession)
{

  Boolean fileExists = True;
  fileExists = (
         strcmp(streamName, "ch0.liv")  == 0 
      || strcmp(streamName, "ch1.liv")  == 0
      || strcmp(streamName, "ch2.liv")  == 0
      || strcmp(streamName, "ch3.liv")  == 0
      || strcmp(streamName, "ch4.liv")  == 0
      || strcmp(streamName, "ch5.liv")  == 0
      || strcmp(streamName, "ch6.liv")  == 0
      || strcmp(streamName, "ch7.liv")  == 0
      || strcmp(streamName, "ch8.liv")  == 0
      || strcmp(streamName, "ch9.liv")  == 0
      || strcmp(streamName, "ch10.liv") == 0
      || strcmp(streamName, "ch11.liv") == 0
      || strcmp(streamName, "ch12.liv") == 0
      || strcmp(streamName, "ch13.liv") == 0
      || strcmp(streamName, "ch14.liv") == 0
      || strcmp(streamName, "ch15.liv") == 0) ? True : False;

  printf("fileExists:%d\n", fileExists);
  // Next, check whether we already have a "ServerMediaSession" for this file:
  if(!fileExists){
    return NULL;
  }

  ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(streamName);
  Boolean smsExists = sms != NULL;
  printf("smsExists:%d\n", smsExists);

  if (!smsExists) {
    // Create a new "ServerMediaSession" object for streaming from the named file.
    sms = createNewSMS(envir(), streamName, 0);
    if(sms){
      addServerMediaSession(sms);
    }
  }
  return sms;
}

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
					char const* streamName, FILE* /*fid*/) {
 
  ServerMediaSession* sms = NULL;

  char const* descStr = "streamed by the LIVE555 Media Server";

  sms = ServerMediaSession::createNew(env, streamName, streamName, descStr);
  ServerMediaSubsession* video_subsession = NULL;
  video_subsession = MyMetaMediaSubsession::createNew(env, streamName, True);
  sms->addSubsession(video_subsession);
  return sms;
}
