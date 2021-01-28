// ==============================================================
//
// class WifiQueue
//
// Copyright (c) 2017, 2021 David Hammond
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

#include "ConsoleOut.h"
#include "WifiSocket.h"

#include <mutex>
#include <queue>
#include <string>

namespace powermon {

	void wifiQueueThreadProc(void);

	class WifiQueue
	{
			ConsoleOut& console;
			WifiSocket& socket;

			std::queue<  char *> packetQueue;
			std::mutex *wifiQueueMutex;

			WifiQueue();

		public:

			static WifiQueue& getWifiQueue(void);
			friend void wifiQueueThreadProc(void);

			void queueOutput(const std::string& packetData);
			bool isPacketQueueEmpty(void);
			bool isWifiQueueActive(void);
			void releaseWifiThread(void);

			~WifiQueue();
	};

} // namespace powermon
