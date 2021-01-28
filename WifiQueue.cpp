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

	bool wifiQueueThreadIsActive;


	void wifiQueueThreadProc(void)
	{
		// Initialize the WiFi queue
		WifiQueue wifiQueue = WifiQueue::getWifiQueue();

		// Indicate the thread is active
		wifiQueue.wifiQueueMutex->lock();
		wifiQueueThreadIsActive = true;
		wifiQueue.wifiQueueMutex->unlock();

		do {
			ostringstream string1;
			bool consoleNotification = true;

			cout << "Is packet queue empty: " << wifiQueue.isPacketQueueEmpty() << endl;

			while (wifiQueue.isPacketQueueEmpty() == false)
			{
				cout << "WiFi queue is not empty" << endl;


				wifiQueue.wifiQueueMutex->lock();
				char *packet = wifiQueue.packetQueue.front();
				cout << "Send to socket" << endl;
				wifiQueue.socket.sendPacket(packet);
				wifiQueue.packetQueue.pop();
				wifiQueue.wifiQueueMutex->unlock();

				if (wifiQueue.socket.isWifiSocketActive())
				{
					if (consoleNotification)
					{
						consoleNotification = false;
						ostringstream ostr;
						ostr << "Sending PowerMon data packet(s)." << endl;
						string textStr = ostr.str();
						wifiQueue.console.queueOutput(textStr);
						string1.str("");
					}
				}

				free(packet);
			}

			this_thread::sleep_for(chrono::seconds(1));

		} while (wifiQueue.isWifiQueueActive() == true);

		cout << "Leaving the WiFi Queue ThreadProc" << endl;
	}

	WifiQueue& WifiQueue::getWifiQueue(void)
	{
		static WifiQueue wifiQueue;

		return (wifiQueue);
	}

	WifiQueue::WifiQueue() :
		console(ConsoleOut::getConsoleOut()),
		socket(WifiSocket::getWifiSocket())
	{
		wifiQueueThreadIsActive = false;
		wifiQueueMutex = new std::mutex;
	}

	void WifiQueue::queueOutput(const string& packet)
	{
		char *pkt = new char[packet.length() + 1];
		strncpy(pkt, packet.c_str(), packet.length() + 1);

		wifiQueueMutex->lock();
		cout << "packetQueue.push" << endl;
		packetQueue.push(pkt);
		wifiQueueMutex->unlock();
	}

	bool WifiQueue::isPacketQueueEmpty(void)
	{
		wifiQueueMutex->lock();
		bool isEmpty = packetQueue.empty();
		wifiQueueMutex->unlock();
		return (isEmpty);
	}
	bool WifiQueue::isWifiQueueActive(void)
	{
		wifiQueueMutex->lock();
		bool isActive = wifiQueueThreadIsActive;
		wifiQueueMutex->unlock();
		return (isActive);
	}

	void WifiQueue::releaseWifiThread(void)
	{
		wifiQueueMutex->lock();
		wifiQueueThreadIsActive = false;
		wifiQueueMutex->unlock();
	}

	WifiQueue::~WifiQueue()
	{
		while (!packetQueue.empty())
		{
			char *packet = packetQueue.front();
			packetQueue.pop();

			free(packet);
		}

		free(wifiQueueMutex);
	}
} // namespace powermon
