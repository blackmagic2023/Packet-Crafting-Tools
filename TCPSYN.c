#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

// Maximum packet size
#define PACKET_SIZE 4096

// TCP header structure
struct tcpheader {
    u_int16_t source_port;
    u_int16_t dest_port;
    u_int32_t seq_num;
    u_int32_t ack_num;
    u_int8_t data_offset;
    u_int8_t flags;
    u_int16_t window_size;
    u_int16_t checksum;
    u_int16_t urgent_pointer;
};

// Pseudo header structure for checksum calculation
struct pseudo_header {
    u_int32_t source_ip;
    u_int32_t dest_ip;
    u_int8_t reserved;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

// Function to calculate checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Function to craft and send TCP SYN packet
void sendTCPSYN(char *target, int count, char *source_ip, int source_port, int dest_port, const char *data) {
    int sockfd;
    char packet[PACKET_SIZE];
    struct ip *iph = (struct ip *) packet;
    struct tcphdr *tcph = (struct tcphdr *) (packet + sizeof(struct ip));
    struct sockaddr_in dest_addr;
    struct pseudo_header psh;

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    // Zero out the packet buffer
    memset(packet, 0, PACKET_SIZE);

    // Set up destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);
    dest_addr.sin_addr.s_addr = inet_addr(target);

    // Fill in IP Header fields
    iph->ip_hl = 5; // Header length
    iph->ip_v = 4; // IPv4
    iph->ip_tos = 0;
    iph->ip_len = sizeof(struct ip) + sizeof(struct tcphdr) + strlen(data);
    iph->ip_id = htonl(54321); // ID of this packet
    iph->ip_off = 0;
    iph->ip_ttl = 255;
    iph->ip_p = IPPROTO_TCP;
    iph->ip_sum = 0;
    iph->ip_src.s_addr = inet_addr(source_ip); // Source IP
    iph->ip_dst.s_addr = dest_addr.sin_addr.s_addr; // Destination IP

    // Calculate the IP checksum
    iph->ip_sum = checksum((unsigned short *)packet, iph->ip_len);

    // Fill in TCP Header fields
    tcph->source = htons(source_port);
    tcph->dest = htons(dest_port);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5; // TCP header size
    tcph->syn = 1; // SYN flag set
    tcph->window = htons(5840); // Arbitrary window size
    tcph->check = 0;
    tcph->urg_ptr = 0;

    // Pseudo header for checksum calculation
    psh.source_ip = inet_addr(source_ip);
    psh.dest_ip = inet_addr(target);
    psh.reserved = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(data));

    // Copy pseudo header, TCP header, and data to checksum buffer
    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
    char *pseudogram = malloc(psize);
    memcpy(pseudogram, &psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), tcph, sizeof(struct tcphdr) + strlen(data));

    // Calculate TCP checksum
    tcph->check = checksum((unsigned short *)pseudogram, psize);

    // Send packets in a loop
    for (int i = 0; i < count; i++) {
        // Send the crafted packet
        if (sendto(sockfd, packet, iph->ip_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("sendto failed");
            break;
        }
        printf("TCP SYN packet %d sent to %s:%d\n", i + 1, target, dest_port);
    }

    close(sockfd);
    free(pseudogram);
}

// Menu to let users select the type of packet to send
void packetMenu() {
    char target[256];
    int packetCount;
    int source_port, dest_port;
    int option;
    char source_ip[256];
    char data[PACKET_SIZE];

    while (1) {
        printf("\n=== Packet Crafting Tool ===\n");
        printf("1. TCP SYN\n");
        printf("2. Exit\n");
        printf("Select the type of packet to send (1-2): ");
        scanf("%d", &option);

        if (option == 2) {
            printf("Exiting...\n");
            break;
        }

        // Prompt for target, source address, and packet count
        printf("Enter the target host (IP or hostname): ");
        scanf("%s", target);
        printf("Enter the source IP address: ");
        scanf("%s", source_ip);
        printf("Enter the source port: ");
        scanf("%d", &source_port);
        printf("Enter the destination port: ");
        scanf("%d", &dest_port);
        printf("How many packets to send: ");
        scanf("%d", &packetCount);

        // Ask for custom packet data (for TCP SYN in this case)
        printf("Enter custom data for the packet (Example: GET / HTTP/1.1\\r\\nHost: example.com\\r\\n\\r\\n): ");
        scanf("%s", data);

        // Send TCP SYN packets with custom data
        sendTCPSYN(target, packetCount, source_ip, source_port, dest_port, data);
    }
}

int main() {
    // Run the packet menu
    packetMenu();
    return 0;
}
