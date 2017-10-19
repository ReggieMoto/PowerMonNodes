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

//#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include "PwrMonUnitTest.h"
#include <Iphlpapi.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <memory.h>

using namespace std;

namespace pwrmon
{

	PwrMonUnitTest& PwrMonUnitTest::getPwrMonUnitTest(unsigned int nodeCount)
	{
		static PwrMonUnitTest unitTest = PwrMonUnitTest(nodeCount);

		return unitTest;
	}

	PwrMonUnitTest::PwrMonUnitTest(unsigned int nodeCount) :
		console(ConsoleOut::getConsoleOut())
	{
		for (unsigned int i = 0; i < nodeCount; i++)
			pwrMonNodes.insert(pwrMonNodes.end(), new PowerMonNode(i+1));
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
		vector<PowerMonNode *>::iterator pwrMonNode;

		for (pwrMonNode = pwrMonNodes.begin(); pwrMonNode < pwrMonNodes.end(); pwrMonNode++)
			(*pwrMonNode)->exitNodeThread();
	}

	PwrMonUnitTest::~PwrMonUnitTest()
	{
	}

	int PwrMonUnitTest::getNetworkAdaptorInfo(void)
	{
		int retVal = 0;

		return retVal;
	}

}