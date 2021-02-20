// ==============================================================
//
// class PwrMonUTMain
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

#include "ConsoleOut.h"
#include "PwrmonSvcClient.h"
#include "PwrMonUnitTest.h"
#include "WifiQueue.h"

#include <iostream>

using namespace std;
using namespace powermon;

#define MAX_TEST_NODES 10
string inputErrStr = "Provide a single node count value greater than 0 and less than or equal to 10.";

void parseArgs(char *argString);

int main(int argc, char *argv[])
{
	if (argc == 2)
		parseArgs(argv[1]);
	else
		cout << inputErrStr << endl;

    return (0);
}

void parseArgs(char *argString)
{
	try
	{
		int nodeCount = stoi(argString, NULL, 10);

		if (nodeCount > 0 && nodeCount <= MAX_TEST_NODES)
		{
			// Create the console
			ConsoleOut console;

			// Create the Powermon Avahi service threads
			PwrmonSvcClient pwrmonSvcClient(console);

			// Create the WiFi queue
			WifiQueue wifiQueue(console);

			// Create the requested number of test nodes.
			PwrMonUnitTest unitTest(nodeCount, console, wifiQueue);

			// Wait here on a keypress before continuing to exit
			std::string msg("Press 'x' and return to continue.");
 			console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));

			char keypress;
			do {
				cin >> keypress;
			} while (keypress != 'x');

			msg.clear();
			msg = "Shutting down threads.";
 			console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));

			unitTest.exitPwrMonUnitTest();

			wifiQueue.exitWifiQueue();
			pwrmonSvcClient.exitPwrmonSvcClient();
			console.exitConsoleOut();
		}
		else
			cout << inputErrStr << endl;
	} catch (const std::invalid_argument&) { cout << "Invalid string parameter: " << inputErrStr << endl; }
}
