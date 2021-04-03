/* ============================================================== */
/*
 * powermon_pkt.h
 *
 * Copyright (c) 2017 David Hammond
 * All Rights Reserved.
 */
/* ============================================================== */
/*
 * NOTICE:  All information contained herein is and remains the
 * property of David Hammond. The intellectual and technical
 * concepts contained herein are proprietary to David Hammond
 * and may be covered by U.S.and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission
 * is obtained David Hammond.
 */
/* ============================================================== */
/*
 *  The contents of this file must contain the same structure and
 *  configuration values as the similar file in the unit test.
 */
/* ============================================================== */
#ifndef __POWERMON_PKT_H__
#define __POWERMON_PKT_H__

#include <stdint.h>

typedef enum _msgType {
	client_Data,
	srvrCmd_Off,
	srcrCmd_Auto
} MsgType_e;

typedef enum _mode {
	Off,
	Auto
} Mode_e;

typedef enum _operation {
	Idle,
	Cooling,
	Defrost
} Operation_e;

typedef struct _serialno {
	uint64_t mfgId;
	uint32_t nodeId;
} SerialNumber_t;

#define NodeVersion 1
#define SerialNoLength sizeof(SerialNumber_t)

typedef struct _node {
	SerialNumber_t serialNumber;
	uint32_t nodeIp;
	Mode_e mode;	/* Auto = 1(default)/Off = 0 */
	Operation_e operation;	/* Idle/Cooling/Defrost */
} Node;

typedef struct _nodeData
{
	uint8_t temp;
	uint8_t amps;
} NodeData;

typedef struct _packet {
	unsigned int version;
	MsgType_e msgType;
	Node node;
	NodeData data;
} Packet;

#endif /* __POWERMON_PKT_H__ */

