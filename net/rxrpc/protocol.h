/* SPDX-License-Identifier: GPL-2.0-or-later */
/* packet.h: Rx packet layout and definitions
 *
 * Copyright (C) 2002, 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#ifndef _LINUX_RXRPC_PACKET_H
#define _LINUX_RXRPC_PACKET_H

typedef u32	rxrpc_seq_t;	/* Rx message sequence number */
typedef u32	rxrpc_serial_t;	/* Rx message serial number */
typedef __be32	rxrpc_seq_net_t; /* on-the-wire Rx message sequence number */
typedef __be32	rxrpc_serial_net_t; /* on-the-wire Rx message serial number */

/*****************************************************************************/
/*
 * on-the-wire Rx packet header
 * - all multibyte fields should be in network byte order
 */
struct rxrpc_wire_header {
	__be32		epoch;		/* client boot timestamp */
#define RXRPC_RANDOM_EPOCH	0x80000000	/* Random if set, date-based if not */

	__be32		cid;		/* connection and channel ID */
#define RXRPC_MAXCALLS		4			/* max active calls per conn */
#define RXRPC_CHANNELMASK	(RXRPC_MAXCALLS-1)	/* mask for channel ID */
#define RXRPC_CIDMASK		(~RXRPC_CHANNELMASK)	/* mask for connection ID */
#define RXRPC_CIDSHIFT		ilog2(RXRPC_MAXCALLS)	/* shift for connection ID */
#define RXRPC_CID_INC		(1 << RXRPC_CIDSHIFT)	/* connection ID increment */

	__be32		callNumber;	/* call ID (0 for connection-level packets) */
	__be32		seq;		/* sequence number of pkt in call stream */
	__be32		serial;		/* serial number of pkt sent to network */

	uint8_t		type;		/* packet type */
#define RXRPC_PACKET_TYPE_DATA		1	/* data */
#define RXRPC_PACKET_TYPE_ACK		2	/* ACK */
#define RXRPC_PACKET_TYPE_BUSY		3	/* call reject */
#define RXRPC_PACKET_TYPE_ABORT		4	/* call/connection abort */
#define RXRPC_PACKET_TYPE_ACKALL	5	/* ACK all outstanding packets on call */
#define RXRPC_PACKET_TYPE_CHALLENGE	6	/* connection security challenge (SRVR->CLNT) */
#define RXRPC_PACKET_TYPE_RESPONSE	7	/* connection secutity response (CLNT->SRVR) */
#define RXRPC_PACKET_TYPE_DEBUG		8	/* debug info request */
#define RXRPC_PACKET_TYPE_PARAMS	9	/* Parameter negotiation (unspec'd, ignore) */
#define RXRPC_PACKET_TYPE_10		10	/* Ignored */
#define RXRPC_PACKET_TYPE_11		11	/* Ignored */
#define RXRPC_PACKET_TYPE_VERSION	13	/* version string request */

	uint8_t		flags;		/* packet flags */
#define RXRPC_CLIENT_INITIATED	0x01		/* signifies a packet generated by a client */
#define RXRPC_REQUEST_ACK	0x02		/* request an unconditional ACK of this packet */
#define RXRPC_LAST_PACKET	0x04		/* the last packet from this side for this call */
#define RXRPC_MORE_PACKETS	0x08		/* more packets to come */
#define RXRPC_JUMBO_PACKET	0x20		/* [DATA] this is a jumbo packet */
#define RXRPC_SLOW_START_OK	0x20		/* [ACK] slow start supported */

	uint8_t		userStatus;	/* app-layer defined status */
#define RXRPC_USERSTATUS_SERVICE_UPGRADE 0x01	/* AuriStor service upgrade request */

	uint8_t		securityIndex;	/* security protocol ID */
	union {
		__be16	_rsvd;		/* reserved */
		__be16	cksum;		/* kerberos security checksum */
	};
	__be16		serviceId;	/* service ID */

} __packed;

/*****************************************************************************/
/*
 * jumbo packet secondary header
 * - can be mapped to read header by:
 *   - new_serial = serial + 1
 *   - new_seq = seq + 1
 *   - new_flags = j_flags
 *   - new__rsvd = j__rsvd
 *   - duplicating all other fields
 */
struct rxrpc_jumbo_header {
	uint8_t		flags;		/* packet flags (as per rxrpc_header) */
	uint8_t		pad;
	union {
		__be16	_rsvd;		/* reserved */
		__be16	cksum;		/* kerberos security checksum */
	};
};

#define RXRPC_JUMBO_DATALEN	1412	/* non-terminal jumbo packet data length */
#define RXRPC_JUMBO_SUBPKTLEN	(RXRPC_JUMBO_DATALEN + sizeof(struct rxrpc_jumbo_header))

/*
 * The maximum number of subpackets that can possibly fit in a UDP packet is:
 *
 *	((max_IP - IP_hdr - UDP_hdr) / RXRPC_JUMBO_SUBPKTLEN) + 1
 *	= ((65535 - 28 - 28) / 1416) + 1
 *	= 46 non-terminal packets and 1 terminal packet.
 */
#define RXRPC_MAX_NR_JUMBO	47

/*****************************************************************************/
/*
 * on-the-wire Rx ACK packet data payload
 * - all multibyte fields should be in network byte order
 */
struct rxrpc_ackpacket {
	__be16		bufferSpace;	/* number of packet buffers available */
	__be16		maxSkew;	/* diff between serno being ACK'd and highest serial no
					 * received */
	__be32		firstPacket;	/* sequence no of first ACK'd packet in attached list */
	__be32		previousPacket;	/* sequence no of previous packet received */
	__be32		serial;		/* serial no of packet that prompted this ACK */

	uint8_t		reason;		/* reason for ACK */
#define RXRPC_ACK_REQUESTED		1	/* ACK was requested on packet */
#define RXRPC_ACK_DUPLICATE		2	/* duplicate packet received */
#define RXRPC_ACK_OUT_OF_SEQUENCE	3	/* out of sequence packet received */
#define RXRPC_ACK_EXCEEDS_WINDOW	4	/* packet received beyond end of ACK window */
#define RXRPC_ACK_NOSPACE		5	/* packet discarded due to lack of buffer space */
#define RXRPC_ACK_PING			6	/* keep alive ACK */
#define RXRPC_ACK_PING_RESPONSE		7	/* response to RXRPC_ACK_PING */
#define RXRPC_ACK_DELAY			8	/* nothing happened since received packet */
#define RXRPC_ACK_IDLE			9	/* ACK due to fully received ACK window */
#define RXRPC_ACK__INVALID		10	/* Representation of invalid ACK reason */

	uint8_t		nAcks;		/* number of ACKs */
#define RXRPC_MAXACKS	255

	uint8_t		acks[0];	/* list of ACK/NAKs */
#define RXRPC_ACK_TYPE_NACK		0
#define RXRPC_ACK_TYPE_ACK		1

} __packed;

/* Some ACKs refer to specific packets and some are general and can be updated. */
#define RXRPC_ACK_UPDATEABLE ((1 << RXRPC_ACK_REQUESTED)	|	\
			      (1 << RXRPC_ACK_PING_RESPONSE)	|	\
			      (1 << RXRPC_ACK_DELAY)		|	\
			      (1 << RXRPC_ACK_IDLE))


/*
 * ACK packets can have a further piece of information tagged on the end
 */
struct rxrpc_ackinfo {
	__be32		rxMTU;		/* maximum Rx MTU size (bytes) [AFS 3.3] */
	__be32		maxMTU;		/* maximum interface MTU size (bytes) [AFS 3.3] */
	__be32		rwind;		/* Rx window size (packets) [AFS 3.4] */
	__be32		jumbo_max;	/* max packets to stick into a jumbo packet [AFS 3.5] */
};

/*****************************************************************************/
/*
 * Kerberos security type-2 challenge packet
 */
struct rxkad_challenge {
	__be32		version;	/* version of this challenge type */
	__be32		nonce;		/* encrypted random number */
	__be32		min_level;	/* minimum security level */
	__be32		__padding;	/* padding to 8-byte boundary */
} __packed;

/*****************************************************************************/
/*
 * Kerberos security type-2 response packet
 */
struct rxkad_response {
	__be32		version;	/* version of this response type */
	__be32		__pad;

	/* encrypted bit of the response */
	struct {
		__be32		epoch;		/* current epoch */
		__be32		cid;		/* parent connection ID */
		__be32		checksum;	/* checksum */
		__be32		securityIndex;	/* security type */
		__be32		call_id[4];	/* encrypted call IDs */
		__be32		inc_nonce;	/* challenge nonce + 1 */
		__be32		level;		/* desired level */
	} encrypted;

	__be32		kvno;		/* Kerberos key version number */
	__be32		ticket_len;	/* Kerberos ticket length  */
} __packed;

#endif /* _LINUX_RXRPC_PACKET_H */
