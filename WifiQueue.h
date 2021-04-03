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

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace powermon {

	class ConsoleOut;
	class WifiSocket;
	class ThreadMsg;

	class WifiQueue
	{
		ConsoleOut& _console;
		bool _serviceAvailable;
		std::unique_ptr<WifiSocket> _socket;

		std::thread _wifiQueueThread;
		std::queue<std::shared_ptr<ThreadMsg>> _wifiPacketQueue;
		std::mutex _wifiQueueMutex;
		std::condition_variable _wifiQueueCondVar;

		WifiQueue(const WifiQueue&) = delete;
		WifiQueue& operator=(const WifiQueue&) = delete;

		// Entry point for the PowerMon WiFi queue thread
		void _threadProcess(void);

	public:

		WifiQueue(ConsoleOut& console);
		~WifiQueue(void);

		void exitWifiQueue(void);
		void queueOutput(const std::shared_ptr<ThreadMsg> message);
	};

} // namespace powermon
