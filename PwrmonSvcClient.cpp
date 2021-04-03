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
//
// This file creates two threads: the Avahi simple poll thread and
// the PwrmonSvcClientThread.
//
// The Avahi simple poll pends forever, so it can't be managed as
// if it were a standard thread. In order to exit the Avahi poll
// thread, the PwrmonSvcClient thread is used to quit the Avahi
// poll thread, and then join.
//
// ==============================================================


#include "PwrmonSvcClient.h"

#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

namespace powermon {

	static void avahiSvcClientCallback(
			AvahiClient *client,
			AvahiClientState state,
			void * userdata);

	static void avahiSvcResolverCallback(
			AvahiServiceResolver *r,
			AvahiIfIndex interface,
			AvahiProtocol protocol,
			AvahiResolverEvent event,
			const char *name,
			const char *type,
			const char *domain,
			const char *host_name,
			const AvahiAddress *address,
			uint16_t port,
			AvahiStringList *txt,
			AvahiLookupResultFlags flags,
			void* userdata);

	static void avahiSvcBrowserCallback(
			AvahiServiceBrowser *b,
			AvahiIfIndex interface,
			AvahiProtocol protocol,
			AvahiBrowserEvent event,
			const char *name,
			const char *type,
			const char *domain,
			AvahiLookupResultFlags flags,
			void* userdata);

	static void releaseAvahiSvcClientResources(void);
	static void initAvahiSvcClientConfig(ConsoleOut& console);

	static avahiSvcClientConfig_t avahiSvcClientConfig;

	void avahiSvcClientProcess(void)
	{
		int error;
		AvahiSvcClientConsoleAccess *console = avahiSvcClientConfig.consoleAccess;

		console->sendConsoleOut("Starting the Avahi service thread.");

		// Allocate main loop object
		avahiSvcClientConfig.simple_poll = avahi_simple_poll_new();

		if (avahiSvcClientConfig.simple_poll == NULL) {
			console->sendConsoleOut("Failed to create the Avahi simple poll.");
			goto failure_exit;
		}

		// Allocate a new client
		avahiSvcClientConfig.client = avahi_client_new(
				avahi_simple_poll_get(avahiSvcClientConfig.simple_poll),
				AVAHI_CLIENT_IGNORE_USER_CONFIG,
				avahiSvcClientCallback,
				NULL,
				&error);

		if (avahiSvcClientConfig.client == NULL) {
			console->sendConsoleOut("Failed to create the Avahi client.");
			goto failure_exit;
		}

		// Create the service browser
		avahiSvcClientConfig.serviceBrowser = avahi_service_browser_new(
				avahiSvcClientConfig.client,
				AVAHI_IF_UNSPEC,
				AVAHI_PROTO_UNSPEC,
				avahiSvcClientConfig.svcType,
				NULL,
				AVAHI_LOOKUP_USE_MULTICAST,
				avahiSvcBrowserCallback,
				avahiSvcClientConfig.client);

		if (avahiSvcClientConfig.serviceBrowser == NULL) {
			std::ostringstream oss;
			oss << "Failed to create service browser: " <<
					avahi_strerror(avahi_client_errno(avahiSvcClientConfig.client));
			console->sendConsoleOut(oss.str().c_str());

			goto failure_exit;
		}

		avahi_simple_poll_loop(avahiSvcClientConfig.simple_poll);

		console->sendConsoleOut("Leaving the Avahi service thread process");

	failure_exit:

	    /* Cleanup things */
		releaseAvahiSvcClientResources();
	}

	static void initAvahiSvcClientConfig(ConsoleOut& console)
	{
		avahiSvcClientConfig.simple_poll = NULL;
		avahiSvcClientConfig.client = NULL;
		avahiSvcClientConfig.serviceBrowser = NULL;

    	avahiSvcClientConfig.interface = 0;
		avahiSvcClientConfig.protocol = AVAHI_PROTO_UNSPEC;
    	avahiSvcClientConfig.domain = NULL;
    	avahiSvcClientConfig.host_name = NULL;
    	avahiSvcClientConfig.address.address = 0;
    	avahiSvcClientConfig.port = 0;

    	avahiSvcClientConfig.consoleAccess = new AvahiSvcClientConsoleAccess(console);
	}

	AvahiIPv4Address getPwrmonSvcAddr(void)
	{
		return (avahiSvcClientConfig.address);
	}

	uint16_t getPwrmonSvcPort(void)
	{
		return (avahiSvcClientConfig.port);
	}

	static void avahiSvcClientCallback(
			AvahiClient *client,
			AvahiClientState state,
			void * userdata)
	{
		AvahiSvcClientConsoleAccess *console = avahiSvcClientConfig.consoleAccess;

		/* Called whenever the client or server state changes */
		if (state == AVAHI_CLIENT_FAILURE) {
			console->sendConsoleOut("Avahi client failure. Quitting the poll");
			//releaseAvahiSvcClientResources();
		}
	}

	static void avahiSvcResolverCallback(
	    AvahiServiceResolver *r,
	    AvahiIfIndex interface,
	    AvahiProtocol protocol,
	    AvahiResolverEvent event,
	    const char *name,
	    const char *type,
	    const char *domain,
	    const char *host_name,
	    const AvahiAddress *address,
	    uint16_t port,
	    AvahiStringList *txt,
	    AvahiLookupResultFlags flags,
	    void* userdata)
	{
		AvahiSvcClientConsoleAccess *console = avahiSvcClientConfig.consoleAccess;

		/* Called whenever a service has been resolved successfully or timed out */
	    switch (event) {
	        case AVAHI_RESOLVER_FAILURE: {
				std::ostringstream oss;
				oss << "Avahi Resolver Failed to resolve service: " <<
						avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r)));
				console->sendConsoleOut(oss.str().c_str());
	        	//getPwrmonSvcClient().releasePwrmonSvcClient();
	            break;
	        }
	        case AVAHI_RESOLVER_FOUND: {
				std::ostringstream oss;
	        	oss << "Resolver found service " << name << " of type " << type << " in domain " << domain;
				console->sendConsoleOut(oss.str().c_str());

	        	avahiSvcClientConfig.interface = interface;
	        	avahiSvcClientConfig.protocol = protocol;
	        	avahiSvcClientConfig.domain = (char *)domain;
	        	avahiSvcClientConfig.host_name = (char *)host_name;
	        	avahiSvcClientConfig.address = address->data.ipv4;
	        	avahiSvcClientConfig.port = port;

				oss.clear();
	        	oss << "Service host: " << host_name;
				console->sendConsoleOut(oss.str().c_str());
#if 0
	        	uint8_t *ipaddr = (uint8_t *)&avahiSvcClientConfig.address.address;

	        	printf("%p: 0x%02x\n", ipaddr, *(ipaddr));
	        	printf("%p: 0x%02x\n", ipaddr+1, *(ipaddr+1));
	        	printf("%p: 0x%02x\n", ipaddr+2, *(ipaddr+2));
	        	printf("%p: 0x%02x\n", ipaddr+3, *(ipaddr+3));

	        	cout << "Service host IP address: " << hex <<
	        			*(ipaddr) << "." <<
						*(ipaddr+1) << "." <<
						*(ipaddr+2) << "." <<
						*(ipaddr+3) << endl;
#endif
				oss.clear();
	        	oss << dec << "Service port: " << port;
				console->sendConsoleOut(oss.str().c_str());
	        }
	    }

	    avahi_service_resolver_free(r);
	}

    /* Called whenever a new service becomes available on the LAN or is removed from the LAN */
	static void avahiSvcBrowserCallback(
	    AvahiServiceBrowser *b,
	    AvahiIfIndex interface,
	    AvahiProtocol protocol,
	    AvahiBrowserEvent event,
	    const char *name,
	    const char *type,
	    const char *domain,
	    AvahiLookupResultFlags flags,
	    void* userdata)
	{
		AvahiSvcClientConsoleAccess *console = avahiSvcClientConfig.consoleAccess;

	    AvahiClient *client = (AvahiClient *)userdata;

	    switch (event) {
	        case AVAHI_BROWSER_FAILURE:
	        {
				std::ostringstream oss;
	        	oss << "Browser Failure: " <<
					avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b)));
				console->sendConsoleOut(oss.str().c_str());
	        	//releaseAvahiSvcClientResources();
	            return;
	        }
	        case AVAHI_BROWSER_NEW:
	        {
				std::ostringstream oss;
	        	oss << "New Browser Service: " << name << " of type " << type <<
				" in domain " << domain;
				console->sendConsoleOut(oss.str().c_str());
	            /* We ignore the returned resolver object. In the callback
	               function we free it. If the server is terminated before
	               the callback function is called the server will free
	               the resolver for us. */
	        	AvahiServiceResolver *resolver = avahi_service_resolver_new(
	            	    client,
	            	    interface,
	            	    protocol,
	            	    name,
	            	    type,
	            	    domain,
	            	    AVAHI_PROTO_UNSPEC,
						AVAHI_LOOKUP_USE_MULTICAST,
						avahiSvcResolverCallback,
	            	    userdata);

	        	if (resolver == NULL) {
	        		oss.clear();
	        		oss << "Failed to resolve service " << name <<
	        				": " << avahi_strerror(avahi_client_errno(client));
					console->sendConsoleOut(oss.str().c_str());
	        	}
	            break;
	        }
	        case AVAHI_BROWSER_REMOVE:
	        {
				std::ostringstream oss;
	        	oss << "Remove Browser Service: " << name << " of type " << type
	        			<< " in domain " << domain;
				console->sendConsoleOut(oss.str().c_str());
	            break;
	        }
	        case AVAHI_BROWSER_ALL_FOR_NOW:
	        case AVAHI_BROWSER_CACHE_EXHAUSTED:
	        {
	            break;
	        }
	    }
	}

	static void releaseAvahiSvcClientResources(void)
	{
        if (avahiSvcClientConfig.serviceBrowser) {
	        avahi_service_browser_free(avahiSvcClientConfig.serviceBrowser);
	        avahiSvcClientConfig.serviceBrowser = NULL;
        }

	    if (avahiSvcClientConfig.client) {
	        avahi_client_free(avahiSvcClientConfig.client);
	        avahiSvcClientConfig.client = NULL;
	    }

	    if (avahiSvcClientConfig.simple_poll) {
	        avahi_simple_poll_free(avahiSvcClientConfig.simple_poll);
	        avahiSvcClientConfig.simple_poll = NULL;
	    }

		delete avahiSvcClientConfig.consoleAccess;
}

	// =============================================
	//
	// AvahiSvcClientConsoleAccess
	//
	// =============================================

	AvahiSvcClientConsoleAccess::AvahiSvcClientConsoleAccess(ConsoleOut& console) :
		_console(console)
	{
	}

	void AvahiSvcClientConsoleAccess::sendConsoleOut(const char *text)
	{
		std::string msg(text);
		_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
	}

	// =============================================
	//
	// PwrmonSvcClient
	//
	// =============================================

	void PwrmonSvcClient::_threadProcess(void)
	{
		{
			std::string msg("Starting the Powermon service thread");
			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
		}

	    while (true)
	    {
	        std::shared_ptr<ThreadMsg> msg;

	        {
	            // Wait for a message to be added to the queue
	            std::unique_lock<std::mutex> lk(_svcClientMutex);
	            while (_svcClientQueue.empty())
	            	_svcClientCondVar.wait(lk);

	            if (_svcClientQueue.empty())
	                continue;

	            msg = _svcClientQueue.front();
	            _svcClientQueue.pop();
	        }

	        switch (msg->getMsgId())
	        {
	            case ThreadMsg::MsgId_MsgExitThread:
	            {
	        		//releaseAvahiSvcClientResources();
	        	    if (avahiSvcClientConfig.simple_poll) {
	        	    	avahi_simple_poll_quit(avahiSvcClientConfig.simple_poll);
	        	    }

	    			std::string msg("Leaving the Powermon service thread process");
	    			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
	                return;
	            }

	            default:
	            {
	    			std::string msg("Received an unhandled message");
	    			_console.queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgConsoleStr, msg));
	                break;
	            }
	        }
	    }
	}

	PwrmonSvcClient::PwrmonSvcClient(ConsoleOut& console) :
			_console(console)
	{
		initAvahiSvcClientConfig(console);

		_threads.push_back(std::thread(&avahiSvcClientProcess));
		_threads.push_back(std::thread(&PwrmonSvcClient::_threadProcess, this));
	}

	void PwrmonSvcClient::queueOutput(const std::shared_ptr<ThreadMsg> message)
	{
	    // Add user data msg to queue and notify worker thread
	    std::unique_lock<std::mutex> lk(_svcClientMutex);
	    _svcClientQueue.push(message);
	    _svcClientCondVar.notify_one();
	}

	void PwrmonSvcClient::exitPwrmonSvcClient(void)
	{
	    // Create a new ThreadMsg
		std::string msg("Exit PwrmonSvcClient thread");
		queueOutput(make_shared<ThreadMsg>(ThreadMsg::MsgId_MsgExitThread, msg));
	}

	PwrmonSvcClient::~PwrmonSvcClient(void)
	{
		for (auto& th : _threads) th.join();

		/* Console's probably gone */
		cout << "Joined the Powermon service threads." << endl;
	}
}

