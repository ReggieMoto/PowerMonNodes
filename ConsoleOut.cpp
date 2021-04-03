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

#include <ConsoleOut.h>
#include <ThreadMsg.h>

#include <iostream>

using namespace std;

namespace powermon {

	void ConsoleOut::_threadProcess(void)
	{
	    while (true)
	    {
	        std::shared_ptr<ThreadMsg> msg;

	        {
	            // Wait for a message to be added to the queue
	            std::unique_lock<std::mutex> lk(_consoleOutMutex);
	            while (_screenLine.empty())
	            	_consoleOutCondVar.wait(lk);

	            if (_screenLine.empty())
	                continue;

	            msg = _screenLine.front();
	            _screenLine.pop();
	        }

	        switch (msg->getMsgId())
	        {
				case ThreadMsg::MsgId_MsgConsoleStr:
				{
					cout << msg->getMsg() << endl;
					break;
				}

				case ThreadMsg::MsgId_MsgExitThread:
				{
					do {
			            msg = _screenLine.front();
			            _screenLine.pop();
			            if (msg->getMsgId() == ThreadMsg::MsgId_MsgConsoleStr) {
			            	cout << msg->getMsg() << endl;
			            }
					} while (!_screenLine.empty());

					cout << "Leaving the Powermon console thread" << endl;
					return;
				}

				default:
				{
					cout << "Received an unhandled message: " << msg->getMsgId() << endl;
					break;
				}
			}
	    }
	}

	ConsoleOut::ConsoleOut(void) :
		_consoleOutThread(make_unique<std::thread>(&ConsoleOut::_threadProcess, this))
	{
		std::string msg("Starting the Powermon console thread");
		queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
	}

	void ConsoleOut::queueOutput(const std::shared_ptr<ThreadMsg> message)
	{
	    // Add user data msg to queue and notify worker thread
	    std::unique_lock<std::mutex> lk(_consoleOutMutex);
	    _screenLine.push(message);
	    _consoleOutCondVar.notify_one();
	}

	void ConsoleOut::exitConsoleOut(void)
	{
		std::string msg("Request to exit the Powermon console thread");
		queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgExitThread, msg));
	}

	ConsoleOut::~ConsoleOut()
	{
		_consoleOutThread->join();
    	cout << "Joined the Powermon console thread." << endl;
	}

} // namespace powermon
