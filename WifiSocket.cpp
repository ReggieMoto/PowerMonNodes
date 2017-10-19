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

//#include "stdafx.h"
#include <sstream>
#include "ConsoleOut.h"
#include "WifiSocket.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

using namespace std;

string Denali("10.0.0.84");
string Yosemite("10.0.0.8");
string SvcPort("52955");

extern "C" {
	DWORD WINAPI wifiSockethreadProc(_In_ LPVOID wifiSocket)
	{
		DWORD retValue = 0;
		((WifiSocket *)wifiSocket)->threadProc();
		return retValue;
	}
}

WifiSocket& WifiSocket::getWifiSocket(void)
{
	static WifiSocket WifiSocket;

	return WifiSocket;
}


WifiSocket::WifiSocket() :
	socketIsActive(TRUE),
	svcPort(SvcPort.c_str()),
	hostName(Denali.c_str()),
	connectSocket(INVALID_SOCKET),
	console(ConsoleOut::getConsoleOut())
{
	ostringstream string1;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		string1 << "Wifi Socket WSAStartup failed: " << iResult << endl;
		string text = string1.str();
		console.queueOutput(text);
		string1.str("");
		setWifiSocketIsNotActive();
	}
	else
		createSocket();
}

void WifiSocket::createSocket(void)
{
	ostringstream string1;
	ADDRINFOA hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //  AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM; //  SOCK_STREAM;
	hints.ai_protocol = IPPROTO_UDP; //  IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(hostName, svcPort, &hints, &result);
	if (iResult != 0) {
		string1 << "getaddrinfo failed: " << iResult << endl;
		string text = string1.str();
		console.queueOutput(text);
		string1.str("");
		setWifiSocketIsNotActive();
	}
	else
		attachSocket();
}

void WifiSocket::attachSocket(void)
{
	ostringstream string1;
	ADDRINFOA *ptr = NULL;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		if ((ptr->ai_family == AF_INET) &&
			(ptr->ai_socktype == SOCK_DGRAM) &&
			(ptr->ai_protocol == IPPROTO_UDP))
		{
			sockaddr_ipv4 = ((PSOCKADDR_IN)(ptr->ai_addr))->sin_addr;
			break;
		}
	}

	// Create a SOCKET for connecting to server
	srvrSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (srvrSocket == INVALID_SOCKET) {
		string1 << "Error at socket(): " << WSAGetLastError() << endl;
		setWifiSocketIsNotActive();
	}
	else
		string1 << "Socket successfully created at " << 
		(unsigned int)sockaddr_ipv4.S_un.S_un_b.s_b1 << "." << 
		(unsigned int)sockaddr_ipv4.S_un.S_un_b.s_b2 << "." <<
		(unsigned int)sockaddr_ipv4.S_un.S_un_b.s_b3 << "." <<
		(unsigned int)sockaddr_ipv4.S_un.S_un_b.s_b4 << "." << endl;

	string text = string1.str();
	console.queueOutput(text);
	string1.str("");

	freeaddrinfo(result);
}

void WifiSocket::send(char *packet)
{
	if (wifiSocketIsActive())
	{
		ostringstream string1;
		string1 << "Request to send a packet" << endl;
		string text = string1.str();
		console.queueOutput(text);
		string1.str("");
	}
}

void WifiSocket::threadProc(void)
{
	ostringstream string1;
	string1 << "Entering the Wifi Socket ThreadProc" << endl;
	string text = string1.str();
	console.queueOutput(text);
	string1.str("");

	do {

		this_thread::sleep_for(chrono::seconds(1));

	} while (socketIsActive);

	string1 << "Leaving the Wifi Socket ThreadProc" << endl;
	text = string1.str();
	console.queueOutput(text);
}

WifiSocket::~WifiSocket()
{
	WSACleanup();
}
