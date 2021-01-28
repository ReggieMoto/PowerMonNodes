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
#include <mutex>

namespace powermon {

	void pwrmonSvcClientThreadProc(void);

	class PwrmonSvcClient
	{
		private:

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

			std::mutex *svcClientMutex;

			PwrmonSvcClient(void);

		public:

			~PwrmonSvcClient(void) { if (svcClientMutex != NULL) free(svcClientMutex); }

			static PwrmonSvcClient& getPwrmonSvcClient(void);

			bool pwrmonSvcClientIsActive(void);
			void releasePwrmonSvcClient(void);
			inline AvahiIPv4Address getPwrmonSvcAddr(void) { return address; }
			inline uint16_t getPwrmonSvcPort(void) { return port; }

			static void clientCallback(
					AvahiClient *client,
					AvahiClientState state,
					void * userdata);

			static void resolveCallback(
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

			static void browseCallback(
				    AvahiServiceBrowser *b,
				    AvahiIfIndex interface,
				    AvahiProtocol protocol,
				    AvahiBrowserEvent event,
				    const char *name,
				    const char *type,
				    const char *domain,
				    AvahiLookupResultFlags flags,
				    void* userdata);

			friend void pwrmonSvcClientThreadProc(void);
	};
}

