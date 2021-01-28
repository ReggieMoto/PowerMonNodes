// ==============================================================
//
// class WifiSocket
//
// Copyright (c) 2017 David Hammond 
// All Rights Reserved.
// 
// ==============================================================
// NOTICE:  All information contained herein is and remains the
// property of David Hammond. The intellectual and technical
// concepts contained herein are proprietary to David Hammond
// and may be covered by U.S.and Foreign Patents, patents in
// process, and are protected by trade secret or copyright law.
// Dissemination of this information or reproduction of this
// material is strictly forbidden unless prior written permission
// is obtained David Hammond.
// ==============================================================

#include "WifiSocket.h"

#include <chrono>         // std::chrono::seconds
#include <errno.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <thread>         // std::this_thread::sleep_for

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

namespace powermon {

	static bool socketIsActive;


	WifiSocket::SockServer::SockServer(void) :
		sockfd(-1),
		serverIsActive(false)
	{
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd == -1) {
			cout << "Failed to server create socket" << endl;
		} else {
#if 1
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port   = htons(52955u);
			addr.sin_addr.s_addr = INADDR_ANY;

			int status = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
			if (status == -1) {
				cout << "Failed to bind to server socket" << endl;
			} else {
				serverIsActive = true;
				cout << "WiFi server socket is active" << endl;
			}
#else
			serverIsActive = true;
			cout << "WiFi server socket is active" << endl;
#endif
		}
	}

	ssize_t WifiSocket::SockServer::receive(uint8_t *data, size_t dataLen)
	{
		ssize_t bytesRcvd;

		if (serverIsActive) {

#if 0
			AvahiIPv4Address svcAddr = PwrmonSvcClient::getPwrmonSvcClient().getPwrmonSvcAddr();
#endif
			uint16_t port = PwrmonSvcClient::getPwrmonSvcClient().getPwrmonSvcPort();

			struct sockaddr_in src_addr;
			socklen_t addrlen = sizeof(struct sockaddr_in);

			int flags = (MSG_WAITALL);
			bytesRcvd = recvfrom(sockfd, data, dataLen, flags,
					(struct sockaddr *)&src_addr, &addrlen);

			if (bytesRcvd == -1) {
				int errnum = errno;

				cout << "Socket receive failed: " << strerror(errnum) << endl;
			} else {
				cout << "Received a packet" << endl;
				cout << "Avahi port: " << port << endl;
				cout << "Received port" << src_addr.sin_port << endl;
			}

		} else {
			cout << "Server socket is not active" << endl;
			bytesRcvd = 0;
		}

		return (bytesRcvd);
	}

	WifiSocket::SockServer& WifiSocket::SockServer::getSockServer(void)
	{
		static WifiSocket::SockServer sockServer;

		return (sockServer);
	}

	WifiSocket::SockClient::SockClient(void) :
		sockfd(-1),
		clientIsActive(false)
	{
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd == -1) {
			cout << "Failed to create client socket" << endl;
		} else {
#if 0
			AvahiIPv4Address svcAddr = PwrmonSvcClient::getPwrmonSvcClient().getPwrmonSvcAddr();
			uint16_t port = PwrmonSvcClient::getPwrmonSvcClient().getPwrmonSvcPort();

			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port   = htons(port);
			addr.sin_addr.s_addr = (in_addr_t)svcAddr.address;

			int status = connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
			if (status == -1) {
				cout << "Failed to connect to client socket service address" << endl;
			} else {
				clientIsActive = true;
				cout << "WiFi client socket is active" << endl;
			}
#else
			clientIsActive = true;
			cout << "WiFi client socket is active" << endl;
#endif
		}
	}

	ssize_t WifiSocket::SockClient::transmit(uint8_t *data, size_t dataLen)
	{
		ssize_t bytesSent;

		if (clientIsActive) {

			int flags = (MSG_DONTROUTE | MSG_DONTWAIT);
			ssize_t bytesSent = sendto(sockfd, data, dataLen, flags, NULL, 0);

			if (bytesSent == -1) {
				int errnum = errno;

				cout << "Socket send failed: " << strerror(errnum) << endl;
			}

		} else {
			cout << "Client socket is not active" << endl;
			bytesSent = 0;
		}

		return (bytesSent);
	}

	WifiSocket::SockClient& WifiSocket::SockClient::getSockClient(void)
	{
		static WifiSocket::SockClient sockClient;

		return (sockClient);
	}

	WifiSocket& WifiSocket::getWifiSocket(void)
	{
		static WifiSocket WifiSocket;

		return (WifiSocket);
	}


	WifiSocket::WifiSocket() :
		server(WifiSocket::SockServer::getSockServer()),
		client(WifiSocket::SockClient::getSockClient()),
		console(ConsoleOut::getConsoleOut())
	{
		wifiSocketMutex = new std::mutex;

		if ((server.isSockServerActive() == true) &&
			(client.isSockClientActive() == true)) {
			socketIsActive = true;
		} else {
			socketIsActive = false;
		}
	}

	void WifiSocket::sendPacket(char *packet)
	{
		if (isWifiSocketActive())
		{
			ostringstream string1;
			string1 << "Request to send a packet" << endl;
			string text = string1.str();
			console.queueOutput(text);
			string1.str("");
		}
	}

	bool WifiSocket::isWifiSocketActive(void)
	{
		wifiSocketMutex->lock();
		bool sockIsActive = socketIsActive;
		wifiSocketMutex->unlock();
		return sockIsActive;
	}

	void WifiSocket::setWifiSocketIsNotActive(void)
	{
		wifiSocketMutex->lock();
		socketIsActive = false;
		wifiSocketMutex->unlock();
	}

	void WifiSocket::setWifiSocketIsActive(void)
	{
		wifiSocketMutex->lock();
		socketIsActive = true;
		wifiSocketMutex->unlock();
	}

	WifiSocket::~WifiSocket()
	{
		free(wifiSocketMutex);
	}

} // namespace powermon
