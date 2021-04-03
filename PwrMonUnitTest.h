// ==============================================================
//
// class PwrMonUnitTest
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
#include <vector>

namespace powermon
{
	class ConsoleOut;
	class WifiQueue;
	class PowerMonNode;

	class PwrMonUnitTest
	{
		std::vector<std::unique_ptr<PowerMonNode>> _pwrMonNodes;
		uint32_t _nodeCount;
		ConsoleOut& _console;
		WifiQueue& _wifiQueue;

	public:

		PwrMonUnitTest(uint32_t nodeCount, ConsoleOut& console, WifiQueue& wifiQueue);
		~PwrMonUnitTest(void);

		void exitPwrMonUnitTest(void);

		void reportNodes(void);
		void startThreads(void);
		void stopThreads(void);
	};
}
