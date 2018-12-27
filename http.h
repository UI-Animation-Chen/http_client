#ifndef HTTP_H
#define HTTP_H

/*
 * return value: 0 success, or failure.
 */
int http_get(const char *api, const char *params, char *recv_buf);

/*
 * return value: 0 success, or failure.
 */
int http_post(const char *api, const char *data, char *recv_buf);

#endif // HTTP_H
