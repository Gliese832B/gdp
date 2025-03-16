#include "gdp.h"
#include <net/if.h>
#include <sys/socket.h>
#include <asm-generic/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

GdpHost* gdp_create_host() {
    GdpHost* gdp = (GdpHost*)malloc(sizeof(GdpHost));
    gdp->iface = "wlan0";
    
   
    if ((gdp->sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    
    struct ifreq if_idx;
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, gdp->iface, IFNAMSIZ-1);
    if (ioctl(gdp->sockfd, SIOCGIFINDEX, &if_idx) < 0) {
        perror("SIOCGIFINDEX");
        close(gdp->sockfd);
        exit(EXIT_FAILURE);
    }

   
    
    memset(&gdp->if_mac, 0, sizeof(struct ifreq));
    strncpy(gdp->if_mac.ifr_name, gdp->iface, IFNAMSIZ-1);
    if (ioctl(gdp->sockfd, SIOCGIFHWADDR, &gdp->if_mac) < 0) {
        perror("SIOCGIFHWADDR");
        close(gdp->sockfd);
        exit(EXIT_FAILURE);
    }

   
    sscanf(DEST_MAC, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &gdp->dest_mac[0], &gdp->dest_mac[1], &gdp->dest_mac[2],
           &gdp->dest_mac[3], &gdp->dest_mac[4], &gdp->dest_mac[5]);

    
    memset(&gdp->dest_addr, 0, sizeof(gdp->dest_addr));
    gdp->dest_addr.sll_family = AF_PACKET;
    gdp->dest_addr.sll_ifindex = if_idx.ifr_ifindex;
    gdp->dest_addr.sll_halen = ETH_ALEN;
    memcpy(gdp->dest_addr.sll_addr, gdp->dest_mac, ETH_ALEN);

    
    if (setsockopt(gdp->sockfd, SOL_SOCKET, SO_BINDTODEVICE, gdp->iface, strlen(gdp->iface)) < 0) {
        perror("setsockopt failed");
        close(gdp->sockfd);
        exit(EXIT_FAILURE);
    }

    return gdp;
}

void SendPacket(GdpHost* gdp, GdpPacket* packet, const char* message, size_t message_size) {
    memset(packet, 0, sizeof(GdpPacket));

   
    struct ethhdr *eth = (struct ethhdr *)packet;
    memcpy(eth->h_dest, gdp->dest_mac, ETH_ALEN); 
    memcpy(eth->h_source, gdp->if_mac.ifr_hwaddr.sa_data, ETH_ALEN); 
    eth->h_proto = htons(ETH_P_IP); 

   
    const char *prefix = "[192.168.1.1:80] ";
    size_t prefix_size = strlen(prefix);
    size_t total_size = prefix_size + message_size;

    
    packet->payload = (char *)malloc(total_size);
    if (!packet->payload) {
        perror("Couldn't allocate space for packet");
        exit(EXIT_FAILURE);
    }

   
    memcpy(packet->payload, prefix, prefix_size);
    memcpy(packet->payload + prefix_size, message, message_size);
    packet->payload_size = total_size;
    if (!packet->payload) {
        perror("Couldn't allocate space for packet");
        exit(EXIT_FAILURE);
    }
    printf(packet->payload);
   

    
    ssize_t bytes_sent = sendto(gdp->sockfd, packet, sizeof(struct ethhdr) + total_size, 0,
                                (struct sockaddr *)&gdp->dest_addr, sizeof(gdp->dest_addr));
    if (bytes_sent < 0) {
        perror("sendto failed");
        close(gdp->sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Packet sent (%zd bytes)\n", bytes_sent);
}

int destroy(GdpHost *gdp) {
    close(gdp->sockfd);
    free(gdp);
}

int main() {
    GdpHost* gdp = gdp_create_host();
    while (1) {
        GdpPacket packet;
        SendPacket(gdp, &packet, "selam", strlen("selam")); 
        sleep(1);
    }
    destroy(gdp);
    return 0;
}