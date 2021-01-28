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

#include "PowerMonNode.h"

#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "ConsoleOut.h"
#include "WifiQueue.h"

using namespace std;

namespace powermon {

	static bool nodeThreadsActive = true;

	#define PWR_MON_NODE_SLEEP_DURATION 2
	#define PWR_MON_NODE_TEMP_INIT 70u

	void powerMonNodeThreadProc(PowerMonNode& node)
	{
		ConsoleOut&  console = ConsoleOut::getConsoleOut();
		WifiQueue& wifiQueue = WifiQueue::getWifiQueue();
		ostringstream packet;

		do {
			this_thread::sleep_for(chrono::seconds(PWR_MON_NODE_SLEEP_DURATION));
			packet << node;
			wifiQueue.queueOutput(packet.str());
			packet << endl;
			console.queueOutput(packet.str());
			packet.str("");
		} while (nodeThreadsAreActive());

		cout << "Leaving the ThreadProc for " << node.nodeIdentifier << endl;
	}

	ostream& operator<<(ostream& os, const PowerMonNode& node)
	{
		os <<
			setfill('0') << setw(sizeof(node.packet.nodeId)) << node.packet.nodeId <<
			setfill('0') << setw(sizeof(node.packet.version)) << (uint32_t)node.packet.version <<
			setfill('0') << setw(sizeof(node.packet.temp)) << (uint32_t)node.packet.temp <<
			setfill('0') << setw(sizeof(node.packet.mode)) << (uint32_t)node.packet.mode <<
			setfill('0') << setw(sizeof(node.packet.operation)) << (uint32_t)node.packet.operation <<
			setfill('0') << setw(sizeof(node.packet.amps)) << (uint32_t)node.packet.amps;

		return (os);
	}

	PowerMonNode::PowerMonNode(uint32_t nodeId)
	{
		struct timespec ts;
		if(clock_gettime(CLOCK_MONOTONIC_RAW,&ts) != 0) {
			ts.tv_nsec = 0;
		}

		nodeIdentifier = to_string(ts.tv_nsec) + "-" + to_string(nodeId);
		strncpy(packet.nodeId, nodeIdentifier.c_str(), NodeIdLength-1);
		packet.version = nodeVersion;
		packet.temp = PWR_MON_NODE_TEMP_INIT;
		packet.mode = Auto;
		packet.operation = Normal;
		packet.amps = 0u;
	}


	PowerMonNode::~PowerMonNode()
	{
	}

	PowerMonNode::PowerMonNode(PowerMonNode& node)
	{
		this->nodeIdentifier = node.nodeIdentifier;

		this->packet.version = node.packet.version;
		this->packet.mode = node.packet.mode;
		this->packet.operation = node.packet.operation;
		this->packet.temp = node.packet.temp;
		this->packet.amps = node.packet.amps;
		memcpy(&(this->packet.nodeId), &(node.packet.nodeId), NodeIdLength);
	}

	void releaseNodeThreads(void)
	{
		std::mutex& nodeMutex = PowerMonNode::getPowerMonNodesMutex();
		nodeMutex.lock();
		nodeThreadsActive = false;
		nodeMutex.unlock();
	}

	bool nodeThreadsAreActive(void)
	{
		std::mutex& nodeMutex = PowerMonNode::getPowerMonNodesMutex();
		nodeMutex.lock();
		bool areThreadsActive = nodeThreadsActive;
		nodeMutex.unlock();
		return (areThreadsActive);
	}

	std::mutex& PowerMonNode::getPowerMonNodesMutex(void)
	{
		static std::mutex powerMonNodeMutex;

		return (powerMonNodeMutex);
	}
}
