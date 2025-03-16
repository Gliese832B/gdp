#include <netinet/ip.h>
#include <netinet/ether.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <netdb.h>

#define DEST_MAC "xx:xx:xx:xx:xx" 
typedef struct  {
    int sockfd;
    struct sockaddr_ll dest_addr;
    
    char *iface;
    unsigned char dest_mac[ETH_ALEN];
    struct ifreq if_mac;
   
    


} GdpHost;

typedef struct  {
    struct ethhdr ethernet_header; 
    char *payload;
    size_t payload_size;



} GdpPacket;