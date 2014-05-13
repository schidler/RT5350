/* Include defines */
#include "ralink-flash.h"

static struct mtd_partition rt2880_partitions[] = {
        {
                name:           "Bootloader",		/* mtdblock0 */
                size:           MTD_BOOT_PART_SIZE,	/* 192K */
                offset:         0
        }, {
                name:           "Config",		/* mtdblock1 */
                size:           MTD_CONFIG_PART_SIZE,	/* 64K */
                offset:         MTDPART_OFS_APPEND
        }, {
                name:           "Factory",		/* mtdblock2 */
                size:           MTD_FACTORY_PART_SIZE,	/* 64K */
                offset:         MTDPART_OFS_APPEND
        }, {
                name:           "Kernel",		/* mtdblock3 */
                size:           MTD_KERN_PART_SIZE,
                offset:         MTDPART_OFS_APPEND
        }
};
