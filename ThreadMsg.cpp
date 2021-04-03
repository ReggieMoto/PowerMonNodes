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

#include <ThreadMsg.h>
#include <cstring>

using namespace std;

namespace powermon {

	ThreadMsg::ThreadMsg(void)
	{
		_id = ThreadMsg::MsgId_NoMsg;
		memset(_msg, 0, MaxMsgLen);
	}

	ThreadMsg::ThreadMsg(MsgId id, DataMsg_t& data)
	{
		_id = id;

		if (data.len > MaxMsgLen) {
			data.len = MaxMsgLen;
		}

		memcpy(_msg, data.buffer, data.len);
	}

	ThreadMsg::ThreadMsg(MsgId id, std::string& data)
	{
		_id = id;

		if (data.size() + 1 > MaxMsgLen) {
			data.resize(MaxMsgLen-1);
		}

		data.copy(_msg, data.size());
		_msg[data.size()] = '\0';
	}
}
