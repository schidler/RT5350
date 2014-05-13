#ifndef __RALINK_FLASH_H__
#define __RALINK_FLASH_H__

#define MTD_BOOT_PART_SIZE	0x30000
#define MTD_CONFIG_PART_SIZE	0x10000
#define MTD_FACTORY_PART_SIZE	0x10000
#if defined (CONFIG_RT2880_FLASH_16M)
#define MTD_STORE_PART_SIZE	0x40000 // 256K
#else
#define MTD_STORE_PART_SIZE	0
#endif

#if defined (CONFIG_RT2880_FLASH_4M)
#define IMAGE1_SIZE		0x400000
#elif defined (CONFIG_RT2880_FLASH_8M)
#define IMAGE1_SIZE		0x800000
#elif defined (CONFIG_RT2880_FLASH_16M)
#define IMAGE1_SIZE		0x1000000
#else
#define IMAGE1_SIZE		CONFIG_MTD_PHYSMAP_LEN
#endif

#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH

#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
#undef  CONFIG_MTD_KERNEL_PART_SIZ
#define CONFIG_MTD_KERNEL_PART_SIZ 0
#endif


#define	MTD_KERN_PART_SIZE	CONFIG_MTD_KERNEL_PART_SIZ
#define MTD_ROOTFS_PART_SIZE	(IMAGE1_SIZE - (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE + MTD_FACTORY_PART_SIZE + CONFIG_MTD_KERNEL_PART_SIZ + MTD_STORE_PART_SIZE))

#else /* in RAM */

#define MTD_KERN_PART_SIZE	(IMAGE1_SIZE - (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE + MTD_FACTORY_PART_SIZE + MTD_STORE_PART_SIZE))
#endif

#endif /* __RALINK_FLASH_H__ */