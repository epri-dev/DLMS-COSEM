#pragma once

#include "appdefs.h"

#define DL_UNIT_DATA    0x00   // connectionless data transmission w/o ack.
#define DL_DATA_ACK     0x01   // connectionless data transmission with ack.
#define DL_DATA         0x02   // connection-oriented data transmission.

#define DL_IDLE              0
#define DL_CONNECTED         1
#define DL_RESET             2

//LLC type 3 commands
#define ACn 0xE6

#define is_ACn(cmd) ((cmd & ACn)==ACn)
#define    sqc(cmd) (cmd & 0x01)

#define Invalid_command 0xFF

// Status
#define _OK 0x00
#define _UN 0x09
#define _RS 0x08
#define _UE 0x0A
#define _PE 0x06

/*----------------------------------------------------------------------*/
//Selected HDLC subset + LLC type 1&2  commands
#define _SABM 0xF4   // with P=1
#define _UA   0xCE   // with F=1
#define _I_P  0x08   // information frame with P=1,NS=NR=0
#define _I    0x00   // information frame with P=0,NS=NR=0
#define __UI   0xC8   // Unumbered information with P=1
#define _RR   0x88   // RR command with F=1
#if 0
//Error Code
#define HDLC_FAILURE 20
#define ERROR_TRANS_DATA 21
#endif
/*----------------------------------------------------------------------*/

/*------------- Logical symbols for MAC sublayer services --------------*/
#define MA_UNIT_DATA    0x00  // connectionless data transmission w/o ack.
#define MA_SYNC_ACQ     0x01  // Synchronisation Acquisition
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
#define MAX_ADDR_SIZE    6   				// Max size in bytes for LSAP addresses
#define MAX_FRAME_SIZE   COMMDATA_MAXSIZE  	// Maximum Frame Size
#define MAC_PACKET       12  				// MAC Frame Size

