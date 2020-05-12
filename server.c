#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "initial.h"

char webPageFirst[] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>webServer</title></head>\r\n"
"<body><center><h3>Welcome to the 8787 server</h3><br>\r\n";

char webPageSecond[] =
"</center></body></html>\r\n";

int create_socket(int port)
{
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
	    perror("Unable to create socket");
	    exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0)
    {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);   // 把 STDOUT buffer 拿掉
    int sock;
    SSL_CTX *ctx;

    char * _CERT;
    char * _KEY;

    pid_t cpid;
    switch (argc)
    {
    case 1:
        _CERT = HOST_CERT;
        _KEY =  HOST_KEY;
        break;
    case 2:
        _CERT = (strcmp(argv[1], "wrong") == 0) ? WRONG_CERT : HOST_CERT;
        _KEY = (strcmp(argv[1], "wrong") == 0) ? WRONG_KEY : HOST_KEY;
        break;
    default:
        fprintf(stderr, "wrong argument number\n");
        exit(EXIT_FAILURE);
        break;
    }

    // 初始化 openssl
    SSL_library_init();
    ctx = create_context(0);
    configure_context(ctx, _CERT, _KEY);
    sock = create_socket(8787);

    /* Handle connections */
    while(1) 
    {
        struct sockaddr_in addr;
        uint len = sizeof(addr);
        SSL *ssl;
        char *reply;
        char receive[1024];
        int count;
        FILE *fp;

        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0)
        {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);    // 配對 SSL 跟新的連線 fd
        SSL_set_verify_depth(ssl, 1);


        // SSL_accept() 處理 TSL handshake
        int acc = SSL_accept(ssl);
        if (acc <= 0)
        {
            printf("acc err: %d\n", acc);
            int err_SSL_get_error = SSL_get_error(ssl, acc);
                if (SSL_get_verify_result(ssl) != X509_V_OK)
                {
                    printf("Client certificate verify error\n");
                    printf("Connection close\n");
                }
                
                if (err_SSL_get_error == SSL_ERROR_WANT_READ)
                {
                    /* Wait for data to be read */
                    printf("SSL_ERROR_WANT_READ\n");
                    printf("%d %s\n", err_SSL_get_error, ERR_error_string(err_SSL_get_error, NULL));
                }
                else if (err_SSL_get_error == SSL_ERROR_WANT_WRITE)
                {
                    /* Write data to continue */
                    printf("SSL_ERROR_WANT_WRITE\n");
                    printf("%d %s\n", err_SSL_get_error, ERR_error_string(err_SSL_get_error, NULL));
                }
                else if (err_SSL_get_error == SSL_ERROR_SYSCALL)
                {
                    /* Hard error */
                    printf("SSL_ERROR_SYSCALL\n");
                    printf("%d %s\n", err_SSL_get_error, ERR_error_string(err_SSL_get_error, NULL));
                    perror("errno");
                    // exit(-1);
                }
                else if (err_SSL_get_error == err_SSL_get_error == SSL_ERROR_SSL)
                {
                    printf("SSL_ERROR_SSL\n");
                    printf("%d %s\n", err_SSL_get_error, ERR_error_string(err_SSL_get_error, NULL));
                    perror("errno");
                    // exit(-1);
                }
                else if (err_SSL_get_error == SSL_ERROR_ZERO_RETURN)
                {
                    /* Same as error */
                    printf("SSL_ERROR_ZERO_RETURN\n");
                    printf("%d %s\n", err_SSL_get_error, ERR_error_string(err_SSL_get_error, NULL));
                    perror("errno");
                    // exit(-1);
                }
        }
        else
        {
            printf("get connect!!\n");
            ShowCerts(ssl, 0);        /* get any certificates */

            count = SSL_read(ssl, receive, sizeof(receive));
            receive[count] = 0;
            printf("Received from client:\n");
            printf("%s\n\n", receive);
            if (0)  // strcmp(receive, "GET")
            {

            }
            else
            {
                SSL_write(ssl, webPageFirst, sizeof(webPageFirst));
                if ((fp = popen("ls | cat", "r")) == NULL)
                {
                    perror("open failed!");
                    return -1;
                }
                char buf[256];
                char linkFirst[] = "<a href='?f=";
                char linkSecond[] = "'>";
                char linkThird[] = "</a><br>";
                while (fgets(buf, 255, fp) != NULL)
                {
                    SSL_write(ssl, "<a href='?f=", strlen("<a href='?f="));
                    SSL_write(ssl, buf, strlen(buf));
                    SSL_write(ssl, "'>", strlen("'>"));
                    SSL_write(ssl, buf, strlen(buf));
                    SSL_write(ssl, "</a><br>", strlen("</a><br>"));
                }
                printf("ls done\n");
                if (pclose(fp) == -1)
                {
                    perror("close failed!");
                    return -2;
                }
                
            }
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client);
    }

    close(sock);
    SSL_CTX_free(ctx);
}