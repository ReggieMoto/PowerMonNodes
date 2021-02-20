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
#include <mutex>
#include <ostream>
#include <string>

#include "WifiQueue.h"
#include "ConsoleOut.h"

namespace powermon
{

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

			static uint32_t const NodeIdLength = 32;
			static uint32_t const nodeVersion = 1;

		private:

			typedef struct _packet {
				uint32_t version;
				char nodeId[NodeIdLength];
				Mode_e mode;	// Auto = 1(default)/Off = 0
				Operation_e operation;	// Normal = 1 (default)/Defrost = 0
				uint32_t temp;
				uint32_t amps;
			} Packet;

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

		public:

			PowerMonNode(uint32_t nodeId, ConsoleOut& console, WifiQueue& wifiQueue);
			~PowerMonNode(void);

			std::string nodeId(void) { return (_nodeIdentifier); }
			inline Mode_e getMode(void) { return (_packet.mode); }
			inline Operation_e getOperation(void) { return (_packet.operation); }
			inline uint32_t getTemp(void) { return (_packet.temp); }
			inline uint32_t getAmps(void) { return (_packet.amps); }

			void exitPowerMonNode(void);
			void queueOutput(const std::shared_ptr<ThreadMsg> message);

			friend std::ostream& operator<<(std::ostream& os, const PowerMonNode& node);
	};
} // namespace powermon
