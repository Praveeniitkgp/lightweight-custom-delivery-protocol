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
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define PROTOCOL_NUM 253
#define BUFFER_SIZE 1024
#define HELLO_TYPE 0x01
#define QUERY_TYPE 0x02
#define RESPONSE_TYPE 0x03

struct cldp_header {
    uint8_t msg_type;
    uint8_t payload_len;
    uint16_t transaction_id;
    uint32_t reserved;
};

void send_response(int sock, struct sockaddr_in *client_addr, uint16_t trans_id, char *hostname, struct timeval *tv) {
    char buffer[BUFFER_SIZE];
    struct iphdr *iph = (struct iphdr *)buffer;
    struct cldp_header *hdr = (struct cldp_header *)(buffer + sizeof(struct iphdr));
    char *payload = (char *)(hdr + 1);

    // IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct cldp_header) + strlen(hostname) + sizeof(struct timeval);
    iph->id = htonl(54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = PROTOCOL_NUM;
    iph->check = 0;
    iph->saddr = inet_addr("172.30.233.130"); // Replace with your IP
    iph->daddr = client_addr->sin_addr.s_addr;

    // CLDP Header
    hdr->msg_type = RESPONSE_TYPE;
    hdr->payload_len = strlen(hostname) + sizeof(struct timeval);
    hdr->transaction_id = trans_id;
    hdr->reserved = 0;

    // Payload: hostname + timestamp
    memcpy(payload, hostname, strlen(hostname));
    memcpy(payload + strlen(hostname), tv, sizeof(struct timeval));

    sendto(sock, buffer, iph->tot_len, 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    printf("Sent RESPONSE to %s\n", inet_ntoa(client_addr->sin_addr));
}

int main() {
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    sock = socket(AF_INET, SOCK_RAW, PROTOCOL_NUM);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));

    printf("CLDP Server started, Listening for packets...\n");

    while (1) {
        int len = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (len < 0) continue;

        struct iphdr *iph = (struct iphdr *)buffer;
        if (iph->protocol != PROTOCOL_NUM) continue;

        struct cldp_header *hdr = (struct cldp_header *)(buffer + sizeof(struct iphdr));
        char *payload = (char *)(hdr + 1);

        if (hdr->msg_type == HELLO_TYPE) {
            printf("Received HELLO from %s: HELLO from %s\n", inet_ntoa(client_addr.sin_addr), payload);
        } else if (hdr->msg_type == QUERY_TYPE) {
            printf("Received QUERY from %s: QUERY: hostname, timestamp\n", inet_ntoa(client_addr.sin_addr));
            struct timeval tv;
            gettimeofday(&tv, NULL);
            send_response(sock, &client_addr, hdr->transaction_id, hostname, &tv);
        }
    }

    close(sock);
    return 0;
}