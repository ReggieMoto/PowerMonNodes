// ==============================================================
//
// class PwrmonSvcClient
//
// Copyright (c) 2021 David Hammond
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

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


namespace powermon {

	class ConsoleOut;
	class WifiQueue;
	class ThreadMsg;

	AvahiIPv4Address getPwrmonSvcAddr(void);
	uint16_t getPwrmonSvcPort(void);

	// =============================================
	class AvahiSvcClientConsoleAccess {
	// =============================================

		ConsoleOut& _console;

	public:
		AvahiSvcClientConsoleAccess(ConsoleOut& console);
		~AvahiSvcClientConsoleAccess(void) {}

		void sendConsoleOut(const char *msg);
	};

	// =============================================
	class AvahiSvcClientWifiAccess {
	// =============================================

		WifiQueue& _wifiQueue;

	public:
		AvahiSvcClientWifiAccess(WifiQueue &wifiQueue);
		~AvahiSvcClientWifiAccess(void) {}

		void sendAvahiSvcUp(void);
		void sendAvahiSvcDown(void);
	};

	typedef struct AvahiSvcClientConfig_s
	{
		const char *svcName = "PowerMon";
		const char *svcType = "_powermon._udp";

		AvahiSimplePoll *simple_poll;
		AvahiClient *client;
		AvahiServiceBrowser *serviceBrowser;

		AvahiIfIndex interface;
		AvahiProtocol protocol;
		char *domain;
		char *host_name;
		AvahiIPv4Address address;
		uint16_t port;

		/*
		 * Avahi function access to the console and the WiFi Queue
		 */
		AvahiSvcClientConsoleAccess *consoleAccess;
		AvahiSvcClientWifiAccess *wifiAccess;

	} avahiSvcClientConfig_t;


	// =============================================
	//
	// AvahiSvcClient
	// =============================================

	class PwrmonSvcClient
	{
		std::vector<std::thread> _threads;
		std::queue<std::shared_ptr<ThreadMsg>> _svcClientQueue;
		std::mutex _svcClientMutex;
		std::condition_variable _svcClientCondVar;

		ConsoleOut& _console;
		WifiQueue& _wifiQueue;

		PwrmonSvcClient(const PwrmonSvcClient&) = delete;
		PwrmonSvcClient& operator=(const PwrmonSvcClient&) = delete;

		// Entry point for the PowerMon Service Client thread
		void _threadProcess(void);

	public:

		PwrmonSvcClient(ConsoleOut& console, WifiQueue& wifiQueue);
		~PwrmonSvcClient(void);

		void exitPwrmonSvcClient(void);
		void queueOutput(const std::shared_ptr<ThreadMsg> message);
	};
}

