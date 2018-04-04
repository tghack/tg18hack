#include <stdio.h>
#include <sys/epoll.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/mman.h>

#define MAX_EVENTS 20
#define MAX_CLIENTS 20

#define pr_debug(fmt, ...) \
	do { \
		fprintf(stderr, "[+] "); \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
	} while (0)

static char *flag;

struct client {
	int fd;
	char *buf;
};

struct daemon {
	int efd; /* epoll file descriptor */
	int sock; /* listening socket */
	struct client clients[MAX_CLIENTS];
};

/*
 * Create socket, bind, and listen
 * @ip		bind to this IP
 * @port	this port
 *
 * Returns socket on success, -1 on error.
 */
int create_socket(char *ip, char *port)
{
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		perror("socket()");
		return -1;
	}

	int port_num = atoi(port);

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_num);
	int ip_ret = inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
	if (ip_ret != 1) {
		if (ip_ret == 0) {
			fprintf(stderr, "Invalid IP address: %s\n", ip);
		} else {
			perror("inet_pton()");
		}
		close(sock);
		return -1;
	}

	int yes = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("setsockopt()");
		close(sock);
		return -1;
	}

	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		perror("bind()");
		close(sock);
		return -1;
	}

	pr_debug("Bound successfully to %s:%d!\n", ip, port_num);

	if (listen(sock, SOMAXCONN)) {
		perror("listen()");
		close(sock);
		return -1;
	}

	return sock;
}

static struct client *find_client(struct daemon *d)
{
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (d->clients[i].fd == 0)
			return &d->clients[i];
	}

	return NULL;
}

static void delete_client(struct client *c)
{
	close(c->fd);
	munmap(c->buf, 4096 * 2);
	c->buf = NULL;
}

static void handle_client(struct client *c)
{
	ssize_t ret;

	if (!c->buf) {
		c->buf = mmap(NULL, 4096 * 2, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (c->buf == MAP_FAILED) {
			perror("mmap()");
			goto out_del;
		}

		if (madvise(c->buf, 4096 * 2, MADV_MERGEABLE)) {
			perror("madvise()");
			goto out_del;
		}
	} else {
		memset(c->buf, 0, 4096 * 2);
	}

	ret = read(c->fd, c->buf, 4096 * 2);
	/* error or closed connection */
	if (ret < 1)
		goto out_del;

	strcat(c->buf, flag);
	ret = write(c->fd, "OK!\n", 4);
	if (ret == -1)
		goto out_del;

	//printf("read %zd bytes from client: %s\n", ret, c->buf);
	pr_debug("page 2: %s\n", c->buf + 4096);

	return;
out_del:
	delete_client(c);
}

static void handle_event(struct daemon *d, struct epoll_event *event)
{
	struct client *c = (struct client *)event->data.ptr;

	if (c->fd == d->sock) {
		struct sockaddr_un addr = { 0 };
		socklen_t addrlen = sizeof(addr);

		int cfd = accept(d->sock, (struct sockaddr *)&addr, &addrlen);
		if (cfd == -1) {
			perror("accept()");
			return;
		}

		pr_debug("Accepted new client: %d\n", cfd);

		struct epoll_event ev;
		ev.events = EPOLLIN;
		struct client *new_c = find_client(d);
		if (!new_c) {
			fprintf(stderr, "no more room!\n");
			close(cfd);
			return;
		}

		new_c->fd = cfd;
		new_c->buf = NULL;
		ev.data.ptr = new_c;

		if (epoll_ctl(d->efd, EPOLL_CTL_ADD, cfd, &ev)) {
			perror("epoll_ctl()");
			close(cfd);
			return;
		}
	} else {
		handle_client(c);
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int sock;
	size_t flag_len;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
		return 0;
	}

	fp = fopen("flag.txt", "r");
	if (!fp) {
		perror("fopen(flag.txt)");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	flag_len = ftell(fp);
	flag = calloc(1, flag_len);
	rewind(fp);

	/* -1 to drop newline */
	if (fread(flag, flag_len - 1, 1, fp) < 1) {
		perror("fread()");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
	pr_debug("flag: %s\n", flag);

	sock = create_socket(argv[1], argv[2]);
	if (sock == -1)
		exit(EXIT_FAILURE);

	int efd = epoll_create1(0);
	if (efd == -1) {
		perror("epoll_create1()");
		exit(EXIT_FAILURE);
	}

	struct client client;
	client.fd = sock;

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = &client;

	if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &ev)) {
		perror("epoll_ctl()");
		exit(EXIT_FAILURE);
	}

	struct epoll_event events[MAX_EVENTS];

	struct daemon daemon;
	daemon.efd = efd;
	daemon.sock = sock;
	memset(daemon.clients, 0, sizeof(daemon.clients));

	pr_debug("Waiting for connections...\n");
	for (;;) {
		int nfds = epoll_wait(efd, events, MAX_EVENTS, 0);
		if (nfds == -1) {
			perror("epoll_wait()");
			break;
		}

		for (int i = 0; i < nfds; i++) {
			handle_event(&daemon, &events[i]);
		}
	}

	return 0;
}
