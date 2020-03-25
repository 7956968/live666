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
// LIVE555 Media Server
// main program

#include <thread>
#include <iostream>

#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>

#include "MyRTSPServer.hh"
#include "version.hh"

using namespace std;

bool RtspShouldExit(RTSPServer* rtspServer)
{
	return rtspServer->fClientConnections->IsEmpty();
}

void SingleRtspServer(int clientSocket, struct sockaddr_in clientAddr)
{
	cout << "new connect from: "<< AddressString(clientAddr).val();
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase* authDB = NULL;

	RTSPServer* rtspServer;
	rtspServer = MyRTSPServer::createNew(*env, authDB, 0);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		return;
	}

	rtspServer->incomingClientSocket(clientSocket, clientAddr);

	BasicTaskScheduler0* sch = (BasicTaskScheduler0*)scheduler;
	while (1) {
		if (RtspShouldExit(rtspServer)) {
			break;
		}
		sch->SingleStep();
	}
	delete (MyRTSPServer*)rtspServer;
	env->reclaim();
	delete scheduler;

	cout << "connect:" << AddressString(clientAddr).val() << " quit";
}

void incomingConnectionHandler(void* instance, int /*mask*/)
{
	struct sockaddr_in clientAddr;
	SOCKLEN_T clientAddrLen = sizeof clientAddr;
	int fServerSocket = *(int*)instance;

	int clientSocket = accept(fServerSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket < 0) {
		printf("accpet error\n");
		return;
	}
	else {
		std::thread t(SingleRtspServer, clientSocket, clientAddr);
		t.detach();
	}
}

int main(int argc, char** argv) {

	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

	*env << "LIVE555 Media Server\n";
	*env << "\tversion " << MEDIA_SERVER_VERSION_STRING
		<< " (LIVE555 Streaming Media library version "
		<< LIVEMEDIA_LIBRARY_VERSION_STRING << ").\n";

	*env << "Play streams from this server using the URL\n\t"
		<< "rtsp://"
		<< AddressString(ourIPAddress(*env)).val()
		<< "<filename>\nwhere <filename> is a file present in the current directory.\n";
	*env << "Each file's type is inferred from its name suffix:\n";
	*env << "\t\"ch(0~15)\" => a live Xf intelligent Stream\n";
	*env << "See http://www.live555.com/mediaServer/ for additional documentation.\n";

	Port a(554);
	int fServerSocket = GenericMediaServer::setUpOurSocket(*env, a);
	if (fServerSocket == -1) return NULL;

	env->taskScheduler().turnOnBackgroundReadHandling(fServerSocket, incomingConnectionHandler, &fServerSocket);

	env->taskScheduler().doEventLoop();

	::closeSocket(fServerSocket);

	return 0; // only to prevent compiler warning
}
