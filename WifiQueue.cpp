// ==============================================================
//
// class WifiQueue
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
#include "WifiQueue.h"
#include <iostream>
#include <sstream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

using namespace std;

extern "C" {
	DWORD WINAPI WifiQueueThreadProc(_In_ LPVOID wifiQueue)
	{
		DWORD retValue = 0;
		((WifiQueue *)wifiQueue)->threadProc();
		return retValue;
	}
}

WifiQueue& WifiQueue::getWifiQueue(void)
{
	static WifiQueue WifiQueue;

	return WifiQueue;
}

WifiQueue::WifiQueue() :
	threadIsActive(TRUE),
	console(ConsoleOut::getConsoleOut()),
	socket(WifiSocket::getWifiSocket())
{
	// Create a mutex with no initial owner
	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);

	SECURITY_ATTRIBUTES securityAttrs;

	securityAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttrs.lpSecurityDescriptor = NULL;
	securityAttrs.bInheritHandle = FALSE;

	// Create this node's thread
	wifiQueueThread = CreateThread(
		&securityAttrs,
		0, // default dwStackSize
		WifiQueueThreadProc, // lpStartAddress,
		(void *)this, //lpParameter,
		0x00000000, // CREATE_SUSPENDED, dwCreationFlags,
		NULL //lpThreadId
	);

	if (wifiQueueThread == NULL)
	{
		ostringstream string1;
		string1 << "Failed to create WiFi Thread Queue." << endl;
		string text = string1.str();
		console.queueOutput(text);
	}
}

void WifiQueue::queueOutput(string& packet)
{
	DWORD semWaitResult;
	char *pkt = new char[packet.length() + 1];
	strcpy_s(pkt, packet.length() + 1, packet.c_str());

	semWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	packetQueue.push(pkt);
	ReleaseMutex(ghMutex);
}

void WifiQueue::threadProc(void)
{
	DWORD semWaitResult;

	ostringstream string1;
	string1 << "Entering the Wifi Queue ThreadProc" << endl;
	string text = string1.str();
	console.queueOutput(text);
	string1.str("");

	do {
		bool consoleNotification = TRUE;

		while (!packetQueue.empty())
		{
			semWaitResult = WaitForSingleObject(ghMutex, INFINITE);
			char *packet = packetQueue.front();
			socket.send(packet);
			packetQueue.pop();
			ReleaseMutex(ghMutex);

			if (socket.wifiSocketIsActive())
			{
				if (consoleNotification)
				{
					consoleNotification = FALSE;
					ostringstream string1;
					string1 << "Sending powermon data packet(s)." << endl;
					string text = string1.str();
					console.queueOutput(text);
					string1.str("");
				}

				// Send the packet here
				socket.send(packet);
			}

			free(packet);
		}

		consoleNotification = TRUE; // Reset for next

		this_thread::sleep_for(chrono::seconds(1));

	} while (threadIsActive);

	string1 << "Leaving the Wifi Queue ThreadProc" <<  endl;
	text = string1.str();
	console.queueOutput(text);
}

WifiQueue::~WifiQueue()
{
	DWORD semWaitResult;

	while (!packetQueue.empty())
	{
		semWaitResult = WaitForSingleObject(ghMutex, INFINITE);
		char *packet = packetQueue.front();
		packetQueue.pop();
		ReleaseMutex(ghMutex);

		free(packet);
	}
}
