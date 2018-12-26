#ifndef HTTP_H
#define HTTP_H

void http_get(const char *url, const char *params, char *recv_buf);

void http_post(const char *url, const char *data, char *recv_buf);

#endif // HTTP_H
