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

#include "ConsoleOut.h"
#include "PowerMonNode.h"
#include "ThreadMsg.h"
#include "WifiQueue.h"

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

namespace powermon {

#define PWR_MON_NODE_SLEEP_DURATION 5000ms
#define PWR_MON_NODE_TEMP_INIT 70u
#define	PWR_MON_MAX_AMPS 12u
#define	PWR_MON_NOM_AMPS 6u
#define PWR_MON_MIN_TEMP 33u
#define PWR_MON_MAX_TEMP 42u

	void PowerMonNode::_calcCurrentStatus(void)
	{
		_secTime -= 5;

		// Stay at MAX AMPs for 5 seconds then drop to Nominal
		if ((_packet.node.operation == Cooling) &&
			(_packet.data.amps == PWR_MON_MAX_AMPS) &&
			(_secTime == (cooldownScale[_type]-5))) {
			_packet.data.amps = PWR_MON_NOM_AMPS;
		}

		if (_secTime == 0) { // We counted down for a temp change

			// See whether we're cooling or not
			if (_packet.node.operation == Cooling) {

				_packet.data.temp -= 1; /* Go down a degree */

				// Now see whether we need to transition to idle/warmup
				if (_packet.data.temp == PWR_MON_MIN_TEMP) {
					_packet.node.operation = Idle;
					_packet.data.amps = 0u;
					_secTime = warmupScale[_type];
				} else {
					// Nope. Just keep cooling
					_secTime = cooldownScale[_type];
				}

			} else { // Not cooling so either Idle, defrost or in OFF mode

				_packet.data.temp += 1; /* Go up a degree */

				// Now see whether we need to transition to cooling
				if (_packet.data.temp == PWR_MON_MAX_TEMP) {
					_packet.node.operation = Cooling;
					_packet.data.amps = PWR_MON_MAX_AMPS;
					_secTime = cooldownScale[_type];
				} else {
					// Nope. Just let it warm up
					_secTime = warmupScale[_type];
				}
			}
		}
	}

	void PowerMonNode::_timerProcess(void)
	{
		std::ostringstream oss;
		oss << "Starting the Powermon node timer thread for " << this->_nodeIdentifier;
		std::string msg = oss.str();
		_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));

		do {
			// Sleep for a bit (5 secs)
	        std::this_thread::sleep_for(PWR_MON_NODE_SLEEP_DURATION);

	        // Update the current temp/amp draw
			_calcCurrentStatus();

			// Create a new ThreadMsg
			std::string msg("Send a PowerMon node packet");
			queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgNodeData, msg));

		} while (_timerActive);

		oss.clear();
		oss << "Leaving the Powermon node timer thread for " << this->_nodeIdentifier;
		msg = oss.str();
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
					ThreadMsg::DataMsg_t data;
					data.len = sizeof(Packet);
					memcpy(data.buffer, &_packet, sizeof(Packet));
				    _wifiQueue.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgNodeData, data));

				    std::ostringstream oss;
					oss << *this;
					std::string nodeData =  oss.str();

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
			setfill('0') << setw(sizeof(node._packet.version)) << (node._packet.version) << "." <<
			setfill('0') << setw(sizeof(node._packet.msgType)) << (node._packet.msgType) << "." <<
			setfill('0') << setw(sizeof(node._packet.node.serialNumber.mfgId)) << node._packet.node.serialNumber.mfgId <<  "-" <<
			setfill('0') << setw(sizeof(node._packet.node.serialNumber.nodeId)) << node._packet.node.serialNumber.nodeId << "." <<
			setfill('0') << setw(sizeof(node._type)) << int(node._type) <<  "." <<
			setfill('0') << setw(sizeof(node._packet.node.mode)) << (node._packet.node.mode) << "." <<
			setfill('0') << setw(sizeof(node._packet.node.operation)) << (node._packet.node.operation) << "." <<
			setfill('0') << setw(sizeof(node._packet.data.temp)) << int(node._packet.data.temp) << "." <<
			setfill('0') << setw(sizeof(node._packet.data.amps)) << int(node._packet.data.amps) << ".";

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

		_type = nodeType_t((rand() % nodeType_count));
		_secTime = cooldownScale[_type];

		uint64_t mfgId = (uint64_t)ts.tv_nsec;
		_nodeIdentifier = to_string(mfgId) + "-" + to_string(nodeId);

		_packet.node.serialNumber.mfgId = mfgId;
		_packet.node.serialNumber.nodeId = nodeId;
		_packet.version = NodeVersion;
		_packet.msgType = client_Data;
		_packet.node.nodeIp = 0u;
		_packet.node.mode = Auto;
		_packet.node.operation = Cooling;
		_packet.data.temp = PWR_MON_NODE_TEMP_INIT;
		_packet.data.amps = PWR_MON_MAX_AMPS;

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
