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

#include "WifiQueue.h"

#include <chrono>         // std::chrono::seconds
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <thread>         // std::this_thread::sleep_for

using namespace std;

namespace powermon {

	void WifiQueue::_threadProcess(void)
	{
		std::string msg("Starting the Powermon WiFi queue thread.");
		_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));

	    while (true)
	    {
	        std::shared_ptr<ThreadMsg> msg;

			// Wait for a message to be added to the queue
			std::unique_lock<std::mutex> lk(_wifiQueueMutex);
			while (_wifiPacketQueue.empty())
				_wifiQueueCondVar.wait(lk);

			if (_wifiPacketQueue.empty())
				continue;

			msg = _wifiPacketQueue.front();
			_wifiPacketQueue.pop();

	        switch (msg->getMsgId())
	        {
				case ThreadMsg::MsgId_MsgNodeData:
				{
					std::string txtMsg("Sending PowerMon data packet");
		 			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, txtMsg));
					_socket->sendPacket(msg->getMsg());
					break;
				}

				case ThreadMsg::MsgId_MsgExitThread:
				{
					std::string msg("Leaving the Powermon WiFi queue process");
		 			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
					return;
				}

				default:
				{
					std::ostringstream oss;
					oss << "Received an unhandled message: " << msg->getMsgId();
					std::string msg = oss.str();
	    			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
					break;
				}
			}
	    }
	}


	WifiQueue::WifiQueue(ConsoleOut& console) :
		_console(console)
	{
		_socket = make_unique<WifiSocket>(console);
		_wifiQueueThread = std::thread(&WifiQueue::_threadProcess, this);
	}

	void WifiQueue::queueOutput(const std::shared_ptr<ThreadMsg> message)
	{
	    // Add user data msg to queue and notify worker thread
	    std::unique_lock<std::mutex> lk(_wifiQueueMutex);
	    _wifiPacketQueue.push(message);
	    _wifiQueueCondVar.notify_one();
	}

	void WifiQueue::exitWifiQueue(void)
	{
	    // Create a new ThreadMsg
		std::string msg("Exit PowerMon WiFi queue thread");
		queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgExitThread, msg));
	}

	WifiQueue::~WifiQueue()
	{
		_wifiQueueThread.join();
    	cout << "Joined the Powermon WiFi queue thread." << endl;
	}
} // namespace powermon
