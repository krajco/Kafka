#include <stdint.h>
#include <netinet/in.h>
#include <math.h>
#include <stdio.h>
/* Headers len */
#define ETHER_HEADER_LEN 14
#define IPV6_HEADER_LEN 40
#define UDP_HEADER_LEN 8

#define TCP_PROT_NUM 6
#define UDP_PROT_NUM 17

struct t_IPv4_header {
    // Version (4 bits) and header length (4 bits)
    uint8_t ip_vhl;
    // Type of Service
    uint8_t ip_tos;
    // Total length
    uint16_t ip_length;
    // Identification
    uint16_t ip_id;
    // Flags and Fragment offset
    uint16_t ip_offset;
    // Time to Live
    uint8_t ip_ttl;
    // Next level protocol
    uint8_t ip_protocol;
    // Header checksum
    uint16_t ip_checksum;
    // Source and destination address
    struct in_addr ip_src, ip_dst;
};

struct t_IPv6_header {
    uint32_t V_TC_FL;
    uint16_t payload_len;
    uint8_t  next_header;
    uint8_t  hop_limit;
    struct in6_addr ip_src;
    struct in6_addr ip_dst;
};

struct t_UDP_header {
    // Source port
    uint16_t src_port;
    // Destination port
    uint16_t dst_port;
    // Total length
    uint16_t udp_length;
    // Checksum
    uint16_t udp_sum;
};

struct t_TCP_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t  data_offset;  // 4 bits
    uint8_t  flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_p;
};
