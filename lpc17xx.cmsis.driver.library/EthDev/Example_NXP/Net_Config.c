/*----------------------------------------------------------------------------
 *      R T L  -  T C P N E T
 *----------------------------------------------------------------------------
 *      Name:    NET_CONFIG.C
 *      Purpose: Configuration of RTL TcpNet by user.
 *      Rev.:    V3.60
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2009 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <Net_Config.h>

/*----------------------------------------------------------------------------
 *      RTL User configuration part BEGIN
 *---------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>System Definitions
// =====================
// <i> Global TCPnet System definitions
//   <s.15>Local Host Name
//   <i> This is the name under which embedded host can be
//   <i> accessed on a local area network.
//   <i> Default: "mcb2100"
#define LHOST_NAME     "mcb1700"

//   <o>Memory Pool size <1500-32000:4><#/4>
//   <i> This is the size of a memory pool in bytes. Buffers for
//   <i> TCPnet packets are allocated from this memory pool.
//   <i> Default: 8000 bytes
#define MEM_SIZE       2048

//   <o>Tick Timer interval <10=> 10 ms <20=> 20 ms <25=> 25 ms
//                          <40=> 40 ms <50=> 50 ms <100=> 100 ms <200=> 200 ms
//   <i> System Tick Timer interval for software timers
//   <i> Default: 100 ms
#define TICK_INTERVAL  100

// </h>
// <e>Ethernet Network Interface
// =============================
// <i> Enable or disable Ethernet Network Interface
#define ETH_ENABLE     1

//   <h>MAC Address
//   ==============
//   <i> Local Ethernet MAC Address
//   <i> Value FF:FF:FF:FF:FF:FF is not allowed.
//   <i> It is an ethernet Broadcast MAC address.
//     <o>Address byte 1 <0x00-0xff:2>
//     <i> LSB is an ethernet Multicast bit.
//     <i> Must be 0 for local MAC address.
//     <i> Default: 0x00
#define _MAC1          0x1E

//     <o>Address byte 2 <0x00-0xff>
//     <i> Default: 0x30
#define _MAC2          0x30

//     <o>Address byte 3 <0x00-0xff>
//     <i> Default: 0x6C
#define _MAC3          0x6C

//     <o>Address byte 4 <0x00-0xff>
//     <i> Default: 0x00
#define _MAC4          0xA2

//     <o>Address byte 5 <0x00-0xff>
//     <i> Default: 0x00
#define _MAC5          0x45

//     <o>Address byte 6 <0x00-0xff>
//     <i> Default: 0x01
#define _MAC6          0x5E

//   </h>
//   <h>IP Address
//   =============
//   <i> Local Static IP Address
//   <i> Value 255.255.255.255 is not allowed.
//   <i> It is a Broadcast IP address.
//     <o>Address byte 1 <0-255>
//     <i> Default: 192
#define _IP1           192

//     <o>Address byte 2 <0-255>
//     <i> Default: 168
#define _IP2           168

//     <o>Address byte 3 <0-255>
//     <i> Default: 0
#define _IP3           0

//     <o>Address byte 4 <0-255>
//     <i> Default: 100
#define _IP4           100

//   </h>
//   <h>Subnet mask
//   ==============
//   <i> Local Subnet mask
//     <o>Mask byte 1 <0-255>
//     <i> Default: 255
#define _MSK1          255

//     <o>Mask byte 2 <0-255>
//     <i> Default: 255
#define _MSK2          255

//     <o>Mask byte 3 <0-255>
//     <i> Default: 255
#define _MSK3          255

//     <o>Mask byte 4 <0-255>
//     <i> Default: 0
#define _MSK4          0

//   </h>
//   <h>Default Gateway
//   ==================
//   <i> Default Gateway IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 192
#define _GW1           192

//     <o>Address byte 2 <0-255>
//     <i> Default: 168
#define _GW2           168

//     <o>Address byte 3 <0-255>
//     <i> Default: 0
#define _GW3           0

//     <o>Address byte 4 <0-255>
//     <i> Default: 254
#define _GW4           254

//   </h>
//   <h>Primary DNS Server
//   =====================
//   <i> Primary DNS Server IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 194
#define _pDNS1         194

//     <o>Address byte 2 <0-255>
//     <i> Default: 25
#define _pDNS2         25

//     <o>Address byte 3 <0-255>
//     <i> Default: 2
#define _pDNS3         2

//     <o>Address byte 4 <0-255>
//     <i> Default: 129
#define _pDNS4         129

//   </h>
//   <h>Secondary DNS Server
//   =======================
//   <i> Secondary DNS Server IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 194
#define _sDNS1         194

//     <o>Address byte 2 <0-255>
//     <i> Default: 25
#define _sDNS2         25

//     <o>Address byte 3 <0-255>
//     <i> Default: 2
#define _sDNS3         2

//     <o>Address byte 4 <0-255>
//     <i> Default: 130
#define _sDNS4         130

//   </h>
//   <h>ARP Definitions
//   ==================
//   <i> Address Resolution Protocol Definitions
//     <o>Cache Table size <5-100>
//     <i> Number of cached hardware/IP addresses
//     <i> Default: 10
#define ARP_TABSIZE    10

//     <o>Cache Timeout in seconds <5-255>
//     <i> A timeout for a cached hardware/IP addresses
//     <i> Default: 150
#define ARP_TIMEOUT    150

//     <o>Number of Retries <0-20>
//     <i> Number of Retries to resolve an IP address
//     <i> before ARP module gives up
//     <i> Default: 4
#define ARP_MAXRETRY   4

//     <o>Resend Timeout in seconds <1-10>
//     <i> A timeout to resend the ARP Request
//     <i> Default: 2
#define ARP_RESEND     2

//   </h>
//   <e>IGMP Group Management
//   ========================
//   <i> Enable or disable Internet Group Management Protocol
#define IGMP_ENABLE    0

//     <o>Membership Table size <2-50>
//     <i> Number of Groups this host can join
//     <i> Default: 5
#define IGMP_TABSIZE   5

//   </e>
//   <q>NetBIOS Name Service
//   =======================
//   <i> When this option is enabled, the embedded host can be
//   <i> accessed by his name on the local LAN using NBNS protocol.
//   <i> You need to modify also the number of UDP Sockets,
//   <i> because NBNS protocol uses one UDP socket to run.
#define NBNS_ENABLE    1

//   <q>Dynamic Host Configuration
//   =============================
//   <i> When this option is enabled, local IP address, Net Mask
//   <i> and Default Gateway are obtained automatically from
//   <i> the DHCP Server on local LAN.
//   <i> You need to modify also the number of UDP Sockets,
//   <i> because DHCP protocol uses one UDP socket to run.
#define DHCP_ENABLE    1

// </e>

// <e>PPP Network Interface
// ========================
// <i> Enable or disable PPP Network Interface
#define PPP_ENABLE     0

//   <h>IP Address
//   =============
//   <i> Local Static IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 192
#define _IP1P          192

//     <o>Address byte 2 <0-255>
//     <i> Default: 168
#define _IP2P          168

//     <o>Address byte 3 <0-255>
//     <i> Default: 125
#define _IP3P          125

//     <o>Address byte 4 <0-255>
//     <i> Default: 1
#define _IP4P          1

//   </h>
//   <h>Subnet mask
//   ==============
//   <i> Local Subnet mask
//     <o>Mask byte 1 <0-255>
//     <i> Default: 255
#define _MSK1P         255

//     <o>Mask byte 2 <0-255>
//     <i> Default: 255
#define _MSK2P         255

//     <o>Mask byte 3 <0-255>
//     <i> Default: 255
#define _MSK3P         255

//     <o>Mask byte 4 <0-255>
//     <i> Default: 0
#define _MSK4P         0

//   </h>
//   <h>Primary DNS Server
//   =====================
//   <i> Primary DNS Server IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 194
#define _pDNS1P        194

//     <o>Address byte 2 <0-255>
//     <i> Default: 25
#define _pDNS2P        25

//     <o>Address byte 3 <0-255>
//     <i> Default: 2
#define _pDNS3P        2

//     <o>Address byte 4 <0-255>
//     <i> Default: 129
#define _pDNS4P        129

//   </h>
//   <h>Secondary DNS Server
//   =======================
//   <i> Secondary DNS Server IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 194
#define _sDNS1P        194

//     <o>Address byte 2 <0-255>
//     <i> Default: 25
#define _sDNS2P        25

//     <o>Address byte 3 <0-255>
//     <i> Default: 2
#define _sDNS3P        2

//     <o>Address byte 4 <0-255>
//     <i> Default: 130
#define _sDNS4P        130

//   </h>
//   <q>Obtain Client IP address automatically
//   =========================================
//   <i> This option only applies when PPP Dial-up is used to dial
//   <i> to remote PPP Server. If checked, network connection
//   <i> dynamically obtains an IP address from remote PPP Server.
#define PPP_GETIP      1

//   <q>Use Default Gateway on remote Network
//   ========================================
//   <i> This option only applies when both Ethernet and PPP Dial-up
//   <i> are used. If checked, data that cannot be sent to local LAN
//   <i> is forwarded to Dial-up network instead.
#define PPP_DEFGW      1

//   <h>Async Control Character Map
//   ==============================
//   <i> A map of control characters 0..31 which are transmitted 
//   <i> escaped as a 2 byte sequence.
//     <o>Map char 31..24 mask <0x00-0xff>
//     <i> Maps control characters from 31...24
//     <i> Default: 0x00
#define _ACCM1         0x00

//     <o>Map char 23..16 mask <0x00-0xff>
//     <i> Maps control characters from 23...16
//     <i> For XON/XOFF set this value to: 0x0A
//     <i> Default: 0x00
#define _ACCM2         0x0A

//     <o>Map char 15..8 mask <0x00-0xff>
//     <i> Maps control characters from 15...8
//     <i> Default: 0x00
#define _ACCM3         0x00

//     <o>Map char 7..0 mask <0x00-0xff>
//     <i> Maps control characters from 7...0
//     <i> Default: 0x00
#define _ACCM4         0x00

//   </h>
//   <h>Retransmissions and Timeouts
//   ===============================
//   <i> Several protocol settings.
//     <o>LCP Number of Retries <0-20>
//     <i> How many times Link Control Protocol will try to retransmit
//     <i> data before giving up. Increase this value for links with
//     <i> low baud rates or high latency.
//     <i> Default: 2
#define LCP_MAXRETRY   2

//     <o>LCP Retry Timeout in seconds <1-10>
//     <i> If no response received within this time frame,
//     <i> LCP module will try to resend data again.
//     <i> Default: 2
#define LCP_RETRYTOUT  2

//     <o>PAP Number of Retries <0-20>
//     <i> How many times Password Authentication Protocol will try to 
//     <i> retransmit data before giving up. Increase this value for links
//     <i> with low baud rates or high latency.
//     <i> Default: 3
#define PAP_MAXRETRY   3

//     <o>PAP Retry Timeout in seconds <1-10>
//     <i> If no response received within this time frame,
//     <i> PAP module will try to resend data again
//     <i> Default: 3
#define PAP_RETRYTOUT  3

//     <o>IPCP Number of Retries <0-20>
//     <i> How many times Internet Protocol Control Protocol will try
//     <i> to retransmit data before giving up. Increase this value for
//     <i> links with low baud rates or high latency.
//     <i> Default: 3
#define IPCP_MAXRETRY  3

//     <o>IPCP Retry Timeout in seconds <1-10>
//     <i> If no response received within this time frame,
//     <i> IPCP module will try to resend data again
//     <i> Default: 2
#define IPCP_RETRYTOUT 2

//   </h>
// </e>
// <e>SLIP Network Interface
// ========================
// <i> Enable or disable SLIP Network Interface
#define SLIP_ENABLE    0

//   <h>IP Address
//   =============
//   <i> Local Static IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 192
#define _IP1S          192

//     <o>Address byte 2 <0-255>
//     <i> Default: 168
#define _IP2S          168

//     <o>Address byte 3 <0-255>
//     <i> Default: 225
#define _IP3S          225

//     <o>Address byte 4 <0-255>
//     <i> Default: 1
#define _IP4S          1

//   </h>
//   <h>Subnet mask
//   ==============
//   <i> Local Subnet mask
//     <o>Mask byte 1 <0-255>
//     <i> Default: 255
#define _MSK1S         255

//     <o>Mask byte 2 <0-255>
//     <i> Default: 255
#define _MSK2S         255

//     <o>Mask byte 3 <0-255>
//     <i> Default: 255
#define _MSK3S         255

//     <o>Mask byte 4 <0-255>
//     <i> Default: 0
#define _MSK4S         0

//   </h>
//   <h>Primary DNS Server
//   =====================
//   <i> Primary DNS Server IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 194
#define _pDNS1S        194

//     <o>Address byte 2 <0-255>
//     <i> Default: 25
#define _pDNS2S        25

//     <o>Address byte 3 <0-255>
//     <i> Default: 2
#define _pDNS3S        2

//     <o>Address byte 4 <0-255>
//     <i> Default: 129
#define _pDNS4S        129

//   </h>
//   <h>Secondary DNS Server
//   =======================
//   <i> Secondary DNS Server IP Address
//     <o>Address byte 1 <0-255>
//     <i> Default: 194
#define _sDNS1S        194

//     <o>Address byte 2 <0-255>
//     <i> Default: 25
#define _sDNS2S        25

//     <o>Address byte 3 <0-255>
//     <i> Default: 2
#define _sDNS3S        2

//     <o>Address byte 4 <0-255>
//     <i> Default: 130
#define _sDNS4S        130

//   </h>
//   <q>Use Default Gateway on remote Network
//   ========================================
//   <i> This option only applies when both Ethernet and SLIP Dial-up
//   <i> are used. If checked, data that cannot be sent to local LAN
//   <i> is forwarded to Dial-up network instead.
#define SLIP_DEFGW     1

// </e>
// <e>UDP Sockets
// ==============
// <i> Enable or disable UDP Sockets
#define UDP_ENABLE     1

//   <o>Number of UDP Sockets <1-20>
//   <i> Number of available UDP sockets
//   <i> Default: 5
#define UDP_NUMSOCKS   2

//   <o>Highest port for autoselect <1-32767>
//   <i> Highest port number for UDP port when the port is not provided
//   <i> by the user but is automatically selected by the system.
//   <i> Default: 1023
#define UDP_LASTPORT   1023

// </e>
// <e>TCP Sockets
// ==============
// <i> Enable or disable TCP Sockets
#define TCP_ENABLE     1

//   <o>Number of TCP Sockets <1-20>
//   <i> Number of available TCP sockets
//   <i> Default: 5
#define TCP_NUMSOCKS   7

//   <o>Highest port for autoselect <1-32767>
//   <i> Highest port number for TCP port when the port is not provided
//   <i> by the user but is automatically selected by the system.
//   <i> Default: 1023
#define TCP_LASTPORT   1023

//   <o>Number of Retries <0-20>
//   <i> How many times TCP module will try to retransmit data
//   <i> before giving up. Increase this value for high-latency
//   <i> and low_throughput networks.
//   <i> Default: 5
#define TCP_MAXRETRY   5

//   <o>Retry Timeout in seconds <1-10>
//   <i> If data frame not acknowledged within this time frame,
//   <i> TCP module will try to resend data again
//   <i> Default: 4
#define TCP_RETRYTOUT  4

//   <o>Default Connect Timeout in seconds <1-600>
//   <i> Default TCP Socket Keep Alive timeout. When it expires
//   <i> with no TCP data frame send, TCP Connection is closed.
//   <i> Default: 120
#define TCP_DEFTOUT    120

/* TCP fixed timeouts */
#define TCP_INIT_RETRY_TOUT 1     /* TCP initial Retransmit period in sec.   */
#define TCP_SYN_RETRY_TOUT  2     /* TCP SYN frame retransmit period in sec. */
#define TCP_CONRETRY        7     /* Number of retries to establish a conn.  */

// </e>
// <e>HTTP Server
// ==============
// <i> Enable or disable HTTP Server
#define HTTP_ENABLE    1

//   <o>Number of HTTP Sessions <1-10>
//   <i> Number of simultaneously active HTTP Sessions.
//   <i> Modify also the number of TCP Sockets because
//   <i> each HTTP session uses it's own TCP socket
//   <i> Default: 3
#define HTTP_NUMSESS   6

//   <o>Port Number <1-65535>
//   <i> Listening port number.
//   <i> Default: 80
#define HTTP_PORTNUM   80

//   <e>Enable User Authentication
//     <i> When enabled, the user will have to authenticate
//     <i> himself by username and password before accessing
//     <i> any page on this Embedded WEB server.
#define HTTP_ENAUTH    1

//     <s.20>Authentication Realm string
//     <i> Default: "Embedded WEB Server"
#define HTTP_AUTHREALM "Embedded WEB Server"

//     <s.15>Authentication Username
//     <i> Default: "admin"
#define HTTP_AUTHUSER  "admin"

//     <s.15>Authentication Password
//     <i> Default: ""
#define HTTP_AUTHPASSW ""

//   </e>
// </e>
// <e>Telnet Server
// ================
// <i> Enable or disable Telnet Server
#define TNET_ENABLE    0

//   <o>Number of Telnet Connections <1-10>
//   <i> Number of simultaneously active Telnet Connections.
//   <i> Modify also the number of TCP Sockets because
//   <i> each Telnet connection uses it's own TCP socket
//   <i> Default: 1
#define TNET_NUMSESS   2

//   <o>Port Number <1-65535>
//   <i> Listening port number.
//   <i> Default: 23
#define TNET_PORTNUM   23

//   <e>Enable User Authentication
//   <i> When enabled, the user will have to authenticate
//   <i> himself by username and password before access
//   <i> to the system is allowed.
#define TNET_ENAUTH    1

//     <s.15>Authentication Username
//     <i> Default: "admin"
#define TNET_AUTHUSER  "admin"

//     <s.15>Authentication Password
//     <i> Default: ""
#define TNET_AUTHPASSW ""

//   </e>
// </e>
// <e>TFTP Server
// ==============
// <i> Enable or disable TFTP Server
#define TFTP_ENABLE    0

//   <o>Number of TFTP Sessions <1-10>
//   <i> Number of simultaneously active TFTP Sessions
//   <i> All TFTP Sessions use the same UDP socket listening
//   <i> on defalut TFTP Server port 69.
//   <i> Default: 1
#define TFTP_NUMSESS   1

//   <o>Port Number <1-65535>
//   <i> Listening port number.
//   <i> Default: 69
#define TFTP_PORTNUM   69

//   <o>Inactive Session Timeout in seconds <5-120>
//   <i> When timeout expires TFTP Session is closed. This timeout
//   <i> is used when the UDP connection is broken because of error.
//   <i> Default: 15
#define TFTP_DEFTOUT   15

//   <o>Number of Retries <1-10>
//   <i> How many times TFTP Server will try to retransmit data
//   <i> before giving up.
//   <i> Default: 4
#define TFTP_MAXRETRY  4

// </e>
// <e>DNS Client
// =============
// <i> Enable or disable DNS Client
#define DNS_ENABLE     0

//     <o>Cache Table size <5-100>
//     <i> Number of cached DNS host names/IP addresses
//     <i> Default: 20
#define DNS_TABSIZE    20

// </e>
// <e>SMTP Client
// ==============
// <i> Enable or disable SMTP Client
#define SMTP_ENABLE     0

//     <o>Response Timeout in seconds <5-120>
//     <i> This is a time for SMTP Client to wait for a response from
//     <i> SMTP Server. If timeout expires, Client aborts operation.
//     <i> Default: 20
#define SMTP_DEFTOUT   20

// </e>
//------------- <<< end of configuration section >>> -----------------------

/*----------------------------------------------------------------------------
 *      RTL User configuration part END
 *---------------------------------------------------------------------------*/

#define TICK_RATE       1000 / TICK_INTERVAL

#if (SLIP_ENABLE | PPP_ENABLE | ETH_ENABLE) == 0
   #error all network interfaces disabled in configuration
#endif

#if (SLIP_ENABLE & PPP_ENABLE) == 1
   #error both SLIP and PPP interfaces enabled
#endif

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

/* Own and Broadcast Ethernet address */
#if ETH_ENABLE == 1
  U8       own_hw_adr[ETH_ADRLEN] = { _MAC1, _MAC2, _MAC3, _MAC4, _MAC5, _MAC6 };
  U8 const MAC_Broadcast[ETH_ADRLEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
#endif
/* Broadcast IP address */
U8 const IP_Broadcast[IP_ADRLEN]  = { 255, 255, 255, 255 };
/* Local Machine info */
LOCALM const nlocalm[3] = {
  /* Ethernet: Local IP address */
#if ETH_ENABLE == 1
  { { _IP1, _IP2, _IP3, _IP4 },
    /* Default Gateway IP address */
    { _GW1, _GW2, _GW3, _GW4 },
    /* Subnet IP Mask */
    { _MSK1, _MSK2, _MSK3, _MSK4 },
    /* Primary DNS Server IP */
    { _pDNS1, _pDNS2, _pDNS3, _pDNS4 },
    /* Secondary DNS Server IP */
    { _sDNS1, _sDNS2, _sDNS3, _sDNS4 }
  },
#else
   { { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 } },
#endif
  /* PPP: Local IP address */
#if PPP_ENABLE == 1
  { { _IP1P, _IP2P, _IP3P, _IP4P },
    /* Default Gateway IP address */
    { 0, 0, 0, 0 },
    /* Subnet IP Mask */
    { _MSK1P, _MSK2P, _MSK3P, _MSK4P },
    /* Primary DNS Server IP */
    { _pDNS1P, _pDNS2P, _pDNS3P, _pDNS4P },
    /* Secondary DNS Server IP */
    { _sDNS1P, _sDNS2P, _sDNS3P, _sDNS4P }
  },
#else
   { { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 } },
#endif
  /* SLIP: Local IP address */
#if SLIP_ENABLE == 1
  { { _IP1S, _IP2S, _IP3S, _IP4S },
    /* Default Gateway IP address */
    { 0, 0, 0, 0 },
    /* Subnet IP Mask */
    { _MSK1S, _MSK2S, _MSK3S, _MSK4S },
    /* Primary DNS Server IP */
    { _pDNS1S, _pDNS2S, _pDNS3S, _pDNS4S },
    /* Secondary DNS Server IP */
    { _sDNS1S, _sDNS2S, _sDNS3S, _sDNS4S }
  }
#else
   { { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 } },
#endif
};
U8 const netif_cfg = (PPP_GETIP<<5 | SLIP_DEFGW<<4 | PPP_DEFGW<<3 |
                      SLIP_ENABLE<<2 | PPP_ENABLE<<1 | ETH_ENABLE);

/* Memory pool for Eternet Frames. */
U32 Mem_Pool[MEM_SIZE];
/* ARP cache table */
#if ETH_ENABLE == 1
  struct arp_info arp_table[ARP_TABSIZE];
#endif
#if (IGMP_ENABLE & ETH_ENABLE) == 1
  struct igmp_info igmp_table[IGMP_TABSIZE];
#endif
/* UDP socket Control block table */
#if UDP_ENABLE == 1
  struct udp_info udp_socket[UDP_NUMSOCKS];
#endif
/* TCP socket Control block table */
#if TCP_ENABLE == 1
  struct tcp_info tcp_socket[TCP_NUMSOCKS+1];
#endif
/* HTTP Server Session Control block table */
#if HTTP_ENABLE == 1
  struct http_info http_session[HTTP_NUMSESS];
#endif
/* Telnet Server Session Control block table */
#if TNET_ENABLE == 1
  struct tnet_info tnet_session[TNET_NUMSESS];
#endif
/* TFTP Server Session Control block table */
#if TFTP_ENABLE == 1
  struct tftp_info tftp_session[TFTP_NUMSESS];
#endif
/* DNS cache table */
#if DNS_ENABLE == 1
  struct dns_info dns_table[DNS_TABSIZE];
#endif

/* Exported defines to other modules */
U32 const MPool_Size     = sizeof (Mem_Pool);
U16 const tick_rate      = TICK_RATE;
U8        lhost_name[16] = LHOST_NAME;
#if ETH_ENABLE == 1
  U16 const arp_TabSize  = ARP_TABSIZE;
  U16 const arp_TimeOut  = ARP_TIMEOUT;
  U8  const arp_MaxRetry = ARP_MAXRETRY;
  U8  const arp_Resend   = ARP_RESEND;
#endif
#if PPP_ENABLE == 1
  U32 const ppp_ACCMap    = _ACCM1<<24 | _ACCM2<<16 | _ACCM3<<8 | _ACCM4;
  U16 const ppp_SerTout   = 5 * TICK_RATE;
  U16 const lcp_RetryTout = LCP_RETRYTOUT * TICK_RATE;
  U16 const pap_RetryTout = PAP_RETRYTOUT * TICK_RATE;
  U16 const ipcp_RetryTout= IPCP_RETRYTOUT * TICK_RATE;
  U8  const lcp_MaxRetry  = LCP_MAXRETRY;
  U8  const pap_MaxRetry  = PAP_MAXRETRY;
  U8  const ipcp_MaxRetry = IPCP_MAXRETRY;
#endif
#if (IGMP_ENABLE & ETH_ENABLE) == 1
  U16 const igmp_TabSize  = IGMP_TABSIZE;
  U8  const igmp_T200ms   = TICK_RATE / 5;
  U8  const IP_AllHosts[IP_ADRLEN] = {224, 0, 0, 1 };
#endif
#if SLIP_ENABLE == 1
  U16 const slip_SerTout  = 5 * TICK_RATE;
#endif
#if UDP_ENABLE == 1
  U8  const udp_NumSocks  = UDP_NUMSOCKS;
  U16 const udp_LastPort  = UDP_LASTPORT;
#endif
#if TCP_ENABLE == 1
  U8  const tcp_NumSocks  = TCP_NUMSOCKS;
  U8  const tcp_MaxRetry  = TCP_MAXRETRY;
  U16 const tcp_LastPort  = TCP_LASTPORT;
  U16 const tcp_RetryTout = TCP_RETRYTOUT * TICK_RATE;
  U16 const tcp_2MSSTout  = (200 + TICK_INTERVAL - 1) / TICK_INTERVAL;
  U16 const tcp_SynRetryTout = TCP_SYN_RETRY_TOUT * TICK_RATE;
  U16 const tcp_InitRetryTout= TCP_INIT_RETRY_TOUT * TICK_RATE;
  U16 const tcp_DefTout   = TCP_DEFTOUT;
  U8  const tcp_ConRetry  = TCP_CONRETRY;
#endif
#if HTTP_ENABLE == 1
  U8  const http_NumSess  = HTTP_NUMSESS;
  U8  const http_EnAuth   = HTTP_ENAUTH;
  U16 const http_PortNum  = HTTP_PORTNUM;
  U8  const http_auth_dev[]     = HTTP_AUTHREALM;
  U8  const http_auth_user[]    = HTTP_AUTHUSER;
  U8        http_auth_passw[20] = HTTP_AUTHPASSW;
#endif
#if TNET_ENABLE == 1
  U8  const tnet_NumSess  = TNET_NUMSESS;
  U8  const tnet_EnAuth   = TNET_ENAUTH;
  U16 const tnet_PortNum  = TNET_PORTNUM;
  U8  const tnet_auth_user[]    = TNET_AUTHUSER;
  U8        tnet_auth_passw[20] = TNET_AUTHPASSW;
#endif
#if TFTP_ENABLE == 1
  U8  const tftp_NumSess  = TFTP_NUMSESS;
  U8  const tftp_MaxRetry = TFTP_MAXRETRY;
  U16 const tftp_PortNum  = TFTP_PORTNUM;
  U16 const tftp_DefTout  = TFTP_DEFTOUT;
#endif
#if DNS_ENABLE == 1
  U16 const dns_TabSize   = DNS_TABSIZE;
#endif
#if SMTP_ENABLE == 1
  U16 const smtp_DefTout  = SMTP_DEFTOUT;
#endif

/*----------------------------------------------------------------------------
 *      Fatal Error Handler
 *---------------------------------------------------------------------------*/

void sys_error (ERROR_CODE code) {
   /* This function is called when a fatal error is encountered. The normal */
   /* program execution is not possible anymore. Add your crytical error   .*/
   /* handler code here.                                                    */

   switch (code) {
      case ERR_MEM_ALLOC:
         /* Out of memory. */
         break;

      case ERR_MEM_FREE:
         /* Trying to release non existing memory block. */
         break;

      case ERR_MEM_CORRUPT:
         /* Memory Link pointer is Corrupted. */
         /* More data written than the size of allocated mem block. */
         break;

      case ERR_UDP_ALLOC:
         /* Out of UDP Sockets. */
         break;

      case ERR_TCP_ALLOC:
         /* Out of TCP Sockets. */
         break;

      case ERR_TCP_STATE:
         /* TCP State machine in undefined state. */
         break;
   }

   /* End-less loop */
   while (1);
}


/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- init_system -----------------------------------*/

void init_system (void) {
   /* Initialize configured interfaces and applications. */

#if ETH_ENABLE == 1
   init_eth_link ();
#endif
#if PPP_ENABLE == 1
   init_ppp_link ();
#endif
#if SLIP_ENABLE == 1
   init_slip_link ();
#endif

#if (IGMP_ENABLE & ETH_ENABLE) == 1
   init_igmp ();
#endif
#if UDP_ENABLE == 1
   init_udp ();
#endif
#if TCP_ENABLE == 1
   init_tcp ();
#endif
#if HTTP_ENABLE == 1
   init_http ();
#endif
#if TNET_ENABLE == 1
   init_tnet ();
#endif
#if TFTP_ENABLE == 1
   init_tftp ();
#endif
#if (NBNS_ENABLE & ETH_ENABLE) == 1
   init_nbns ();
#endif
#if (DHCP_ENABLE & ETH_ENABLE) == 1
   init_dhcp ();
#endif
#if DNS_ENABLE == 1
   init_dns ();
#endif
#if SMTP_ENABLE == 1
   init_smtp ();
#endif
}


/*--------------------------- run_system ------------------------------------*/

void run_system (void) {
   /* Run configured interfaces and applications. */

#if ETH_ENABLE == 1
   run_eth_link ();
#endif
#if PPP_ENABLE == 1
   run_ppp_link ();
#endif
#if SLIP_ENABLE == 1
   run_slip_link ();
#endif

#if (IGMP_ENABLE & ETH_ENABLE) == 1
   run_igmp_host ();
#endif
#if TCP_ENABLE == 1
   tcp_poll_sockets ();
#endif
#if HTTP_ENABLE == 1
   run_http_server ();
#endif
#if TNET_ENABLE == 1
   run_tnet_server ();
#endif
#if TFTP_ENABLE == 1
   run_tftp_server ();
#endif
#if (DHCP_ENABLE & ETH_ENABLE) == 1
   run_dhcp_client ();
#endif
#if DNS_ENABLE == 1
   run_dns_client ();
#endif
#if SMTP_ENABLE == 1
   run_smtp_client ();
#endif
}


/*--------------------------- process_hl_igmp -------------------------------*/

#if (IGMP_ENABLE & ETH_ENABLE) == 1
void process_hl_igmp (OS_FRAME *frame) {
   /* Process IGMP protocol frame */
   process_igmp (frame);
}
#endif


/*--------------------------- process_hl_udp --------------------------------*/

#if UDP_ENABLE == 1
void process_hl_udp (OS_FRAME *frame) {
   /* Process udp protocol frame */
   process_udp (frame);
}
#endif


/*--------------------------- process_hl_tcp --------------------------------*/

#if TCP_ENABLE == 1
void process_hl_tcp (OS_FRAME *frame) {
   /* Process tcp protocol frame */
   process_tcp (frame);
}
#endif


/*--------------------------- dispatch_frame --------------------------------*/

BOOL dispatch_frame (OS_FRAME *frame, U8 netif) {
   /* Dispatch the outgoing frame to selected network interface. */

switch (netif) {
#if ETH_ENABLE == 1
      case NETIF_ETH:
         return (eth_send_frame (frame));
#endif
#if PPP_ENABLE == 1
      case NETIF_PPP:
         return (ppp_send_frame (frame, PPP_PROT_IP));
#endif
#if SLIP_ENABLE == 1
      case NETIF_SLIP:
         return (slip_send_frame (frame));
#endif
   }
   return (__FALSE);
}


/*--------------------------- eth_chk_adr -----------------------------------*/

#if ETH_ENABLE == 0
BOOL eth_chk_adr (OS_FRAME *frame) {
   /* Provide an empty function when the 'Ethernet Interface' is disabled. */
   frame = frame;
   return (__FALSE);
}
#endif


/*--------------------------- eth_get_adr -----------------------------------*/

#if ETH_ENABLE == 0
U8 *eth_get_adr (U8 *ipadr) {
   /* Provide an empty function when the 'Ethernet Interface' is disabled. */
   ipadr = ipadr;
   return (NULL);
}
#endif


/*--------------------------- igmp_is_member --------------------------------*/

#if (IGMP_ENABLE & ETH_ENABLE) == 0
BOOL igmp_is_member (U8 *ipadr) {
   /* Provide an empty function when IP Multicasting is not enabled. */
   ipadr = ipadr;
   return (__FALSE);
}
#endif


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
