/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2013 John Crispin <blogic@openwrt.org>
 */

#include <linux/kernel.h>
#include <linux/of.h>

#include <asm/mach-ralink/ralink_regs.h>

#include "common.h"

#define SYSC_REG_GPIO_MODE	0x60

static u32 ralink_mux_mask(const char *name, struct ralink_pinmux_grp *grps)
{
	for (; grps->name; grps++)
		if (!strcmp(grps->name, name))
			return grps->mask;

	return 0;
}

void ralink_pinmux(void)
{
	const __be32 *wdt;
	struct device_node *np;
	struct property *prop;
	const char *uart, *pci, *pin;
	u32 mode = 0;

	np = of_find_compatible_node(NULL, NULL, "ralink,rt3050-sysc");
	if (!np)
		return;

	of_property_for_each_string(np, "ralink,gpiomux", prop, pin) {
		int m = ralink_mux_mask(pin, rt_pinmux.mode);
		if (m) {
			mode |= m;
			pr_debug("pinmux: registered gpiomux \"%s\"\n", pin);
		} else {
			pr_err("pinmux: failed to load \"%s\"\n", pin);
		}
	}

	of_property_for_each_string(np, "ralink,pinmux", prop, pin) {
		int m = ralink_mux_mask(pin, rt_pinmux.mode);
		if (m) {
			mode &= ~m;
			pr_debug("pinmux: registered pinmux \"%s\"\n", pin);
		} else {
			pr_err("pinmux: failed to load group \"%s\"\n", pin);
		}
	}

	uart = NULL;
	if (rt_pinmux.uart)
		of_property_read_string(np, "ralink,uartmux", &uart);

	if (uart) {
		int m = ralink_mux_mask(uart, rt_pinmux.uart);

		if (m) {
			mode &= ~(rt_pinmux.uart_mask << rt_pinmux.uart_shift);
			mode |= m << rt_pinmux.uart_shift;
			printk("pinmux: registered uartmux \"%s\"\n", uart);
		} else {
			printk("pinmux: unknown uartmux \"%s\"\n", uart);
		}
	} else {
        printk("pinmux: uart not found ...\n");
    }

	wdt = of_get_property(np, "ralink,wdtmux", NULL);
	if (wdt && *wdt && rt_pinmux.wdt_reset)
		rt_pinmux.wdt_reset();

	pci = NULL;
	if (rt_pinmux.pci)
		of_property_read_string(np, "ralink,pcimux", &pci);

	if (pci) {
		int m = ralink_mux_mask(pci, rt_pinmux.pci);
		mode &= ~(rt_pinmux.pci_mask << rt_pinmux.pci_shift);
		if (m) {
			mode |= (m << rt_pinmux.pci_shift);
			pr_debug("pinmux: registered pcimux \"%s\"\n", pci);
		} else {
			pr_debug("pinmux: registered pcimux \"gpio\"\n");
		}
	}

	rt_sysc_w32(mode, SYSC_REG_GPIO_MODE);
}
