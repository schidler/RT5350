/*
 * spi-ralink.c -- Ralink RT288x/RT305x SPI controller driver
 *
 * Copyright (C) 2011 Sergiy <piratfm@gmail.com>
 * Copyright (C) 2011-2013 Gabor Juhos <juhosg@openwrt.org>
 *
 * Some parts are based on spi-orion.c:
 *   Author: Shadi Ammouri <shadi@marvell.com>
 *   Copyright (C) 2007-2008 Marvell Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/spi/spi.h>

#define DRIVER_NAME			"spi-ralink"
#define RALINK_NUM_CHIPSELECTS		1 /* only one slave is supported*/
#define RALINK_SPI_WAIT_RDY_MAX_LOOP	2000 /* in usec */

#define RAMIPS_SPI_STAT			0x00
#define RAMIPS_SPI_CFG			0x10
#define RAMIPS_SPI_CTL			0x14
#define RAMIPS_SPI_DATA			0x20

/* SPISTAT register bit field */
#define SPISTAT_BUSY			BIT(0)

/* SPICFG register bit field */
#define SPICFG_LSBFIRST			0
#define SPICFG_MSBFIRST			BIT(8)
#define SPICFG_SPICLKPOL		BIT(6)
#define SPICFG_RXCLKEDGE_FALLING	BIT(5)
#define SPICFG_TXCLKEDGE_FALLING	BIT(4)
#define SPICFG_SPICLK_PRESCALE_MASK	0x7
#define SPICFG_SPICLK_DIV2		0
#define SPICFG_SPICLK_DIV4		1
#define SPICFG_SPICLK_DIV8		2
#define SPICFG_SPICLK_DIV16		3
#define SPICFG_SPICLK_DIV32		4
#define SPICFG_SPICLK_DIV64		5
#define SPICFG_SPICLK_DIV128		6
#define SPICFG_SPICLK_DISABLE		7

/* SPICTL register bit field */
#define SPICTL_HIZSDO			BIT(3)
#define SPICTL_STARTWR			BIT(2)
#define SPICTL_STARTRD			BIT(1)
#define SPICTL_SPIENA			BIT(0)

#ifdef DEBUG
#define spi_debug(args...) printk(args)
#else
#define spi_debug(args...)
#endif

struct ralink_spi {
	struct spi_master	*master;
	void __iomem		*base;
	unsigned int		sys_freq;
	unsigned int		speed;
	struct clk		*clk;
};

static inline struct ralink_spi *spidev_to_ralink_spi(struct spi_device *spi)
{
	return spi_master_get_devdata(spi->master);
}

static inline u32 ralink_spi_read(struct ralink_spi *rs, u32 reg)
{
	return ioread32(rs->base + reg);
}

static inline void ralink_spi_write(struct ralink_spi *rs, u32 reg, u32 val)
{
	iowrite32(val, rs->base + reg);
}

static inline void ralink_spi_setbits(struct ralink_spi *rs, u32 reg, u32 mask)
{
	void __iomem *addr = rs->base + reg;
	u32 val;

	val = ioread32(addr);
	val |= mask;
	iowrite32(val, addr);
}

static inline void ralink_spi_clrbits(struct ralink_spi *rs, u32 reg, u32 mask)
{
	void __iomem *addr = rs->base + reg;
	u32 val;

	val = ioread32(addr);
	val &= ~mask;
	iowrite32(val, addr);
}

static int ralink_spi_baudrate_set(struct spi_device *spi, unsigned int speed)
{
	struct ralink_spi *rs = spidev_to_ralink_spi(spi);
	u32 rate;
	u32 prescale;
	u32 reg;

	spi_debug("%s: speed:%u\n", __func__, speed);

	/*
	 * the supported rates are: 2, 4, 8, ... 128
	 * round up as we look for equal or less speed
	 */
	rate = DIV_ROUND_UP(rs->sys_freq, speed);
	spi_debug("%s: rate-1:%u\n", __func__, rate);
	rate = roundup_pow_of_two(rate);
	spi_debug("%s: rate-2:%u\n", __func__, rate);

	/* check if requested speed is too small */
	if (rate > 128)
		return -EINVAL;

	if (rate < 2)
		rate = 2;

	/* Convert the rate to SPI clock divisor value.	*/
	prescale = ilog2(rate/2);
	spi_debug("%s: prescale:%u\n", __func__, prescale);

	reg = ralink_spi_read(rs, RAMIPS_SPI_CFG);
	reg = ((reg & ~SPICFG_SPICLK_PRESCALE_MASK) | prescale);
	ralink_spi_write(rs, RAMIPS_SPI_CFG, reg);
	rs->speed = speed;
	return 0;
}

/*
 * called only when no transfer is active on the bus
 */
static int
ralink_spi_setup_transfer(struct spi_device *spi, struct spi_transfer *t)
{
	struct ralink_spi *rs = spidev_to_ralink_spi(spi);
	unsigned int speed = spi->max_speed_hz;
	int rc;
	unsigned int bits_per_word = 8;

	if ((t != NULL) && t->speed_hz)
		speed = t->speed_hz;

	if ((t != NULL) && t->bits_per_word)
		bits_per_word = t->bits_per_word;

	if (rs->speed != speed) {
		spi_debug("%s: speed_hz:%u\n", __func__, speed);
		rc = ralink_spi_baudrate_set(spi, speed);
		if (rc)
			return rc;
	}

	if (bits_per_word != 8) {
		spi_debug("%s: bad bits_per_word: %u\n", __func__,
			  bits_per_word);
		return -EINVAL;
	}

	return 0;
}

static void ralink_spi_set_cs(struct ralink_spi *rs, int enable)
{
	if (enable)
		ralink_spi_clrbits(rs, RAMIPS_SPI_CTL, SPICTL_SPIENA);
	else
		ralink_spi_setbits(rs, RAMIPS_SPI_CTL, SPICTL_SPIENA);
}

static inline int ralink_spi_wait_till_ready(struct ralink_spi *rs)
{
	int i;

	for (i = 0; i < RALINK_SPI_WAIT_RDY_MAX_LOOP; i++) {
		u32 status;

		status = ralink_spi_read(rs, RAMIPS_SPI_STAT);
		if ((status & SPISTAT_BUSY) == 0)
			return 0;

		udelay(1);
	}

	return -ETIMEDOUT;
}

static unsigned int
ralink_spi_write_read(struct spi_device *spi, struct spi_transfer *xfer)
{
	struct ralink_spi *rs = spidev_to_ralink_spi(spi);
	unsigned count = 0;
	u8 *rx = xfer->rx_buf;
	const u8 *tx = xfer->tx_buf;
	int err;

	spi_debug("%s(%d): %s %s\n", __func__, xfer->len,
		  (tx != NULL) ? "tx" : "  ",
		  (rx != NULL) ? "rx" : "  ");

	if (tx) {
		for (count = 0; count < xfer->len; count++) {
			ralink_spi_write(rs, RAMIPS_SPI_DATA, tx[count]);
			ralink_spi_setbits(rs, RAMIPS_SPI_CTL, SPICTL_STARTWR);
			err = ralink_spi_wait_till_ready(rs);
			if (err) {
				dev_err(&spi->dev, "TX failed, err=%d\n", err);
				goto out;
			}
		}
	}

	if (rx) {
		for (count = 0; count < xfer->len; count++) {
			ralink_spi_setbits(rs, RAMIPS_SPI_CTL, SPICTL_STARTRD);
			err = ralink_spi_wait_till_ready(rs);
			if (err) {
				dev_err(&spi->dev, "RX failed, err=%d\n", err);
				goto out;
			}
			rx[count] = (u8) ralink_spi_read(rs, RAMIPS_SPI_DATA);
		}
	}

out:
	return count;
}

static int ralink_spi_transfer_one_message(struct spi_master *master,
					   struct spi_message *m)
{
	struct ralink_spi *rs = spi_master_get_devdata(master);
	struct spi_device *spi = m->spi;
	struct spi_transfer *t = NULL;
	int par_override = 0;
	int status = 0;
	int cs_active = 0;

	/* Load defaults */
	status = ralink_spi_setup_transfer(spi, NULL);
	if (status < 0)
		goto msg_done;

	list_for_each_entry(t, &m->transfers, transfer_list) {
		unsigned int bits_per_word = spi->bits_per_word;

		if (t->tx_buf == NULL && t->rx_buf == NULL && t->len) {
			dev_err(&spi->dev,
				"message rejected: invalid transfer data buffers\n");
			status = -EIO;
			goto msg_done;
		}

		if (t->bits_per_word)
			bits_per_word = t->bits_per_word;

		if (bits_per_word != 8) {
			dev_err(&spi->dev,
				"message rejected: invalid transfer bits_per_word (%d bits)\n",
				bits_per_word);
			status = -EIO;
			goto msg_done;
		}

		if (t->speed_hz && t->speed_hz < (rs->sys_freq / 128)) {
			dev_err(&spi->dev,
				"message rejected: device min speed (%d Hz) exceeds required transfer speed (%d Hz)\n",
				(rs->sys_freq / 128), t->speed_hz);
			status = -EIO;
			goto msg_done;
		}

		if (par_override || t->speed_hz || t->bits_per_word) {
			par_override = 1;
			status = ralink_spi_setup_transfer(spi, t);
			if (status < 0)
				goto msg_done;
			if (!t->speed_hz && !t->bits_per_word)
				par_override = 0;
		}

		if (!cs_active) {
			ralink_spi_set_cs(rs, 1);
			cs_active = 1;
		}

		if (t->len)
			m->actual_length += ralink_spi_write_read(spi, t);

		if (t->delay_usecs)
			udelay(t->delay_usecs);

		if (t->cs_change) {
			ralink_spi_set_cs(rs, 0);
			cs_active = 0;
		}
	}

msg_done:
	if (cs_active)
		ralink_spi_set_cs(rs, 0);

	m->status = status;
	spi_finalize_current_message(master);

	return 0;
}

static int ralink_spi_setup(struct spi_device *spi)
{
	struct ralink_spi *rs = spidev_to_ralink_spi(spi);

	if ((spi->max_speed_hz == 0) ||
	    (spi->max_speed_hz > (rs->sys_freq / 2)))
		spi->max_speed_hz = (rs->sys_freq / 2);

	if (spi->max_speed_hz < (rs->sys_freq / 128)) {
		dev_err(&spi->dev, "setup: requested speed is too low %d Hz\n",
			spi->max_speed_hz);
		return -EINVAL;
	}

	if (spi->bits_per_word != 0 && spi->bits_per_word != 8) {
		dev_err(&spi->dev,
			"setup: requested bits per words - os wrong %d bpw\n",
			spi->bits_per_word);
		return -EINVAL;
	}

	if (spi->bits_per_word == 0)
		spi->bits_per_word = 8;

	/*
	 * baudrate & width will be set ralink_spi_setup_transfer
	 */
	return 0;
}

static void ralink_spi_reset(struct ralink_spi *rs)
{
	ralink_spi_write(rs, RAMIPS_SPI_CFG,
			 SPICFG_MSBFIRST | SPICFG_TXCLKEDGE_FALLING |
			 SPICFG_SPICLK_DIV16 | SPICFG_SPICLKPOL);
	ralink_spi_write(rs, RAMIPS_SPI_CTL, SPICTL_HIZSDO | SPICTL_SPIENA);
}

static int ralink_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct ralink_spi *rs;
	struct resource *r;
	int status = 0;

	master = spi_alloc_master(&pdev->dev, sizeof(*rs));
	if (master == NULL) {
		dev_dbg(&pdev->dev, "master allocation failed\n");
		return -ENOMEM;
	}

	//if (pdev->id != -1)
		master->bus_num = 0;

	/* we support only mode 0, and no options */
	master->mode_bits = 0;

	master->setup = ralink_spi_setup;
	master->transfer_one_message = ralink_spi_transfer_one_message;
	master->num_chipselect = RALINK_NUM_CHIPSELECTS;
	master->dev.of_node = pdev->dev.of_node;

	dev_set_drvdata(&pdev->dev, master);

	rs = spi_master_get_devdata(master);
	rs->master = master;

	rs->clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(rs->clk)) {
		status = PTR_ERR(rs->clk);
		dev_err(&pdev->dev, "unable to get SYS clock, err=%d\n",
			status);
		goto out_put_master;
	}

	status = clk_enable(rs->clk);
	if (status)
		goto out_put_clk;

	rs->sys_freq = clk_get_rate(rs->clk);
	spi_debug("%s: sys_freq: %u\n", __func__, rs->sys_freq);

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		status = -ENODEV;
		goto out_disable_clk;
	}

	rs->base = devm_request_and_ioremap(&pdev->dev, r);
	if (!rs->base) {
		status = -EADDRNOTAVAIL;
		goto out_disable_clk;
	}

	ralink_spi_reset(rs);

	status = spi_register_master(master);
	if (status)
		goto out_disable_clk;

	return 0;

out_disable_clk:
	clk_disable(rs->clk);
out_put_clk:
	clk_put(rs->clk);
out_put_master:
	spi_master_put(master);
	return status;
}

static int ralink_spi_remove(struct platform_device *pdev)
{
	struct spi_master *master;
	struct ralink_spi *rs;

	master = dev_get_drvdata(&pdev->dev);
	rs = spi_master_get_devdata(master);

	clk_disable(rs->clk);
	clk_put(rs->clk);
	spi_unregister_master(master);

	return 0;
}

MODULE_ALIAS("platform:" DRIVER_NAME);

static const struct of_device_id ralink_spi_match[] = {
	{ .compatible = "ralink,rt2880-spi" },
	{},
};
MODULE_DEVICE_TABLE(of, ralink_spi_match);

static struct platform_driver ralink_spi_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = ralink_spi_match,
	},
	.probe = ralink_spi_probe,
	.remove = ralink_spi_remove,
};

module_platform_driver(ralink_spi_driver);

MODULE_DESCRIPTION("Ralink SPI driver");
MODULE_AUTHOR("Sergiy <piratfm@gmail.com>");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL");
