// ==============================================================
//
// class PowerMonNode
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
#pragma once

#include <cstdint>
#include <mutex>
#include <ostream>
#include <string>

namespace powermon {

	class PowerMonNode
	{
		public:

			enum Mode_e {
				Off,
				Auto
			};

			enum Operation_e {
				Normal,
				Defrost
			};

			static uint32_t const NodeIdLength = 32;
			static uint32_t const nodeVersion = 1;

		private:

			typedef struct _packet {
				uint32_t version;
				char nodeId[NodeIdLength];
				Mode_e mode;	// Auto = 1(default)/Off = 0
				Operation_e operation;	// Normal = 1 (default)/Defrost = 0
				uint32_t temp;
				uint32_t amps;
			} Packet;

			std::string nodeIdentifier;
			Packet packet;

		public:
			PowerMonNode(uint32_t nodeId);
			PowerMonNode(PowerMonNode &node);

			~PowerMonNode();

			static std::mutex& getPowerMonNodesMutex(void);

			std::string nodeId(void) { return (nodeIdentifier); }
			friend void powerMonNodeThreadProc(PowerMonNode& node);

			inline void resumeNodeThread(void) { }

			inline Mode_e getMode(void) { return (packet.mode); }
			inline Operation_e getOperation(void) { return (packet.operation); }
			inline uint32_t getTemp(void) { return (packet.temp); }
			inline uint32_t getAmps(void) { return (packet.amps); }

			friend std::ostream& operator<<(std::ostream& os, const PowerMonNode& node);
	};

	void powerMonNodeThreadProc(PowerMonNode& node);
	void releaseNodeThreads(void);
	bool nodeThreadsAreActive(void);


} // namespace powermon
