// ==============================================================
//
// class ConsoleOut
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

#include <ThreadMsg.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace powermon {

	class ConsoleOut
	{
		std::unique_ptr<std::thread> _consoleOutThread;
		std::queue<std::shared_ptr<ThreadMsg>> _screenLine;
		std::mutex _consoleOutMutex;
		std::condition_variable _consoleOutCondVar;

		void _threadProcess(void);

	public:

		ConsoleOut(void);
		~ConsoleOut(void);

		void exitConsoleOut(void);
		void queueOutput(const std::shared_ptr<ThreadMsg> message);
	};

} // namespace powermon
