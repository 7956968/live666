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
// Header file

#ifndef _MY_RTSP_SERVER_HH
#define _MY_RTSP_SERVER_HH

#ifndef _RTSP_SERVER_SUPPORTING_HTTP_STREAMING_HH
#include "RTSPServerSupportingHTTPStreaming.hh"
#endif

#define NAME_MAX (128)

class MyRTSPServer: public RTSPServerSupportingHTTPStreaming {
public:
  static MyRTSPServer* createNew(UsageEnvironment& env, 
				      UserAuthenticationDatabase* authDatabase,
				      unsigned reclamationTestSeconds = 65);

  virtual ServerMediaSession *
      lookupServerMediaSession(char const *streamName, Boolean isFirstLookupInSession)override;

  MyRTSPServer(UsageEnvironment& env, 
	  UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds);
  // called only by createNew();
  virtual ~MyRTSPServer();

protected: // redefined virtual functions
  
};

#endif
