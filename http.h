#ifndef HTTP_H
#define HTTP_H

void http_get(const char *api, const char *params, char *recv_buf);

void http_post(const char *api, const char *data, char *recv_buf);

#endif // HTTP_H
