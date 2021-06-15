#include <netinet/in.h>

struct packet_info {
    int  payload_size;
    time_t timestamp;
};

struct flow_id {
    char dst[INET6_ADDRSTRLEN];
    char src[INET6_ADDRSTRLEN];
    int  protocol_num;
    uint16_t src_port;
    uint16_t dst_port;
};

struct packet_info_buffer {
    uint8_t isExported;
    uint8_t index;
    struct packet_info data[20];
};
struct Flow{
    struct flow_id *id;
    struct packet_info_buffer packets;
};

#define is_ipv4(ip_header) (( (ip_header->ip_vhl >> 4 ) == 4) ? 1 : 0)
#define is_ipv6(ip_header) (( (ip_header->ip_vhl >> 4 ) == 6) ? 1 : 0)

#define ipv4_header_prefix(ip_header) ((ip_header->ip_vhl & 0xF) * 4)

struct Flow *init_flow(struct flow_id *id);

void add_info_to_flow(struct Flow *flow, struct packet_info * data);

void parse_ipv4_header(struct flow_id *id, const char *buffer);

void parse_transport_header(struct flow_id *id, const char *buffer, int prefix);

void parse_udp_header(struct flow_id *id, const char *buffer, int prefix);

void parse_tcp_header(struct flow_id *id, const char *buffer, int prefix);

void set_timestamp(struct packet_info *data);

void set_payload_size(struct packet_info *data, const char* buffer, int prefix, int protocol);

void create_packet_info(struct Flow *flow, const char *buffer, int prefix, int type);

struct flow_id *create_flow_id(const char *buffer, int prefix);

uint8_t is_eq_v4_flow(struct flow_id *flow1, struct flow_id *flow2);

int get_first_timestamp(struct Flow *flow);

void flow_to_str(struct Flow *flow);

char *packets_to_str(struct Flow * flow);

struct flow_id *create_flow_id_v6(const char *buffer, int prefix);

void parse_ipv6_header(struct flow_id *id, const char *buffer);

void create_packet_info_v6(struct Flow *flow, const char *buffer, int prefix, int type);

void set_payload_size_v6(struct packet_info *data, const char* buffer, int prefix, int protocol);
