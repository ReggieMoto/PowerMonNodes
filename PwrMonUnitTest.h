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

#include <vector>
#include "PowerMonNode.h"
#include "ConsoleOut.h"

namespace pwrmon
{

	class PwrMonUnitTest
	{
		PwrMonUnitTest(unsigned int nodeCount);
		int getNetworkAdaptorInfo(void);

		vector<PowerMonNode *> pwrMonNodes;
		ConsoleOut& console;

	public:
		static PwrMonUnitTest& getPwrMonUnitTest(unsigned int nodeCount);
		~PwrMonUnitTest();

		void reportNodes(void);
		void startThreads(void);
		void stopThreads(void);
	};

}
