// ==============================================================
//
// class PowerMonNode
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
#include "PowerMonNode.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iomanip>

using namespace std;


extern "C" {
	DWORD WINAPI NodeThreadProc(_In_ LPVOID pwrMonNode)
	{
		DWORD retValue = 0;
		((PowerMonNode *)pwrMonNode)->threadProc();
		return retValue;
	}
}

ostream& operator<<(ostream& os, const PowerMonNode& node)
{
	os <<
		setfill('0') << setw(sizeof(node.packet.nodeId)) << node.packet.nodeId <<
		setfill('0') << setw(sizeof(node.packet.version)) << (unsigned int)node.packet.version <<
		setfill('0') << setw(sizeof(node.packet.temp)) << (unsigned int)node.packet.temp <<
		setfill('0') << setw(sizeof(node.packet.mode)) << (unsigned int)node.packet.mode <<
		setfill('0') << setw(sizeof(node.packet.operation)) << (unsigned int)node.packet.operation <<
		setfill('0') << setw(sizeof(node.packet.amps)) << (unsigned int)node.packet.amps;

	return os;
}

PowerMonNode::PowerMonNode(unsigned int nodeId) :
	threadIsActive(TRUE),
	console(ConsoleOut::getConsoleOut()),
	wifiQueue(WifiQueue::getWifiQueue())
{
	SECURITY_ATTRIBUTES securityAttrs;

	securityAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttrs.lpSecurityDescriptor = NULL;
	securityAttrs.bInheritHandle = FALSE;

	ULONGLONG uptime = GetTickCount64(); //Serial number

	nodeIdentifier = to_string(uptime) + "-" + to_string(nodeId);
	strncpy_s(packet.nodeId, NodeIdLength, nodeIdentifier.c_str(), _TRUNCATE);
	packet.version = nodeVersion;
	packet.temp = 70;
	packet.mode = Auto;
	packet.operation = Normal;
	packet.amps = 0;

	// Create this node's thread
	nodeThread = CreateThread(
		&securityAttrs,
		0, // default dwStackSize
		NodeThreadProc, // lpStartAddress,
		(void *)this, //lpParameter,
		0x00000004, // CREATE_SUSPENDED, dwCreationFlags,
		NULL //lpThreadId
	);

	if (nodeThread == NULL)
	{
		ostringstream string1;
		string1 << "Failed to create thread for " << nodeId << endl;
		string text = string1.str();
		console.queueOutput(text);
	}
}


PowerMonNode::~PowerMonNode()
{
}

void PowerMonNode::threadProc(void)
{
	ostringstream string1, packet;
	string1 << "Entering the ThreadProc for " << nodeIdentifier << endl;
	string text = string1.str();
	console.queueOutput(text);
	string1.str("");

	do {
		this_thread::sleep_for(chrono::seconds(2));
		packet << *this;
		wifiQueue.queueOutput(packet.str());
		packet << endl;
		console.queueOutput(packet.str());
		packet.str("");
	} while (threadIsActive);

	string1 << "Leaving the ThreadProc for " << nodeIdentifier << endl;
	text = string1.str();
	console.queueOutput(text);
}
