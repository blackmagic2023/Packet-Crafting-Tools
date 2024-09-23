#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

// TCP header structure
struct tcpheader {
    unsigned short source_port;
    unsigned short dest_port;
    unsigned int seq;
    unsigned int ack_seq;
    unsigned short data_offset_res_flags;
    unsigned short window;
    unsigned short checksum;
    unsigned short urg_ptr;
};

// Function to create TCP packet
void createTCPPacket(char *source_ip, char *target_ip, int src_port, int dst_port, char *data) {
    char packet[4096];
    memset(packet, 0, 4096);

    struct iphdr *iph = (struct iphdr *) packet;
    struct tcpheader *tcph = (struct tcpheader *) (packet + sizeof(struct iphdr));

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(dst_port);
    sin.sin_addr.s_addr = inet_addr(target_ip);

    // Create IP header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcpheader) + strlen(data);
    iph->id = htonl(rand());
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->saddr = inet_addr(source_ip);
    iph->daddr = sin.sin_addr.s_addr;
    iph->check = 0; // Kernel will fill the checksum

    // Create TCP header
    tcph->source_port = htons(src_port);
    tcph->dest_port = htons(dst_port);
    tcph->seq = rand();
    tcph->ack_seq = 0;
    tcph->data_offset_res_flags = htons((5 << 12) | 0x02); // SYN flag set
    tcph->window = htons(5840);
    tcph->checksum = 0;
    tcph->urg_ptr = 0;

    // Send the packet
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    int one = 1;
    const int *val = &one;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("Error setting IP_HDRINCL");
        exit(1);
    }

    // Send packet loop
    printf("Sending crafted TCP packets...\n");
    if (sendto(sockfd, packet, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("Packet send failed");
    } else {
        printf("Packet sent to %s:%d\n", target_ip, dst_port);
    }

    close(sockfd);
}

// Main menu function
void showMenu() {
    int choice, count;
    char source_ip[20], target_ip[20], payload[100];
    int src_port, dst_port;

    while (1) {
        printf("\n*** Packet Crafting Tool ***\n");
        printf("1. Send TCP SYN Packet\n");
        printf("2. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter source IP: ");
                scanf("%s", source_ip);
                printf("Enter target IP: ");
                scanf("%s", target_ip);
                printf("Enter source port: ");
                scanf("%d", &src_port);
                printf("Enter destination port: ");
                scanf("%d", &dst_port);
                printf("Enter custom payload (Example: 'exploit-data'): ");
                scanf("%s", payload);
                printf("How many packets to send: ");
                scanf("%d", &count);

                for (int i = 0; i < count; i++) {
                    createTCPPacket(source_ip, target_ip, src_port, dst_port, payload);
                }
                break;

            case 2:
                exit(0);
            default:
                printf("Invalid choice, try again.\n");
        }
    }
}

int main() {
    // Display the packet crafting menu
    showMenu();
    return 0;
}
