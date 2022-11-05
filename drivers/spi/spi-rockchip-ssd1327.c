// SPDX-License-Identifier: GPL-2.0
/**
 * @copyright (C) 2022 Robin <jiangrenbin329@gmail.com>
 *
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/platform_data/spi-rockchip.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/syscalls.h>
#include <linux/workqueue.h>
#include <uapi/linux/spi/spidev.h>

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ":[%s:%d] " fmt, __func__, __LINE__

/*
 * This supports access to SPI devices using normal userspace I/O calls.
 * Note that while traditional UNIX/POSIX I/O semantics are half duplex,
 * and often mask message boundaries, full SPI support requires full duplex
 * transfers.  There are several kinds of internal message boundaries to
 * handle chipselect management and other protocol options.
 *
 * SPI has a character major number assigned.  We allocate minor numbers
 * dynamically using a bitmask.  You must use hotplug tools, such as udev
 * (or mdev with busybox) to create and destroy the /dev/spidevB.C device
 * nodes, since there is no fixed association of minor numbers with any
 * particular SPI bus or device.
 */
#define SPI_SSD1327_MAJOR 154 /* assigned */
#define N_SPI_MINORS 32 /* ... up to 256 */

static DECLARE_BITMAP(minors, N_SPI_MINORS);
/* Bit masks for spi_device.mode management.  Note that incorrect
 * settings for some settings can cause *lots* of trouble for other
 * devices on a shared bus:
 *
 *  - CS_HIGH ... this device will be active when it shouldn't be
 *  - 3WIRE ... when active, it won't behave as it should
 *  - NO_CS ... there will be no explicit message boundaries; this
 *	is completely incompatible with the shared bus model
 *  - READY ... transfers may proceed when they shouldn't.
 *
 * REVISIT should changing those flags be privileged?
 */
#define SPI_MODE_MASK                                                          \
	(SPI_MODE_X_MASK | SPI_CS_HIGH | SPI_LSB_FIRST | SPI_3WIRE |           \
	 SPI_LOOP | SPI_NO_CS | SPI_READY | SPI_TX_DUAL | SPI_TX_QUAD |        \
	 SPI_TX_OCTAL | SPI_RX_DUAL | SPI_RX_QUAD | SPI_RX_OCTAL)

static LIST_HEAD(device_list);
static DEFINE_MUTEX(device_list_lock);

/* The main reason to have this class is to make mdev/udev create the
 * /dev/spidevB.C character device nodes exposing our userspace API.
 * It also simplifies memory management.
 */

static struct class *spi_ssd1327_class;

#define MAX_SPI_DEV_NUM 8
#define SPI_MAX_RATE 50000000
#define BUF_SIZE 4096
#define DRIVER_NAME "ssd1327"
#define DEV_NAME "spi-rockchip-ssd1327"

struct spi_ssd1327_data {
	dev_t devt;
	spinlock_t spi_lock;
	struct device *dev;
	struct spi_device *spi;
	struct list_head device_entry;
	struct mutex buf_lock;
	unsigned users;
	char *rx_buf;
	int rx_len;
	char *tx_buf;
	int tx_len;
	u32 speed_hz;
};

static struct spi_ssd1327_data *g_spi_ssd1327_data[MAX_SPI_DEV_NUM];
static u32 bit_per_word = 8;

static int spi_write_slt(int id, const void *txbuf, size_t n)
{
	int ret = -1;
	struct spi_device *spi = NULL;
	struct spi_transfer t = {
		.tx_buf = txbuf,
		.len = n,
		.bits_per_word = bit_per_word,
	};
	struct spi_message m;

	dev_info(g_spi_ssd1327_data[id]->dev, "[%s]:txbuf:%s", __func__,
		 (char *)txbuf);
	if (id >= MAX_SPI_DEV_NUM)
		return ret;
	if (!g_spi_ssd1327_data[id]) {
		pr_err("g_spi.%d is NULL\n", id);
		return ret;
	} else {
		spi = g_spi_ssd1327_data[id]->spi;
	}

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	return spi_sync(spi, &m);
}

static int spi_read_slt(int id, void *rxbuf, size_t n)
{
	int ret = -1;
	struct spi_device *spi = NULL;
	struct spi_transfer t = {
		.rx_buf = rxbuf,
		.len = n,
		.bits_per_word = bit_per_word,
	};
	struct spi_message m;

	if (id >= MAX_SPI_DEV_NUM)
		return ret;
	if (!g_spi_ssd1327_data[id]) {
		pr_err("g_spi.%d is NULL\n", id);
		return ret;
	} else {
		spi = g_spi_ssd1327_data[id]->spi;
	}

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	return spi_sync(spi, &m);
}

static int __attribute((unused))
spi_write_then_read_slt(int id, const void *txbuf, unsigned n_tx, void *rxbuf,
			unsigned n_rx)
{
	int ret = -1;
	struct spi_device *spi = NULL;

	if (id >= MAX_SPI_DEV_NUM)
		return ret;
	if (!g_spi_ssd1327_data[id]) {
		pr_err("g_spi.%d is NULL\n", id);
		return ret;
	} else {
		spi = g_spi_ssd1327_data[id]->spi;
	}

	ret = spi_write_then_read(spi, txbuf, n_tx, rxbuf, n_rx);
	return ret;
}

static int spi_write_and_read_slt(int id, const void *tx_buf, void *rx_buf,
				  size_t len)
{
	int ret = -1;
	struct spi_device *spi = NULL;
	struct spi_transfer t = {
		.tx_buf = tx_buf,
		.rx_buf = rx_buf,
		.len = len,
	};
	struct spi_message m;

	if (id >= MAX_SPI_DEV_NUM)
		return ret;
	if (!g_spi_ssd1327_data[id]) {
		pr_err("g_spi.%d is NULL\n", id);
		return ret;
	} else {
		spi = g_spi_ssd1327_data[id]->spi;
	}

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	return spi_sync(spi, &m);
}
static ssize_t spi_ssd1327_write(struct file *file, const char __user *buf,
				 size_t n, loff_t *offset)
{
	int argc = 0, i;
	char *argv[32];
	char cmd[64] = { 0 };
	char *txbuf = NULL, *rxbuf = NULL;
	char *cmds, *data;
	unsigned int id = 0, times = 0, size = 0;
	unsigned long us = 0, bytes = 0;
	ktime_t start_time;
	ktime_t end_time;
	ktime_t cost_time;

	if (copy_from_user(cmd, buf, n))
		return -EFAULT;
	cmds = cmd;
	data = cmd;
	while (data < (cmd + n)) {
		data = strstr(data, " ");
		if (!data)
			break;
		*data = 0;
		argv[argc++] = ++data;
		if (argc >= 16)
			break;
	}
	cmd[n - 1] = 0;
	if (!strcmp(cmds, "write")) {
		sscanf(argv[0], "%d", &id);
		sscanf(argv[1], "%d", &times);
		sscanf(argv[2], "%d", &size);

		txbuf = kzalloc(size, GFP_KERNEL);
		if (!txbuf) {
			pr_info("spi write alloc buf size %d fail\n", size);
			return n;
		}

		for (i = 0; i < size; i++)
			txbuf[i] = i % 256;

		start_time = ktime_get();
		for (i = 0; i < times; i++)
			spi_write_slt(id, txbuf, size);
		end_time = ktime_get();
		cost_time = ktime_sub(end_time, start_time);
		us = ktime_to_us(cost_time);

		bytes = size * times * 1;
		bytes = bytes * 1000 / us;
		pr_info("spi write %d*%d cost %ldus speed:%ldKB/S\n", size,
			times, us, bytes);

		kfree(txbuf);
	} else if (!strcmp(cmd, "loop")) {
		sscanf(argv[0], "%d", &id);
		sscanf(argv[1], "%d", &times);
		sscanf(argv[2], "%d", &size);

		txbuf = kzalloc(size, GFP_KERNEL);
		if (!txbuf) {
			pr_info("spi tx alloc buf size %d fail\n", size);
			return n;
		}

		rxbuf = kzalloc(size, GFP_KERNEL);
		if (!rxbuf) {
			kfree(txbuf);
			pr_info("spi rx alloc buf size %d fail\n", size);
			return n;
		}

		for (i = 0; i < size; i++)
			txbuf[i] = i % 256;

		start_time = ktime_get();
		for (i = 0; i < times; i++) {
			spi_write_and_read_slt(id, txbuf, rxbuf, size);
			if (memcmp(txbuf, rxbuf, size)) {
				pr_info("spi loop test fail\n");
				break;
			}
		}

		end_time = ktime_get();
		cost_time = ktime_sub(end_time, start_time);
		us = ktime_to_us(cost_time);

		bytes = size * times;
		bytes = bytes * 1000 / us;
		pr_info("spi loop %d*%d cost %ldus speed:%ldKB/S\n", size,
			times, us, bytes);

		kfree(txbuf);
		kfree(rxbuf);
	} else if (!strcmp(cmd, "read")) {
		sscanf(argv[0], "%d", &id);
		sscanf(argv[1], "%d", &times);
		sscanf(argv[2], "%d", &size);

		rxbuf = kzalloc(size, GFP_KERNEL);
		if (!rxbuf) {
			pr_info("spi read alloc buf size %d fail\n", size);
			return n;
		}

		start_time = ktime_get();
		for (i = 0; i < times; i++)
			spi_read_slt(id, rxbuf, size);
		end_time = ktime_get();
		cost_time = ktime_sub(end_time, start_time);
		us = ktime_to_us(cost_time);

		bytes = size * times * 1;
		bytes = bytes * 1000 / us;
		pr_info("spi read %d*%d cost %ldus speed:%ldKB/S\n", size,
			times, us, bytes);
		print_hex_dump(KERN_ERR, "SPI RX: ", DUMP_PREFIX_OFFSET, 16, 1,
			       rxbuf, size, 1);

		kfree(rxbuf);
	} else if (!strcmp(cmd, "setspeed")) {
		int id = 0, val;
		struct spi_device *spi = NULL;

		sscanf(argv[0], "%d", &id);
		sscanf(argv[1], "%d", &val);

		if (id >= MAX_SPI_DEV_NUM)
			return n;
		if (!g_spi_ssd1327_data[id]) {
			pr_err("g_spi.%d is NULL\n", id);
			return n;
		} else {
			spi = g_spi_ssd1327_data[id]->spi;
		}
		spi->max_speed_hz = val;
	} else {
		pr_info("echo id number size > /dev/%s\n", DRIVER_NAME);
		pr_info("echo write 0 10 255 > /dev/%s\n", DRIVER_NAME);
		pr_info("echo read 0 10 255 > /dev/%s\n", DRIVER_NAME);
		pr_info("echo loop 0 10 255 > /dev/%s\n", DRIVER_NAME);
		pr_info("echo setspeed 0 1000000 > /dev/%s\n", DRIVER_NAME);
		pr_info("echo config 8 > /dev/%s\n", DRIVER_NAME);
	}
	return n;
}

static ssize_t spi_ssd1327_read(struct file *filp, char __user *buf, size_t len,
				loff_t *off)
{
	pr_info("[%s] file read...!!!\n", __func__);
	return 0;
}
static ssize_t spi_ssd1327_sync(struct spi_ssd1327_data *spidev,
				struct spi_message *message)
{
	int status;
	struct spi_device *spi;

	// spin_lock_irq(&spidev->spi_lock);
	spi = spidev->spi;
	// spin_unlock_irq(&spidev->spi_lock);

	if (spi == NULL)
		status = -ESHUTDOWN;
	else
		status = spi_sync(spi, message);

	if (status == 0)
		status = message->actual_length;

	return status;
}

static int spi_ssd1327_message(struct spi_ssd1327_data *spidev,
			       struct spi_ioc_transfer *u_xfers,
			       unsigned n_xfers)
{
	struct spi_message msg;
	struct spi_transfer *k_xfers;
	struct spi_transfer *k_tmp;
	struct spi_ioc_transfer *u_tmp;
	unsigned n, total, tx_total, rx_total;
	u8 *tx_buf, *rx_buf;
	int status = -EFAULT;

	spi_message_init(&msg);
	pr_info("spidev:%p  spidev->spi:%p\n", spidev, spidev->spi);
	k_xfers = kcalloc(n_xfers, sizeof(*k_tmp), GFP_KERNEL);
	if (k_xfers == NULL)
		return -ENOMEM;

	/* Construct spi_message, copying any tx data to bounce buffer.
	 * We walk the array of user-provided transfers, using each one
	 * to initialize a kernel version of the same transfer.
	 */
	tx_buf = spidev->tx_buf;
	rx_buf = spidev->rx_buf;
	total = 0;
	tx_total = 0;
	rx_total = 0;
	for (n = n_xfers, k_tmp = k_xfers, u_tmp = u_xfers; n;
	     n--, k_tmp++, u_tmp++) {
		/* Ensure that also following allocations from rx_buf/tx_buf will meet
		 * DMA alignment requirements.
		 */
		unsigned int len_aligned =
			ALIGN(u_tmp->len, ARCH_KMALLOC_MINALIGN);

		k_tmp->len = u_tmp->len;

		total += k_tmp->len;
		/* Since the function returns the total length of transfers
		 * on success, restrict the total to positive int values to
		 * avoid the return value looking like an error.  Also check
		 * each transfer length to avoid arithmetic overflow.
		 */
		if (total > INT_MAX || k_tmp->len > INT_MAX) {
			status = -EMSGSIZE;
			goto done;
		}

		if (u_tmp->rx_buf) {
			/* this transfer needs space in RX bounce buffer */
			rx_total += len_aligned;
			if (rx_total > BUF_SIZE) {
				status = -EMSGSIZE;
				goto done;
			}
			k_tmp->rx_buf = rx_buf;
			rx_buf += len_aligned;
		}
		if (u_tmp->tx_buf) {
			/* this transfer needs space in TX bounce buffer */
			tx_total += len_aligned;
			if (tx_total > BUF_SIZE) {
				status = -EMSGSIZE;
				goto done;
			}
			k_tmp->tx_buf = tx_buf;
			if (copy_from_user(
				    tx_buf,
				    (const u8 __user *)(uintptr_t)u_tmp->tx_buf,
				    u_tmp->len))
				goto done;
			tx_buf += len_aligned;
		}

		k_tmp->cs_change = !!u_tmp->cs_change;
		k_tmp->tx_nbits = u_tmp->tx_nbits;
		k_tmp->rx_nbits = u_tmp->rx_nbits;
		k_tmp->bits_per_word = u_tmp->bits_per_word;
		k_tmp->delay.value = u_tmp->delay_usecs;
		k_tmp->delay.unit = SPI_DELAY_UNIT_USECS;
		k_tmp->speed_hz = u_tmp->speed_hz;
		k_tmp->word_delay.value = u_tmp->word_delay_usecs;
		k_tmp->word_delay.unit = SPI_DELAY_UNIT_USECS;
		if (!k_tmp->speed_hz)
			k_tmp->speed_hz = spidev->speed_hz;
#ifdef DEBUG
		dev_dbg(&spidev->spi->dev,
			"  xfer len %u %s%s%s%dbits %u usec %u usec %uHz\n",
			k_tmp->len, k_tmp->rx_buf ? "rx " : "",
			k_tmp->tx_buf ? "tx " : "",
			k_tmp->cs_change ? "cs " : "",
			k_tmp->bits_per_word ?: spidev->spi->bits_per_word,
			k_tmp->delay.value, k_tmp->word_delay.value,
			k_tmp->speed_hz ?: spidev->spi->max_speed_hz);
#endif
		spi_message_add_tail(k_tmp, &msg);
	}

	status = spi_ssd1327_sync(spidev, &msg);
	if (status < 0)
		goto done;

	/* copy any rx data out of bounce buffer */
	for (n = n_xfers, k_tmp = k_xfers, u_tmp = u_xfers; n;
	     n--, k_tmp++, u_tmp++) {
		if (u_tmp->rx_buf) {
			if (copy_to_user((u8 __user *)(uintptr_t)u_tmp->rx_buf,
					 k_tmp->rx_buf, u_tmp->len)) {
				status = -EFAULT;
				goto done;
			}
		}
	}
	status = total;

done:
	kfree(k_xfers);
	return status;
}
static struct spi_ioc_transfer *
spi_ssd1327_get_ioc_message(unsigned int cmd,
			    struct spi_ioc_transfer __user *u_ioc,
			    unsigned *n_ioc)
{
	u32 tmp;

	/* Check type, command number and direction */
	if (_IOC_TYPE(cmd) != SPI_IOC_MAGIC ||
	    _IOC_NR(cmd) != _IOC_NR(SPI_IOC_MESSAGE(0)) ||
	    _IOC_DIR(cmd) != _IOC_WRITE)
		return ERR_PTR(-ENOTTY);

	tmp = _IOC_SIZE(cmd);
	if ((tmp % sizeof(struct spi_ioc_transfer)) != 0)
		return ERR_PTR(-EINVAL);
	*n_ioc = tmp / sizeof(struct spi_ioc_transfer);
	if (*n_ioc == 0)
		return NULL;

	/* copy into scratch area */
	return memdup_user(u_ioc, tmp);
}

static long spi_ssd1327_ioctl(struct file *filp, unsigned int cmd,
			      unsigned long arg)
{
	int retval = 0;
	struct spi_ssd1327_data *spidev;
	struct spi_device *spi;
	u32 tmp;
	unsigned n_ioc;
	struct spi_ioc_transfer *ioc;

	/* Check type and command number */
	if (_IOC_TYPE(cmd) != SPI_IOC_MAGIC)
		return -ENOTTY;

	/* guard against device removal before, or while,
	 * we issue this ioctl.
	 */
	/* spidev = g_spi_ssd1327_data[0]; */
	spidev = filp->private_data;
	spin_lock_irq(&spidev->spi_lock);
	/* spi = g_spi_ssd1327_data[0]->spi; */
	spi = spi_dev_get(spidev->spi);
	spin_unlock_irq(&spidev->spi_lock);

	if (spi == NULL)
		return -ESHUTDOWN;

	/* use the buffer lock here for triple duty:
	 *  - prevent I/O (from us) so calling spi_setup() is safe;
	 *  - prevent concurrent SPI_IOC_WR_* from morphing
	 *    data fields while SPI_IOC_RD_* reads them;
	 *  - SPI_IOC_MESSAGE needs the buffer locked "normally".
	 */
	mutex_lock(&spidev->buf_lock);

	switch (cmd) {
	/* read requests */
	case SPI_IOC_RD_MODE:
		retval =
			put_user(spi->mode & SPI_MODE_MASK, (__u8 __user *)arg);
		break;
	case SPI_IOC_RD_MODE32:
		retval = put_user(spi->mode & SPI_MODE_MASK,
				  (__u32 __user *)arg);
		break;
	case SPI_IOC_RD_LSB_FIRST:
		retval = put_user((spi->mode & SPI_LSB_FIRST) ? 1 : 0,
				  (__u8 __user *)arg);
		break;
	case SPI_IOC_RD_BITS_PER_WORD:
		retval = put_user(spi->bits_per_word, (__u8 __user *)arg);
		break;
	case SPI_IOC_RD_MAX_SPEED_HZ:
		retval = put_user(spidev->speed_hz, (__u32 __user *)arg);
		break;

	/* write requests */
	case SPI_IOC_WR_MODE:
	case SPI_IOC_WR_MODE32:
		if (cmd == SPI_IOC_WR_MODE)
			retval = get_user(tmp, (u8 __user *)arg);
		else
			retval = get_user(tmp, (u32 __user *)arg);
		if (retval == 0) {
			struct spi_controller *ctlr = spi->controller;
			u32 save = spi->mode;

			if (tmp & ~SPI_MODE_MASK) {
				retval = -EINVAL;
				break;
			}

			if (ctlr->use_gpio_descriptors && ctlr->cs_gpiods &&
			    ctlr->cs_gpiods[spi->chip_select])
				tmp |= SPI_CS_HIGH;

			tmp |= spi->mode & ~SPI_MODE_MASK;
			spi->mode = tmp & SPI_MODE_USER_MASK;
			retval = spi_setup(spi);
			if (retval < 0)
				spi->mode = save;
			else
				dev_dbg(&spi->dev, "spi mode %x\n", tmp);
		}
		break;
	case SPI_IOC_WR_LSB_FIRST:
		retval = get_user(tmp, (__u8 __user *)arg);
		if (retval == 0) {
			u32 save = spi->mode;

			if (tmp)
				spi->mode |= SPI_LSB_FIRST;
			else
				spi->mode &= ~SPI_LSB_FIRST;
			retval = spi_setup(spi);
			if (retval < 0)
				spi->mode = save;
			else
				dev_dbg(&spi->dev, "%csb first\n",
					tmp ? 'l' : 'm');
		}
		break;
	case SPI_IOC_WR_BITS_PER_WORD:
		retval = get_user(tmp, (__u8 __user *)arg);
		if (retval == 0) {
			u8 save = spi->bits_per_word;

			spi->bits_per_word = tmp;
			retval = spi_setup(spi);
			if (retval < 0)
				spi->bits_per_word = save;
			else
				dev_dbg(&spi->dev, "%d bits per word\n", tmp);
		}
		break;
	case SPI_IOC_WR_MAX_SPEED_HZ:
		retval = get_user(tmp, (__u32 __user *)arg);
		if (retval == 0) {
			u32 save = spi->max_speed_hz;

			spi->max_speed_hz = tmp;
			retval = spi_setup(spi);
			if (retval == 0) {
				spidev->speed_hz = tmp;
				dev_dbg(&spi->dev, "%d Hz (max)\n",
					spidev->speed_hz);
			}
			spi->max_speed_hz = save;
		}
		break;

	default:
		/* segmented and/or full-duplex I/O request */
		/* Check message and copy into scratch area */
		ioc = spi_ssd1327_get_ioc_message(
			cmd, (struct spi_ioc_transfer __user *)arg, &n_ioc);
		if (IS_ERR(ioc)) {
			retval = PTR_ERR(ioc);
			break;
		}
		if (!ioc)
			break; /* n_ioc is also 0 */

		/* translate to spi_message, execute */
		retval = spi_ssd1327_message(spidev, ioc, n_ioc);
		kfree(ioc);
		break;
	}

	mutex_unlock(&spidev->buf_lock);
	spi_dev_put(spi);
	return retval;
}

#ifdef CONFIG_COMPAT
static long spi_ssd1327_compat_ioc_message(struct file *filp, unsigned int cmd,
					   unsigned long arg)
{
	struct spi_ioc_transfer __user *u_ioc;
	int retval = 0;
	struct spi_ssd1327_data *spidev;
	struct spi_device *spi;
	unsigned n_ioc, n;
	struct spi_ioc_transfer *ioc;

	u_ioc = (struct spi_ioc_transfer __user *)compat_ptr(arg);

	/* guard against device removal before, or while,
	 * we issue this ioctl.
	 */
	spidev = filp->private_data;
	/* spidev = g_spi_ssd1327_data[0]; */
	spin_lock_irq(&spidev->spi_lock);
	/* spi = g_spi_ssd1327_data[0]->spi; */
	spi = spi_dev_get(spidev->spi);
	spin_unlock_irq(&spidev->spi_lock);

	if (spi == NULL)
		return -ESHUTDOWN;

	/* SPI_IOC_MESSAGE needs the buffer locked "normally" */
	mutex_lock(&spidev->buf_lock);

	/* Check message and copy into scratch area */
	ioc = spi_ssd1327_get_ioc_message(cmd, u_ioc, &n_ioc);
	if (IS_ERR(ioc)) {
		retval = PTR_ERR(ioc);
		goto done;
	}
	if (!ioc)
		goto done; /* n_ioc is also 0 */

	/* Convert buffer pointers */
	for (n = 0; n < n_ioc; n++) {
		ioc[n].rx_buf = (uintptr_t)compat_ptr(ioc[n].rx_buf);
		ioc[n].tx_buf = (uintptr_t)compat_ptr(ioc[n].tx_buf);
	}

	/* translate to spi_message, execute */
	retval = spi_ssd1327_message(spidev, ioc, n_ioc);
	kfree(ioc);

done:
	mutex_unlock(&spidev->buf_lock);
	spi_dev_put(spi);
	return retval;
}

static int spi_ssd1327_open(struct inode *inode, struct file *filp)
{
	struct spi_ssd1327_data *spidev; //g_spi_ssd1327_data[0];
	int status = -ENXIO;

	mutex_lock(&device_list_lock);
	list_for_each_entry (spidev, &device_list, device_entry) {
		if (spidev->devt == inode->i_rdev) {
			status = 0;
			break;
		}
	}

	if (status) {
		pr_debug("spidev: nothing for minor %d\n", iminor(inode));
		goto err_find_dev;
	}

	if (!spidev->tx_buf) {
		spidev->tx_buf = kmalloc(BUF_SIZE, GFP_KERNEL);
		if (!spidev->tx_buf) {
			dev_dbg(&spidev->spi->dev, "open/ENOMEM\n");
			status = -ENOMEM;
			goto err_find_dev;
		}
	}

	if (!spidev->rx_buf) {
		spidev->rx_buf = kmalloc(BUF_SIZE, GFP_KERNEL);
		if (!spidev->rx_buf) {
			dev_dbg(&spidev->spi->dev, "open/ENOMEM\n");
			status = -ENOMEM;
			goto err_alloc_rx_buf;
		}
	}

	spidev->users++;
	filp->private_data = spidev;
	stream_open(inode, filp);

	mutex_unlock(&device_list_lock);
	return 0;

err_alloc_rx_buf:
	kfree(spidev->tx_buf);
	spidev->tx_buf = NULL;
err_find_dev:
	mutex_unlock(&device_list_lock);
	return status;
}

static int spi_ssd1327_release(struct inode *inode, struct file *filp)
{
	struct spi_ssd1327_data *spidev;
	int dofree;

	mutex_lock(&device_list_lock);
	spidev = filp->private_data;
	filp->private_data = NULL;

	spin_lock_irq(&spidev->spi_lock);
	/* ... after we unbound from the underlying device? */
	dofree = (spidev->spi == NULL);
	spin_unlock_irq(&spidev->spi_lock);

	/* last close? */
	spidev->users--;
	if (!spidev->users) {
		kfree(spidev->tx_buf);
		spidev->tx_buf = NULL;

		kfree(spidev->rx_buf);
		spidev->rx_buf = NULL;

		if (dofree)
			kfree(spidev);
		else
			spidev->speed_hz = spidev->spi->max_speed_hz;
	}
#ifdef CONFIG_SPI_SLAVE
	if (!dofree)
		spi_slave_abort(spidev->spi);
#endif
	mutex_unlock(&device_list_lock);

	return 0;
}
static long spi_ssd1327_compat_ioctl(struct file *filp, unsigned int cmd,
				     unsigned long arg)
{
	if (_IOC_TYPE(cmd) == SPI_IOC_MAGIC &&
	    _IOC_NR(cmd) == _IOC_NR(SPI_IOC_MESSAGE(0)) &&
	    _IOC_DIR(cmd) == _IOC_WRITE)
		return spi_ssd1327_compat_ioc_message(filp, cmd, arg);

	return spi_ssd1327_ioctl(filp, cmd, (unsigned long)compat_ptr(arg));
}
#else
#define spi_ssd1327_compat_ioctl NULL
#endif /* CONFIG_COMPAT */

static const struct file_operations spi_ssd1327_fops = {
	.write = spi_ssd1327_write,
	.read = spi_ssd1327_read,
	.unlocked_ioctl = spi_ssd1327_ioctl,
	.compat_ioctl = spi_ssd1327_compat_ioctl,
	.open = spi_ssd1327_open,
	.release = spi_ssd1327_release,
	.llseek = no_llseek,

};
/*  static struct miscdevice spi_ssd1327 = {
 * 	.minor = MISC_DYNAMIC_MINOR,
 * 	.name = DRIVER_NAME,
 * 	.fops = &spi_ssd1327_fops,
 * };
 */

static int spi_ssd1327_probe(struct spi_device *spi)
{
	int ret;
	int id = 0;
	u32 max_freq;
	unsigned long minor;
	struct spi_ssd1327_data *spi_data = NULL;

	if (!spi)
		return -ENOMEM;
	if (!spi->dev.of_node)
		return -ENOMEM;
	spi_data = (struct spi_ssd1327_data *)kzalloc(
		sizeof(struct spi_ssd1327_data), GFP_KERNEL);
	if (!spi_data) {
		dev_err(&spi->dev, "spi_data allocation failed\n");
		return -ENOMEM;
	}
	if (of_property_read_u32(spi->dev.of_node, "spi-max-frequency",
				 &max_freq))
		max_freq = SPI_MAX_RATE;
	if (!max_freq || max_freq > SPI_MAX_RATE) {
		dev_err(&spi->dev, "invalid clock frequency %d\n", max_freq);
		return -ENXIO;
	}

	spi->bits_per_word = 8;
	spi->max_speed_hz = max_freq;
	spi_data->spi = spi;
	spi_data->speed_hz = max_freq;
	spin_lock_init(&spi_data->spi_lock);
	mutex_init(&spi_data->buf_lock);

	INIT_LIST_HEAD(&spi_data->device_entry);
	/* If we can allocate a minor number, hook up this device.
	 * Reusing minors is fine so long as udev or mdev is working.
	 */
	mutex_lock(&device_list_lock);
	minor = find_first_zero_bit(minors, N_SPI_MINORS);
	if (minor < N_SPI_MINORS) {
		struct device *dev;

		spi_data->devt = MKDEV(SPI_SSD1327_MAJOR, minor);
		dev = device_create(spi_ssd1327_class, &spi->dev,
				    spi_data->devt, spi_data,
				    DRIVER_NAME "-%d.%d", spi->master->bus_num,
				    spi->chip_select);
		ret = PTR_ERR_OR_ZERO(dev);
	} else {
		dev_dbg(&spi->dev, "no minor number available!\n");
		ret = -ENODEV;
	}
	if (ret == 0) {
		set_bit(minor, minors);
		list_add(&spi_data->device_entry, &device_list);
	}
	mutex_unlock(&device_list_lock);
	spi_data->dev = &spi->dev;

	/*  setup SPI mode and clock rate */
	ret = spi_setup(spi);
	if (ret < 0) {
		dev_err(spi_data->dev, "spi setup failed, ret = %d\n", ret);
		goto out;
	}
	ret = of_property_read_u32(spi->dev.of_node, "id", &id);
	if (ret != 0) {
		dev_warn(&spi->dev,
			 "Failed to get id, default set 0 ret = %d\n", ret);
		id = 0;
	}
	g_spi_ssd1327_data[id] = spi_data;

	dev_info(&spi->dev, "%s:name=%s,bus_num=%d,cs=%d,mode=%d,speed=%d\n",
		 __func__, spi->modalias, spi->master->bus_num,
		 spi->chip_select, spi->mode, spi->max_speed_hz);
	if (!ret)
		spi_set_drvdata(spi, spi_data);
	else
		kfree(spi_data);
out:
	return ret;
}

static int spi_ssd1327_remove(struct spi_device *spi)
{
	struct spi_ssd1327_data *spi_data = spi_get_drvdata(spi);
	/* prevent new opens */
	mutex_lock(&device_list_lock);
	/* make sure ops on existing fds can abort cleanly */
	spin_lock_irq(&spi_data->spi_lock);
	spi_data->spi = NULL;
	spin_unlock_irq(&spi_data->spi_lock);
	list_del(&spi_data->device_entry);
	device_destroy(spi_ssd1327_class, spi_data->devt);
	clear_bit(MINOR(spi_data->devt), minors);
	if (spi_data->users == 0) {
		kfree(spi_data);
	}
	mutex_unlock(&device_list_lock);
	dev_info(&spi->dev, "spi ssd1327 remove\n");
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id spi_ssd1327_dt_match[] = {
	{
		.compatible = "rockchip,ssd1327",
	},
	{}
};
MODULE_DEVICE_TABLE(of, spi_ssd1327_dt_match);
#endif /* CONFIG_OF */

static struct spi_driver spi_ssd1327_driver = {
        .driver =
                {
                        .name = "ssd1327",
                        .owner = THIS_MODULE,
                        .of_match_table = of_match_ptr(spi_ssd1327_dt_match),
                },
        .probe = spi_ssd1327_probe,
        .remove = spi_ssd1327_remove,
};

static int __init spi_ssd1327_init(void)
{
	int ret;
	/* Claim our 256 reserved device numbers.  Then register a class
	 * that will key udev/mdev to add/remove /dev nodes.  Last, register
	 * the driver which manages those device numbers.
	 */
	BUILD_BUG_ON(N_SPI_MINORS > 256);
	pr_info("spi_ssd1327_init\n");
	ret = register_chrdev(SPI_SSD1327_MAJOR, "spi", &spi_ssd1327_fops);
	if (ret < 0)
		return ret;

	spi_ssd1327_class = class_create(THIS_MODULE, DRIVER_NAME);
	if (IS_ERR(spi_ssd1327_class)) {
		unregister_chrdev(SPI_SSD1327_MAJOR,
				  spi_ssd1327_driver.driver.name);
		return PTR_ERR(spi_ssd1327_class);
	}

	ret = spi_register_driver(&spi_ssd1327_driver);
	if (ret < 0) {
		class_destroy(spi_ssd1327_class);
		unregister_chrdev(SPI_SSD1327_MAJOR,
				  spi_ssd1327_driver.driver.name);
	}
	/* misc_register(&spi_ssd1327); */
	return ret;
}

static void __exit spi_ssd1327_exit(void)
{
	spi_unregister_driver(&spi_ssd1327_driver);
	class_destroy(spi_ssd1327_class);
	unregister_chrdev(SPI_SSD1327_MAJOR, spi_ssd1327_driver.driver.name);
	/* misc_deregister(&spi_ssd1327); */
	pr_info("spi_ssd1327_exit\n");
}

module_init(spi_ssd1327_init);
module_exit(spi_ssd1327_exit);

MODULE_AUTHOR("Robin <jiangrenbin329@gmail.com>");
MODULE_DESCRIPTION("SSD1327 1.5 inch OLED module Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:spi-ssd1327");