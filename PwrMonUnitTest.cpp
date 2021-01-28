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

using namespace std;

namespace powermon
{

	PwrMonUnitTest& PwrMonUnitTest::getPwrMonUnitTest(uint32_t nodeCount)
	{
		static PwrMonUnitTest unitTest(nodeCount);

		return (unitTest);
	}

	PwrMonUnitTest::PwrMonUnitTest(uint32_t nodeCount) :
		console(ConsoleOut::getConsoleOut())
	{
		for (uint32_t i = 0; i < nodeCount; i++) {
			PowerMonNode *node = new PowerMonNode(i+1u);
			pwrMonNodes.insert(pwrMonNodes.end(), node);
			thread pwrMonNodeThread(powerMonNodeThreadProc, std::ref(*node));
			pwrMonNodeThreads.push_back(std::move(pwrMonNodeThread));
		}
	}

	void PwrMonUnitTest::reportNodes(void)
	{
		vector<PowerMonNode *>::iterator pwrMonNode;

		for (pwrMonNode = pwrMonNodes.begin(); pwrMonNode < pwrMonNodes.end(); pwrMonNode++)
			cout << "PowerMon Unit Test Node: " << (*pwrMonNode)->nodeId() << endl;
	}

	void PwrMonUnitTest::startThreads(void)
	{
		vector<PowerMonNode *>::iterator pwrMonNode;

		for (pwrMonNode = pwrMonNodes.begin(); pwrMonNode < pwrMonNodes.end(); pwrMonNode++)
			(*pwrMonNode)->resumeNodeThread();
	}

	void PwrMonUnitTest::stopThreads(void)
	{
		releaseNodeThreads();

		vector<std::thread>::iterator pwrMonNodeThread;

		for (pwrMonNodeThread = pwrMonNodeThreads.begin(); pwrMonNodeThread < pwrMonNodeThreads.end(); pwrMonNodeThread++) {
			pwrMonNodeThread->join();
		}
	}

	PwrMonUnitTest::~PwrMonUnitTest()
	{
		vector<PowerMonNode *>::iterator pwrMonNode;

		for (pwrMonNode = pwrMonNodes.begin(); pwrMonNode < pwrMonNodes.end(); pwrMonNode++)
			free(*pwrMonNode);
	}

	int PwrMonUnitTest::getNetworkAdaptorInfo(void)
	{
		int retVal = 0;

		return (retVal);
	}

} // namespace powermon
