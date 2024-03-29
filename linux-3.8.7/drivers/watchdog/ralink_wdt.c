/*
 * Ralink RT288X/RT305X built-in hardware watchdog timer
 *
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This driver was based on: drivers/watchdog/ixp4xx_wdt.c
 *	Author: Deepak Saxena <dsaxena@plexity.net>
 *	Copyright 2004 (c) MontaVista, Software, Inc.
 *
 * which again was based on sa1100 driver,
 *	Copyright (C) 2000 Oleg Drokin <green@crimea.edu>
 *
 * parts of the driver are based on Ralink's 2.6.21 BSP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/clk.h>
#include <linux/err.h>

#define DRIVER_NAME	"ralink-wdt"

#define RALINK_WDT_TIMEOUT		30	/* seconds */
#define RALINK_WDT_PRESCALE		65536

#define TIMER_REG_TMR1LOAD		0x00
#define TIMER_REG_TMR1CTL		0x08

#define TMRSTAT_TMR1RST			BIT(5)

#define TMR1CTL_ENABLE			BIT(7)
#define TMR1CTL_MODE_SHIFT		4
#define TMR1CTL_MODE_MASK		0x3
#define TMR1CTL_MODE_FREE_RUNNING	0x0
#define TMR1CTL_MODE_PERIODIC		0x1
#define TMR1CTL_MODE_TIMEOUT		0x2
#define TMR1CTL_MODE_WDT		0x3
#define TMR1CTL_PRESCALE_MASK		0xf
#define TMR1CTL_PRESCALE_65536		0xf

static int nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started "
			   "(default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

static int ralink_wdt_timeout = RALINK_WDT_TIMEOUT;
module_param_named(timeout, ralink_wdt_timeout, int, 0);
MODULE_PARM_DESC(timeout, "Watchdog timeout in seconds, 0 means use maximum "
			  "(default=" __MODULE_STRING(RALINK_WDT_TIMEOUT) "s)");

static unsigned long ralink_wdt_flags;

#define WDT_FLAGS_BUSY		0
#define WDT_FLAGS_EXPECT_CLOSE	1

static struct clk *ralink_wdt_clk;
static unsigned long ralink_wdt_freq;
static int ralink_wdt_max_timeout;
static void __iomem *ralink_wdt_base;

static inline void rt_wdt_w32(unsigned reg, u32 val)
{
	__raw_writel(val, ralink_wdt_base + reg);
}

static inline u32 rt_wdt_r32(unsigned reg)
{
	return __raw_readl(ralink_wdt_base + reg);
}

static inline void ralink_wdt_keepalive(void)
{
	rt_wdt_w32(TIMER_REG_TMR1LOAD, ralink_wdt_timeout * ralink_wdt_freq);
}

static inline void ralink_wdt_enable(void)
{
	u32 t;

	ralink_wdt_keepalive();

	t = rt_wdt_r32(TIMER_REG_TMR1CTL);
	t |= TMR1CTL_ENABLE;
	rt_wdt_w32(TIMER_REG_TMR1CTL, t);
}

static inline void ralink_wdt_disable(void)
{
	u32 t;

	ralink_wdt_keepalive();

	t = rt_wdt_r32(TIMER_REG_TMR1CTL);
	t &= ~TMR1CTL_ENABLE;
	rt_wdt_w32(TIMER_REG_TMR1CTL, t);
}

static int ralink_wdt_set_timeout(int val)
{
	if (val < 1 || val > ralink_wdt_max_timeout) {
		pr_warn(DRIVER_NAME
			": timeout value %d must be 0 < timeout <= %d, using %d\n",
			val, ralink_wdt_max_timeout, ralink_wdt_timeout);
		return -EINVAL;
	}

	ralink_wdt_timeout = val;
	ralink_wdt_keepalive();

	return 0;
}

static int ralink_wdt_open(struct inode *inode, struct file *file)
{
	u32 t;

	if (test_and_set_bit(WDT_FLAGS_BUSY, &ralink_wdt_flags))
		return -EBUSY;

	clear_bit(WDT_FLAGS_EXPECT_CLOSE, &ralink_wdt_flags);

	t = rt_wdt_r32(TIMER_REG_TMR1CTL);
	t &= ~(TMR1CTL_MODE_MASK << TMR1CTL_MODE_SHIFT |
	       TMR1CTL_PRESCALE_MASK);
	t |= (TMR1CTL_MODE_WDT << TMR1CTL_MODE_SHIFT |
	      TMR1CTL_PRESCALE_65536);
	rt_wdt_w32(TIMER_REG_TMR1CTL, t);

	ralink_wdt_enable();

	return nonseekable_open(inode, file);
}

static int ralink_wdt_release(struct inode *inode, struct file *file)
{
	if (test_bit(WDT_FLAGS_EXPECT_CLOSE, &ralink_wdt_flags))
		ralink_wdt_disable();
	else {
		pr_crit(DRIVER_NAME ": device closed unexpectedly, "
			"watchdog timer will not stop!\n");
		ralink_wdt_keepalive();
	}

	clear_bit(WDT_FLAGS_BUSY, &ralink_wdt_flags);
	clear_bit(WDT_FLAGS_EXPECT_CLOSE, &ralink_wdt_flags);

	return 0;
}

static ssize_t rt_wdt_w32ite(struct file *file, const char *data,
				size_t len, loff_t *ppos)
{
	if (len) {
		if (!nowayout) {
			size_t i;

			clear_bit(WDT_FLAGS_EXPECT_CLOSE, &ralink_wdt_flags);

			for (i = 0; i != len; i++) {
				char c;

				if (get_user(c, data + i))
					return -EFAULT;

				if (c == 'V')
					set_bit(WDT_FLAGS_EXPECT_CLOSE,
						&ralink_wdt_flags);
			}
		}

		ralink_wdt_keepalive();
	}

	return len;
}

static const struct watchdog_info ralink_wdt_info = {
	.options		= WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING |
				  WDIOF_MAGICCLOSE,
	.firmware_version	= 0,
	.identity		= "RALINK watchdog",
};

static long ralink_wdt_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	int __user *p = argp;
	int err;
	int t;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		err = copy_to_user(argp, &ralink_wdt_info,
				   sizeof(ralink_wdt_info)) ? -EFAULT : 0;
		break;

	case WDIOC_GETSTATUS:
		err = put_user(0, p);
		break;

	case WDIOC_KEEPALIVE:
		ralink_wdt_keepalive();
		err = 0;
		break;

	case WDIOC_SETTIMEOUT:
		err = get_user(t, p);
		if (err)
			break;

		err = ralink_wdt_set_timeout(t);
		if (err)
			break;

		/* fallthrough */
	case WDIOC_GETTIMEOUT:
		err = put_user(ralink_wdt_timeout, p);
		break;

	default:
		err = -ENOTTY;
		break;
	}

	return err;
}

static const struct file_operations ralink_wdt_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= rt_wdt_w32ite,
	.unlocked_ioctl	= ralink_wdt_ioctl,
	.open		= ralink_wdt_open,
	.release	= ralink_wdt_release,
};

static struct miscdevice ralink_wdt_miscdev = {
	.minor = WATCHDOG_MINOR,
	.name = "watchdog",
	.fops = &ralink_wdt_fops,
};

static int ralink_wdt_probe(struct platform_device *pdev)
{
	struct resource *res;
	int err;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "no memory resource found\n");
		return -EINVAL;
	}

	ralink_wdt_base = ioremap(res->start, resource_size(res));
	if (!ralink_wdt_base)
		return -ENOMEM;

	ralink_wdt_clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(ralink_wdt_clk)) {
		err = PTR_ERR(ralink_wdt_clk);
		goto err_unmap;
	}

	err = clk_enable(ralink_wdt_clk);
	if (err)
		goto err_clk_put;

	ralink_wdt_freq = clk_get_rate(ralink_wdt_clk) / RALINK_WDT_PRESCALE;
	if (!ralink_wdt_freq) {
		err = -EINVAL;
		goto err_clk_disable;
	}

	ralink_wdt_max_timeout = (0xfffful / ralink_wdt_freq);
	if (ralink_wdt_timeout < 1 ||
	    ralink_wdt_timeout > ralink_wdt_max_timeout) {
		ralink_wdt_timeout = ralink_wdt_max_timeout;
		dev_info(&pdev->dev,
			"timeout value must be 0 < timeout <= %d, using %d\n",
			ralink_wdt_max_timeout, ralink_wdt_timeout);
	}

	err = misc_register(&ralink_wdt_miscdev);
	if (err) {
		dev_err(&pdev->dev,
			"unable to register misc device, err=%d\n", err);
		goto err_clk_disable;
	}

	return 0;

err_clk_disable:
	clk_disable(ralink_wdt_clk);
err_clk_put:
	clk_put(ralink_wdt_clk);
err_unmap:
	iounmap(ralink_wdt_base);
	return err;
}

static int ralink_wdt_remove(struct platform_device *pdev)
{
	misc_deregister(&ralink_wdt_miscdev);
	clk_disable(ralink_wdt_clk);
	clk_put(ralink_wdt_clk);
	iounmap(ralink_wdt_base);
	return 0;
}

static void ralink_wdt_shutdown(struct platform_device *pdev)
{
	ralink_wdt_disable();
}

static const struct of_device_id ralink_wdt_match[] = {
	{ .compatible = "ralink,rt2880-wdt" },
	{},
};
MODULE_DEVICE_TABLE(of, ralink_wdt_match);

static struct platform_driver ralink_wdt_driver = {
	.probe		= ralink_wdt_probe,
	.remove		= ralink_wdt_remove,
	.shutdown	= ralink_wdt_shutdown,
	.driver		= {
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
		.of_match_table	= ralink_wdt_match,
	},
};

module_platform_driver(ralink_wdt_driver);

MODULE_DESCRIPTION("MediaTek/Ralink RT288X/RT305X hardware watchdog driver");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
