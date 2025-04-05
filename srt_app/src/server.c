#include <srt/srt.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "client.h"

#define PORT		"8890"
#define OUT_PORT	"8891"
#define BUFFER_SZ	1316
#define BACKLOG_COUNT	5
#define MAX_CLIENTS	10

#define BUFFER_SIZE 5   // Number of buffers in the pool
#define PACKET_SIZE 188 // Size of each buffer (adjust for SRT packet size)

struct server_config_t {
	struct addrinfo *servinfo;
	SRTSOCKET sck;
	size_t free_client_spot;
	int epoll_id; 	
	struct list_head clients_list;
};

struct server_config_t g_server_config = {
	.servinfo = NULL,
	.sck = -1,
	.epoll_id = -1,
	.free_client_spot = 0,
	.clients_list = LIST_INIT(g_server_config.clients_list), 
};

typedef struct {
	char data[PACKET_SIZE];
	int size;  // Actual data size in the packet
} Packet;

typedef struct {
	Packet packets[BUFFER_SIZE];
	int head;  // Points to the newest packet
	int tail;  // Points to the oldest packet
	int count; // Number of packets in the buffer
	pthread_mutex_t lock;
} CircularBuffer;

void init_buffer(CircularBuffer *cb) {
	cb->head = 0;
	cb->tail = 0;
	cb->count = 0;
	pthread_mutex_init(&cb->lock, NULL);
}

int add_packet(CircularBuffer *cb,int sck) {
	pthread_mutex_lock(&cb->lock);

	// Store the new packet
	int ret = srt_recv(sck, cb->packets[cb->head].data, BUFFER_SZ);
		if (ret <= 0) {
			pthread_mutex_unlock(&cb->lock);
			return -1;
		}

	cb->packets[cb->head].size = ret;

	// Move head forward
	cb->head = (cb->head + 1) % BUFFER_SIZE;

	// If buffer is full, move tail (overwrite oldest)
	if (cb->count == BUFFER_SIZE) {
		cb->tail = (cb->tail + 1) % BUFFER_SIZE;
	} else {
		cb->count++;
	}

	pthread_mutex_unlock(&cb->lock);

	return 0;
}

int get_packet(CircularBuffer *cb, int out_sck) {
	pthread_mutex_lock(&cb->lock);

	if (cb->count == 0) {  // No packets available
		pthread_mutex_unlock(&cb->lock);
		return -1;
	}

	// Copy the oldest packet
	int size = cb->packets[cb->tail].size;
	srt_send(out_sck, cb->packets[cb->tail].data, size);

	// Move tail forward
	cb->tail = (cb->tail + 1) % BUFFER_SIZE;
	cb->count--;

	pthread_mutex_unlock(&cb->lock);
	return size;
}

int srt_listen_cb(void* opaque, SRTSOCKET ns, int hs_version,
			const struct sockaddr* peeraddr, const char* streamid) {
	fprintf(stdout, "client connected %s:%d\n",
			inet_ntoa(((struct sockaddr_in*)peeraddr)->sin_addr),
			ntohs(((struct sockaddr_in*)peeraddr)->sin_port));

	if (streamid) {
		fprintf(stdout, "recv stream id :%s\n", streamid);
		return 0;
	}

	return -1;
}

void handle_force_kill(int signum, siginfo_t *info, void *ctx)
{
	fprintf(stdout, "recived signal %d\n", signum);

	//TODO: Handle closing clients sockets

	if(g_server_config.servinfo)
		freeaddrinfo(g_server_config.servinfo);

	if (g_server_config.sck != -1
		&& srt_close(g_server_config.sck) == SRT_ERROR)
		fprintf(stderr, "faild to srt_close errno %d\n", errno);

	if (g_server_config.epoll_id != -1)
		srt_epoll_release(g_server_config.epoll_id);

	g_server_config.epoll_id = -1;

	srt_cleanup();
	_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	SRTSOCKET *sck, out_sck;
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo, *out_servinfo;
	struct sockaddr_storage client_addr;
	int addr_sz;
	SRTSOCKET client_sck = -1;
	SRTSOCKET out_client_sck = -1;
	struct sigaction sa;

	int *epoll_id;
	SRTSOCKET ready_scks[MAX_CLIENTS + 1] = {0};
	int num_scks = MAX_CLIENTS + 1;

	const int in_events = SRT_EPOLL_IN;
	const int out_events = SRT_EPOLL_IN;
	int ret = 0;

	sck = &g_server_config.sck;
	epoll_id = &g_server_config.epoll_id;

	sa.sa_sigaction = &handle_force_kill;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);


	FILE *fp = fopen("output.ts", "wb");
	if (!fp) {
		perror("File open failed");
		return 1;
	}

	if (srt_startup() < 0) {
		fprintf(stderr, "srt_startup errno %d\n", errno);
		exit(1);
	}

	/* in server socket */
	if ((*sck = srt_create_socket()) == SRT_INVALID_SOCK) {
		fprintf(stderr, "srt_create_socket errno %d\n", errno);
		goto SRT_CLEAN;
	}

	/* out server socket */
	if ((out_sck = srt_create_socket()) == SRT_INVALID_SOCK) {
		fprintf(stderr, "srt_create_socket errno %d\n", errno);
		goto SRT_CLEAN;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	/* in sever */
	if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error:%s\n", gai_strerror(status));
		goto SRV_SCK_CLOSE;
	}

	g_server_config.servinfo = servinfo;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	/* out server */
	if ((status = getaddrinfo(NULL, OUT_PORT, &hints, &out_servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error:%s\n", gai_strerror(status));
		goto SRV_SCK_CLOSE;
	}

	/* in sever bind */
	if ((status = srt_bind(*sck, servinfo->ai_addr, servinfo->ai_addrlen)) != 0) {
		fprintf(stderr, "srt_bind error:%d\n", errno);
		goto SRV_INFO_CLEAN;
	}

	/* out server bind */
	if ((status = srt_bind(out_sck, out_servinfo->ai_addr, out_servinfo->ai_addrlen)) != 0) {
		fprintf(stderr, "srt_bind error:%d\n", errno);
		goto SRV_INFO_CLEAN;
	}

	/* in sever list */
	/* set listern callback to check proper stream id */
	srt_listen_callback(*sck, srt_listen_cb, NULL);

	if ((srt_listen(*sck, BACKLOG_COUNT)) != 0) {
		fprintf(stderr, "srt_listen error:%d\n", errno);
		goto SRV_INFO_CLEAN;
	}

	/* out sever listen */
	if ((srt_listen(out_sck, BACKLOG_COUNT)) != 0) {
		fprintf(stderr, "srt_listen error:%d\n", errno);
		goto SRV_INFO_CLEAN;
	}


	if ((*epoll_id = srt_epoll_create()) < 0) {
		fprintf(stderr, "srt_epoll_create error:%d\n", errno);
		goto SRV_INFO_CLEAN;
	}
	
	/* subscribe to all events for sck */
	if (srt_epoll_add_usock(*epoll_id, *sck, &in_events) < 0) {
		fprintf(stderr, "srt_epoll_add_usock error:%d\n", errno);
		//TODO: RELEAE EPOLL_ID
		goto SRV_INFO_CLEAN;
	}

	if (srt_epoll_add_usock(*epoll_id, out_sck, &out_events) < 0) {
		fprintf(stderr, "srt_epoll_add_usock error:%d\n", errno);
		//TODO: RELEAE EPOLL_ID
		goto SRV_INFO_CLEAN;
	}

	CircularBuffer buffer;
	init_buffer(&buffer);
	int rcv_latency = 50;

	while (1) {
		memset(&ready_scks, 0, sizeof(ready_scks));
		status = srt_epoll_wait(*epoll_id, ready_scks, &num_scks, 0, 0, -1, 0, 0, 0, 0);

		if (status <= 0)
			continue;

		/* status has no of ready fds */
		for (int i = 0; i < status; i++) {
			/* check if server sock recived anything */
			if (ready_scks[i] == *sck) {
				addr_sz = sizeof client_addr;
				client_sck = srt_accept(*sck, (struct sockaddr *)&client_addr, &addr_sz);
				fprintf(stdout, "New client connected %d\n", client_sck);

				if (client_sck > 0) {
					struct client_t *client = client_init(client_sck, client_sck);  
					list_add(&g_server_config.clients_list, &client->node);
					srt_epoll_add_usock(*epoll_id, client_sck, &in_events);
				}
			} else if (ready_scks[i] == out_sck) {
				addr_sz = sizeof client_addr;
				out_client_sck = srt_accept(out_sck, (struct sockaddr *)&client_addr, &addr_sz);
				fprintf(stdout, "New client connected %d\n", out_client_sck);

				if (client_sck > 0) {
					struct client_t *client = client_init(out_client_sck, client_sck);  
					list_add(&g_server_config.clients_list, &client->node);
					srt_epoll_add_usock(*epoll_id, out_client_sck, &in_events);
					srt_setsockopt(out_client_sck, 0, SRTO_RCVLATENCY, &rcv_latency, sizeof(rcv_latency));
				}

			} else if (ready_scks[i] > 0) {
				//TODO: Handle recived data
				ret = add_packet(&buffer, ready_scks[i]);
				if (ret < 0) {
					fprintf(stdout, "%d client dis-connected \n",ready_scks[i]);
					struct client_t *c, *nxt;
					list_for_each_entry_safe(c, nxt, &g_server_config.clients_list, node) {
						if (c->in_fd == ready_scks[i]) {
							list_del(&c->node);

							client_del(&c);
							srt_epoll_remove_usock(*epoll_id, ready_scks[i]);
							if (srt_close(ready_scks[i]) == SRT_ERROR)
								fprintf(stderr, "faild to srt_close errno %d\n", errno);	
						}
					}	
				} else {
					if (out_client_sck != -1) {
						get_packet(&buffer, out_client_sck);
					}
				}
			}
		}
	}
	/* Clean up */
	struct client_t *c, *nxt;
	list_for_each_entry_safe(c, nxt, &g_server_config.clients_list, node) {
		list_del(&c->node);

		if (c->in_fd != -1 && srt_close(c->in_fd) == SRT_ERROR)
			fprintf(stderr, "faild to srt_close errno %d\n", errno);	
		
		client_del(&c);
	}

SRV_INFO_CLEAN:
	freeaddrinfo(servinfo);
SRV_SCK_CLOSE:
	if (*sck != -1 && srt_close(*sck) == SRT_ERROR)
		fprintf(stderr, "faild to srt_close errno %d\n", errno);
SRT_CLEAN:
	srt_cleanup();
	return 0;
}
