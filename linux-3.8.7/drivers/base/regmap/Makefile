regmap-core-objs = regmap.o regcache.o regcache-rbtree.o regcache-lzo.o
ifdef CONFIG_DEBUG_FS
regmap-core-objs += regmap-debugfs.o
endif
obj-$(CONFIG_REGMAP) += regmap-core.o
obj-$(CONFIG_REGMAP_I2C) += regmap-i2c.o
obj-$(CONFIG_REGMAP_SPI) += regmap-spi.o
obj-$(CONFIG_REGMAP_MMIO) += regmap-mmio.o
obj-$(CONFIG_REGMAP_IRQ) += regmap-irq.o
