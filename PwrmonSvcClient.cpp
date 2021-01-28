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

#include "PwrmonSvcClient.h"

#include <cstring>
#include <iostream>

using namespace std;

namespace powermon {

	bool pwrmonSvcClientThreadIsActive;

	void pwrmonSvcClientThreadProc(void)
	{
		// Get a reference object to the powermon svc client
		PwrmonSvcClient& pwrmonSvcClient = PwrmonSvcClient::getPwrmonSvcClient();
		int error;

		// Allocate main loop object
		pwrmonSvcClient.simple_poll = avahi_simple_poll_new();

		if (pwrmonSvcClient.simple_poll == NULL) {
			cout << "Failed to create the Avahi simple poll.";
			goto failure_exit;
		}

		// Allocate a new client
		pwrmonSvcClient.client = avahi_client_new(
				avahi_simple_poll_get(pwrmonSvcClient.simple_poll),
				AVAHI_CLIENT_IGNORE_USER_CONFIG,
				PwrmonSvcClient::clientCallback,
				NULL,
				&error);

		if (pwrmonSvcClient.client == NULL) {
			cout << "Failed to create the Avahi client.";
            avahi_simple_poll_quit(pwrmonSvcClient.simple_poll);
			goto failure_exit;
		}

		// Create the service browser
		pwrmonSvcClient.serviceBrowser = avahi_service_browser_new(
				pwrmonSvcClient.client,
				AVAHI_IF_UNSPEC,
				AVAHI_PROTO_UNSPEC,
				pwrmonSvcClient.svcType,
				NULL,
				AVAHI_LOOKUP_USE_MULTICAST,
				PwrmonSvcClient::browseCallback,
				pwrmonSvcClient.client);

		if (pwrmonSvcClient.serviceBrowser == NULL) {
			cout << "Failed to create service browser: " << avahi_strerror(avahi_client_errno(pwrmonSvcClient.client)) << endl;
            avahi_simple_poll_quit(pwrmonSvcClient.simple_poll);
			goto failure_exit;
		}

		pwrmonSvcClient.svcClientMutex->lock();
    	pwrmonSvcClientThreadIsActive = true;
    	pwrmonSvcClient.svcClientMutex->unlock();

		avahi_simple_poll_loop(pwrmonSvcClient.simple_poll);

		cout << "Leaving the Avahi Client ThreadProc" << endl;

	failure_exit:

	    /* Cleanup things */
	    if (pwrmonSvcClient.serviceBrowser)
	        avahi_service_browser_free(pwrmonSvcClient.serviceBrowser);

	    if (pwrmonSvcClient.client)
	        avahi_client_free(pwrmonSvcClient.client);

	    if (pwrmonSvcClient.simple_poll)
	        avahi_simple_poll_free(pwrmonSvcClient.simple_poll);
	}

	void PwrmonSvcClient::clientCallback(
			AvahiClient *client,
			AvahiClientState state,
			void * userdata)
	{
		/* Called whenever the client or server state changes */
		if (state == AVAHI_CLIENT_FAILURE) {
			cout << "Avahi client failure. Quitting the poll" << endl;
			getPwrmonSvcClient().releasePwrmonSvcClient();
		}
	}

	void PwrmonSvcClient::resolveCallback(
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
	    /* Called whenever a service has been resolved successfully or timed out */
	    switch (event) {
	        case AVAHI_RESOLVER_FAILURE:
	        	cout << "Avahi Resolver Failed to resolve service: " << avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r))) << endl;
	        	//getPwrmonSvcClient().releasePwrmonSvcClient();
	            break;

	        case AVAHI_RESOLVER_FOUND: {
	        	cout << "Resolver found service " << name << " of type " << type << " in domain " << domain << endl;
	        	PwrmonSvcClient::getPwrmonSvcClient().interface = interface;
	        	PwrmonSvcClient::getPwrmonSvcClient().protocol = protocol;
	        	PwrmonSvcClient::getPwrmonSvcClient().domain = (char *)domain;
	        	PwrmonSvcClient::getPwrmonSvcClient().host_name = (char *)host_name;
	        	PwrmonSvcClient::getPwrmonSvcClient().address = address->data.ipv4;
	        	PwrmonSvcClient::getPwrmonSvcClient().port = port;

	        	cout << "Service host: " << host_name << endl;
	        	uint8_t *ipaddr = (uint8_t *)&address;

	        	printf("%p: 0x%02x\n", ipaddr, *(ipaddr));
	        	printf("%p: 0x%02x\n", ipaddr+1, *(ipaddr+1));
	        	printf("%p: 0x%02x\n", ipaddr+2, *(ipaddr+2));
	        	printf("%p: 0x%02x\n", ipaddr+3, *(ipaddr+3));

	        	cout << "Service host IP address: " << hex <<
	        			*(ipaddr) << "." <<
						*(ipaddr+1) << "." <<
						*(ipaddr+2) << "." <<
						*(ipaddr+3) << endl;

	        	cout << dec << "Service port: " << port << endl;
	        }
	    }

	    avahi_service_resolver_free(r);
	}

    /* Called whenever a new service becomes available on the LAN or is removed from the LAN */
	void PwrmonSvcClient::browseCallback(
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
	    AvahiClient *client = (AvahiClient *)userdata;

	    switch (event) {
	        case AVAHI_BROWSER_FAILURE:
	        {
	        	cout << "Browser Failure: " <<
					avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b))) << endl;
	        	getPwrmonSvcClient().releasePwrmonSvcClient();
	            return;
	        }
	        case AVAHI_BROWSER_NEW:
	        {
	        	cout << "New Browser Service: " << name << " of type " << type <<
				" in domain " << domain << endl;
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
						PwrmonSvcClient::resolveCallback,
	            	    userdata);

	        	if (resolver == NULL) {
	        		cout << "Failed to resolve service " << name <<
	        				": " << avahi_strerror(avahi_client_errno(client)) << endl;
	        	}
	            break;
	        }
	        case AVAHI_BROWSER_REMOVE:
	        {
	        	cout << "Remove Browser Service: " << name << " of type " << type
				<< " in domain " << domain << endl;
	            break;
	        }
	        case AVAHI_BROWSER_ALL_FOR_NOW:
	        case AVAHI_BROWSER_CACHE_EXHAUSTED:
	        {
	            break;
	        }
	    }
	}

	PwrmonSvcClient& PwrmonSvcClient::getPwrmonSvcClient(void)
	{
		static PwrmonSvcClient pwrmonSvcClient;

		return (pwrmonSvcClient);
	}

	PwrmonSvcClient::PwrmonSvcClient(void) :
		simple_poll(NULL),
		client(NULL),
		serviceBrowser(NULL),
	    interface(0),
	    protocol(AVAHI_PROTO_UNSPEC),
	    domain(NULL),
	    host_name(NULL),
	    port(0)
	{
		pwrmonSvcClientThreadIsActive = false;
		svcClientMutex = new std::mutex;
	}

	bool PwrmonSvcClient::pwrmonSvcClientIsActive(void)
	{
		svcClientMutex->lock();
		bool threadIsActive = pwrmonSvcClientThreadIsActive;
		svcClientMutex->unlock();
		return (threadIsActive);
	}

	void PwrmonSvcClient::releasePwrmonSvcClient(void)
	{
		svcClientMutex->lock();
		pwrmonSvcClientThreadIsActive = false;
        avahi_simple_poll_quit(simple_poll);
		svcClientMutex->unlock();
	}

}
