/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
	chip_id.h
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */

#ifndef __CHIP_ID_H__
#define __CHIP_ID_H__


#define NIC_PCI_VENDOR_ID		0x1814

#define NIC2860_PCI_DEVICE_ID	0x0601
#define NIC2860_PCIe_DEVICE_ID	0x0681
#define NIC2760_PCI_DEVICE_ID	0x0701		/* 1T/2R Cardbus ??? */
#define NIC2790_PCIe_DEVICE_ID  0x0781		/* 1T/2R miniCard */

#define VEN_AWT_PCIe_DEVICE_ID	0x1059
#define VEN_AWT_PCI_VENDOR_ID	0x1A3B

#define EDIMAX_PCI_VENDOR_ID	0x1432

#define NIC3090_PCIe_DEVICE_ID  0x3090		/* 1T/1R miniCard */
#define NIC3091_PCIe_DEVICE_ID  0x3091		/* 1T/2R miniCard */
#define NIC3092_PCIe_DEVICE_ID  0x3092		/* 2T/2R miniCard */
#define NIC3390_PCIe_DEVICE_ID  0x3390		/* 1T/1R miniCard */

#define NIC3062_PCI_DEVICE_ID	0x3062		/* 2T/2R miniCard */
#define NIC3562_PCI_DEVICE_ID	0x3562		/* 2T/2R miniCard */
#define NIC3060_PCI_DEVICE_ID	0x3060		/* 1T/1R miniCard */

#define NIC3592_PCIe_DEVICE_ID	0x3592		/* 2T/2R miniCard */


#define NIC3593_PCI_OR_PCIe_DEVICE_ID	0x3593
#define NIC5390_PCIe_DEVICE_ID	0x5390
#define NIC539F_PCIe_DEVICE_ID 	0x539F
#define NIC5392_PCIe_DEVICE_ID 	0x5392
#define NIC5360_PCI_DEVICE_ID   	0x5360
#define NIC5362_PCI_DEVICE_ID	0x5362

#define NIC5592_PCIe_DEVICE_ID  0x5592

#endif /* __CHIP_ID_H__ */
