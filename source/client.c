/**
 * File Transfer Program in C using Socket
 * Implementation of Client
 * Author: Aman Pratap Singh
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8080

bool getFile(int, char*);
void handleClient(int);

int main(int argc , char **argv) {
    // Set stdout as unbuffered.
    setvbuf(stdout, NULL, _IONBF, 0);

    // Detect port and IP from command line argument, if provided.
    int portnum = PORT;
    char *IP = SERVER_IP;
    if (argc >= 2) {
        portnum = atoi(argv[1]);
    }
    if (argc >= 3) {
        IP = argv[2];
    }
    printf("Pinging on PORT %d at %s\n", portnum, IP);

    // Create Socket and start listening on `portnum`.
    int sock_fd = connect_inet_socket(portnum, IP);
    if (sock_fd < 0) {
        perror_die("Connection Failed!");
    }
    else {
        printf("Connected to the client\n");
    }
    handleClient(sock_fd);

	return 0;
}

bool getFile(int sockt, char* request) {
    printf("Creating file: %s\n", request);    

    FILE *received_file;
    received_file = fopen(request, "w");
    if (received_file == NULL) {
            perror_die("Failed to create file!\n");
            return false;
    }

    // Recieve size of the file
    char response[BUFSIZ];
    int file_size = 0;
    int len = recv(sockt, &file_size, sizeof(file_size), 0);
    if (len < 0) {
        perror_die("recv");
    }
    else if (len == 0) {
        // Connection closed by peer.
        return 0;
    }

    // Convert File size from Network byte order to host byte order
    file_size = ntohl(file_size);
    printf("Total Size of the file: %d KB\n\n", file_size/1024);
    int remaining = file_size, recieved = 0;

    // Recieve File
    memset(&response, 0, BUFSIZ);
    while (((len = recv(sockt, response, BUFSIZ, 0)) > 0) && (remaining > 0)) {
        fwrite(response, sizeof(char), len, received_file);{
        remaining -= len;
        recieved += len;
        printf("\rReceived: %6d KB\tRemaining: %6d KB      ", recieved/1024, remaining/1024);}
        fflush(stdout);
    }
    printf("\n");

    fclose(received_file);

    close(sockt);

    printf("\nFile transfer Successful..!\n");

    return true;
}

void handleClient(int sockt) {
    char request[BUFSIZ], response[BUFSIZ];

    // Read available file at Server.
    int len = recv(sockt, response, BUFSIZ, 0);
    if (len < 0) {
        perror_die("recv");
    }
    else if (len == 0) {
        // Connection closed by peer.
        return;
    }

    // Show available files.
    printf("Following files are available at Server:\n");
    printf("\n%s\n", response);

    // Ask for required file.
    printf("Required File: ");
    scanf("%s", request);
    printf("Trying to get file: %s\n", request);

    // Request file from server
    len = send(sockt, request, BUFSIZ, 0);
    if (len < 0) {
        perror_die("send");
    }
    else if (len == 0) {
        // Connection closed by peer.
        return;
    }

    // Read response of server on file request
    memset(&response, 0, BUFSIZ);
    len = recv(sockt, response, BUFSIZ, 0);
    if (len < 0) {
        perror_die("recv");
    }
    else if (len == 0) {
        // Connection closed by peer.
        return;
    }

    if (strcmp(response, "Okay") == 0) {
        getFile(sockt, request);
    }
    else {
        fprintf(stderr, "Error 404: File Not found!\n");
    }

    return;
}