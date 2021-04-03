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

#include <cstdint>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <string>
#include <thread>

#include "powermon_pkt.h"

namespace powermon
{

	class WifiQueue;
	class ConsoleOut;
	class ThreadMsg;

	class PowerMonNode
	{
		public:
			typedef enum nodeType_e {
				nodeType_first = 0,
				nodeType_oldest = nodeType_first,
				nodeType_older,
				nodeType_old,
				nodeType_mid,
				nodeType_new,
				nodeType_newer,
				nodeType_newest,
				nodeType_last = nodeType_newest,
				nodeType_count
			} nodeType_t;

		private:
			/*
			 * temps run from 33 to 42, 9 degrees
			 * old inefficient nodes cool slowly and warm quickly
			 * new efficient nodes cool quickly and warm slowly
			 * oldest = cool down in 36.0 minutes; warm up in 18.0 minutes
			 * older  = cool down in 31.5 minutes; warm up in 27.0 minutes
			 * old    = cool down in 27.0 minutes; warm up in 36.0 minutes
			 * mid    = cool down in 22.5 minutes; warm up in 45.0 minutes
			 * new    = cool down in 18.0 minutes; warm up in 54.0 minutes
			 * newer  = cool down in 13.5 minutes; warm up in 63.0 minutes
			 * newest = cool down in  9.0 minutes; warm up in 72.0 minutes
			 */
			const uint32_t warmupScale[nodeType_count]   = {120,180,240,300,360,420,480};
			const uint32_t cooldownScale[nodeType_count] = {240,210,180,150,120,90,60};

			nodeType_t _type;
			uint32_t _secTime;

			Packet _packet;
			std::string _nodeIdentifier;
			ConsoleOut& _console;
			WifiQueue& _wifiQueue;

			std::thread _powerMonNodeTimerThread;
			std::thread _powerMonNodeThread;
			std::queue<std::shared_ptr<ThreadMsg>> _nodeData;
			std::mutex _powerMonNodeMutex;
			std::condition_variable _powerMonNodeCondVar;

			bool _timerActive;
			void _timerProcess(void);
			void _threadProcess(void);
			void _calcCurrentStatus(void);
		public:

			PowerMonNode(uint32_t nodeId, ConsoleOut& console, WifiQueue& wifiQueue);
			~PowerMonNode(void);

			std::string nodeId(void) { return (_nodeIdentifier); }
			inline Mode_e getMode(void) { return (_packet.node.mode); }
			inline Operation_e getOperation(void) { return (_packet.node.operation); }
			inline uint8_t getTemp(void) { return (_packet.data.temp); }
			inline uint8_t getAmps(void) { return (_packet.data.amps); }

			void exitPowerMonNode(void);
			void queueOutput(const std::shared_ptr<ThreadMsg> message);

			friend std::ostream& operator<<(std::ostream& os, const PowerMonNode& node);
	};
} // namespace powermon
