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
#include <thread>
#include <vector>

#include "ConsoleOut.h"
#include "PowerMonNode.h"

namespace powermon
{

	class PwrMonUnitTest
	{
		PwrMonUnitTest(uint32_t nodeCount);
		int getNetworkAdaptorInfo(void);

		std::vector<std::thread> pwrMonNodeThreads;
		std::vector<PowerMonNode *> pwrMonNodes;
		ConsoleOut& console;

	public:
		static PwrMonUnitTest& getPwrMonUnitTest(uint32_t nodeCount);
		~PwrMonUnitTest();

		// This component would not build. It would received the error
		// "result type must be constructible from value type of input range"
		// From stackoverflow: "This appears to be an instance of the old
		// 'move if noexcept' issue with std::vector"
		// In order to force std::vector<pwrMonNodeThread>::push_back to
		// use the move ctor, the copy ctor of pwrMonNodeThread has to be
		// marked as deleted
		PwrMonUnitTest(PwrMonUnitTest&&) noexcept(false) = default;

		void reportNodes(void);
		void startThreads(void);
		void stopThreads(void);
	};

}
