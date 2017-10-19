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
#pragma once

#include <string>
#include <iostream>
#include "Processthreadsapi.h"
#include "ConsoleOut.h"
#include "WifiQueue.h"


using namespace std;

class PowerMonNode
{
public:

	enum Mode_e {
		Off,
		Auto
	};

	enum Operation_e {
		Normal,
		Defrost
	};

	static unsigned int const NodeIdLength = 18;
	static unsigned int const nodeVersion = 1;

private:
	HANDLE nodeThread;
	bool threadIsActive;
	ConsoleOut& console;
	WifiQueue& wifiQueue;

	typedef struct _packet {
		unsigned int version;
		char nodeId[NodeIdLength];
		Mode_e mode;	// Auto = 1(default)/Off = 0
		Operation_e operation;	// Normal = 1 (default)/Defrost = 0
		unsigned int temp;
		unsigned int amps;
	} Packet;

	string nodeIdentifier;
	Packet packet;

public:
	PowerMonNode(unsigned int nodeId);
	~PowerMonNode();

	string nodeId(void) { return nodeIdentifier; }
	void threadProc(void);

	inline void resumeNodeThread(void) { if (nodeThread) ResumeThread(nodeThread); }
	inline void exitNodeThread(void) { threadIsActive = FALSE; }

	inline Mode_e getMode(void) { return packet.mode; }
	inline Operation_e getOperation(void) { return packet.operation; }
	inline unsigned int getTemp(void) { return packet.temp; }
	inline unsigned int getAmps(void) { return packet.amps; }

	friend ostream& operator<<(ostream& os, const PowerMonNode& node);
};

