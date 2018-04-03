#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#ifdef DEBUG
#define pr_debug(fmt, ...) \
	do { \
		fprintf(stderr, "[+] " fmt, ## __VA_ARGS__); \
	} while (0);
#else
#define pr_debug(fmt, ...)
#endif

#define __unused __attribute__((unused))
#define ROUND_UP_PAGE_SIZE(N) ((((N) + 4096 - 1) / 4096) * 4096)

struct user {
	uint64_t key;
	void *data;
	size_t data_size;
	size_t secure_size;
	int init_done;
	int processed;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

static struct user g_user;
static pthread_t thread;
static int data_ready;
static int done;

static void *thread_func(void __unused *arg)
{
	size_t ret = 1;

	for (;;) {
		pthread_mutex_lock(&g_user.mutex);

		while (!data_ready)
			pthread_cond_wait(&g_user.cond, &g_user.mutex);

		if (done)
			break;

		uint64_t *ptr = (uint64_t *)g_user.data;
#if 0
		size_t lim = (g_user.data_size / 8) - 1;
#else
		size_t lim = (g_user.secure_size / 8);
#endif
		for (size_t i = 0; i < lim; i++)
			ptr[i] ^= g_user.key;

		g_user.processed = 1;
		pthread_mutex_unlock(&g_user.mutex);

		data_ready = 0;
	}

	return (void *)ret;
}

/*
 * Get a (positive) number from the user.
 * @msg		display this message before reading number
 * @returns	number entered on success, -1 on failure
 */
static size_t get_num(const char *msg)
{
	char buf[16] = { 0 };

	printf("%s", msg);
	fflush(stdout);

	if (!fgets(buf, sizeof(buf), stdin)) {
		perror("fgets()");
		return -1;
	}

	return atoi(buf);
}

static inline void print_welcome(void)
{
	printf("Welcome to TG File Uploader v1.42\n");
}

static inline void print_menu(void)
{
	printf("1. Register\n");
	printf("2. Upload data\n");
	printf("3. Show data\n");
	printf("4. Update data\n");
	printf("5. Store data securely\n");
	printf("6. Delete data\n");
	printf("7. Log out\n");
	printf("8. Exit\n");
}

static inline int is_logged_in(void)
{
	return g_user.init_done;
}

static int show_data(void)
{
	int num = 0;
	unsigned int start, stop;

	if (!is_logged_in()) {
		fprintf(stderr, "You must log in first!\n");
		return 0;
	}

	printf("How would you like to view the data?\n");
	printf("1. Text\n");
	printf("2. Array of numbers\n");

	while (num != 1 && num != 2) {
		num = get_num(">");
		if (num != 1 && num != 2)
			printf("Invalid number!\n");
	}
	
	if (num == 1) {
		printf("%s\n", (char *)g_user.data);
		return 0;
	}

	printf("Please enter start and stop index\n");
	start = (unsigned int)get_num("start: ");
	stop = (unsigned int)get_num("stop: ");

	uint64_t *arr = (uint64_t *)g_user.data;
	for (unsigned int i = start; i < stop; i++)
		printf("%u: %lu\n", i, arr[i]);

	return 0;
}

static int upload_data(void)
{
	void *data;
	size_t size;

	if (!is_logged_in()) {
		fprintf(stderr, "You must log in first!\n");
		return 0;
	}

	size = get_num("Size: ");

	pr_debug("%s: data_size: %zu\n", __func__, size);

	data = mmap(NULL, ROUND_UP_PAGE_SIZE(size), PROT_READ | PROT_WRITE,
		    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (data == MAP_FAILED) {
		perror("mmap()");
		return -1;
	}

	printf("Data: ");
	if (fread(data, size, 1, stdin) != 1) {
		perror("fread()");
		munmap(data, size);
		return -1;
	}

	g_user.data = data;
	g_user.data_size = size;

	pr_debug("%s: data/size: %p/%zu\n", __func__, data, size);
	pr_debug("%s: OK!\n", __func__);

	return 0;
}

static int update_data(void)
{
	if (!is_logged_in()) {
		fprintf(stderr, "You must log in first!\n");
		return 0;
	}

	if (!g_user.data || !g_user.data_size) {
		fprintf(stderr, "Invalid size or buffer!\n");
		return 0;
	}

	printf("Data: ");
	if (fread(g_user.data, g_user.data_size, 1, stdin) != 1) {
		perror("fread()");
		return -1;
	}

	pr_debug("%s: OK!\n", __func__);

	return 0;
}

static int store_data(void)
{
	size_t size;

	printf("How many bytes? (max: %zu)", g_user.data_size);
	size = get_num(" ");
	if (size > g_user.data_size) {
		fprintf(stderr, "Invalid size!\n");
		return -1;
	}

	g_user.secure_size = size;
	printf("Encrypting data...\n");

	if (!thread) {
		if (pthread_create(&thread, NULL, thread_func, 0)) {
			perror("pthread_create()");
			exit(EXIT_FAILURE);
		}
	}

	data_ready = 1;
	pthread_cond_signal(&g_user.cond);

	return 0;
}

static int register_user(void)
{
	FILE *fp;
	uint64_t rnd;

	if (g_user.init_done) {
		fprintf(stderr, "Already registered! Log out first.\n");
		return 0;
	}

	fp = fopen("/dev/urandom", "rb");
	if (!fp) {
		perror("fopen()");
		return -1;
	}

	if (fread(&rnd, sizeof(rnd), 1, fp) != 1) {
		perror("fread()");
		fclose(fp);
		return -1;
	}

	pr_debug("%s: key: %lx\n", __func__, rnd);

	pthread_mutex_init(&g_user.mutex, NULL);
	pthread_cond_init(&g_user.cond, NULL);

	g_user.key = rnd;
	g_user.init_done = 1;

	return 0;
}

static int log_out(void)
{
	char buf[10] = { 0 };

	if (!g_user.init_done) {
		fprintf(stderr, "You're not logged in!\n");
		return 0;
	}

	/* XXX: hack to make thread exit */
	done = 1;
	data_ready = 1;
	pthread_cond_signal(&g_user.cond);

	pthread_join(thread, NULL);

	printf("Are you sure you want to quit? (y/n)\n");
	if (!fgets(buf, sizeof(buf), stdin))
		return 0;

	if (*buf == 'y')
		memset(&g_user, 0, sizeof(struct user));

	return 0;
}

static int delete_data(void)
{
	if (!is_logged_in()) {
		fprintf(stderr, "You must log in first!\n");
		return 0;
	}

	if (!g_user.data || !g_user.data_size) {
		fprintf(stderr, "Invalid size or buffer!\n");
		return 0;
	}

	munmap(g_user.data, ROUND_UP_PAGE_SIZE(g_user.data_size));

	pr_debug("%s: OK!\n", __func__);

	return 0;
}

static int (*funcs[])(void) = {
	register_user, upload_data, show_data,
	update_data, store_data, delete_data, log_out
};

static void main_loop(void)
{
	int n, ret = 0;

	for (;;) {
		print_menu();
		n = get_num("> ") - 1;

		if (n < 0 || n > 7) {
			fprintf(stderr, "Invalid menu option\n");
			continue;
		}

		if (n == 7)
			break;

		ret = funcs[n]();

		/* some error occurred */
		if (ret) {
			break;
		}
	}

	if (thread)
		pthread_join(thread, NULL);

	printf("Bye! <3\n");
}

int main(void)
{
	alarm(30);
	setvbuf(stdout, NULL, _IONBF, 0);

	print_welcome();
	main_loop();
	
	return 0;
}
