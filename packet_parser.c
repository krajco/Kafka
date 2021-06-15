#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "packet_parser.h"
#include "packet.h"

#define int_len(x) ((x == 0) ? 2 : (int)((ceil(log10(x))+2)*sizeof(char)))

void parse_ipv4_header(struct flow_id *id, const char *buffer) {
    struct t_IPv4_header *ip_header = (struct t_IPv4_header*)(buffer + ETHER_HEADER_LEN);
    inet_ntop(AF_INET, &(ip_header->ip_src), id->src, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_header->ip_dst), id->dst, INET_ADDRSTRLEN);
    id->protocol_num = ip_header->ip_protocol;
}

void parse_transport_header(struct flow_id *id, const char *buffer, int prefix) {
    if(id->protocol_num == TCP_PROT_NUM) {
        parse_tcp_header(id,buffer,prefix);
    }else if(id->protocol_num == UDP_PROT_NUM) {
        parse_udp_header(id,buffer,prefix);
    }
}

void parse_udp_header(struct flow_id *id, const char *buffer, int prefix) {
    struct t_UDP_header *udp_header = (struct t_UDP_header*)(buffer + ETHER_HEADER_LEN + prefix);
    id->src_port = ntohs(udp_header->src_port);
    id->dst_port = ntohs(udp_header->dst_port);
}

void parse_tcp_header(struct flow_id *id, const char *buffer, int prefix) {
    struct t_TCP_header *tcp_header = (struct t_TCP_header*)(buffer + ETHER_HEADER_LEN + prefix);
    id->src_port = ntohs(tcp_header->src_port);
    id->dst_port = ntohs(tcp_header->dst_port);
}

void create_packet_info(struct Flow *flow, const char *buffer, int prefix, int type){
    struct packet_info data;

    set_timestamp(&data);
    set_payload_size(&data,buffer,prefix,type);
    add_info_to_flow(flow,&data);
}

void set_timestamp(struct packet_info *data) {
    time_t seconds = time(NULL);
    data->timestamp = seconds;
}

void set_payload_size(struct packet_info *data, const char* buffer, int prefix, int protocol) {
    if(protocol == UDP_PROT_NUM) {
        struct t_UDP_header *udp_header = (struct t_UDP_header*)(buffer + ETHER_HEADER_LEN + prefix);
        data->payload_size = ntohs(udp_header->udp_length) - UDP_HEADER_LEN;
    }else if(protocol == TCP_PROT_NUM){
        struct t_IPv4_header *ip_header = (struct t_IPv4_header*)(buffer + ETHER_HEADER_LEN);
        struct t_TCP_header *tcp_header = (struct t_TCP_header*)(buffer + ETHER_HEADER_LEN + prefix);
        data->payload_size = ntohs(ip_header->ip_length) - (4 * (tcp_header->data_offset >> 4)) - (4 * (ip_header->ip_vhl & 0xF));
    }
}

struct Flow *init_flow(struct flow_id *id){
    struct Flow *flow = (struct Flow *)malloc(sizeof(struct Flow));
    flow->packets.index = 0;
    flow->packets.isExported = 0;
    flow->id = id;

    return flow;
}

void add_info_to_flow(struct Flow *flow, struct packet_info * data){
    if(flow->packets.index < 20){
        flow->packets.data[flow->packets.index].timestamp = data->timestamp;
        flow->packets.data[flow->packets.index].payload_size = data->payload_size;
        flow->packets.index++;
    }
}

uint8_t is_eq_v4_flow(struct flow_id *flow1, struct flow_id *flow2) {
    if( strcmp(flow1->src,flow2->src) == 0 && strcmp(flow1->dst,flow2->dst) == 0
        && flow1->protocol_num == flow2->protocol_num
        && flow1->src_port == flow2->src_port
        && flow1->dst_port == flow2->dst_port
            )
    {
        return 1;
    }

    if( strcmp(flow1->dst,flow2->src) == 0 && strcmp(flow1->src,flow2->dst) == 0
        && flow1->protocol_num == flow2->protocol_num
        && flow1->src_port == flow2->dst_port
        && flow1->dst_port == flow2->src_port
            )
    {
        return 1;
    }

    return 0;
}

struct flow_id *create_flow_id(const char *buffer, int prefix) {
    struct flow_id *id = (struct flow_id *)malloc(sizeof(struct flow_id));
    parse_ipv4_header(id, buffer);
    parse_transport_header(id,buffer,prefix);

    return id;
}

int get_first_timestamp(struct Flow *flow){
    return flow->packets.data[0].timestamp;
}

void flow_to_str(struct Flow *flow){
    char *rows = packets_to_str(flow);
    int size = strlen(flow->id->src) + strlen(flow->id->dst) + 5 + int_len(flow->id->src_port) + int_len(flow->id->dst_port) +  strlen(rows);

    char *buffer = (char *)malloc(size);
    char port[4];
    if(flow->id->protocol_num == TCP_PROT_NUM){
        strcpy(port,"TCP");
    }else{
        strcpy(port,"UDP");
    }

    snprintf(buffer, size,"%s,%s,%s,%d,%d\n%s",flow->id->src, flow->id->dst, port, flow->id->src_port,flow->id->dst_port,rows);
    printf("%s",buffer);
    free(rows);
    free(buffer);
}

char *packets_to_str(struct Flow * flow){
    int row_size[flow->packets.index];
    int size = 0;

    for (size_t i = 0; i < flow->packets.index; i++) {
        row_size[i] = int_len(flow->packets.data[i].payload_size) + int_len(flow->packets.data[i].timestamp) + 3;
        size += row_size[i];
    }

    char *buffer = (char *)malloc(size + flow->packets.index);
    buffer[0] = '\0';

    char *row;
    for (size_t i = 0; i < flow->packets.index; i++) {
        row = (char *)malloc(row_size[i]);
        snprintf(row,row_size[i], "%d,%ld\n",flow->packets.data[i].payload_size,flow->packets.data[i].timestamp);
        strcat(buffer,row);
        free(row);
    }
    return buffer;
}


struct flow_id *create_flow_id_v6(const char *buffer, int prefix) {
    struct flow_id *id = (struct flow_id *)malloc(sizeof(struct flow_id));
    parse_ipv6_header(id, buffer);
    parse_transport_header(id,buffer,prefix);

    return id;
}

void parse_ipv6_header(struct flow_id *id, const char *buffer) {
    struct t_IPv6_header *ip_header = (struct t_IPv6_header*)(buffer + ETHER_HEADER_LEN);
    inet_ntop(AF_INET6, &(ip_header->ip_src), id->src, INET6_ADDRSTRLEN);
    inet_ntop(AF_INET6, &(ip_header->ip_dst), id->dst, INET6_ADDRSTRLEN);
    id->protocol_num = ip_header->next_header;
}

void create_packet_info_v6(struct Flow *flow, const char *buffer, int prefix, int type){
    struct packet_info data;

    set_timestamp(&data);
    set_payload_size_v6(&data,buffer,prefix,type);
    add_info_to_flow(flow,&data);
}

void set_payload_size_v6(struct packet_info *data, const char* buffer, int prefix, int protocol) {
    if(protocol == UDP_PROT_NUM) {
        struct t_UDP_header *udp_header = (struct t_UDP_header*)(buffer + ETHER_HEADER_LEN + prefix);
        data->payload_size = ntohs(udp_header->udp_length) - UDP_HEADER_LEN;
    }else if(protocol == TCP_PROT_NUM){
        struct t_IPv6_header *ip_header = (struct t_IPv6_header*)(buffer + ETHER_HEADER_LEN);
        struct t_TCP_header *tcp_header = (struct t_TCP_header*)(buffer + ETHER_HEADER_LEN + prefix);
        data->payload_size = ntohs(ip_header->payload_len) - (4 * (tcp_header->data_offset >> 4));
    }
}
