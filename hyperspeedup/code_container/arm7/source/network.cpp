#include "sgIP_sockets.h"
#include "sgIP_TCP.h"
#include "sgIP_UDP.h"
#include "sgIP_ICMP.h"
#include "sgIP_DNS.h"


typedef void (* FN_importfunctions)(void* irqSetb, void* irqEnableb,void* fifoSendAddressb, void* fifoSendValue32b) ;
typedef struct hostent * (* FN_gethostbyname)(const char * name) ;
typedef int (* FN_socket)(int domain, int type, int protocol) ;
typedef unsigned short (* FN_htons)(unsigned short num) ;
typedef bool (* FN_connect)(int socket, const struct sockaddr * addr, int addr_len) ;
typedef int (* FN_send)(int socket, const void * data, int sendlength, int flags) ;
typedef int (* FN_recv)(int socket, void * data, int recvlength, int flags) ;
typedef int (* FN_shutdown)(int socket, int shutdown_type) ;
typedef int (* FN_closesocket)(int socket) ;
typedef bool (* FN_Wifi_InitDefault)(bool useFirmwareSettings) ;
typedef void (* FN_wifiValue32Handlerarm9)(u32 value) ;
typedef void (* FN_wifisendcallback)() ;
typedef unsigned long (* FN_inet_addr)(const char *cp);

struct NET_INTERFACE_STRUCT {
	unsigned int			name ;
	FN_importfunctions importfunctions;
	FN_gethostbyname gethostbyname;
	FN_socket socket;
	FN_htons htons;
	FN_connect connect;
	FN_send send;
	FN_recv recv;
	FN_shutdown shutdown;
	FN_closesocket closesocket;
	FN_Wifi_InitDefault Wifi_InitDefault;
	FN_wifiValue32Handlerarm9 wifiValue32Handlerarm9;
	FN_inet_addr inet_addr;
	FN_wifisendcallback wifisendcallback;
} ;

const NET_INTERFACE_STRUCT* netinter = (NET_INTERFACE_STRUCT*)0x2380000;

