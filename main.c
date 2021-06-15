#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "packet.h"
#include "flow_list.h"

#define TIMER 20

volatile int run = 1;

void sig_handler() {
    run = 0;
}

int main() {
    struct sockaddr saddr;
    struct t_TCP_header *tcp_header;
    struct t_IPv4_header *ip_header;
    struct t_IPv6_header *ipv6_header;
    struct flow_id *id;
    struct List *list;

    list = list_init();
    time_t fd, actual_time, last_clear_time, saddr_size;

    last_clear_time  = time(NULL);
    saddr_size = sizeof(saddr);

    char *buffer = (char *) malloc(65536);

    if (signal(SIGINT, &sig_handler) == SIG_ERR) {
        fprintf(stderr, "Could not set signal handler\n");
        return 1;
    }

    if ((fd = socket(AF_PACKET,SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        perror("Can not create RAW socket");
        exit(1);
    }

    while(run) {
        actual_time = time(NULL);

        if(actual_time - last_clear_time > TIMER){
            last_clear_time = actual_time;
            clear_timers(list,TIMER);
        }

        if((recvfrom(fd, buffer, 65536, 0, &saddr, (socklen_t *) &saddr_size)) < 0){
            continue;
        }

        ip_header = (struct t_IPv4_header*)(buffer + ETHER_HEADER_LEN);

        if(is_ipv4(ip_header)) {
            if(ip_header->ip_protocol == TCP_PROT_NUM || ip_header->ip_protocol == UDP_PROT_NUM){
                int prefix = ipv4_header_prefix((unsigned int)ip_header);
                id = create_flow_id((const char*)buffer,prefix);

                if(!list_find_item(list,id)){
                    struct Flow * flow = init_flow(id);
                    list_insert(list, flow);
                }

                create_packet_info(list->actual->flow,(const char *)buffer,prefix,id->protocol_num);

                if(list->actual->flow->packets.index == 20 && list->actual->flow->packets.isExported == 0){
                    list->actual->flow->packets.isExported = 1;
                    flow_to_str(list->actual->flow);

                    ip_header = NULL;
                    id = NULL;
                    continue;
                }

                tcp_header = (struct t_TCP_header*)(buffer + ETHER_HEADER_LEN + prefix);
                if(id->protocol_num == TCP_PROT_NUM && tcp_header->flags & 0x01){
                    list_remove(list,id);
                }

                ip_header = NULL;
                id = NULL;
            }
        }else if(is_ipv6(ip_header)){
            ipv6_header = (struct t_IPv6_header*)(buffer + ETHER_HEADER_LEN);
            if(ipv6_header->next_header == TCP_PROT_NUM ||  ipv6_header->next_header == UDP_PROT_NUM){
                id = create_flow_id_v6((const char*)buffer,IPV6_HEADER_LEN);

                if(!list_find_item(list,id)){
                    struct Flow *flow = init_flow(id);
                    list_insert(list, flow);
                }

                create_packet_info_v6(list->actual->flow,(const char *)buffer,IPV6_HEADER_LEN,id->protocol_num);

                if(list->actual->flow->packets.index == 20 && list->actual->flow->packets.isExported == 0){
                    list->actual->flow->packets.isExported = 1;
                    flow_to_str(list->actual->flow);
                    ipv6_header = NULL;
                    id = NULL;
                    continue;
                }

                tcp_header = (struct t_TCP_header*)(buffer + ETHER_HEADER_LEN + IPV6_HEADER_LEN);
                if(id->protocol_num == TCP_PROT_NUM && tcp_header->flags & 0x01){
                    list_remove(list,id);
                }
            }
        }
    }

    free(buffer);
    dispose_list(list);
    close(fd);
    return 0;
}
