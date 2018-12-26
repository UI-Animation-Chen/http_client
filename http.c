#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "http.h"

static void set_http_req(char *http_req, const char *method, const char *api,
				  const char *data);

static int make_socket_conn(const char *host_ip, int port);

static void make_http_request(int socket_df, const char *http_req, char *recv_buf);

static int hostname_to_ip(const char *hostname, char *ip);

void http_get(const char *api, const char *params, char *recv_buf)
{
    char http_req[8192];
    memset(http_req, 0, 8192);
    set_http_req(http_req, "GET", api, params);
	int socket_fd = make_socket_conn("172.16.0.139", 5030);
    make_http_request(socket_fd, http_req, recv_buf);
}

void http_post(const char *api, const char *data, char *recv_buf)
{	
	char http_req[8192];
	memset(http_req, 0, 8192);
	set_http_req(http_req, "POST", api, data);
	char ip[17];
	memset(ip, 0, sizeof(ip));
	if (hostname_to_ip("gclass.jd100.com", ip) == 1)
	{
		exit(1);
	}
	int socket_fd = make_socket_conn(ip, 80);
    make_http_request(socket_fd, http_req, recv_buf);
}

static void set_http_req(char *http_req, const char *method, const char *api,
                  const char *data)
{
	// --- request line ---
	strcat(http_req, method);
    strcat(http_req, " ");
    strcat(http_req, api);
    if (data != NULL && (strcmp(method, "GET") == 0))
    {
        strcat(http_req, "/?");
        strcat(http_req, data);
    }
    strcat(http_req, " ");
    strcat(http_req, "HTTP/1.1\r\n");

	// --- header lines ---
	strcat(http_req, "HOST: https://gclass.jd100.com\r\n");
	strcat(http_req, "Content-Type: application/json; charset=UTF-8\r\n");
	if (data != NULL && (strcmp(method, "POST") == 0))
	{
		char len_str[64];
		memset(len_str, 0, sizeof(len_str));
		sprintf(len_str, "%lu", strlen(data));
		strcat(http_req, "Content-Length: ");
		strcat(http_req, len_str);
		strcat(http_req, "\r\n");
	}
	strcat(http_req, "\r\n");

	// --- request body ---
    if (data != NULL && (strcmp(method, "POST") == 0))
    {
        strcat(http_req, data);
    }
    printf("requset info:\n%s\n", http_req);
}

static int hostname_to_ip(const char *hostname, char *ip)
{
	struct hostent *hostent_ptr;
	struct in_addr **addr_ptr_list;
	
	if ((hostent_ptr = gethostbyname(hostname)) == NULL)
	{
		perror("gethostbyname");
		return 1;
	}
	addr_ptr_list = (struct in_addr **)hostent_ptr->h_addr_list;
	for (int i = 0; addr_ptr_list[i] != NULL; i++)
	{
		strcpy(ip, inet_ntoa(*addr_ptr_list[i]));
		printf("\nip:%s\n", ip);
		return 0;
	}
	return 1;
}

static int make_socket_conn(const char *host_ip, int port)
{	
	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		perror("socket err");
		exit(1);
	}
	
	struct sockaddr_in socket_addr;
	memset(&socket_addr, 0, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(port);
	inet_pton(AF_INET, host_ip, &socket_addr.sin_addr);
	
	if (connect(socket_fd, (struct sockaddr *)(&socket_addr),
				sizeof(socket_addr)) < 0)
	{
		close(socket_fd);
		perror("connect error");
		exit(1);
	}
	
	return socket_fd;
}

static void make_http_request(int socket_fd, const char *http_req, char *recv_buf)
{
    int sended_len;
    if ((sended_len = send(socket_fd, http_req, strlen(http_req), 0)) < 0)
    {
        close(socket_fd);
        perror("send err");
        exit(1);
    }
    printf("\nsended len: %d\n", sended_len);
    
    if (recv(socket_fd, recv_buf, 8192, 0) < 0)
    {
        close(socket_fd);
        perror("recv err");
        exit(1);
    }
    close(socket_fd);
}
