#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cJSON/cJSON.h"

#include "http.h"

int main(int argc, char *argv[], char *arge[])
{
    char recv_buf[8192];
    memset(recv_buf, 0, sizeof(recv_buf));
	//http_get("/api/getConfig", NULL, recv_buf);
	char data[8192];
	memset(data, 0, sizeof(data));
	strcat(data, "{\"username\":\"zhangyabin\",");
	strcat(data, "\"password\":\"11111\",");
	strcat(data, "\"role\":2,");
	strcat(data, "\"remember\":true}");

	cJSON *obj = cJSON_Parse(data);
	if (obj == NULL)
	{
		printf("parse err ------\n");
		exit(1);
	}

	http_post("/api/login", data, recv_buf);

	cJSON_Delete(obj);

	return 0;
}
