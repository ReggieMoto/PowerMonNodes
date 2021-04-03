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

	#define PWR_MON_NODE_SLEEP_DURATION 2
	#define PWR_MON_NODE_TEMP_INIT 70u

	void PowerMonNode::_timerProcess(void)
	{
		do {
			// Sleep for a bit (2 secs)
	        std::this_thread::sleep_for(2000ms);

		    // Create a new ThreadMsg
			std::string msg("Send a PowerMon node packet");
			queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgNodeData, msg));

		} while (_timerActive);

		std::ostringstream oss;
		oss << "Leaving the Powermon node timer thread for " << this->_nodeIdentifier;
		std::string msg = oss.str();
		_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
}

	void PowerMonNode::_threadProcess(void)
	{
		{
			std::ostringstream oss;
			oss << "Starting the Powermon node thread for " << this->_nodeIdentifier;
			std::string msg = oss.str();
			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
		}

		while (true)
	    {
	        std::shared_ptr<ThreadMsg> msg;

			std::unique_lock<std::mutex> lk(_powerMonNodeMutex);
			while (_nodeData.empty())
				_powerMonNodeCondVar.wait(lk);

			if (_nodeData.empty())
				continue;

			msg = _nodeData.front();
			_nodeData.pop();

	        switch (msg->getMsgId())
	        {
				case ThreadMsg::MsgId_MsgNodeData:
				{
					std::ostringstream oss;
					oss << *this;
					std::string nodeData =  oss.str();

				    _wifiQueue.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgNodeData, nodeData));
	    			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, nodeData));

					break;
				}

				case ThreadMsg::MsgId_MsgExitThread:
				{
					std::ostringstream oss;
					oss << "Leaving the Powermon node thread for " << this->_nodeIdentifier;
					std::string msg = oss.str();
	    			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
					return;
				}

				default:
				{
					std::ostringstream oss;
					oss << "Received an unhandled message: " << msg->getMsgId();
					std::string msg = oss.str();
	    			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
					break;
				}
			}
	    }
	}

	ostream& operator<<(ostream& os, const PowerMonNode& node)
	{
		os <<
			setfill('0') << setw(sizeof(node._packet.nodeId)) << node._packet.nodeId <<
			setfill('0') << setw(sizeof(node._packet.version)) << uint32_t(node._packet.version) <<
			setfill('0') << setw(sizeof(node._packet.temp)) << uint32_t(node._packet.temp) <<
			setfill('0') << setw(sizeof(node._packet.mode)) << uint32_t(node._packet.mode) <<
			setfill('0') << setw(sizeof(node._packet.operation)) << uint32_t(node._packet.operation) <<
			setfill('0') << setw(sizeof(node._packet.amps)) << uint32_t(node._packet.amps);

		return (os);
	}

	PowerMonNode::PowerMonNode(uint32_t nodeId, ConsoleOut& console, WifiQueue& wifiQueue) :
		_console(console),
		_wifiQueue(wifiQueue),
		_timerActive(true)
	{
		struct timespec ts;
		if(clock_gettime(CLOCK_MONOTONIC_RAW,&ts) != 0) {
			ts.tv_nsec = 0;
		}

		_nodeIdentifier = to_string(ts.tv_nsec) + "-" + to_string(nodeId);
		strncpy(_packet.nodeId, _nodeIdentifier.c_str(), NodeIdLength-1);
		_packet.version = nodeVersion;
		_packet.temp = PWR_MON_NODE_TEMP_INIT;
		_packet.mode = Auto;
		_packet.operation = Normal;
		_packet.amps = 0u;

		_powerMonNodeThread = std::thread(&PowerMonNode::_threadProcess, this);
		_powerMonNodeTimerThread = std::thread(&PowerMonNode::_timerProcess, this);
	}

	void PowerMonNode::queueOutput(const std::shared_ptr<ThreadMsg> message)
	{
	    // Add user data msg to queue and notify worker thread
	    std::unique_lock<std::mutex> lk(_powerMonNodeMutex);
	    _nodeData.push(message);
	    _powerMonNodeCondVar.notify_one();
	}

	void PowerMonNode::exitPowerMonNode(void)
	{
		_timerActive = false; // Exit the timer thread

		std::ostringstream oss;
		oss << "Request to exit the Powermon node thread for " << this->_nodeIdentifier;
		std::string msg = oss.str();
		queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgExitThread, msg));
	}

	PowerMonNode::~PowerMonNode(void)
	{
		_powerMonNodeThread.join();
		_powerMonNodeTimerThread.join();
    	cout << "Joined the Powermon node threads for " << this->_nodeIdentifier << endl;
	}

}
