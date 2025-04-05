#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client.h"

struct client_t *client_init(int in_fd, int out_fd)
{
	struct client_t *client = NULL;

	client = malloc(sizeof(struct client_t));
	if (!client){
		fprintf(stderr, "malloc failure %d\n", errno);
		exit(1);
	}

	client->in_fd = in_fd; 
	client->out_fd = out_fd; 
	client->active = false;

	return client;
}

void client_del(struct client_t **client)
{
	if (!client && !*client)
		return;

	memset(*client, 0, sizeof(struct client_t));

	free(*client);
	*client = NULL;
}
