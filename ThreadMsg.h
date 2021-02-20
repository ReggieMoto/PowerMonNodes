// ==============================================================
//
// class ThreadMsg
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

#include <stdint.h>
#include <array>
#include <string>

namespace powermon {

	class ThreadMsg
	{
	public:

		static const uint8_t MaxMsgLen = 128;

		typedef enum msgId_e {
			MsgId_First = 0,
			MsgId_NoMsg = MsgId_First,
			MsgId_MsgNodeData,
			MsgId_MsgConsoleStr,
			MsgId_MsgTimer,
			MsgId_MsgExitThread,
			MsgId_Last = MsgId_MsgExitThread
		} MsgId;

	private:

		MsgId _id;
		char _msg[MaxMsgLen];

	public:
		ThreadMsg(void);
		ThreadMsg(MsgId id, std::string& data);

		inline MsgId getMsgId(void) { return (_id); }
		inline char* getMsg(void) { return (_msg); }

	};
}
