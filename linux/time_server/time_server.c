#include "headers.h"
#include <time.h>

#define PORT "8080"
#define FALSE 0
#define BUFFER_LENGTH 1024

int main()
{
    int sd=-1, sd2=-1;
    int rc, length, on=1;
    char buffer[BUFFER_LENGTH];
    struct pollfd fds;
    nfds_t nfds = 1;
    int timeout;
    struct addrinfo hints, *serveraddr;

    do
    {
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET6;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        getaddrinfo(0, PORT, &hints, &serveraddr);

#ifdef TEST_SOCKET_FAIL
        sd = -1;
#else
        sd = socket(serveraddr->ai_family,
                serveraddr->ai_socktype, serveraddr->ai_protocol);
#endif
        if (sd < 0)
        {
            perror("socket() failed");
            freeaddrinfo(serveraddr);
            break;
        }

#ifdef TEST_SETSOCKOPT_FAIL
        rc = -1;
#else
        rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
                (char *)&on, sizeof(on));
#endif
        if (rc < 0)
        {
            perror("setsockopt(SO_REUSEADDR) failed");
            freeaddrinfo(serveraddr);
            break;
        }

#ifdef  TEST_BIND_FAIL
        rc = -1
#else
        rc = bind(sd, serveraddr->ai_addr, serveraddr->ai_addrlen);
#endif
        if (rc < 0)
        {
            perror("bind() failed");
            freeaddrinfo(serveraddr);
            break;
        }
        freeaddrinfo(serveraddr);

#ifdef TEST_LISTEN_FAIL
        rc = -1
#else
            rc = listen(sd, 10);
#endif
        if (rc < 0)
        {
            perror("listen() failed");
            break;
        }


        printf("Ready for client connect().\n");

#ifdef TEST_ACCEPT_FAIL
        sd2 = -1
#else
        sd2 = accept(sd, NULL, NULL);
#endif
        if (sd2 < 0)
        {
            perror("accept() failed");
            break;
        }

        timeout = 30000;

        memset(&fds, 0, sizeof(fds));
        fds.fd = sd2;
        fds.events = POLLIN;
        fds.revents = 0;

#ifdef TEST_POLL_FAIL
        rc = -1;
#elif TEST_POLL_TIMEOUT
        rc = 0;
#else
        rc = poll(&fds, nfds, timeout);
#endif
        if (rc < 0)
        {
            perror("poll() failed");
            break;
        }

        if (rc == 0)
        {
            printf("poll() timed out.\n");
            break;
        }

#ifdef TEST_RECV_FAIL
        rc = -1;
#else
        rc = recv(sd2, buffer, BUFFER_LENGTH, 0);
#endif
        if (rc < 0)
        {
            perror("recv() failed");
            break;
        }
        printf("Received %d bytes.\n", rc);

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Connection: close\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Local time is: ";
        rc = send(sd2, response, strlen(response), 0);
        printf("Sent %d of %d bytes.\n", rc, (int)strlen(response));

        time_t timer;
        time(&timer);
        char *time_msg = ctime(&timer);
        rc = send(sd2, time_msg, strlen(time_msg), 0);
        printf("Sent %d of %d bytes.\n", rc, (int)strlen(time_msg));
    } while (FALSE);

    if (sd != -1)
        CLOSESOCKET(sd);
    if (sd2 != -1)
        CLOSESOCKET(sd2);

    return 0;
}
