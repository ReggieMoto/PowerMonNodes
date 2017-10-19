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

//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif

#include <winsock2.h>
//#include <windows.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

class WifiSocket
{
	WifiSocket();

	WSADATA wsaData;
	SOCKET srvrSocket;
	const char *svcPort; // UDP
	const char *hostName;
	ADDRINFOA *result = NULL;
	IN_ADDR  sockaddr_ipv4;

	HANDLE wifiSocketThread;
	bool socketIsActive;
	ConsoleOut& console;

	HANDLE ghMutex;

	void createSocket(void);
	void attachSocket(void);

public:

	static WifiSocket& getWifiSocket(void);
	~WifiSocket();

	void send(char *packet);

	void threadProc(void);
	inline bool wifiSocketIsActive(void) { return (socketIsActive == TRUE); }
	inline bool wifiSocketIsNotActive(void) { return (socketIsActive == FALSE); }
	inline void setWifiSocketIsNotActive(void) { socketIsActive = FALSE; }
	inline void setWifiSocketIsActive(void) { socketIsActive = TRUE; }

};

