#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdbool.h>
#include "list.h"

struct client_t {
	int in_fd;
	int out_fd;

	bool active;
	struct list_head node;
};

struct client_t *client_init(int in_fd, int out_fd);
void client_del(struct client_t **client);

#endif
