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

#include "ConsoleOut.h"
#include "powermon_pkt.h"
#include "PwrmonSvcClient.h"
#include "ThreadMsg.h"
#include "WifiSocket.h"

#include <chrono>         // std::chrono::seconds
#include <errno.h>
#include <iostream>
#include <ostream>
#include <sstream>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace powermon {

	static bool socketIsActive;

#define PWRMON_PORT 52955u

	WifiSocket::SockServer::SockServer(ConsoleOut& console) :
		_sockfd(-1),
		_console(console)
	{
		_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (_sockfd == -1) {
			std::string msg("Failed to create server socket");
			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
		} else {

			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port   = htons(PWRMON_PORT);
			addr.sin_addr.s_addr = INADDR_ANY;

			int status = bind(_sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
			if (status == -1) {
				std::string msg("Failed to bind to server socket");
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
				close(_sockfd);
			} else {
				std::string msg("WiFi server socket is active");
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
			}
		}
	}

	ssize_t WifiSocket::SockServer::receive(uint8_t *data, size_t dataLen)
	{
		ssize_t bytesRcvd;

		if (_sockfd != NO_SOCKET) {

			uint16_t port = getPwrmonSvcPort();

			struct sockaddr_in src_addr;
			socklen_t addrlen = sizeof(struct sockaddr_in);

			int flags = (MSG_WAITALL);
			bytesRcvd = recvfrom(_sockfd, data, dataLen, flags,
					(struct sockaddr *)&src_addr, &addrlen);

			if (bytesRcvd == -1) {
				int errnum = errno;

				std::ostringstream oss;
				oss << "Socket receive failed: " << strerror(errnum);
				std::string msg = oss.str();
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
			} else {
				std::string msg(nullptr);
				msg = "Received a packet";
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));

				std::ostringstream oss;
				oss << "Avahi port: " << port;
				msg = oss.str();
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));

				oss.str("");
				oss << "Received port" << src_addr.sin_port;
				msg = oss.str();
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
			}

		} else {
			std::string msg("Server socket is not active");
			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
			bytesRcvd = 0;
		}

		return (bytesRcvd);
	}

	WifiSocket::SockClient::SockClient(ConsoleOut& console) :
		_sockfd(-1),
		_console(console)
	{
		_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (_sockfd == -1) {
			std::string msg("Failed to create client socket");
			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
			close(_sockfd);
		} else {

			AvahiIPv4Address svcAddr = getPwrmonSvcAddr();
			uint16_t port = getPwrmonSvcPort();

			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port   = htons(port);
			addr.sin_addr.s_addr = (in_addr_t)svcAddr.address;

			int status = connect(_sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
			if (status == -1) {
				cout << "Failed to connect to client socket service address" << endl;
				close(_sockfd);
			} else {
				std::string msg("WiFi client socket is active");
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
			}
		}
	}

	ssize_t WifiSocket::SockClient::transmit(void *data, size_t dataLen)
	{
		ssize_t bytesSent;

		if (_sockfd != -1) {

			int flags = (MSG_DONTROUTE | MSG_DONTWAIT);
			bytesSent = sendto(_sockfd, data, dataLen, flags, NULL, 0);

			if (bytesSent == -1) {
				int errnum = errno;

				std::ostringstream oss;
				oss << "Socket send failed: " << strerror(errnum);
				std::string msg = oss.str();
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
			} else {
#if 0
				std::ostringstream oss;
				oss << "Socket send succeeded. (sent " << bytesSent << "bytes)";
				std::string msg = oss.str();
				_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
#endif
			}

		} else {
			std::string msg("Client socket is not active");
			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
			bytesSent = 0;
		}

		return (bytesSent);
	}

	WifiSocket::WifiSocket(ConsoleOut& console) :
		_console(console),
		_socketIsActive(false),
		_server(SockServer(console)),
		_client(SockClient(console))
	{
		if ((_server.isSockServerActive() == true) &&
			(_client.isSockClientActive() == true)) {
			_socketIsActive = true;
		}

		AvahiIPv4Address svcAddr = getPwrmonSvcAddr();
		uint16_t port = getPwrmonSvcPort();

		std::ostringstream oss;
		oss << "Socket " << svcAddr.address << ":" << port << " is active";
		std::string msg = oss.str();
		_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
	}

	void WifiSocket::sendPacket(char *packet)
	{
		if (_server.isSockServerActive())
		{
 			ssize_t sentCount = _client.transmit(packet, sizeof(Packet));
		} else {
			std::string msg("Sock: Socket is not active");
 			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
		}
	}

	bool WifiSocket::isWifiSocketActive(void)
	{
		return socketIsActive;
	}

	void WifiSocket::setWifiSocketIsNotActive(void)
	{
		socketIsActive = false;
	}

	void WifiSocket::setWifiSocketIsActive(void)
	{
		socketIsActive = true;
	}

	WifiSocket::~WifiSocket()
	{
	}

} // namespace powermon
