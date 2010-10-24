#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef enum ProtocolBytes {
	pbNull=			0x00,
	pbRequestConnect=	0x02,
	pbConfirmConnect=	0x03,
	pbDisconnect=		0x40
} ProtocolBytes;

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
