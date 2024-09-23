# Packet-Crafting-Tools

This repository contains a collection of C programs designed for crafting and sending various types of DoS (Denial of Service) attack packets, along with other exploit packet types. These tools allow the user to manually customize packet headers and data payloads, providing useful information for penetration testing and network security analysis.

Note: These tools are strictly for educational purposes and authorized penetration testing. Misuse of these tools can result in serious legal consequences.

# Tools Overview

- ExploitPacket.c
Description: Allows the user to craft and send exploit packets with custom data. The user can input header data, source addresses, and craft the payload manually. This tool is intended for testing vulnerabilities in network applications.
Usage: Craft custom exploit packets, inputting header data and payload.

- HttpFlood.c
Description: A tool for sending large amounts of HTTP requests to a target server, simulating a DoS attack via HTTP flood. The user can customize the HTTP request data and set the number of packets to send.
Usage: Send HTTP flood packets to overwhelm the target's HTTP service.

- MalwarePacket.c
Description: This tool enables the user to craft packets that mimic malware traffic. It allows customization of both the header and payload, enabling users to test defenses against malicious traffic.
Usage: Manually create malware-like traffic to test network defenses.

- PingOfDeath.c
Description: Generates a Ping of Death attack by sending oversized ICMP packets, potentially causing target systems to crash or malfunction. The user can set the payload size and data.
Usage: Send malformed ICMP packets (oversized) to test systems for Ping of Death vulnerability.

- TCPSYN.c
Description: A tool for crafting and sending TCP SYN packets to initiate a TCP SYN flood attack, overwhelming the target’s connection queue. The user can customize the TCP header and payload.
Usage: Send SYN flood packets to flood the target’s connection queue.

- TCPSYNAttackSpoofed.c
Description: Similar to `TCPSYN.c`, but with the added feature of source address spoofing. The user can input a custom source IP address to obscure the true origin of the traffic.
Usage: Send spoofed SYN packets to conduct a SYN flood attack with a custom source IP.

- UDPFlood.c
Description: A tool for launching a UDP flood attack by sending large numbers of UDP packets to a target server. The user can input a payload and configure the header for each packet.
Usage: Flood the target with UDP packets to overwhelm its network bandwidth.

- VolumetricAttackPackets.c
Description: Sends volumetric DoS packets, which are designed to consume the target’s bandwidth or processing power. This tool allows the user to customize the data payload for the attack.
Usage: Conduct a volumetric DoS attack with customizable packet data.

# Compilation

Each of these tools can be compiled using the `gcc` compiler. For example:

```
gcc -o ExploitPacket ExploitPacket.c
gcc -o HttpFlood HttpFlood.c
gcc -o MalwarePacket MalwarePacket.c
gcc -o PingOfDeath PingOfDeath.c
gcc -o TCPSYN TCPSYN.c
gcc -o TCPSYNAttackSpoofed TCPSYNAttackSpoofed.c
gcc -o UDPFlood UDPFlood.c
gcc -o VolumetricAttackPackets VolumetricAttackPackets.c

```

# Usage

All tools follow a similar usage pattern:

Run the compiled program:
```
sudo ./ToolName
```
Make sure to run the tools with root privileges as they require raw socket access.

- Input data: Follow the on-screen prompts to input the packet data, header information, and the number of packets you wish to send.

- Send the packets: After crafting the packet, the tool will send it to the target, and any response from the target will be displayed on the console.

# Legal Disclaimer

These tools are provided for educational purposes and authorized penetration testing only. Unauthorized use of these tools may violate applicable laws. The author assumes no responsibility for any misuse or damage caused by these tools.
