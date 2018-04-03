#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/crypto.h>
#include <linux/random.h>
#include <crypto/sha.h>
#include <crypto/hash.h>
#include <linux/scatterlist.h>
#include <crypto/skcipher.h>

struct tcrypt_result {
	struct completion completion;
	int err;
};

/* tie all data structures together */
struct skcipher_def {
	struct scatterlist sg;
	struct crypto_skcipher *tfm;
	struct skcipher_request *req;
	struct tcrypt_result result;
};

static const char enc[] = "\x45\x30\xec\x69\xa3\xb4\x30\x15\xcb\x5e\xde\x35\x00\xc9\xea\x2e\x30\x6a\x01\x3e\xa1\xab\xd4\x64\xd7\xa5\x12\xe1\x81\xee\xfd\x39\x26\xa7\x72\x87\xf4\x94\x72\x70\xe9\x2a\x9d\x54\x9c\x67\x4b\xd8\xf8\x7a\x79\x71\x44\xdf\xb0\x47\xde\xd7\xeb\xac\x94\xb3\xf8\xbe";

static inline void to_hex(char *dst, char *src, size_t src_size)
{
	int x;

	for (x = 0; x < src_size; x++)
		sprintf(&dst[x * 2], "%.2x", (unsigned char)src[x]);
}

/* Callback function */
static void test_skcipher_cb(struct crypto_async_request *req, int error)
{
	struct tcrypt_result *result = req->data;

	if (error == -EINPROGRESS)
		return;
	result->err = error;
	complete(&result->completion);
	pr_info("Encryption finished successfully\n");
}

/* Perform cipher operation */
static unsigned int test_skcipher_encdec(struct skcipher_def *sk, int enc)
{
	int rc = 0;

	if (enc)
		rc = crypto_skcipher_encrypt(sk->req);
	else
		rc = crypto_skcipher_decrypt(sk->req);

	switch (rc) {
	case 0:
		break;
	case -EINPROGRESS:
	case -EBUSY:
		rc = wait_for_completion_interruptible(
				&sk->result.completion);
		if (!rc && !sk->result.err) {
			reinit_completion(&sk->result.completion);
			break;
		}
	default:
		pr_info("skcipher encrypt returned with %d result %d\n",
			rc, sk->result.err);
		break;
	}
	init_completion(&sk->result.completion);

	return rc;
}

static char *do_decrypt(u8 *key, size_t len)
{
	int ret;
	u8 iv[16] = { 0 };
	char *scratchpad = NULL;
	struct skcipher_def sk;
	struct skcipher_request *req = NULL;
	struct crypto_skcipher *skcipher = NULL;
	char hexdigest[(64 * 2) + 1] = { 0 };

	skcipher = crypto_alloc_skcipher("cbc-aes-aesni", 0, 0);
	if (IS_ERR(skcipher)) {
		pr_info("could not allocate skcipher handle\n");
		return NULL;
	}

	req = skcipher_request_alloc(skcipher, GFP_KERNEL);
	if (!req) {
		pr_info("could not allocate skcipher request\n");
		return NULL;
	}

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
				      test_skcipher_cb, &sk.result);

	if (crypto_skcipher_setkey(skcipher, key, len)) {
		pr_info("key could not be set\n");
		return NULL;
	}

	sk.tfm = skcipher;
	sk.req = req;

	scratchpad = kmalloc(64 + 1, GFP_KERNEL);
	if (!scratchpad) {
		pr_info("could not allocate scratchpad\n");
		return NULL;
	}
	memcpy(scratchpad, enc, 64);

	/* We encrypt one block */
	sg_init_one(&sk.sg, scratchpad, 64);
	skcipher_request_set_crypt(req, &sk.sg, &sk.sg, 64, iv);
	init_completion(&sk.result.completion);

	/* encrypt data */
	ret = test_skcipher_encdec(&sk, 0);
	if (ret)
		return NULL;

	to_hex(hexdigest, scratchpad, 64);
	pr_debug("aes(flag): %s\n", hexdigest);
	pr_info("Decryption triggered successfully\n");

	return scratchpad;
}

static ssize_t tg_write(struct file *file, const char __user *buf,
			size_t len, loff_t *off)
{
	char *dec;
	ssize_t ret;
	loff_t pos = 0;
	char data[256] = { 0 };

	ret = simple_write_to_buffer(data, sizeof(data) - 1, &pos, buf, len);
	if (ret < 0)
		goto out;

	pr_debug("%s: got %zd bytes: %s\n", __func__, ret, data);

	if (ret != 32)
		goto out;

	ret = 1;
	if (data[1] - 3 != data[3]) ret = 0;
	if (data[0] != data[28]) ret = 0;
	if (data[3] != data[10]) ret = 0;
	if (data[16] != data[19]) ret = 0;
	if (data[23] + 2 != data[24]) ret = 0;
	if (data[25] + data[26] != 106) ret = 0;
	if (data[24] != data[25]) ret = 0;
	if (data[20] - data[16] != 15) ret = 0;
	if (data[28] - data[24] != 44) ret = 0;
	if (data[8] + 17 != data[9]) ret = 0;
	if (data[0] != 95) ret = 0;
	if (data[1] - 1 != data[5]) ret = 0;
	if (data[2] - data[3] != 5) ret = 0;
	if (data[12] != 114) ret = 0;
	if (data[13] != 95) ret = 0;
	if (data[31] != 121) ret = 0;
	if (data[10] != data[31] - 9) ret = 0;
	if (data[4] != data[11]) ret = 0;
	if (data[16] != data[4]) ret = 0;
	if (data[21] != data[22]) ret = 0;
	if (data[21] + data[22] != 190) ret = 0;
	if (data[21] != data[6]) ret = 0;
	if (data[22] != data[7]) ret = 0;
	if (data[11] - data[0] != 6) ret = 0;
	if (data[4] != data[8] + 1) ret = 0;
	if (data[30] + data[29] - data[31] != 87) ret = 0;
	if (data[4] != data[30]) ret = 0;
	if (data[1] != data[15]) ret = 0;
	if (data[20] - 2 != data[18]) ret = 0;
	if (data[19] != data[30]) ret = 0;
	if (data[7] != data[27]) ret = 0;
	if (data[7] != data[14]) ret = 0;
	if (data[17] != 99) ret = 0;

	/* success! */
	dec = do_decrypt(data, 32);
	if (!dec)
		goto out;
	simple_read_from_buffer((char *)buf, strlen(dec), off, dec, strlen(dec));
	kfree(dec);
out:
	return ret;
}

static ssize_t tg_read(struct file *file, char __user *buf,
		       size_t len, loff_t *off)
{
	char msg[] = "Password please!\n";

	return simple_read_from_buffer(buf, len, off, msg, sizeof(msg) - 1);
}

static int tg_flush(struct file *file, fl_owner_t id)
{
	return 0;
}

static const struct file_operations tg_fops = {
	.owner = THIS_MODULE,
	.write = tg_write,
	.read = tg_read,
	.flush = tg_flush
};

static struct miscdevice tg_dev = {
	.name = "tghack",
	.fops = &tg_fops,
	.mode = S_IRUGO | S_IWUGO
};

static int __init tg_init(void)
{
	pr_debug("helloooo\n");

	return misc_register(&tg_dev);
}

static void __exit tg_exit(void)
{
	pr_debug("byeeee\n");
	misc_deregister(&tg_dev);
}

module_init(tg_init);
module_exit(tg_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Test");
MODULE_DESCRIPTION("Test");

