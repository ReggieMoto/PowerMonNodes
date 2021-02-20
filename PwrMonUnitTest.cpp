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

#include "PwrMonUnitTest.h"

#include <iostream>
#include "PowerMonNode.h"

using namespace std;

namespace powermon
{

	PwrMonUnitTest::PwrMonUnitTest(uint32_t nodeCount, ConsoleOut& console, WifiQueue& wifiQueue) :
		_nodeCount(nodeCount),
		_console(console),
		_wifiQueue(wifiQueue)
	{
		for (uint32_t i = 0; i < nodeCount; i++) {

			_pwrMonNodes.push_back(std::make_unique<PowerMonNode>(i+1u, _console, _wifiQueue));
		}
	}

	void PwrMonUnitTest::reportNodes(void)
	{
		std::vector<std::unique_ptr<PowerMonNode>>::iterator pwrMonNode;

		for (pwrMonNode = _pwrMonNodes.begin(); pwrMonNode < _pwrMonNodes.end(); pwrMonNode++) {
			cout << "PowerMon Unit Test Node: " << (*pwrMonNode)->nodeId() << endl;
		}
	}

	void PwrMonUnitTest::startThreads(void)
	{
#if 0
		std::vector<std::unique_ptr<PowerMonNode>>::iterator pwrMonNode;

		for (pwrMonNode = _pwrMonNodes.begin(); pwrMonNode < _pwrMonNodes.end(); pwrMonNode++)
			(*pwrMonNode)->resumeNodeThread();
#endif
	}

	void PwrMonUnitTest::stopThreads(void)
	{
#if 0
		std::vector<std::unique_ptr<PowerMonNode>>::iterator pwrMonNode;

		for (pwrMonNode = _pwrMonNodes.begin(); pwrMonNode < _pwrMonNodes.end(); pwrMonNode++) {
			(*pwrMonNode)->exitPowerMonNode();
		}
#endif
	}

	void PwrMonUnitTest::exitPwrMonUnitTest(void)
	{
		std::vector<std::unique_ptr<PowerMonNode>>::iterator pwrMonNode;

		for (pwrMonNode = _pwrMonNodes.begin(); pwrMonNode < _pwrMonNodes.end(); pwrMonNode++) {
			(*pwrMonNode)->exitPowerMonNode();
		}
	}

	PwrMonUnitTest::~PwrMonUnitTest(void)
	{
		_pwrMonNodes.clear();
	}

} // namespace powermon
