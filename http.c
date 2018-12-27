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

#define HOST_IP_STRING "172.16.0.139"
#define PORT 5030

static void set_http_req(char *http_req, const char *method, const char *api,
						 const char *data);

static int make_socket_conn(const char *host_ip, int port);

static int socket_send(int socket_fd, const char *http_req, char *recv_buf);

static int make_http_request(const char *method, const char *params,
							 const char *http_req, char *recv_buf);

static int hostname_to_ip(const char *hostname, char *ip);

int http_get(const char *api, const char *params, char *recv_buf)
{
	return make_http_request("GET", api, params, recv_buf);
}

int http_post(const char *api, const char *data, char *recv_buf)
{	
	return make_http_request("POST", api, data, recv_buf);
}

static int make_http_request(const char *method, const char *api,
							 const char *params, char *recv_buf)
{
	char http_req[8192];
	memset(http_req, 0, 8192);
    set_http_req(http_req, method, api, params);

	char ip[17];
	memset(ip, 0, sizeof(ip));
	if (hostname_to_ip("www.baidu.com", ip) == 1)
	{
		return 1;
	}

	// no ssl implementation :(
	int socket_fd = make_socket_conn(HOST_IP_STRING, PORT);
	if (socket_fd < 0)
	{
		return 1;
	}
 
	if (socket_send(socket_fd, http_req, recv_buf) != 0)
	{
		return 1;
	}

	return 0;
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
	strcat(http_req, "HOST: baidu.com\r\n");
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
	int socket_fd = -1;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		perror("socket err");
		return -1;
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
		return -1;
	}
	
	return socket_fd;
}

static int socket_send(int socket_fd, const char *http_req, char *recv_buf)
{
    int sended_len;
    if ((sended_len = send(socket_fd, http_req, strlen(http_req), 0)) < 0)
    {
        close(socket_fd);
        perror("send err");
        return 1;
    }
    printf("\nsended len: %d\n", sended_len);
    
    if (recv(socket_fd, recv_buf, 8192, 0) < 0)
    {
        close(socket_fd);
        perror("recv err");
        return 1;
    }

    close(socket_fd);
	return 0;
}

