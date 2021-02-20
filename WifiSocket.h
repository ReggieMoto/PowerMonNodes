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
#pragma once

#include <mutex>
#include <netinet/in.h>

#include "ConsoleOut.h"
#include "PwrmonSvcClient.h"

namespace powermon {

#define NO_SOCKET -1

	class WifiSocket
	{
	// =============================
	class SockClient	// Talker
	// =============================
	{
		int _sockfd;
		ConsoleOut& _console;

	public:

		SockClient(ConsoleOut& console);

		bool isSockClientActive(void) { return (_sockfd != NO_SOCKET); }
		/*
		 * uint8_t *data - buffer of data to send
		 * uint16_t dataLen - number of bytes to send
		 * returns number of bytes sent
		 * A return of -1 means the transmit failed
		 */
		ssize_t transmit(uint8_t *data, size_t dataLen);
	};

	// =============================
	class SockServer	// Listener
	// =============================
	{
		int _sockfd;
		ConsoleOut& _console;

	public:

		SockServer(ConsoleOut& console);

		bool isSockServerActive(void) { return (_sockfd != NO_SOCKET); }

		/*
		 * uint8_t *data - buffer to place data into
		 * uint16_t dataLen - size of buffer
		 * returns number of bytes received
		 * A return of -1 means the receive failed
		 */
		ssize_t receive(uint8_t *data, size_t dataLen);
	};


		ConsoleOut& _console;

		bool _socketIsActive;
		SockServer _server;
		SockClient _client;

		WifiSocket(WifiSocket&) = delete;
		WifiSocket& operator=(WifiSocket&) = delete;

	public:

		WifiSocket(ConsoleOut& console);
		~WifiSocket(void);

		void sendPacket(char *packet);

		bool isWifiSocketActive(void);
		void setWifiSocketIsNotActive(void);
		void setWifiSocketIsActive(void);

	};

}
