/**
 * File Transfer Program in C using Socket
 * Implementation of Server
 * Author: Aman Pratap Singh
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include "utils.h"

#define PORT 8080

void* clientHandler(void* sockt);
bool sendFiletoClient(int sockt, char* file_name);
void ls(char *);

int main(int argc, char **argv) {
    // Set stdout as unbuffered.
    setvbuf(stdout, NULL, _IONBF, 0);

    // Detect port from command line argument, if provided.
    int portnum = PORT;
    if (argc >= 2) {
        portnum = atoi(argv[1]);
    }
    printf("Serving on PORT %d\n", portnum);

    // Create Socket and start listening on `portnum`.
    int sock_fd = listen_inet_socket(portnum);
    
	while (true) {
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);

        int peer_sock_fd = accept(sock_fd, (struct sockaddr *)&peer_addr, &peer_addr_len);

        if (peer_sock_fd < 0) {
            printf("Error while accepting connection\n");
        }

        report_peer_connected(&peer_addr, peer_addr_len);

		pthread_t peer_thread;

        // Create thread for serving Peer.
		pthread_create(&peer_thread, NULL,  clientHandler, (void*)&peer_sock_fd);

        // Detach the thread - when it's done, its resources will be cleaned up.
        // Since the main thread lives forever, it will outlive the serving threads.
		pthread_detach(peer_thread);
	}

	return 0;
}

void ls(char* msg) {
    // List all files available in directory.
    memset(msg, 0, BUFSIZ);
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            // Remove current & previous directory and file without extension from list.
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 
                && strchr(ent->d_name, '.')) {
                strcat(msg, ent->d_name);
                strcat(msg, "\n");
            }
        }
        closedir (dir);
    }
    else {
        perror_die("DIR: Unable to access files.");
    }
}

void* clientHandler(void *sockt) {
    unsigned long tid = (unsigned long)pthread_self();
	int	sock_peer = *(int*) sockt;

    printf("Thread %lu created to handle connection with socket %d\n", tid,
        sock_peer);

	char response[BUFSIZ], request[BUFSIZ], available_files[BUFSIZ];
    
    // Store all available files in available_files;
    ls(available_files);

    // Send available files to Client
    int len = send(sock_peer, available_files, BUFSIZ, 0);
    if (len < 0) {
        perror_die("send");
    }
    else if (len == 0) {
        // Connection closed by peer.
        return 0;
    }

    // Read File asked by client.
	len = recv(sock_peer, request, BUFSIZ, 0);
    if (len < 0) {
        perror_die("recv");
    }
    else if (len == 0) {
        // Connection closed by peer.
        return 0;
    }

    printf("Client Asked: %s\n", request);

    // Try to access File asked by Client and send confirmation accordingly.
	if (access(request, F_OK) != -1) {
		strcpy(response, "Okay");
		len = send(sock_peer, response, 4, 0);
        if (len < 0) {
            perror_die("send");
        }
        else if (len == 0) {
            // Connection closed by peer.
            return 0;
        }

        // Send actual file to Client.
		if (sendFiletoClient(sock_peer, request)) {
            printf("Thread %lu succesfully sent %s to Client %d\n\n", tid, request, sock_peer);
        }
        else {
            fprintf(stderr, "Error encountered while sending file.\n");
            return 0;
        }
	}
	else {
		strcpy(response, "Nope");
		len = send(sock_peer, response, 4, 0);
        if (len < 0) {
            perror_die("send");
        }
        else if (len == 0) {
            // Connection closed by peer.
            return 0;
        }
        return 0;
	}

	return 0;
}

bool sendFiletoClient(int sockt, char* file_name) {
    // Open File
    int fp = open(file_name, O_RDONLY);
    if (fp == -1) {
        perror_die("Unable to open file");
        return false;
    }

    // Get file stats
    struct stat file_stat;
    if (fstat(fp, &file_stat) < 0) {
        perror_die("Unable to fetch status of file");
        return false;
    }
    int file_size = file_stat.st_size;
    printf("File retrieved succesfully. File Size: %d KB\n", file_size/1024);

    // Send File Size to client
    int converted_file_size = htonl(file_size);
    int len = send(sockt, &converted_file_size, sizeof(converted_file_size), 0);
    if (len < 0) {
        perror_die("send");
    }
    else if (len == 0) {
        // Connection closed by peer.
        return 0;
    }

    off_t offset = 0, sent_bytes = 0, remaining = file_stat.st_size;

    // Start sending file
    while (((sent_bytes = sendfile(fp, sockt, 0, &offset, NULL, 0)) > 0) && (remaining > 0)) {
        remaining -= sent_bytes;
        printf("\rBytes Sent: %lld Offset: %lld Remaining: %lld", sent_bytes, offset, remaining);
    }

    close(fp);
    close(sockt);

	return true;
}