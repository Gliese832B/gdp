#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/udp.h>

#include <stdbool.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#define MAX_MESSAGE_SIZE 256
#define PACKET_HEADER_OFFSET sizeof(struct ethhdr)
static inline __u64 ether_addr_to_u64(const __u8 *addr)
{
	__u64 u = 0;
	int i;

	for (i = ETH_ALEN - 1; i >= 0; i--)
		u = u << 8 | addr[i];
	return u;
}

SEC("xdp")
int  xdp_prog(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
	void *data     = (void *)(long)ctx->data;
    struct ethhdr *eth = data;
    __u64 offset = sizeof(*eth);
    if ((void *)eth + offset > data_end)
		return 0;
	
        __u16 h_proto = bpf_ntohs(eth->h_proto);
        if (h_proto == ETH_P_IP) { 
            struct iphdr *ip = (struct iphdr *)(eth + 1);
            if ((void *)(ip + 1) > data_end)
                return XDP_PASS;
    
            __u32 src_ip = bpf_ntohl(ip->saddr);
            __u32 dst_ip = bpf_ntohl(ip->daddr);
            
            bpf_printk("IPv4 src: %u.%u.%u.%u, dst: %u.%u.%u.%u\n",
                (src_ip >> 24) & 0xFF, (src_ip >> 16) & 0xFF, 
                (src_ip >> 8) & 0xFF, src_ip & 0xFF,
                (dst_ip >> 24) & 0xFF, (dst_ip >> 16) & 0xFF, 
                (dst_ip >> 8) & 0xFF, dst_ip & 0xFF);
                

            
    
              
                if (src_ip == 0) {
                    bpf_printk("Got a packet from client\n");
                   
                        


                }
        }
   



}

char _license[] SEC("license") = "GPL";