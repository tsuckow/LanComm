#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef enum ProtocolBytes {
	pbNull=			0x00,
	pbDisconnect=		0x04,
	pbRequestConnect=	0x05,
	pbConfirmConnect=	0x06,
	pbStopByteArray=	0x08,
	pbRequestByteArray=	0x09,
	pbConfirmByteArray=	0x0A
} ProtocolBytes;

typedef enum DeviceMode {
	dmIdle, dmPlaying, dmRecording
} DeviceMode;

extern DeviceMode deviceMode;

typedef enum ConnectionStatus {
	csDisconnected,csConnected
} ConnectionStatus;
typedef enum ConnectionRole {
	crNull,crMaster,crSlave
} ConnectionRole;

typedef struct Connection {
	ConnectionStatus status;
	ConnectionRole role;
} Connection;

#endif
