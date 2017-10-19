// ==============================================================
//
// class PwrMonUTMain
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
#include <stdexcept>      // std::invalid_argument
#include "PwrMonUnitTest.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "ConsoleOut.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "windows.h"
#include "windef.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
using namespace pwrmon;

#define MAX_TEST_NODES 10
string inputErrStr = "Provide a single node count value greater than 0 and less than or equal to 10.";
string keypressPrompt = "Press 'x' and return to continue.";

void parseArgs(char *argString);

int main(int argc, char *argv[])
{
	if (argc == 2)
		parseArgs(argv[1]);
	else
		cout << inputErrStr << endl;

    return 0;
}

void parseArgs(char *argString)
{
	try
	{
		int nodeCount = stoi(argString, NULL, 10);

		if (nodeCount > 0 && nodeCount <= MAX_TEST_NODES)
		{
			// Args check out. Create the console.
			ConsoleOut& console = ConsoleOut::getConsoleOut();
			// Create the WiFi queue
			WifiQueue& wifiQueue = WifiQueue::getWifiQueue();

			// Create the requested number of test nodes.
			static PwrMonUnitTest unitTest = PwrMonUnitTest::getPwrMonUnitTest((unsigned int)nodeCount);
			unitTest.reportNodes();
			unitTest.startThreads();

			// Wait here on a keypress before continuing to exit
			cout << keypressPrompt << endl;

			char keypress;
			do {
				cin >> keypress;
			} while (keypress != 'x');

			cout << "Shutting down threads." << endl;

			unitTest.stopThreads();
			wifiQueue.releaseWifiQueue();

			for (int i = 6; i > 0; --i)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				cout << i << "...";
			}

			cout << endl;
		}
		else
			cout << inputErrStr << endl;
	} catch (const std::invalid_argument&) { cout << "Invalid string parameter: " << inputErrStr << endl; }
}
