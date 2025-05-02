// Assignment 7 Submission
// Name: Praveen Kumar
// Roll number: 22CS10054

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define PROTOCOL_NUM 253
#define BUFFER_SIZE 1024
#define HELLO_TYPE 0x01
#define QUERY_TYPE 0x02
#define BROADCAST_IP "255.255.255.255"

struct cldp_header {
    uint8_t msg_type;
    uint8_t payload_len;
    uint16_t transaction_id;
    uint32_t reserved;
};

void send_packet(int sock, const char *ip, uint8_t msg_type, const char *payload, uint16_t trans_id) {
    char buffer[BUFFER_SIZE];
    struct iphdr *iph = (struct iphdr *)buffer;
    struct cldp_header *hdr = (struct cldp_header *)(buffer + sizeof(struct iphdr));
    char *data = (char *)(hdr + 1);

    // IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct cldp_header) + (payload ? strlen(payload) : 0);
    iph->id = htonl(54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = PROTOCOL_NUM;
    iph->check = 0;
    iph->saddr = inet_addr("172.30.233.130"); // Replace with your IP
    iph->daddr = inet_addr(ip);

    // CLDP Header
    hdr->msg_type = msg_type;
    hdr->payload_len = payload ? strlen(payload) : 0;
    hdr->transaction_id = trans_id;
    hdr->reserved = 0;

    if (payload) memcpy(data, payload, strlen(payload));

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = iph->daddr;

    sendto(sock, buffer, iph->tot_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (msg_type == HELLO_TYPE) {
        printf("Sending HELLO: HELLO from %s\n", payload);
    } else if (msg_type == QUERY_TYPE) {
        printf("Sending QUERY: QUERY: hostname, timestamp\n");
    }
}

int main() {
    int sock;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    sock = socket(AF_INET, SOCK_RAW, PROTOCOL_NUM);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

    printf("CLDP Client starting...\n");

    uint16_t trans_id = 1;
    while (1) {
        send_packet(sock, BROADCAST_IP, HELLO_TYPE, hostname, trans_id);
        send_packet(sock, BROADCAST_IP, QUERY_TYPE, NULL, trans_id++);
        printf("Waiting 10 seconds before next cycle...\n");
        printf("\n");
        sleep(10);
    }

    close(sock);
    return 0;
}