/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rtmp_init.c

	Abstract:
	Miniport generic portion header file

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/
#include	"rt_config.h"

#ifdef OS_ABL_FUNC_SUPPORT
/* Os utility link: printk, scanf */
RTMP_OS_ABL_OPS RaOsOps, *pRaOsOps = &RaOsOps;
#endif /* OS_ABL_FUNC_SUPPORT */

#define RT3090A_DEFAULT_INTERNAL_LNA_GAIN	0x0A
UCHAR    NUM_BIT8[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
#ifdef DBG
char*   CipherName[] = {"none","wep64","wep128","TKIP","AES","CKIP64","CKIP128","CKIP152","SMS4"};
#endif


/* BBP register initialization set*/

REG_PAIR   BBPRegTable[] = {
	{BBP_R65,		0x2C},		/* fix rssi issue*/
	{BBP_R66,		0x38},	/* Also set this default value to pAd->BbpTuning.R66CurrentValue at initial*/
	{BBP_R68,		0x0B},  /* improve Rx sensitivity. */
	{BBP_R69,		0x12},
	{BBP_R70,		0xa},	/* BBP_R70 will change to 0x8 in ApStartUp and LinkUp for rt2860C, otherwise value is 0xa*/
	{BBP_R73,		0x10},
	{BBP_R81,		0x37},
	{BBP_R82,		0x62},
	{BBP_R83,		0x6A},
	{BBP_R84,		0x99},	/* 0x19 is for rt2860E and after. This is for extension channel overlapping IOT. 0x99 is for rt2860D and before*/
	{BBP_R86,		0x00},	/* middle range issue, Rory @2008-01-28 	*/
	{BBP_R91,		0x04},	/* middle range issue, Rory @2008-01-28*/
	{BBP_R92,		0x00},	/* middle range issue, Rory @2008-01-28*/
	{BBP_R103,		0x00}, 	/* near range high-power issue, requested from Gary @2008-0528*/
#ifdef RTMP_RBUS_SUPPORT
	{BBP_R105,		0x01},/*kurtis:0x01 ori 0x05 is for rt2860E to turn on FEQ control. It is safe for rt2860D and before, because Bit 7:2 are reserved in rt2860D and before.*/
#else
	{BBP_R105,		0x05},	/* 0x05 is for rt2860E to turn on FEQ control. It is safe for rt2860D and before, because Bit 7:2 are reserved in rt2860D and before.*/
#endif /* RTMP_RBUS_SUPPORT */
#ifdef DOT11_N_SUPPORT
	{BBP_R106,		0x35},	/* Optimizing the Short GI sampling request from Gray @2009-0409*/
#endif /* DOT11_N_SUPPORT */
};
#define	NUM_BBP_REG_PARMS	(sizeof(BBPRegTable) / sizeof(REG_PAIR))



/* 
	ASIC register initialization sets
*/
#ifdef SPECIFIC_BCN_BUF_SUPPORT
RTMP_REG_PAIR	BcnSpecMACRegTable[] =	{
	/* 	
		That means all beacon's size are 512 bytes 
		and their starting address are "0x4000, 0x4200, 0x4400, 0x4600, ....." 
		in the second(higher) 8KB shared memory . 

		The formula is : 0x4000 + BCNx_OFFSET*64
			ex : the address of BSS0 = 0x4000 + 0x00 * 64 = 0x4000
				 the address of BSS1 = 0x4000 + 0x08 * 64 = 0x4200
	*/
	{BCN_OFFSET0,			0x18100800}, 
	{BCN_OFFSET1,			0x38302820}, 
	{BCN_OFFSET2,			0x58504840}, 
	{BCN_OFFSET3,			0x78706860}, 
};
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

RTMP_REG_PAIR	MACRegTable[] =	{
#if defined(HW_BEACON_OFFSET) && (HW_BEACON_OFFSET == 0x200)
	{BCN_OFFSET0,			0xf8f0e8e0}, /* 0x3800(e0), 0x3A00(e8), 0x3C00(f0), 0x3E00(f8), 512B for each beacon */
	{BCN_OFFSET1,			0x6f77d0c8}, /* 0x3200(c8), 0x3400(d0), 0x1DC0(77), 0x1BC0(6f), 512B for each beacon */
#elif defined(HW_BEACON_OFFSET) && (HW_BEACON_OFFSET == 0x100)
	{BCN_OFFSET0,			0xece8e4e0}, /* 0x3800, 0x3A00, 0x3C00, 0x3E00, 512B for each beacon */
	{BCN_OFFSET1,			0xfcf8f4f0}, /* 0x3800, 0x3A00, 0x3C00, 0x3E00, 512B for each beacon */
#endif /* HW_BEACON_OFFSET */

	{LEGACY_BASIC_RATE,		0x0000013f}, /*  Basic rate set bitmap*/
	{HT_BASIC_RATE,		0x00008003}, /* Basic HT rate set , 20M, MCS=3, MM. Format is the same as in TXWI.*/
	{MAC_SYS_CTRL,		0x00}, /* 0x1004, , default Disable RX*/
	{RX_FILTR_CFG,		0x17f97}, /*0x1400  , RX filter control,  */
	{BKOFF_SLOT_CFG,	0x209}, /* default set short slot time, CC_DELAY_TIME should be 2	 */
	/*{TX_SW_CFG0,		0x40a06},  Gary,2006-08-23 */
	{TX_SW_CFG0,		0x0}, 		/* Gary,2008-05-21 for CWC test */
	{TX_SW_CFG1,		0x80606}, /* Gary,2006-08-23 */
	{TX_LINK_CFG,		0x1020},		/* Gary,2006-08-23 */
	/*{TX_TIMEOUT_CFG,	0x00182090},	 CCK has some problem. So increase timieout value. 2006-10-09 MArvek RT*/
	{TX_TIMEOUT_CFG,	0x000a2090},	/* CCK has some problem. So increase timieout value. 2006-10-09 MArvek RT , Modify for 2860E ,2007-08-01*/
	{MAX_LEN_CFG,		MAX_AGGREGATION_SIZE | 0x00001000},	/* 0x3018, MAX frame length. Max PSDU = 16kbytes.*/
	{LED_CFG,		0x7f031e46}, /* Gary, 2006-08-23*/

#ifdef INF_AMAZON_SE
	{PBF_MAX_PCNT,			0x1F3F6F6F}, 	/*iverson modify for usb issue, 2008/09/19*/
											/* 6F + 6F < total page count FE*/
											/* so that RX doesn't occupy TX's buffer space when WMM congestion.*/
#else
	{PBF_MAX_PCNT,			0x1F3FBF9F}, 	/*0x1F3f7f9f},		Jan, 2006/04/20*/
#endif /* INF_AMAZON_SE */
	/*{TX_RTY_CFG,			0x6bb80408},	 Jan, 2006/11/16*/
/* WMM_ACM_SUPPORT*/
/*	{TX_RTY_CFG,			0x6bb80101},	 sample*/
	{TX_RTY_CFG,			0x47d01f0f},	/* Jan, 2006/11/16, Set TxWI->ACK =0 in Probe Rsp Modify for 2860E ,2007-08-03*/
	
	{AUTO_RSP_CFG,			0x00000013},	/* Initial Auto_Responder, because QA will turn off Auto-Responder*/
	{CCK_PROT_CFG,			0x05740003 /*0x01740003*/},	/* Initial Auto_Responder, because QA will turn off Auto-Responder. And RTS threshold is enabled. */
	{OFDM_PROT_CFG,			0x05740003 /*0x01740003*/},	/* Initial Auto_Responder, because QA will turn off Auto-Responder. And RTS threshold is enabled. */
	{GF20_PROT_CFG,			0x01744004},    /* set 19:18 --> Short NAV for MIMO PS*/
	{GF40_PROT_CFG,			0x03F44084},    
	{MM20_PROT_CFG,			0x01744004},    
#ifdef RTMP_MAC_PCI
	{MM40_PROT_CFG,			0x03F54084},	
#endif /* RTMP_MAC_PCI */
	{TXOP_CTRL_CFG,			0x0000583f, /*0x0000243f*/ /*0x000024bf*/},	/*Extension channel backoff.*/
	{TX_RTS_CFG,			0x00092b20},	

	{EXP_ACK_TIME,			0x002400ca},	/* default value */
	{TXOP_HLDR_ET, 			0x00000002},

	/* Jerry comments 2008/01/16: we use SIFS = 10us in CCK defaultly, but it seems that 10us
		is too small for INTEL 2200bg card, so in MBSS mode, the delta time between beacon0
		and beacon1 is SIFS (10us), so if INTEL 2200bg card connects to BSS0, the ping
		will always lost. So we change the SIFS of CCK from 10us to 16us. */
	{XIFS_TIME_CFG,			0x33a41010},
	{PWR_PIN_CFG,			0x00000003},	/* patch for 2880-E*/
	{HT_FBK_CFG1,			0xedc4a980},	// default value
};

#ifdef CONFIG_AP_SUPPORT
RTMP_REG_PAIR	APMACRegTable[] =	{
	{WMM_AIFSN_CFG,		0x00001173},
	{WMM_CWMIN_CFG,	0x00002344},
	{WMM_CWMAX_CFG,	0x000034a6},
	{WMM_TXOP0_CFG,		0x00100020},
	{WMM_TXOP1_CFG,		0x002F0038},
	{TBTT_SYNC_CFG,		0x00012000},
};
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
RTMP_REG_PAIR	STAMACRegTable[] =	{
	{WMM_AIFSN_CFG,		0x00002273},
	{WMM_CWMIN_CFG,	0x00002344},
	{WMM_CWMAX_CFG,	0x000034aa},
};
#endif /* CONFIG_STA_SUPPORT */

#ifdef SPECIFIC_BCN_BUF_SUPPORT
#define	NUM_BCN_SPEC_MAC_REG_PARMS		(sizeof(BcnSpecMACRegTable) / sizeof(RTMP_REG_PAIR))
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

#define	NUM_MAC_REG_PARMS		(sizeof(MACRegTable) / sizeof(RTMP_REG_PAIR))
#ifdef CONFIG_AP_SUPPORT
#define	NUM_AP_MAC_REG_PARMS	(sizeof(APMACRegTable) / sizeof(RTMP_REG_PAIR))
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
#define	NUM_STA_MAC_REG_PARMS	(sizeof(STAMACRegTable) / sizeof(RTMP_REG_PAIR))
#endif /* CONFIG_STA_SUPPORT */


/*
	Use the global variable is not a good solution.
	But we can not put it to pAd and use the lock in pAd of RALINK_TIMER_STRUCT;
	Or when the structure is cleared, we maybe get NULL for pAd and can not lock.
	Maybe we can put pAd in RTMPSetTimer/ RTMPModTimer/ RTMPCancelTimer.
*/
NDIS_SPIN_LOCK TimerSemLock;


/*
	========================================================================
	
	Routine Description:
		Allocate RTMP_ADAPTER data block and do some initialization

	Arguments:
		Adapter		Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS
		NDIS_STATUS_FAILURE

	IRQL = PASSIVE_LEVEL

	Note:
	
	========================================================================
*/
NDIS_STATUS	RTMPAllocAdapterBlock(
	IN  PVOID	handle,
	OUT	VOID	**ppAdapter)
{
	PRTMP_ADAPTER	pAd = NULL;
	NDIS_STATUS		Status;
	INT 			index;
	UCHAR			*pBeaconBuf = NULL;


#ifdef OS_ABL_FUNC_SUPPORT
	/* must put the function before any print message */
	/* init OS utilities provided from UTIL module */
	RtmpOsOpsInit(&RaOsOps);
#endif /* OS_ABL_FUNC_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("--> RTMPAllocAdapterBlock\n"));

	/* init UTIL module */
	RtmpUtilInit();

	*ppAdapter = NULL;

	do
	{
		/* Allocate RTMP_ADAPTER memory block*/
/*		pBeaconBuf = kmalloc(MAX_BEACON_SIZE, MEM_ALLOC_FLAG);*/
		os_alloc_mem(NULL, (UCHAR **)&pBeaconBuf, MAX_BEACON_SIZE);
		if (pBeaconBuf == NULL)
		{
			Status = NDIS_STATUS_FAILURE;
			DBGPRINT_ERR(("Failed to allocate memory - BeaconBuf!\n"));
			break;
		}
		NdisZeroMemory(pBeaconBuf, MAX_BEACON_SIZE);

		Status = AdapterBlockAllocateMemory(handle, (PVOID *)&pAd, sizeof(RTMP_ADAPTER));
		if (Status != NDIS_STATUS_SUCCESS)
		{
			DBGPRINT_ERR(("Failed to allocate memory - ADAPTER\n"));
			break;
		}
		else
		{
			/* init resource list (must be after pAd allocation) */
			initList(&pAd->RscTimerMemList);
			initList(&pAd->RscTaskMemList);
			initList(&pAd->RscLockMemList);
			initList(&pAd->RscTaskletMemList);
			initList(&pAd->RscSemMemList);
			initList(&pAd->RscAtomicMemList);

			initList(&pAd->RscTimerCreateList);

#ifdef WORKQUEUE_BH
			POS_COOKIE cookie;
#endif /* WORKQUEUE_BH */
			pAd->OS_Cookie = handle;
#ifdef WORKQUEUE_BH
			cookie = (POS_COOKIE)(pAd->OS_Cookie);
			cookie->pAd_va = pAd;
#endif /* WORKQUEUE_BH */
		}
		pAd->BeaconBuf = pBeaconBuf;
		DBGPRINT(RT_DEBUG_OFF, ("\n\n=== pAd = %p, size = %d ===\n\n", pAd, (UINT32)sizeof(RTMP_ADAPTER)));

		if (RtmpOsStatsAlloc(&pAd->stats, &pAd->iw_stats) == FALSE)
		{
			Status = NDIS_STATUS_FAILURE;
			break;
		}

		/* Init spin locks*/
		NdisAllocateSpinLock(pAd, &pAd->MgmtRingLock);
#ifdef RTMP_MAC_PCI
		NdisAllocateSpinLock(pAd, &pAd->RxRingLock);
#endif /* RTMP_MAC_PCI */

		for (index =0 ; index < NUM_OF_TX_RING; index++)
		{
			NdisAllocateSpinLock(pAd, &pAd->TxSwQueueLock[index]);
			NdisAllocateSpinLock(pAd, &pAd->DeQueueLock[index]);
			pAd->DeQueueRunning[index] = FALSE;
		}

#ifdef RESOURCE_PRE_ALLOC
		/* 
			move this function from rt28xx_init() to here. now this function only allocate memory and
			leave the initialization job to RTMPInitTxRxRingMemory() which called in rt28xx_init().
		*/
		Status = RTMPAllocTxRxRingMemory(pAd);
		if (Status != NDIS_STATUS_SUCCESS)
		{
			DBGPRINT_ERR(("Failed to allocate memory - TxRxRing\n"));
			break;
		}
#endif /* RESOURCE_PRE_ALLOC */

		NdisAllocateSpinLock(pAd, &pAd->irq_lock);
		NdisAllocateSpinLock(pAd, &TimerSemLock);

#ifdef SPECIFIC_BCN_BUF_SUPPORT
		NdisAllocateSpinLock(pAd, &pAd->ShrMemLock);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */


#ifdef RALINK_ATE
#endif /* RALINK_ATE */

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_AP_SUPPORT
/*        UAPSD_Init(pAd);  move to rt28xx_init*/
#endif /* UAPSD_AP_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

		/* assign function pointers*/
#ifdef MAT_SUPPORT
		/* init function pointers, used in OS_ABL */
/*		RTMP_MATOpsInit(pAd);  move to rt28xx_init*/
#endif /* MAT_SUPPORT */
	} while (FALSE);

	if ((Status != NDIS_STATUS_SUCCESS) && (pBeaconBuf))
/*		kfree(pBeaconBuf);*/
		os_free_mem(NULL, pBeaconBuf);

	if ((Status != NDIS_STATUS_SUCCESS) && (pAd != NULL))
	{
		if (pAd->stats != NULL)
			os_free_mem(NULL, pAd->stats);

		if (pAd->iw_stats != NULL)
			os_free_mem(NULL, pAd->iw_stats);
	}

	if (pAd != NULL) /* compile warning: avoid use NULL pointer when pAd == NULL */
		*ppAdapter = (VOID *)pAd;


	/*
		Init ProbeRespIE Table
	*/
	for (index = 0; index < MAX_LEN_OF_BSS_TABLE; index++) 
	{
		if (os_alloc_mem(pAd,&pAd->ProbeRespIE[index].pIe, MAX_VIE_LEN) == NDIS_STATUS_SUCCESS)
			RTMPZeroMemory(pAd->ProbeRespIE[index].pIe, MAX_VIE_LEN);
		else
			pAd->ProbeRespIE[index].pIe = NULL;
	}	

	DBGPRINT_S(Status, ("<-- RTMPAllocAdapterBlock, Status=%x\n", Status));
	return Status;
}




/*
	========================================================================
	
	Routine Description:
		Read initial Tx power per MCS and BW from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	RTMPReadTxPwrPerRate(
	IN	PRTMP_ADAPTER	pAd)
{
	ULONG		data, Adata, Gdata;
	USHORT		i, value, value2;
	USHORT		value_1, value_2, value_3, value_4;
	INT			Apwrdelta, Gpwrdelta;
	UCHAR		t1,t2,t3,t4;
	BOOLEAN		bApwrdeltaMinus = TRUE, bGpwrdeltaMinus = TRUE;
	
	{	
		
		/* Get power delta for 20MHz and 40MHz.*/
		
		DBGPRINT(RT_DEBUG_TRACE, ("Txpower per Rate\n"));
		RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_DELTA, value2);
		Apwrdelta = 0;
		Gpwrdelta = 0;

		if ((value2 & 0xff) != 0xff)
		{
			if ((value2 & 0x80))
				Gpwrdelta = (value2&0xf);
			
			if ((value2 & 0x40))
				bGpwrdeltaMinus = FALSE;
			else
				bGpwrdeltaMinus = TRUE;
		}
		if ((value2 & 0xff00) != 0xff00)
		{
			if ((value2 & 0x8000))
				Apwrdelta = ((value2&0xf00)>>8);

			if ((value2 & 0x4000))
				bApwrdeltaMinus = FALSE;
			else
				bApwrdeltaMinus = TRUE;
		}	
		DBGPRINT(RT_DEBUG_TRACE, ("Gpwrdelta = %x, Apwrdelta = %x .\n", Gpwrdelta, Apwrdelta));

		
		/* Get Txpower per MCS for 20MHz in 2.4G.*/
		
		for (i=0; i<5; i++)
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_BYRATE_20MHZ_2_4G + i*4, value);
			data = value;

			/* use value_1 ~ value_4 for code size reduce */
			value_1 = value&0xf;
			value_2 = (value&0xf0)>>4;
			value_3 = (value&0xf00)>>8;
			value_4 = (value&0xf000)>>12;

			if (bApwrdeltaMinus == FALSE)
			{
				t1 = value_1+(Apwrdelta);
				if (t1 > 0xf)
					t1 = 0xf;
				t2 = value_2+(Apwrdelta);
				if (t2 > 0xf)
					t2 = 0xf;
				t3 = value_3+(Apwrdelta);
				if (t3 > 0xf)
					t3 = 0xf;
				t4 = value_4+(Apwrdelta);
				if (t4 > 0xf)
					t4 = 0xf;
			}
			else
			{
				if (value_1 > Apwrdelta)
					t1 = value_1-(Apwrdelta);
				else
					t1 = 0;
				if (value_2 > Apwrdelta)
					t2 = value_2-(Apwrdelta);
				else
					t2 = 0;
				if (value_3 > Apwrdelta)
					t3 = value_3-(Apwrdelta);
				else
					t3 = 0;
				if (value_4 > Apwrdelta)
					t4 = value_4-(Apwrdelta);
				else
					t4 = 0;
			}				
			Adata = t1 + (t2<<4) + (t3<<8) + (t4<<12);
			if (bGpwrdeltaMinus == FALSE)
			{
				t1 = value_1+(Gpwrdelta);
				if (t1 > 0xf)
					t1 = 0xf;
				t2 = value_2+(Gpwrdelta);
				if (t2 > 0xf)
					t2 = 0xf;
				t3 = value_3+(Gpwrdelta);
				if (t3 > 0xf)
					t3 = 0xf;
				t4 = value_4+(Gpwrdelta);
				if (t4 > 0xf)
					t4 = 0xf;
			}
			else
			{
				if (value_1 > Gpwrdelta)
					t1 = value_1-(Gpwrdelta);
				else
					t1 = 0;
				if (value_2 > Gpwrdelta)
					t2 = value_2-(Gpwrdelta);
				else
					t2 = 0;
				if (value_3 > Gpwrdelta)
					t3 = value_3-(Gpwrdelta);
				else
					t3 = 0;
				if (value_4 > Gpwrdelta)
					t4 = value_4-(Gpwrdelta);
				else
					t4 = 0;
			}				
			Gdata = t1 + (t2<<4) + (t3<<8) + (t4<<12);
			
			RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_BYRATE_20MHZ_2_4G + i*4 + 2, value);

			/* use value_1 ~ value_4 for code size reduce */
			value_1 = value&0xf;
			value_2 = (value&0xf0)>>4;
			value_3 = (value&0xf00)>>8;
			value_4 = (value&0xf000)>>12;

			if (bApwrdeltaMinus == FALSE)
			{
				t1 = value_1+(Apwrdelta);
				if (t1 > 0xf)
					t1 = 0xf;
				t2 = value_2+(Apwrdelta);
				if (t2 > 0xf)
					t2 = 0xf;
				t3 = value_3+(Apwrdelta);
				if (t3 > 0xf)
					t3 = 0xf;
				t4 = value_4+(Apwrdelta);
				if (t4 > 0xf)
					t4 = 0xf;
			}
			else
			{
				if (value_1 > Apwrdelta)
					t1 = value_1-(Apwrdelta);
				else
					t1 = 0;
				if (value_2 > Apwrdelta)
					t2 = value_2-(Apwrdelta);
				else
					t2 = 0;
				if (value_3 > Apwrdelta)
					t3 = value_3-(Apwrdelta);
				else
					t3 = 0;
				if (value_4 > Apwrdelta)
					t4 = value_4-(Apwrdelta);
				else
					t4 = 0;
			}				
			Adata |= ((t1<<16) + (t2<<20) + (t3<<24) + (t4<<28));
			if (bGpwrdeltaMinus == FALSE)
			{
				t1 = value_1+(Gpwrdelta);
				if (t1 > 0xf)
					t1 = 0xf;
				t2 = value_2+(Gpwrdelta);
				if (t2 > 0xf)
					t2 = 0xf;
				t3 = value_3+(Gpwrdelta);
				if (t3 > 0xf)
					t3 = 0xf;
				t4 = value_4+(Gpwrdelta);
				if (t4 > 0xf)
					t4 = 0xf;
			}
			else
			{
				if (value_1 > Gpwrdelta)
					t1 = value_1-(Gpwrdelta);
				else
					t1 = 0;
				if (value_2 > Gpwrdelta)
					t2 = value_2-(Gpwrdelta);
				else
					t2 = 0;
				if (value_3 > Gpwrdelta)
					t3 = value_3-(Gpwrdelta);
				else
					t3 = 0;
				if (value_4 > Gpwrdelta)
					t4 = value_4-(Gpwrdelta);
				else
					t4 = 0;
			}				
			Gdata |= ((t1<<16) + (t2<<20) + (t3<<24) + (t4<<28));
			data |= (value<<16);

			/* For 20M/40M Power Delta issue */		
			pAd->Tx20MPwrCfgABand[i] = data;
			pAd->Tx20MPwrCfgGBand[i] = data;
			pAd->Tx40MPwrCfgABand[i] = Adata;
			pAd->Tx40MPwrCfgGBand[i] = Gdata;
			
			if (data != 0xffffffff)
				RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + i*4, data);
			DBGPRINT_RAW(RT_DEBUG_TRACE, ("20MHz BW, 2.4G band-%lx,  Adata = %lx,  Gdata = %lx \n", data, Adata, Gdata));
		}
	}
}


/*
	========================================================================
	
	Routine Description:
		Read initial channel power parameters from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	RTMPReadChannelPwr(
	IN	PRTMP_ADAPTER	pAd)
{
	UINT32					i, choffset;
	EEPROM_TX_PWR_STRUC	    Power;
	EEPROM_TX_PWR_STRUC	    Power2;

	/* Read Tx power value for all channels*/
	/* Value from 1 - 0x7f. Default value is 24.*/
	/* Power value : 2.4G 0x00 (0) ~ 0x1F (31)*/
	/*             : 5.5G 0xF9 (-7) ~ 0x0F (15)*/

	/* 0. 11b/g, ch1 - ch 14*/
	for (i = 0; i < 7; i++)
	{
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX_PWR_OFFSET + i * 2, Power.word);
			RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX2_PWR_OFFSET + i * 2, Power2.word);
			pAd->TxPower[i * 2].Channel = i * 2 + 1;
			pAd->TxPower[i * 2 + 1].Channel = i * 2 + 2;

			if ((Power.field.Byte0 > 31) || (Power.field.Byte0 < 0))
				pAd->TxPower[i * 2].Power = DEFAULT_RF_TX_POWER;
			else
				pAd->TxPower[i * 2].Power = Power.field.Byte0;

			if ((Power.field.Byte1 > 31) || (Power.field.Byte1 < 0))
				pAd->TxPower[i * 2 + 1].Power = DEFAULT_RF_TX_POWER;
			else
				pAd->TxPower[i * 2 + 1].Power = Power.field.Byte1;

			if ((Power2.field.Byte0 > 31) || (Power2.field.Byte0 < 0))
				pAd->TxPower[i * 2].Power2 = DEFAULT_RF_TX_POWER;
			else
				pAd->TxPower[i * 2].Power2 = Power2.field.Byte0;

			if ((Power2.field.Byte1 > 31) || (Power2.field.Byte1 < 0))
				pAd->TxPower[i * 2 + 1].Power2 = DEFAULT_RF_TX_POWER;
			else
				pAd->TxPower[i * 2 + 1].Power2 = Power2.field.Byte1;
		}
	}
	
	{
		/* 1. U-NII lower/middle band: 36, 38, 40; 44, 46, 48; 52, 54, 56; 60, 62, 64 (including central frequency in BW 40MHz)*/
		/* 1.1 Fill up channel*/
		choffset = 14;
		for (i = 0; i < 4; i++)
		{
			pAd->TxPower[3 * i + choffset + 0].Channel	= 36 + i * 8 + 0;
			pAd->TxPower[3 * i + choffset + 0].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 0].Power2	= DEFAULT_RF_TX_POWER;

			pAd->TxPower[3 * i + choffset + 1].Channel	= 36 + i * 8 + 2;
			pAd->TxPower[3 * i + choffset + 1].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 1].Power2	= DEFAULT_RF_TX_POWER;

			pAd->TxPower[3 * i + choffset + 2].Channel	= 36 + i * 8 + 4;
			pAd->TxPower[3 * i + choffset + 2].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 2].Power2	= DEFAULT_RF_TX_POWER;
		}

		/* 1.2 Fill up power*/
		for (i = 0; i < 6; i++)
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + i * 2, Power.word);
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + i * 2, Power2.word);

			if ((Power.field.Byte0 < 16) && (Power.field.Byte0 >= -7))
				pAd->TxPower[i * 2 + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 < 16) && (Power.field.Byte1 >= -7))
				pAd->TxPower[i * 2 + choffset + 1].Power = Power.field.Byte1;			

			if ((Power2.field.Byte0 < 16) && (Power2.field.Byte0 >= -7))
				pAd->TxPower[i * 2 + choffset + 0].Power2 = Power2.field.Byte0;

			if ((Power2.field.Byte1 < 16) && (Power2.field.Byte1 >= -7))
				pAd->TxPower[i * 2 + choffset + 1].Power2 = Power2.field.Byte1;			
		}
		
		/* 2. HipperLAN 2 100, 102 ,104; 108, 110, 112; 116, 118, 120; 124, 126, 128; 132, 134, 136; 140 (including central frequency in BW 40MHz)*/
		/* 2.1 Fill up channel*/
		choffset = 14 + 12;
		for (i = 0; i < 5; i++)
		{
			pAd->TxPower[3 * i + choffset + 0].Channel	= 100 + i * 8 + 0;
			pAd->TxPower[3 * i + choffset + 0].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 0].Power2	= DEFAULT_RF_TX_POWER;

			pAd->TxPower[3 * i + choffset + 1].Channel	= 100 + i * 8 + 2;
			pAd->TxPower[3 * i + choffset + 1].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 1].Power2	= DEFAULT_RF_TX_POWER;

			pAd->TxPower[3 * i + choffset + 2].Channel	= 100 + i * 8 + 4;
			pAd->TxPower[3 * i + choffset + 2].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 2].Power2	= DEFAULT_RF_TX_POWER;
		}
		pAd->TxPower[3 * 5 + choffset + 0].Channel		= 140;
		pAd->TxPower[3 * 5 + choffset + 0].Power		= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * 5 + choffset + 0].Power2		= DEFAULT_RF_TX_POWER;

		/* 2.2 Fill up power*/
		for (i = 0; i < 8; i++)
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + i * 2, Power.word);
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + (choffset - 14) + i * 2, Power2.word);

			if ((Power.field.Byte0 < 16) && (Power.field.Byte0 >= -7))
				pAd->TxPower[i * 2 + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 < 16) && (Power.field.Byte1 >= -7))
				pAd->TxPower[i * 2 + choffset + 1].Power = Power.field.Byte1;			

			if ((Power2.field.Byte0 < 16) && (Power2.field.Byte0 >= -7))
				pAd->TxPower[i * 2 + choffset + 0].Power2 = Power2.field.Byte0;

			if ((Power2.field.Byte1 < 16) && (Power2.field.Byte1 >= -7))
				pAd->TxPower[i * 2 + choffset + 1].Power2 = Power2.field.Byte1;			
		}

		/* 3. U-NII upper band: 149, 151, 153; 157, 159, 161; 165, 167, 169; 171, 173 (including central frequency in BW 40MHz)*/
		/* 3.1 Fill up channel*/
		choffset = 14 + 12 + 16;
		/*for (i = 0; i < 2; i++)*/
		for (i = 0; i < 3; i++)
		{
			pAd->TxPower[3 * i + choffset + 0].Channel	= 149 + i * 8 + 0;
			pAd->TxPower[3 * i + choffset + 0].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 0].Power2	= DEFAULT_RF_TX_POWER;

			pAd->TxPower[3 * i + choffset + 1].Channel	= 149 + i * 8 + 2;
			pAd->TxPower[3 * i + choffset + 1].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 1].Power2	= DEFAULT_RF_TX_POWER;

			pAd->TxPower[3 * i + choffset + 2].Channel	= 149 + i * 8 + 4;
			pAd->TxPower[3 * i + choffset + 2].Power	= DEFAULT_RF_TX_POWER;
			pAd->TxPower[3 * i + choffset + 2].Power2	= DEFAULT_RF_TX_POWER;
		}
		pAd->TxPower[3 * 3 + choffset + 0].Channel		= 171;
		pAd->TxPower[3 * 3 + choffset + 0].Power		= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * 3 + choffset + 0].Power2		= DEFAULT_RF_TX_POWER;

		pAd->TxPower[3 * 3 + choffset + 1].Channel		= 173;
		pAd->TxPower[3 * 3 + choffset + 1].Power		= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * 3 + choffset + 1].Power2		= DEFAULT_RF_TX_POWER;

		/* 3.2 Fill up power*/
		/*for (i = 0; i < 4; i++)*/
		for (i = 0; i < 6; i++)
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + i * 2, Power.word);
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + (choffset - 14) + i * 2, Power2.word);

			if ((Power.field.Byte0 < 16) && (Power.field.Byte0 >= -7))
				pAd->TxPower[i * 2 + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 < 16) && (Power.field.Byte1 >= -7))
				pAd->TxPower[i * 2 + choffset + 1].Power = Power.field.Byte1;			

			if ((Power2.field.Byte0 < 16) && (Power2.field.Byte0 >= -7))
				pAd->TxPower[i * 2 + choffset + 0].Power2 = Power2.field.Byte0;

			if ((Power2.field.Byte1 < 16) && (Power2.field.Byte1 >= -7))
				pAd->TxPower[i * 2 + choffset + 1].Power2 = Power2.field.Byte1;			
		}
	}


	/* 4. Print and Debug*/
	/*choffset = 14 + 12 + 16 + 7;*/
	choffset = 14 + 12 + 16 + 11;
	

}


/*
	========================================================================
	
	Routine Description:
		Read initial parameters from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	NICReadEEPROMParameters(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			mac_addr)
{
	UINT32			data = 0;
	USHORT			i, value, value2;
	EEPROM_TX_PWR_STRUC	    Power;
	EEPROM_VERSION_STRUC    Version;
	EEPROM_ANTENNA_STRUC	Antenna;
	EEPROM_NIC_CONFIG2_STRUC    NicConfig2;
#ifdef CONFIG_RT2860V2_SET_MAC_FROM_EEPROM
	USHORT  Addr01,Addr23,Addr45 ;
#endif
	MAC_DW0_STRUC csr2;
	MAC_DW1_STRUC csr3;


	DBGPRINT(RT_DEBUG_TRACE, ("--> NICReadEEPROMParameters\n"));	

	if (pAd->chipOps.AsicEeBufferInit)
		pAd->chipOps.AsicEeBufferInit(pAd);

	if (pAd->chipOps.eeinit)
	{
		pAd->chipOps.eeinit(pAd);
#ifdef RTMP_EFUSE_SUPPORT
#endif /* RTMP_EFUSE_SUPPORT */
	}

	/* Init EEPROM Address Number, before access EEPROM; if 93c46, EEPROMAddressNum=6, else if 93c66, EEPROMAddressNum=8*/
	RTMP_IO_READ32(pAd, E2PROM_CSR, &data);
	DBGPRINT(RT_DEBUG_TRACE, ("--> E2PROM_CSR = 0x%x\n", data));

	if((data & 0x30) == 0)
		pAd->EEPROMAddressNum = 6;		/* 93C46*/
	else if((data & 0x30) == 0x10)
		pAd->EEPROMAddressNum = 8;     /* 93C66*/
	else
		pAd->EEPROMAddressNum = 8;     /* 93C86*/
	DBGPRINT(RT_DEBUG_TRACE, ("--> EEPROMAddressNum = %d\n", pAd->EEPROMAddressNum ));

#ifdef CONFIG_RT2860V2_SET_MAC_FROM_EEPROM
	/* Read MAC setting from EEPROM and record as permanent MAC address */
	DBGPRINT(RT_DEBUG_TRACE, ("Initialize MAC Address from E2PROM \n"));

	RT28xx_EEPROM_READ16(pAd, 0x04, Addr01);
	RT28xx_EEPROM_READ16(pAd, 0x06, Addr23);
	RT28xx_EEPROM_READ16(pAd, 0x08, Addr45);

	pAd->PermanentAddress[0] = (UCHAR)(Addr01 & 0xff);
	pAd->PermanentAddress[1] = (UCHAR)(Addr01 >> 8);
	pAd->PermanentAddress[2] = (UCHAR)(Addr23 & 0xff);
	pAd->PermanentAddress[3] = (UCHAR)(Addr23 >> 8);
	pAd->PermanentAddress[4] = (UCHAR)(Addr45 & 0xff);
	pAd->PermanentAddress[5] = (UCHAR)(Addr45 >> 8);

	/*more conveninet to test mbssid, so ap's bssid &0xf1*/
	if (pAd->PermanentAddress[0] == 0xff)
		pAd->PermanentAddress[0] = RandomByte(pAd)&0xf8;
			
	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM MAC: =%02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(pAd->PermanentAddress)));

	/* Assign the actually working MAC Address */
	if (pAd->bLocalAdminMAC)
		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from Configuration file(.dat). \n"));
	else 
#endif
	if (mac_addr &&  strlen((PSTRING)mac_addr) == 17 && (strcmp(mac_addr, "00:00:00:00:00:00") != 0))
	{
		INT		j;
		PSTRING	macptr;

		macptr = (PSTRING) mac_addr;

		for (j=0; j<MAC_ADDR_LEN; j++)
		{
			AtoH(macptr, &pAd->CurrentAddress[j], 1);
			macptr=macptr+3;
		}	
		
		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from Moudle Parameter. \n"));
	}
#ifdef CONFIG_RT2860V2_SET_MAC_FROM_EEPROM
	else {
		COPY_MAC_ADDR(pAd->CurrentAddress, pAd->PermanentAddress);
		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from EEPROM. \n"));
	}
#endif

/* FIX: For correct work APCLI/MBSSID need correct MAC */
#ifndef NEW_MBSSID_MODE
#if defined(MBSS_SUPPORT)
	/* Test MAC[5] for 8 MAC support */
	if ((pAd->CurrentAddress[5] % 8) != 0)
	{
	    pAd->CurrentAddress[0] |= 0x02;
	    pAd->CurrentAddress[5] &= 0xf8;
	}
#elif defined(APCLI_SUPPORT)
	/* Test MAC[5] for 2 MAC support */
	if ((pAd->CurrentAddress[5] % 2) != 0)
	{
	    pAd->CurrentAddress[0] |= 0x02;
	    pAd->CurrentAddress[5] &= 0xfe;
        }
#endif
#endif

	/* Set the current MAC to ASIC */
	csr2.field.Byte0 = pAd->CurrentAddress[0];
	csr2.field.Byte1 = pAd->CurrentAddress[1];
	csr2.field.Byte2 = pAd->CurrentAddress[2];
	csr2.field.Byte3 = pAd->CurrentAddress[3];
	RTMP_IO_WRITE32(pAd, MAC_ADDR_DW0, csr2.word);
	csr3.word = 0;
	csr3.field.Byte4 = pAd->CurrentAddress[4];
	csr3.field.Byte5 = pAd->CurrentAddress[5];
	csr3.field.U2MeMask = 0xff;
	RTMP_IO_WRITE32(pAd, MAC_ADDR_DW1, csr3.word);
	DBGPRINT_RAW(RT_DEBUG_TRACE,("Current MAC: =%02x:%02x:%02x:%02x:%02x:%02x\n",
					PRINT_MAC(pAd->CurrentAddress)));

	/* if not return early. cause fail at emulation.*/
	/* Init the channel number for TX channel power*/
		RTMPReadChannelPwr(pAd);

	/* if E2PROM version mismatch with driver's expectation, then skip*/
	/* all subsequent E2RPOM retieval and set a system error bit to notify GUI*/
	RT28xx_EEPROM_READ16(pAd, EEPROM_VERSION_OFFSET, Version.word);
	pAd->EepromVersion = Version.field.Version + Version.field.FaeReleaseNumber * 256;
	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: Version = %d, FAE release #%d\n", Version.field.Version, Version.field.FaeReleaseNumber));

	if (Version.field.Version > VALID_EEPROM_VERSION)
	{
		DBGPRINT_ERR(("E2PROM: WRONG VERSION 0x%x, should be %d\n",Version.field.Version, VALID_EEPROM_VERSION));
	}

	/* Read BBP default value from EEPROM and store to array(EEPROMDefaultValue) in pAd*/
	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC1_OFFSET, value);
	pAd->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET] = value;

	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC2_OFFSET, value);
	pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET] = value;

	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_COUNTRY_REGION, value);	/* Country Region*/
		pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] = value;
	}


	for(i = 0; i < 8; i++)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_BBP_BASE_OFFSET + i*2, value);
		pAd->EEPROMDefaultValue[i+EEPROM_BBP_ARRAY_OFFSET] = value;
	}

	/* We have to parse NIC configuration 0 at here.*/
	/* If TSSI did not have preloaded value, it should reset the TxAutoAgc to false*/
	/* Therefore, we have to read TxAutoAgc control beforehand.*/
	/* Read Tx AGC control bit*/
	Antenna.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG1_OFFSET];


	/* if (Antenna.word == 0xFFFF) */
	if ((Antenna.word & 0x0F00) == 0x0F00)
	{
		RTMP_CHIP_ANTENNA_INFO_DEFAULT_RESET(pAd, &Antenna);
	}

	/* Choose the desired Tx&Rx stream.*/
	if ((pAd->CommonCfg.TxStream == 0) || (pAd->CommonCfg.TxStream > Antenna.field.TxPath))
		pAd->CommonCfg.TxStream = Antenna.field.TxPath;

	if ((pAd->CommonCfg.RxStream == 0) || (pAd->CommonCfg.RxStream > Antenna.field.RxPath))
	{
		pAd->CommonCfg.RxStream = Antenna.field.RxPath;

		if ((pAd->MACVersion != RALINK_3883_VERSION) &&
			(pAd->MACVersion != RALINK_2883_VERSION) &&
			(pAd->CommonCfg.RxStream > 2))
		{
			/* only 2 Rx streams for RT2860 series*/
			pAd->CommonCfg.RxStream = 2;
		}
	}


	/* EEPROM offset 0x36 - NIC Configuration 1 */
	NicConfig2.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET];

#ifdef WSC_INCLUDED
	/* WSC hardware push button function 0811 */
	if ((pAd->MACVersion == 0x28600100) || (pAd->MACVersion == 0x28700100))
	WSC_HDR_BTN_MR_HDR_SUPPORT_SET(pAd, NicConfig2.field.EnableWPSPBC);
#endif /* WSC_INCLUDED */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (NicConfig2.word == 0xffff)
		{
			NicConfig2.word = 0;
		}
	}
#endif /* CONFIG_AP_SUPPORT */	

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if ((NicConfig2.word & 0x00ff) == 0xff)
		{
			NicConfig2.word &= 0xff00;
		}

		if ((NicConfig2.word >> 8) == 0xff)
		{
			NicConfig2.word &= 0x00ff;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	if (NicConfig2.field.DynamicTxAgcControl == 1)
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = TRUE;
	else
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = FALSE;
	
	/* Save value for future using */
	pAd->NicConfig2.word = NicConfig2.word;
	
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("NICReadEEPROMParameters: RxPath = %d, TxPath = %d\n", Antenna.field.RxPath, Antenna.field.TxPath));

	/* Save the antenna for future use*/
	pAd->Antenna.word = Antenna.word;

	/* Set the RfICType here, then we can initialize RFIC related operation callbacks*/
	pAd->Mlme.RealRxPath = (UCHAR) Antenna.field.RxPath;

	pAd->RfIcType = (UCHAR) Antenna.field.RfIcType;

	/* check if the chip supports 5G band */
	if (PHY_MODE_IS_5G_BAND(pAd->CommonCfg.PhyMode))
	{
		if (!RFIC_IS_5G_BAND(pAd))
		{
			DBGPRINT_RAW(RT_DEBUG_ERROR,
						("phy mode> Error! The chip does not support 5G band %d!\n",
						pAd->RfIcType));
#ifdef DOT11_N_SUPPORT
			/* change to bgn mode */
			Set_WirelessMode_Proc(pAd, "9");
#else
			/* change to bg mode */
			Set_WirelessMode_Proc(pAd, "0");
#endif /* DOT11_N_SUPPORT */
		}
	}


	RTMP_NET_DEV_NICKNAME_INIT(pAd);


	/* Shall activate one time in rt28xx_int() */
	//RtmpChipOpsHook(pAd);

	
	/* Reset PhyMode if we don't support 802.11a*/
	/* Only RFIC_2850 & RFIC_2750 support 802.11a*/
	
	if ((Antenna.field.RfIcType != RFIC_2850)
		&& (Antenna.field.RfIcType != RFIC_2750)
		&& (Antenna.field.RfIcType != RFIC_3052)
		&& (Antenna.field.RfIcType != RFIC_2853)
		)
	{
		if ((pAd->CommonCfg.PhyMode == PHY_11ABG_MIXED) || 
			(pAd->CommonCfg.PhyMode == PHY_11A))
			pAd->CommonCfg.PhyMode = PHY_11BG_MIXED;
#ifdef DOT11_N_SUPPORT
		else if ((pAd->CommonCfg.PhyMode == PHY_11ABGN_MIXED)	|| 
				 (pAd->CommonCfg.PhyMode == PHY_11AN_MIXED) 	|| 
				 (pAd->CommonCfg.PhyMode == PHY_11AGN_MIXED) 	||
				 (pAd->CommonCfg.PhyMode == PHY_11N_5G))
			pAd->CommonCfg.PhyMode = PHY_11BGN_MIXED;
#endif /* DOT11_N_SUPPORT */

		pAd->RFICType = RFIC_24GHZ; /* CRDA*/
	}
	else
	{
		pAd->RFICType = RFIC_24GHZ | RFIC_5GHZ; /* CRDA*/
	}
	
	/* Read TSSI reference and TSSI boundary for temperature compensation. This is ugly*/
	/* 0. 11b/g*/
	{
		/* these are tempature reference value (0x00 ~ 0xFE)
		   ex: 0x00 0x15 0x25 0x45 0x88 0xA0 0xB5 0xD0 0xF0
		   TssiPlusBoundaryG [4] [3] [2] [1] [0] (smaller) +
		   TssiMinusBoundaryG[0] [1] [2] [3] [4] (larger) */
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND1, Power.word);
			pAd->TssiMinusBoundaryG[4] = Power.field.Byte0;
			pAd->TssiMinusBoundaryG[3] = Power.field.Byte1;
			RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND2, Power.word);
			pAd->TssiMinusBoundaryG[2] = Power.field.Byte0;
			pAd->TssiMinusBoundaryG[1] = Power.field.Byte1;
			RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND3, Power.word);
			pAd->TssiRefG   = Power.field.Byte0; /* reference value [0] */
			pAd->TssiPlusBoundaryG[1] = Power.field.Byte1;
			RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND4, Power.word);
			pAd->TssiPlusBoundaryG[2] = Power.field.Byte0;
			pAd->TssiPlusBoundaryG[3] = Power.field.Byte1;
			RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND5, Power.word);
			pAd->TssiPlusBoundaryG[4] = Power.field.Byte0;
			pAd->TxAgcStepG = Power.field.Byte1;    
			pAd->TxAgcCompensateG = 0;
			pAd->TssiMinusBoundaryG[0] = pAd->TssiRefG;
			pAd->TssiPlusBoundaryG[0]  = pAd->TssiRefG;

			/* Disable TxAgc if the based value is not right*/
			if (pAd->TssiRefG == 0xff)
				pAd->bAutoTxAgcG = FALSE;
		}

		DBGPRINT(RT_DEBUG_TRACE,("E2PROM: G Tssi[-4 .. +4] = %d %d %d %d - %d -%d %d %d %d, step=%d, tuning=%d\n",
			pAd->TssiMinusBoundaryG[4], pAd->TssiMinusBoundaryG[3], pAd->TssiMinusBoundaryG[2], pAd->TssiMinusBoundaryG[1],
			pAd->TssiRefG,
			pAd->TssiPlusBoundaryG[1], pAd->TssiPlusBoundaryG[2], pAd->TssiPlusBoundaryG[3], pAd->TssiPlusBoundaryG[4],
			pAd->TxAgcStepG, pAd->bAutoTxAgcG));
	}	
	/* 1. 11a*/
	{
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND1, Power.word);
			pAd->TssiMinusBoundaryA[4] = Power.field.Byte0;
			pAd->TssiMinusBoundaryA[3] = Power.field.Byte1;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND2, Power.word);
			pAd->TssiMinusBoundaryA[2] = Power.field.Byte0;
			pAd->TssiMinusBoundaryA[1] = Power.field.Byte1;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND3, Power.word);
			pAd->TssiRefA = Power.field.Byte0;
			pAd->TssiPlusBoundaryA[1] = Power.field.Byte1;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND4, Power.word);
			pAd->TssiPlusBoundaryA[2] = Power.field.Byte0;
			pAd->TssiPlusBoundaryA[3] = Power.field.Byte1;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND5, Power.word);
			pAd->TssiPlusBoundaryA[4] = Power.field.Byte0;
			pAd->TxAgcStepA = Power.field.Byte1;    
			pAd->TxAgcCompensateA = 0;
			pAd->TssiMinusBoundaryA[0] = pAd->TssiRefA;
			pAd->TssiPlusBoundaryA[0]  = pAd->TssiRefA;

			/* Disable TxAgc if the based value is not right*/
			if (pAd->TssiRefA == 0xff)
				pAd->bAutoTxAgcA = FALSE;
		}

		DBGPRINT(RT_DEBUG_TRACE,("E2PROM: A Tssi[-4 .. +4] = %d %d %d %d - %d -%d %d %d %d, step=%d, tuning=%d\n",
			pAd->TssiMinusBoundaryA[4], pAd->TssiMinusBoundaryA[3], pAd->TssiMinusBoundaryA[2], pAd->TssiMinusBoundaryA[1],
			pAd->TssiRefA,
			pAd->TssiPlusBoundaryA[1], pAd->TssiPlusBoundaryA[2], pAd->TssiPlusBoundaryA[3], pAd->TssiPlusBoundaryA[4],
			pAd->TxAgcStepA, pAd->bAutoTxAgcA));
	}	
	pAd->BbpRssiToDbmDelta = 0x0;
	
	/* Read frequency offset setting for RF*/
		RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, value);

	if ((value & 0x00FF) != 0x00FF)
		pAd->RfFreqOffset = (ULONG) (value & 0x00FF);
	else
		pAd->RfFreqOffset = 0;
#ifdef RTMP_RBUS_SUPPORT
	if (pAd->RfFreqDelta & 0x10)
	{
		pAd->RfFreqOffset = (pAd->RfFreqOffset >= pAd->RfFreqDelta)? (pAd->RfFreqOffset - (pAd->RfFreqDelta & 0xf)) : 0;
	}
	else
	{
		pAd->RfFreqOffset = ((pAd->RfFreqOffset + pAd->RfFreqDelta) < 0x40)? (pAd->RfFreqOffset + (pAd->RfFreqDelta & 0xf)) : 0x3f;
	}
#endif /* RTMP_RBUS_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: RF FreqOffset=0x%lx \n", pAd->RfFreqOffset));

	/*CountryRegion byte offset (38h)*/
	value = pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] >> 8;		/* 2.4G band*/
	value2 = pAd->EEPROMDefaultValue[EEPROM_COUNTRY_REG_OFFSET] & 0x00FF;	/* 5G band*/
	
	if ((value <= REGION_MAXIMUM_BG_BAND) || (value == REGION_32_BG_BAND) || (value == REGION_33_BG_BAND))
	{
		pAd->CommonCfg.CountryRegion = ((UCHAR) value) | 0x80;
	}

	if (value2 <= REGION_MAXIMUM_A_BAND)
	{
		pAd->CommonCfg.CountryRegionForABand = ((UCHAR) value2) | 0x80;
	}


	
	/* Get RSSI Offset on EEPROM 0x9Ah & 0x9Ch.*/
	/* The valid value are (-10 ~ 10) */
	/* */
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_BG_OFFSET, value);
		pAd->BGRssiOffset0 = value & 0x00ff;
		pAd->BGRssiOffset1 = (value >> 8);
	}

	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_BG_OFFSET+2, value);
/*		if (IS_RT2860(pAd))  RT2860 supports 3 Rx and the 2.4 GHz RSSI #2 offset is in the EEPROM 0x48*/
			pAd->BGRssiOffset2 = value & 0x00ff;
		pAd->ALNAGain1 = (value >> 8);
	}

	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_LNA_OFFSET, value);
		pAd->BLNAGain = value & 0x00ff;
		pAd->ALNAGain0 = (value >> 8);
	}
	
	/* Validate 11b/g RSSI_0 offset.*/
	if ((pAd->BGRssiOffset0 < -10) || (pAd->BGRssiOffset0 > 10))
		pAd->BGRssiOffset0 = 0;

	/* Validate 11b/g RSSI_1 offset.*/
	if ((pAd->BGRssiOffset1 < -10) || (pAd->BGRssiOffset1 > 10))
		pAd->BGRssiOffset1 = 0;

	/* Validate 11b/g RSSI_2 offset.*/
	if ((pAd->BGRssiOffset2 < -10) || (pAd->BGRssiOffset2 > 10))
		pAd->BGRssiOffset2 = 0;

	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_A_OFFSET, value);
		pAd->ARssiOffset0 = value & 0x00ff;
		pAd->ARssiOffset1 = (value >> 8);
	}

	{
		RT28xx_EEPROM_READ16(pAd, (EEPROM_RSSI_A_OFFSET+2), value);
		pAd->ARssiOffset2 = value & 0x00ff;
		pAd->ALNAGain2 = (value >> 8);
	}


	if (((UCHAR)pAd->ALNAGain1 == 0xFF) || (pAd->ALNAGain1 == 0x00))
		pAd->ALNAGain1 = pAd->ALNAGain0;
	if (((UCHAR)pAd->ALNAGain2 == 0xFF) || (pAd->ALNAGain2 == 0x00))
		pAd->ALNAGain2 = pAd->ALNAGain0;

	/* Validate 11a RSSI_0 offset.*/
	if ((pAd->ARssiOffset0 < -10) || (pAd->ARssiOffset0 > 10))
		pAd->ARssiOffset0 = 0;

	/* Validate 11a RSSI_1 offset.*/
	if ((pAd->ARssiOffset1 < -10) || (pAd->ARssiOffset1 > 10))
		pAd->ARssiOffset1 = 0;

	/*Validate 11a RSSI_2 offset.*/
	if ((pAd->ARssiOffset2 < -10) || (pAd->ARssiOffset2 > 10))
		pAd->ARssiOffset2 = 0;

	
#ifdef LED_CONTROL_SUPPORT
	/* LED Setting */
	RTMPGetLEDSetting(pAd);
#endif /* LED_CONTROL_SUPPORT */
		
		RTMPReadTxPwrPerRate(pAd);

#ifdef SINGLE_SKU
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_DEFINE_MAX_TXPWR, pAd->CommonCfg.DefineMaxTxPwr);
	}

	if ((pAd->CommonCfg.DefineMaxTxPwr & 0xFF) <= 0x50 && pAd->CommonCfg.AntGain > 0 && pAd->CommonCfg.BandedgeDelta >= 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Single SKU Mode is enabled\n"));
		pAd->CommonCfg.bSKUMode = TRUE;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Single SKU Mode is disabled\n"));
		pAd->CommonCfg.bSKUMode = FALSE;
	}
#endif /* SINGLE_SKU */



#ifdef RTMP_INTERNAL_TX_ALC
	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC2_OFFSET, value);

	if(value==0xFFFF) { /*EEPROM is empty*/
	    pAd->TxPowerCtrl.bInternalTxALC = FALSE;
	}else if(value & 1<<13) {
	    pAd->TxPowerCtrl.bInternalTxALC = TRUE;
	}else {
	    pAd->TxPowerCtrl.bInternalTxALC = FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("TXALC> bInternalTxALC = %d\n",
			pAd->TxPowerCtrl.bInternalTxALC));
#endif /* RTMP_INTERNAL_TX_ALC */

#ifdef HW_ANTENNA_DIVERSITY_SUPPORT
	if (pAd->chipCap.FlgIsHwAntennaDiversitySup == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("HwAnDi> Read from EEPROM!\n"));

		/*
			0b00: OFF (Disable antenna diversity function)
			0b01: ON (Enable antenna diversity function)
			0b10: Fix to main antenna
			0b11: Fix to aux antenna
		*/
		RT28xx_EEPROM_READ16(pAd, EEPROM_NIC2_OFFSET, value);
		value &= 0x1800;
		value = value >> 11;
		if (value == 0)
		{
			// hw antenna divsersity is disabled, fix to main antenna by default
			pAd->bHardwareAntennaDivesity = FALSE;
			pAd->FixDefaultAntenna = 0;
		}
		else if (value == 1)
			pAd->bHardwareAntennaDivesity = TRUE;
		else if (value == 2)
		{
			// hw antenna divsersity is disabled, fix to main antenna
			pAd->bHardwareAntennaDivesity = FALSE;
			pAd->FixDefaultAntenna = 0;
		}
		else if (value == 3)
		{
			// hw antenna divsersity is disabled, fix to aux antenna
			pAd->bHardwareAntennaDivesity = FALSE;
			pAd->FixDefaultAntenna = 1;
		}
	}
#endif // HW_ANTENNA_DIVERSITY_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICReadEEPROMParameters\n"));
}


/*
	========================================================================
	
	Routine Description:
		Set default value from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	
	Note:
		
	========================================================================
*/
VOID	NICInitAsicFromEEPROM(
	IN	PRTMP_ADAPTER	pAd)
{
#ifdef CONFIG_STA_SUPPORT
	UINT32					data = 0;
	UCHAR	BBPR1 = 0; 
#endif /* CONFIG_STA_SUPPORT */
	USHORT					i;
	EEPROM_NIC_CONFIG2_STRUC    NicConfig2;
	UCHAR	BBPR3 = 0;
	
	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitAsicFromEEPROM\n"));
	for(i = EEPROM_BBP_ARRAY_OFFSET; i < NUM_EEPROM_BBP_PARMS; i++)
	{
		UCHAR BbpRegIdx, BbpValue;
	
		if ((pAd->EEPROMDefaultValue[i] != 0xFFFF) && (pAd->EEPROMDefaultValue[i] != 0))
		{
			BbpRegIdx = (UCHAR)(pAd->EEPROMDefaultValue[i] >> 8);
			BbpValue  = (UCHAR)(pAd->EEPROMDefaultValue[i] & 0xff);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BbpRegIdx, BbpValue);
		}
	}

	NicConfig2.word = pAd->NicConfig2.word;


#ifdef RTMP_RF_RW_SUPPORT
	/*Init RT30xx RFRegisters after read RFIC type from EEPROM*/
	NICInitRFRegisters(pAd);
#endif /* RTMP_RF_RW_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Read Hardware controlled Radio state enable bit*/
		if (NicConfig2.field.HardwareRadioControl == 1)
		{
			pAd->StaCfg.bHardwareRadio = TRUE;

			/* Read GPIO pin2 as Hardware controlled radio state*/
			RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &data);
			if ((data & 0x04) == 0)
			{
				pAd->StaCfg.bHwRadio = FALSE;
				pAd->StaCfg.bRadio = FALSE;
/*				RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x00001818);*/
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			}
		}
		else
			pAd->StaCfg.bHardwareRadio = FALSE;		

#ifdef LED_CONTROL_SUPPORT
		if (pAd->StaCfg.bRadio == FALSE)
		{
			RTMPSetLED(pAd, LED_RADIO_OFF);
		}
		else
		{
			RTMPSetLED(pAd, LED_RADIO_ON);
		}
#endif /* LED_CONTROL_SUPPORT */

#ifdef RTMP_MAC_PCI
		if (pAd->StaCfg.bRadio == TRUE)
		{
			AsicSendCommandToMcu(pAd, 0x30, PowerRadioOffCID, 0xff, 0x02);
			AsicCheckCommanOk(pAd, PowerRadioOffCID);

			/*AsicSendCommandToMcu(pAd, 0x30, 0xff, 0xff, 0x02);*/
			AsicSendCommandToMcu(pAd, 0x31, PowerWakeCID, 0x00, 0x00);
			/* 2-1. wait command ok.*/
			AsicCheckCommanOk(pAd, PowerWakeCID);
		}
#endif /* RTMP_MAC_PCI */		
	}
#ifdef PCIE_PS_SUPPORT
#endif /* PCIE_PS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
	/* Turn off patching for cardbus controller*/
	if (NicConfig2.field.CardbusAcceleration == 1)
	{
/*		pAd->bTest1 = TRUE;*/
	}

	if (NicConfig2.field.DynamicTxAgcControl == 1)
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = TRUE;
	else
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = FALSE;

#ifdef RTMP_INTERNAL_TX_ALC
	/*
	    Internal Tx ALC support is starting from RT3370 / RT3390, which combine PA / LNA in single chip.
	    The old chipset don't have this, add new feature flag RTMP_INTERNAL_ALC.
	 */

	/* Internal Tx ALC */
	if (((NicConfig2.field.DynamicTxAgcControl == 1) && 
            (NicConfig2.field.bInternalTxALC == 1)) || 
            ((!IS_RT3390(pAd)) &&
            ((!IS_RT3350(pAd)) && (!IS_RT3352(pAd)) && (!IS_RT5350(pAd)))))
	{
		/*
			If both DynamicTxAgcControl and bInternalTxALC are enabled,
			it is a wrong configuration.
			If the chipset does not support internal ALC, we shall disable it.
		*/
		pAd->TxPowerCtrl.bInternalTxALC = FALSE;
	}
	else
	{
		if (NicConfig2.field.bInternalTxALC == 1)
		{
			pAd->TxPowerCtrl.bInternalTxALC = TRUE;
		}
		else
		{
			pAd->TxPowerCtrl.bInternalTxALC = FALSE;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s: pAd->TxPowerCtrl.bInternalTxALC = %d\n", 
		__FUNCTION__, 
		pAd->TxPowerCtrl.bInternalTxALC));
#endif /* RTMP_INTERNAL_TX_ALC */

	
	/* Since BBP has been progamed, to make sure BBP setting will be */
	/* upate inside of AsicAntennaSelect, so reset to UNKNOWN_BAND!!*/
	
	pAd->CommonCfg.BandState = UNKNOWN_BAND;
	
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BBPR3);
	BBPR3 &= (~0x18);
	if(pAd->Antenna.field.RxPath == 3)
	{
		BBPR3 |= (0x10);
	}
	else if(pAd->Antenna.field.RxPath == 2)
	{
		BBPR3 |= (0x8);
	}
	else if(pAd->Antenna.field.RxPath == 1)
	{
		BBPR3 |= (0x0);
	}
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BBPR3);
	
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Handle the difference when 1T*/
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);

        {
			if(pAd->Antenna.field.TxPath == 1)
			{
			BBPR1 &= (~0x18);
			}
		}

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);
    
		DBGPRINT(RT_DEBUG_TRACE, ("Use Hw Radio Control Pin=%d; if used Pin=%d;\n", 
					pAd->StaCfg.bHardwareRadio, pAd->StaCfg.bHardwareRadio));
	}
#endif /* CONFIG_STA_SUPPORT */

	RTMP_EEPROM_ASIC_INIT(pAd);


#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
		/*
			Only for RT3593, RT5390 (Maybe add other chip in the future)
			Sometimes the frequency will be shift, we need to adjust it.
		*/
		if (pAd->StaCfg.AdaptiveFreq == TRUE) /*Todo: iwpriv and profile support.*/
		pAd->FreqCalibrationCtrl.bEnableFrequencyCalibration = TRUE;

		DBGPRINT(RT_DEBUG_TRACE, ("%s: pAd->FreqCalibrationCtrl.bEnableFrequencyCalibration = %d\n", 
			__FUNCTION__, 
			pAd->FreqCalibrationCtrl.bEnableFrequencyCalibration));

#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("TxPath = %d, RxPath = %d, RFIC=%d\n", 
				pAd->Antenna.field.TxPath, pAd->Antenna.field.RxPath, pAd->RfIcType));
	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitAsicFromEEPROM\n"));
}

/*
	========================================================================
	
	Routine Description:
		Initialize NIC hardware

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
NDIS_STATUS	NICInitializeAdapter(
	IN	PRTMP_ADAPTER	pAd,
	IN   BOOLEAN    bHardReset)
{
	NDIS_STATUS     Status = NDIS_STATUS_SUCCESS;
	WPDMA_GLO_CFG_STRUC	GloCfg;
#ifdef RTMP_MAC_PCI
	UINT32			Value;
	DELAY_INT_CFG_STRUC	IntCfg;
#endif /* RTMP_MAC_PCI */
/*	INT_MASK_CSR_STRUC		IntMask;*/
	ULONG	i =0;
#ifndef RTMP_RBUS_SUPPORT
	ULONG	j=0;
#endif
	/*AC_TXOP_CSR0_STRUC	csr0;*/

	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitializeAdapter\n"));
	
	/* 3. Set DMA global configuration except TX_DMA_EN and RX_DMA_EN bits:*/
#ifndef RTMP_RBUS_SUPPORT
retry:
#endif /* RTMP_RBUS_SUPPORT */
	i = 0;
	do
	{
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
		if ((GloCfg.field.TxDMABusy == 0)  && (GloCfg.field.RxDMABusy == 0))
			break;
		
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))			
			return NDIS_STATUS_FAILURE;
		RTMPusecDelay(1000);
		i++;
	}while ( i<100);
	DBGPRINT(RT_DEBUG_TRACE, ("<== DMA offset 0x208 = 0x%x\n", GloCfg.word));	
	GloCfg.word &= 0xff0;
	GloCfg.field.EnTXWriteBackDDONE =1;
	RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, GloCfg.word);
	
	/* Record HW Beacon offset*/
	{
		pAd->BeaconOffset[0] = pAd->chipCap.BcnBase[0];
		pAd->BeaconOffset[1] = pAd->chipCap.BcnBase[1];
		pAd->BeaconOffset[2] = pAd->chipCap.BcnBase[2];
		pAd->BeaconOffset[3] = pAd->chipCap.BcnBase[3];
		pAd->BeaconOffset[4] = pAd->chipCap.BcnBase[4];
		pAd->BeaconOffset[5] = pAd->chipCap.BcnBase[5];
		pAd->BeaconOffset[6] = pAd->chipCap.BcnBase[6];
		pAd->BeaconOffset[7] = pAd->chipCap.BcnBase[7];
	}
	
#ifdef SPECIFIC_BCN_BUF_SUPPORT
	if (pAd->chipCap.FlgIsSupSpecBcnBuf == TRUE)
	{
		pAd->BeaconOffset[8] = pAd->chipCap.BcnBase[8];
		pAd->BeaconOffset[9] = pAd->chipCap.BcnBase[9];
		pAd->BeaconOffset[10] = pAd->chipCap.BcnBase[10];
		pAd->BeaconOffset[11] = pAd->chipCap.BcnBase[11];
		pAd->BeaconOffset[12] = pAd->chipCap.BcnBase[12];
		pAd->BeaconOffset[13] = pAd->chipCap.BcnBase[13];
		pAd->BeaconOffset[14] = pAd->chipCap.BcnBase[14];
		pAd->BeaconOffset[15] = pAd->chipCap.BcnBase[15];
	}
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

	
	/* write all shared Ring's base address into ASIC*/
	

	/* asic simulation sequence put this ahead before loading firmware.*/
	/* pbf hardware reset*/
#ifdef RTMP_MAC_PCI
	RTMP_IO_WRITE32(pAd, WPDMA_RST_IDX, 0x1003f);	/* 0x10000 for reset rx, 0x3f resets all 6 tx rings.*/
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe1f);
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe00);
#endif /* RTMP_MAC_PCI */

	/* Initialze ASIC for TX & Rx operation*/
	if (NICInitializeAsic(pAd , bHardReset) != NDIS_STATUS_SUCCESS)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))			
			return NDIS_STATUS_FAILURE;
#ifndef RTMP_RBUS_SUPPORT
		if (j++ == 0)
		{
			NICLoadFirmware(pAd);
			goto retry;
		}
#endif /* RTMP_RBUS_SUPPORT */
		return NDIS_STATUS_FAILURE;
	}

#ifdef RTMP_MAC_PCI
	/* Write AC_BK base address register*/
	Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_AC_BK].Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR1, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR1 : 0x%x\n", Value));

	/* Write AC_BE base address register*/
	Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_AC_BE].Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR0, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR0 : 0x%x\n", Value));

	/* Write AC_VI base address register*/
	Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_AC_VI].Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR2, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR2 : 0x%x\n", Value));

	/* Write AC_VO base address register*/
	Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_AC_VO].Cell[0].AllocPa);	
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR3, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR3 : 0x%x\n", Value));

	/* Write HCCA base address register*/
	  Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_HCCA].Cell[0].AllocPa);
	  RTMP_IO_WRITE32(pAd, TX_BASE_PTR4, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR4 : 0x%x\n", Value));

	/* Write MGMT_BASE_CSR register*/
	Value = RTMP_GetPhysicalAddressLow(pAd->MgmtRing.Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR5, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR5 : 0x%x\n", Value));

	/* Write RX_BASE_CSR register*/
	Value = RTMP_GetPhysicalAddressLow(pAd->RxRing.Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, RX_BASE_PTR, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> RX_BASE_PTR : 0x%x\n", Value));

	/* Init RX Ring index pointer*/
	pAd->RxRing.RxSwReadIdx = 0;
	pAd->RxRing.RxCpuIdx = RX_RING_SIZE-1;
	RTMP_IO_WRITE32(pAd, RX_CRX_IDX, pAd->RxRing.RxCpuIdx);
	
	/* Init TX rings index pointer*/
	{
		for (i=0; i<NUM_OF_TX_RING; i++)
		{
			pAd->TxRing[i].TxSwFreeIdx = 0;
			pAd->TxRing[i].TxCpuIdx = 0;
			RTMP_IO_WRITE32(pAd, (TX_CTX_IDX0 + i * 0x10) ,  pAd->TxRing[i].TxCpuIdx);
		}
	}

	/* init MGMT ring index pointer*/
	pAd->MgmtRing.TxSwFreeIdx = 0;
	pAd->MgmtRing.TxCpuIdx = 0;
	RTMP_IO_WRITE32(pAd, TX_MGMTCTX_IDX,  pAd->MgmtRing.TxCpuIdx);

	
	/* set each Ring's SIZE  into ASIC. Descriptor Size is fixed by design.*/
	

	/* Write TX_RING_CSR0 register*/
	Value = TX_RING_SIZE;
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT0, Value);
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT1, Value);
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT2, Value);
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT3, Value);
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT4, Value);
	Value = MGMT_RING_SIZE;
	RTMP_IO_WRITE32(pAd, TX_MGMTMAX_CNT, Value);

	/* Write RX_RING_CSR register*/
	Value = RX_RING_SIZE;
	RTMP_IO_WRITE32(pAd, RX_MAX_CNT, Value);
#endif /* RTMP_MAC_PCI */



#ifdef RTMP_MAC_PCI
	/* 3. Set DMA global configuration except TX_DMA_EN and RX_DMA_EN bits:*/
	i = 0;
	do
	{
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
		if ((GloCfg.field.TxDMABusy == 0)  && (GloCfg.field.RxDMABusy == 0))
			break;
		
		RTMPusecDelay(1000);
		i++;
	}while ( i < 100);

	GloCfg.word &= 0xff0;
	GloCfg.field.EnTXWriteBackDDONE =1;
	RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, GloCfg.word);
	
	IntCfg.word = 0;
	RTMP_IO_WRITE32(pAd, DELAY_INT_CFG, IntCfg.word);
#endif /* RTMP_MAC_PCI */


	/* reset action*/
	/* Load firmware*/
	/*  Status = NICLoadFirmware(pAd);*/

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitializeAdapter\n"));
	return Status;
}

/*
	========================================================================
	
	Routine Description:
		Initialize ASIC

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
NDIS_STATUS	NICInitializeAsic(
	IN	PRTMP_ADAPTER	pAd,
	IN  BOOLEAN		bHardReset)
{
	ULONG			Index = 0;
	UCHAR			R0 = 0xff;
	UINT32			MacCsr12 = 0, Counter = 0;
#ifdef RTMP_PCI_SUPPORT
	UINT32			Value;
#endif /* RTMP_PCI_SUPPORT */
	USHORT			KeyIdx;
	INT				i,apidx;

	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitializeAsic\n"));

#ifdef RTMP_MAC_PCI
	RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x3);	/* To fix driver disable/enable hang issue when radio off*/
#ifdef RT3352
	if ((pAd->CommonCfg.CID & 0x0000000f) < 0x00000006)
		RtmpOsMsDelay(100);
#endif /* RT3352 */

	if (bHardReset == TRUE)
	{
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x3);

	}
	else
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x1);

	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x0);
	/* Initialize MAC register to default value*/
	for (Index = 0; Index < NUM_MAC_REG_PARMS; Index++)
	{
		RTMP_IO_WRITE32(pAd, MACRegTable[Index].Register, MACRegTable[Index].Value);
#ifdef RT3352
		if ((pAd->CommonCfg.CID & 0x0000000f) < 0x00000006)
		{
			if ((MACRegTable[Index].Register == PWR_PIN_CFG) ||
				(MACRegTable[Index].Register == TX_PIN_CFG))
				RtmpOsMsDelay(100);
		}
#endif /* RT3352 */
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		for (Index = 0; Index < NUM_AP_MAC_REG_PARMS; Index++)
		{
			RTMP_IO_WRITE32(pAd, APMACRegTable[Index].Register, APMACRegTable[Index].Value);
		}
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		for (Index = 0; Index < NUM_STA_MAC_REG_PARMS; Index++)
		{
			RTMP_IO_WRITE32(pAd, STAMACRegTable[Index].Register, STAMACRegTable[Index].Value);
		}
	}
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_PCI_SUPPORT
	pAd->CommonCfg.bPCIeBus = FALSE; /* PCI bus*/

	/* PCI and PCIe have different value in US_CYC_CNT*/
	RTMP_IO_READ32(pAd, PCI_CFG, &Value);
	if ((Value & 0x10000) == 0)
	{
		US_CYC_CNT_STRUC	USCycCnt;
		UINT32 				Value;

		RTMP_IO_READ32(pAd, US_CYC_CNT, &Value);
		USCycCnt.word = Value;
		USCycCnt.field.UsCycCnt = 0x7D;
		RTMP_IO_WRITE32(pAd, US_CYC_CNT, USCycCnt.word);
		
		pAd->CommonCfg.bPCIeBus = TRUE;
		
		DBGPRINT(RT_DEBUG_TRACE, ("NICInitializeAsic::act as PCIe driver \n"));
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("NICInitializeAsic::act as PCI driver \n"));
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("%s: pAd->CommonCfg.bPCIeBus = %d\n", 
		__FUNCTION__, 
		pAd->CommonCfg.bPCIeBus));
#endif /* RTMP_PCI_SUPPORT */



#ifdef SPECIFIC_BCN_BUF_SUPPORT
	if (pAd->chipCap.FlgIsSupSpecBcnBuf == TRUE)
	{
		/* re-set beacon offset */
		for(Index=0; Index<NUM_BCN_SPEC_MAC_REG_PARMS; Index++)
		{
			RTMP_IO_WRITE32(pAd, (USHORT)BcnSpecMACRegTable[Index].Register, BcnSpecMACRegTable[Index].Value);
		}
	}
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

	/* re-set specific MAC registers */
	if (pAd->chipOps.AsicMacInit != NULL)
		pAd->chipOps.AsicMacInit(pAd);

	
	/* Before program BBP, we need to wait BBP/RF get wake up.*/
	
	Index = 0;
	do
	{
		RTMP_IO_READ32(pAd, MAC_STATUS_CFG, &MacCsr12);

		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))			
			return NDIS_STATUS_FAILURE;

		if ((MacCsr12 & 0x03) == 0)	/* if BB.RF is stable*/
			break;
		
		DBGPRINT(RT_DEBUG_TRACE, ("Check MAC_STATUS_CFG  = Busy = %x\n", MacCsr12));
		RTMPusecDelay(1000);
	} while (Index++ < 100);

#ifdef RTMP_MAC_PCI
	/* The commands to firmware should be after these commands, these commands will init firmware*/
	/* PCI and USB are not the same because PCI driver needs to wait for PCI bus ready*/
	RTMP_IO_WRITE32(pAd, H2M_BBP_AGENT, 0);	/* initialize BBP R/W access agent*/
	RTMP_IO_WRITE32(pAd, H2M_MAILBOX_CSR, 0);
	AsicSendCommandToMcu(pAd, 0x72, 0, 0, 0);
#endif /* RTMP_MAC_PCI */

	/* Wait to be stable.*/
	RTMPusecDelay(1000);
	pAd->LastMCUCmd = 0x72;

	/* Read BBP register, make sure BBP is up and running before write new data*/
	Index = 0;
	do 
	{
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R0, &R0);
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))			
			return NDIS_STATUS_FAILURE;
		DBGPRINT(RT_DEBUG_TRACE, ("BBP version = %x\n", R0));
	} while ((++Index < 20) && ((R0 == 0xff) || (R0 == 0x00)));
	/*ASSERT(Index < 20); this will cause BSOD on Check-build driver*/

	if ((R0 == 0xff) || (R0 == 0x00))
		return NDIS_STATUS_FAILURE;

	/* Initialize BBP register to default value*/
	for (Index = 0; Index < NUM_BBP_REG_PARMS; Index++)
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBPRegTable[Index].Register, BBPRegTable[Index].Value);
	}
	if (pAd->chipCap.pBBPRegTable)
	{
		REG_PAIR *pbbpRegTb = pAd->chipCap.pBBPRegTable;
		
		for (Index = 0; Index < pAd->chipCap.bbpRegTbSize; Index++)
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, pbbpRegTb[Index].Register, pbbpRegTb[Index].Value);
			DBGPRINT(RT_DEBUG_TRACE, ("BBP_R%d=%d\n", pbbpRegTb[Index].Register, pbbpRegTb[Index].Value));
		}
	}

	if (pAd->chipOps.AsicBbpInit != NULL)
		pAd->chipOps.AsicBbpInit(pAd);


	RTMP_VDR_TUNING1(pAd);

	
#ifdef RTMP_MAC_PCI
	/* TODO: shiang, check MACVersion, currently, rbus-based chip use this.*/
	if (pAd->MACVersion == 0x28720200)
	{
		/*UCHAR value;*/
		UINT32 value2;

		/*disable MLD by Bruce 20080704*/
		/*BBP_IO_READ8_BY_REG_ID(pAd, BBP_R105, &value);*/
		/*BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R105, value | 4);*/
	
		/*Maximum PSDU length from 16K to 32K bytes	*/
		RTMP_IO_READ32(pAd, MAX_LEN_CFG, &value2);
		value2 &= ~(0x3<<12);
		value2 |= (0x2<<12);
		RTMP_IO_WRITE32(pAd, MAX_LEN_CFG, value2);
	}
#endif /* RTMP_MAC_PCI */

	/* for rt2860E and after, init BBP_R84 with 0x19. This is for extension channel overlapping IOT.*/
	/* RT3090 should not program BBP R84 to 0x19, otherwise TX will block.*/
	/*3070/71/72,3090,3090A( are included in RT30xx),3572,3390*/
#if !defined(RT5350)
	if (((pAd->MACVersion & 0xffff) != 0x0101) &&
		!(IS_RT30xx(pAd)|| IS_RT3572(pAd)))
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R84, 0x19);
#endif /* RT5350 */


	if (pAd->MACVersion == 0x28600100)
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x16);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x12);
	}
	
	if ((pAd->MACVersion == RALINK_3883_VERSION) ||
		((pAd->MACVersion >= RALINK_2880E_VERSION) &&
		(pAd->MACVersion < RALINK_3070_VERSION))) /* 3*3*/
	{
		/* enlarge MAX_LEN_CFG*/
		UINT32 csr;
		RTMP_IO_READ32(pAd, MAX_LEN_CFG, &csr);
		{
		csr &= 0xFFF;
		csr |= 0x2000;
		}
		RTMP_IO_WRITE32(pAd, MAX_LEN_CFG, csr);
	}


#ifdef CONFIG_STA_SUPPORT
	/* Add radio off control*/
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->StaCfg.bRadio == FALSE)
		{
/*			RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x00001818);*/
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			DBGPRINT(RT_DEBUG_TRACE, ("Set Radio Off\n"));
		}
	}
#endif /* CONFIG_STA_SUPPORT */	

	/* Clear raw counters*/
	RTMP_IO_READ32(pAd, RX_STA_CNT0, &Counter);
	RTMP_IO_READ32(pAd, RX_STA_CNT1, &Counter);
	RTMP_IO_READ32(pAd, RX_STA_CNT2, &Counter);
	RTMP_IO_READ32(pAd, TX_STA_CNT0, &Counter);
	RTMP_IO_READ32(pAd, TX_STA_CNT1, &Counter);
	RTMP_IO_READ32(pAd, TX_STA_CNT2, &Counter);
	
	/* ASIC will keep garbage value after boot*/
	/* Clear all shared key table when initial*/
	/* This routine can be ignored in radio-ON/OFF operation. */
	if (bHardReset)
	{
		for (KeyIdx = 0; KeyIdx < 4; KeyIdx++)
		{
			RTMP_IO_WRITE32(pAd, SHARED_KEY_MODE_BASE + 4*KeyIdx, 0);
		}

		/* Clear all pairwise key table when initial*/
		for (KeyIdx = 0; KeyIdx < 256; KeyIdx++)
		{
			RTMP_IO_WRITE32(pAd, MAC_WCID_ATTRIBUTE_BASE + (KeyIdx * HW_WCID_ATTRI_SIZE), 1);
		}
	}
	
	/* assert HOST ready bit*/
/*  RTMP_IO_WRITE32(pAd, MAC_CSR1, 0x0);  2004-09-14 asked by Mark*/
/*  RTMP_IO_WRITE32(pAd, MAC_CSR1, 0x4);*/

	/* It isn't necessary to clear this space when not hard reset. 	*/
	if (bHardReset == TRUE)
	{
#ifdef SPECIFIC_BCN_BUF_SUPPORT
		unsigned long irqFlag = 0;
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
	
		/* clear all on-chip BEACON frame space			*/
#ifdef SPECIFIC_BCN_BUF_SUPPORT
		/*
			Shared memory access selection (higher 8KB shared memory)
		*/
		RTMP_MAC_SHR_MSEL_LOCK(pAd, HIGHER_SHRMEM, irqFlag);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

		for (apidx = 0; apidx < HW_BEACON_MAX_COUNT(pAd); apidx++)
		{
			for (i = 0; i < HW_BEACON_OFFSET>>2; i+=4)
				RTMP_IO_WRITE32(pAd, pAd->BeaconOffset[apidx] + i, 0x00); 
#ifdef CONFIG_AP_SUPPORT
#endif /* CONFIG_AP_SUPPORT */
		}
		
#ifdef SPECIFIC_BCN_BUF_SUPPORT
		/*
			Shared memory access selection (lower 16KB shared memory)
		*/
		RTMP_MAC_SHR_MSEL_UNLOCK(pAd, LOWER_SHRMEM, irqFlag);	
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
	}
	

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* for rt2860E and after, init TXOP_CTRL_CFG with 0x583f. This is for extension channel overlapping IOT.*/
		if ((pAd->MACVersion&0xffff) != 0x0101)
			RTMP_IO_WRITE32(pAd, TXOP_CTRL_CFG, 0x583f);
	}
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitializeAsic\n"));
	return NDIS_STATUS_SUCCESS;
}


#ifdef RTMP_RBUS_SUPPORT
/*
	========================================================================
	
	Routine Description:
		Reset NIC from error

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		Reset NIC from error state
		
	========================================================================
*/
VOID	NICResetFromError(
	IN	PRTMP_ADAPTER	pAd)
{
	/* Reset BBP (according to alex, reset ASIC will force reset BBP*/
	/* Therefore, skip the reset BBP*/
	/* RTMP_IO_WRITE32(pAd, MAC_CSR1, 0x2);*/
		
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x1);
	/* Remove ASIC from reset state*/
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x0);

	NICInitializeAdapter(pAd, FALSE);	
	NICInitAsicFromEEPROM(pAd);

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		AsicBBPAdjust(pAd);
	}

#ifdef CONFIG_STA_SUPPORT	
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		AsicStaBbpTuning(pAd);
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (INFRA_ON(pAd) && (pAd->CommonCfg.CentralChannel != pAd->CommonCfg.Channel) 
			&& (pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth == BW_40))
		{	
			/* Must using 40MHz.*/
			AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
			AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
		}
		else
		{	
			/* Must using 20MHz.*/
			AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
			AsicLockChannel(pAd, pAd->CommonCfg.Channel);
		}
	}
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
		AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
	}
#endif /* CONFIG_AP_SUPPORT */
}
#endif /* RTMP_RBUS_SUPPORT */


#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_MAC_PCI
/*
	========================================================================
	
	Routine Description:
		In the case, Client may be silent left without sending DeAuth or DeAssoc.
		AP'll continue retry packets for the client since AP doesn't know the STA
		is gone. To Minimum affection of exist traffic is disable retransmition for
		all those packet relative to the STA.
		So decide to change ack required setting of all packet in TX ring
		to "no ACK" requirement for specific Client.

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL
	
	========================================================================
*/
static VOID ClearTxRingClientAck(
	IN PRTMP_ADAPTER pAd,
	IN MAC_TABLE_ENTRY *pEntry)

{
	INT index;
	USHORT TxIdx;
	PRTMP_TX_RING pTxRing;
	PTXD_STRUC pTxD;
	PTXWI_STRUC pTxWI;
#ifdef RT_BIG_ENDIAN
	PTXD_STRUC	pDestTxD;
	TXD_STRUC	TxD;
	PTXWI_STRUC	pDestTxWI;
	TXWI_STRUC	TxWI;
#endif /* RT_BIG_ENDIAN */

	if (!pAd || !pEntry)
		return;

	for (index = 3; index >= 0; index--)
	{
		pTxRing = &pAd->TxRing[index];
		for (TxIdx = 0; TxIdx < TX_RING_SIZE; TxIdx++)
		{
#ifdef RT_BIG_ENDIAN
			pDestTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
			TxD = *pDestTxD;
			pTxD = &TxD;
			RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
#else
			pTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
#endif /* RT_BIG_ENDIAN */

			if (!pTxD->DMADONE)
			{
#ifdef RT_BIG_ENDIAN
				pDestTxWI = (PTXWI_STRUC) pTxRing->Cell[TxIdx].DmaBuf.AllocVa;
				NdisMoveMemory((PUCHAR)&TxWI, (PUCHAR)pDestTxWI, sizeof(TXWI_STRUC));
				pTxWI = &TxWI;
				RTMPWIEndianChange((PUCHAR)pTxWI, TYPE_TXWI);
#else
				 pTxWI = (PTXWI_STRUC) pTxRing->Cell[TxIdx].DmaBuf.AllocVa;
#endif /* RT_BIG_ENDIAN */
				 if (pTxWI->WirelessCliID == pEntry->Aid)
					pTxWI->ACK = FALSE;
#ifdef RT_BIG_ENDIAN
				RTMPWIEndianChange((PUCHAR)pTxWI, TYPE_TXWI);
				NdisMoveMemory((PUCHAR)pDestTxWI, (PUCHAR)pTxWI, sizeof(TXWI_STRUC));
#endif /* RT_BIG_ENDIAN */
			}
		}
	}
}
#endif /* RTMP_MAC_PCI */
#endif /* CONFIG_AP_SUPPORT */

VOID NICUpdateFifoStaCounters(
	IN PRTMP_ADAPTER pAd)
{
	TX_STA_FIFO_STRUC	StaFifo;
	MAC_TABLE_ENTRY		*pEntry;
	UINT32				i = 0;
	UCHAR				pid = 0, wcid = 0;
	INT32				reTry;
	UCHAR				succMCS;

#ifdef RALINK_ATE		
	/* Nothing to do in ATE mode */
	if (ATE_ON(pAd))
		return;
#endif /* RALINK_ATE */



		do
		{
			RTMP_IO_READ32(pAd, TX_STA_FIFO, &StaFifo.word);

			if (StaFifo.field.bValid == 0)
				break;
		
			wcid = (UCHAR)StaFifo.field.wcid;

#ifdef CONFIG_AP_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			{
#ifdef DFS_SUPPORT
				/* the CTS frame for DFS and Carrier-Sense. */
				if ((pAd->CommonCfg.Channel > 14)
					&& (pAd->CommonCfg.bIEEE80211H == TRUE)
					&& (pAd->CommonCfg.RadarDetect.RDMode == RD_NORMAL_MODE)
					&& (wcid == DFS_CTS_WCID(pAd)))
				{
					i++;
					DFSStartTrigger(pAd);
					continue;
				}
#endif /* DFS_SUPPORT */

#ifdef CARRIER_DETECTION_SUPPORT
				if ((pAd->CommonCfg.CarrierDetect.Enable == TRUE)
					&& (wcid == CS_CTS_WCID(pAd)))
				{
					i++;
					CarrierDetectStartTrigger(pAd);
					continue;
				}
#endif /* CARRIER_DETECTION_SUPPORT */
			}
#endif /* CONFIG_AP_SUPPORT */

		/* ignore NoACK and MGMT frame use 0xFF as WCID */
			if ((StaFifo.field.TxAckRequired == 0) || (wcid >= MAX_LEN_OF_MAC_TABLE))
			{
				i++;
				continue;
			}

			/* PID store Tx MCS Rate */
			pid = (UCHAR)StaFifo.field.PidType;

			pEntry = &pAd->MacTab.Content[wcid];

			pEntry->DebugFIFOCount++;

#ifdef DOT11_N_SUPPORT
#ifdef RTMP_RBUS_SUPPORT
#endif /* RTMP_RBUS_SUPPORT */
#endif /* DOT11_N_SUPPORT */

#ifdef UAPSD_AP_SUPPORT
			UAPSD_SP_AUE_Handle(pAd, pEntry, StaFifo.field.TxSuccess);
#endif /* UAPSD_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
			if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
				continue;
#endif /* CONFIG_STA_SUPPORT */

			if (!StaFifo.field.TxSuccess)
			{
				pEntry->FIFOCount++;
				pEntry->OneSecTxFailCount++;
									
				if (pEntry->FIFOCount >= 1)
				{			
					DBGPRINT(RT_DEBUG_TRACE, ("#"));
#ifdef DOT11_N_SUPPORT
					pEntry->NoBADataCountDown = 64;
#endif /* DOT11_N_SUPPORT */

					/* Update the continuous transmission counter.*/
					pEntry->ContinueTxFailCnt++;

					if(pEntry->PsMode == PWR_ACTIVE)
					{
#ifdef DOT11_N_SUPPORT					
						int tid;
						for (tid=0; tid<NUM_OF_TID; tid++)
						{
							BAOriSessionTearDown(pAd, pEntry->Aid,  tid, FALSE, FALSE);
						}
#endif /* DOT11_N_SUPPORT */


#ifdef WDS_SUPPORT
						/* fix WDS Jam issue*/
						if(IS_ENTRY_WDS(pEntry)
							&& (pEntry->LockEntryTx == FALSE)
							&& (pEntry->ContinueTxFailCnt >= pAd->ApCfg.EntryLifeCheck))
						{ 
							DBGPRINT(RT_DEBUG_TRACE, ("Entry %02x:%02x:%02x:%02x:%02x:%02x Blocked!! (Fail Cnt = %d)\n",
								pEntry->Addr[0],pEntry->Addr[1],pEntry->Addr[2],pEntry->Addr[3],
								pEntry->Addr[4],pEntry->Addr[5],pEntry->ContinueTxFailCnt ));

							pEntry->LockEntryTx = TRUE;
						}
#endif /* WDS_SUPPORT */
					}

					/*pEntry->FIFOCount = 0;*/
				}
				/*pEntry->bSendBAR = TRUE;*/
#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_MAC_PCI
			/* if Tx fail >= 20, then clear TXWI ack in Tx Ring*/
			if (pEntry->ContinueTxFailCnt >= pAd->ApCfg.EntryLifeCheck)
				ClearTxRingClientAck(pAd, pEntry);	
#endif /* RTMP_MAC_PCI */				
#endif /* CONFIG_AP_SUPPORT */
			}
			else
			{
#ifdef DOT11_N_SUPPORT
				if ((pEntry->PsMode != PWR_SAVE) && (pEntry->NoBADataCountDown > 0))
				{
					pEntry->NoBADataCountDown--;
					if (pEntry->NoBADataCountDown==0)
					{
						DBGPRINT(RT_DEBUG_TRACE, ("@\n"));
					}
				}
#endif /* DOT11_N_SUPPORT */
				pEntry->FIFOCount = 0;
				pEntry->OneSecTxNoRetryOkCount++;
				/* update NoDataIdleCount when sucessful send packet to STA.*/
				pEntry->NoDataIdleCount = 0;
				pEntry->ContinueTxFailCnt = 0;
#ifdef RTMP_RBUS_SUPPORT
#endif /* RTMP_RBUS_SUPPORT */
#ifdef WDS_SUPPORT
				pEntry->LockEntryTx = FALSE;
#endif /* WDS_SUPPORT */
			}

			succMCS = StaFifo.field.SuccessRate & 0x7F;

#ifdef DOT11N_SS3_SUPPORT
			if (pEntry->HTCapability.MCSSet[2] == 0xff)
			{
				if (succMCS > pid)
					pid = pid + 16;
			}
#endif /* DOT11N_SS3_SUPPORT */


			reTry = pid - succMCS;

			if (StaFifo.field.TxSuccess)
			{
				pEntry->TXMCSExpected[pid]++;
				if (pid == succMCS)
				{
					pEntry->TXMCSSuccessful[pid]++;
				}
				else 
				{
					pEntry->TXMCSAutoFallBack[pid][succMCS]++;
				}
			}
			else
			{
				pEntry->TXMCSFailed[pid]++;
			}

#ifdef DOT11N_SS3_SUPPORT
#ifdef NEW_RATE_ADAPT_SUPPORT
			if (pEntry->HTCapability.MCSSet[2] == 0xff)
			{
				reTry = (pid % 3) - (succMCS % 3) + (pid >> 3) - (succMCS >> 3);

				if (reTry > 0)
					pEntry->OneSecTxRetryOkCount += reTry;
			}
			else
			{
#endif /* NEW_RATE_ADAPT_SUPPORT */
#endif /* DOT11N_SS3_SUPPORT */
			if (reTry > 0)
			{
				if ((pid >= 12) && succMCS <=7)
				{
					reTry -= 4;
				} 
				pEntry->OneSecTxRetryOkCount += reTry;
			}
#ifdef DOT11N_SS3_SUPPORT
#ifdef NEW_RATE_ADAPT_SUPPORT
			}
#endif /* NEW_RATE_ADAPT_SUPPORT */
#endif /* DOT11N_SS3_SUPPORT */

			i++;
			/* ASIC store 16 stack*/
		} while ( i < (TX_RING_SIZE<<1) );

}

/*
	========================================================================
	
	Routine Description:
		Read statistical counters from hardware registers and record them
		in software variables for later on query

	Arguments:
		pAd					Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL
	
	========================================================================
*/
VOID NICUpdateRawCounters(
	IN PRTMP_ADAPTER pAd)
{
	UINT32	OldValue;/*, Value2;*/
	/*ULONG	PageSum, OneSecTransmitCount;*/
	/*ULONG	TxErrorRatio, Retry, Fail;*/
	RX_STA_CNT0_STRUC	 RxStaCnt0;
	RX_STA_CNT1_STRUC   RxStaCnt1;
	RX_STA_CNT2_STRUC   RxStaCnt2;
	TX_STA_CNT0_STRUC 	 TxStaCnt0;
	TX_STA_CNT1_STRUC	 StaTx1;
	TX_STA_CNT2_STRUC	 StaTx2;
#ifdef STATS_COUNT_SUPPORT
	TX_AGG_CNT_STRUC	TxAggCnt;
	TX_AGG_CNT0_STRUC	TxAggCnt0;
	TX_AGG_CNT1_STRUC	TxAggCnt1;
	TX_AGG_CNT2_STRUC	TxAggCnt2;
	TX_AGG_CNT3_STRUC	TxAggCnt3;
	TX_AGG_CNT4_STRUC	TxAggCnt4;
	TX_AGG_CNT5_STRUC	TxAggCnt5;
	TX_AGG_CNT6_STRUC	TxAggCnt6;
	TX_AGG_CNT7_STRUC	TxAggCnt7;
#endif /* STATS_COUNT_SUPPORT */
	COUNTER_RALINK		*pRalinkCounters;


	pRalinkCounters = &pAd->RalinkCounters;



	RTMP_IO_READ32(pAd, RX_STA_CNT0, &RxStaCnt0.word);
	RTMP_IO_READ32(pAd, RX_STA_CNT2, &RxStaCnt2.word);

#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT
	if ((pAd->CommonCfg.CarrierDetect.Enable == FALSE) || (pAd->OpMode == OPMODE_STA))
#endif /* CARRIER_DETECTION_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
	{
		RTMP_IO_READ32(pAd, RX_STA_CNT1, &RxStaCnt1.word);
	    /* Update RX PLCP error counter*/
	    pAd->PrivateInfo.PhyRxErrCnt += RxStaCnt1.field.PlcpErr;
		/* Update False CCA counter*/
		pAd->RalinkCounters.OneSecFalseCCACnt += RxStaCnt1.field.FalseCca;
	}

#ifdef STATS_COUNT_SUPPORT
	/* Update FCS counters*/
	OldValue= pAd->WlanCounters.FCSErrorCount.u.LowPart;
	pAd->WlanCounters.FCSErrorCount.u.LowPart += (RxStaCnt0.field.CrcErr); /* >> 7);*/
	if (pAd->WlanCounters.FCSErrorCount.u.LowPart < OldValue)
		pAd->WlanCounters.FCSErrorCount.u.HighPart++;
#endif /* STATS_COUNT_SUPPORT */

	/* Add FCS error count to private counters*/
	pRalinkCounters->OneSecRxFcsErrCnt += RxStaCnt0.field.CrcErr;
	OldValue = pRalinkCounters->RealFcsErrCount.u.LowPart;
	pRalinkCounters->RealFcsErrCount.u.LowPart += RxStaCnt0.field.CrcErr;
	if (pRalinkCounters->RealFcsErrCount.u.LowPart < OldValue)
		pRalinkCounters->RealFcsErrCount.u.HighPart++;

	/* Update Duplicate Rcv check*/
	pRalinkCounters->DuplicateRcv += RxStaCnt2.field.RxDupliCount;
#ifdef STATS_COUNT_SUPPORT
	pAd->WlanCounters.FrameDuplicateCount.u.LowPart += RxStaCnt2.field.RxDupliCount;
#endif /* STATS_COUNT_SUPPORT */
	/* Update RX Overflow counter*/
	pAd->Counters8023.RxNoBuffer += (RxStaCnt2.field.RxFifoOverflowCount);
	
	/*pAd->RalinkCounters.RxCount = 0;*/

	
	/*if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_TX_RATE_SWITCH_ENABLED) || */
	/*	(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_TX_RATE_SWITCH_ENABLED) && (pAd->MacTab.Size != 1)))*/
	if (!pAd->bUpdateBcnCntDone)
	{
	/* Update BEACON sent count*/
	RTMP_IO_READ32(pAd, TX_STA_CNT0, &TxStaCnt0.word);
	RTMP_IO_READ32(pAd, TX_STA_CNT1, &StaTx1.word);
	RTMP_IO_READ32(pAd, TX_STA_CNT2, &StaTx2.word);
	pRalinkCounters->OneSecBeaconSentCnt += TxStaCnt0.field.TxBeaconCount;
	pRalinkCounters->OneSecTxRetryOkCount += StaTx1.field.TxRetransmit;
	pRalinkCounters->OneSecTxNoRetryOkCount += StaTx1.field.TxSuccess;
	pRalinkCounters->OneSecTxFailCount += TxStaCnt0.field.TxFailCount;

#ifdef STATS_COUNT_SUPPORT
	pAd->WlanCounters.TransmittedFragmentCount.u.LowPart += StaTx1.field.TxSuccess;
	pAd->WlanCounters.RetryCount.u.LowPart += StaTx1.field.TxRetransmit;
	pAd->WlanCounters.FailedCount.u.LowPart += TxStaCnt0.field.TxFailCount;
#endif /* STATS_COUNT_SUPPORT */
	}


	/*if (pAd->bStaFifoTest == TRUE)*/
#ifdef STATS_COUNT_SUPPORT
	{
		RTMP_IO_READ32(pAd, TX_AGG_CNT, &TxAggCnt.word);
	RTMP_IO_READ32(pAd, TX_AGG_CNT0, &TxAggCnt0.word);
	RTMP_IO_READ32(pAd, TX_AGG_CNT1, &TxAggCnt1.word);
	RTMP_IO_READ32(pAd, TX_AGG_CNT2, &TxAggCnt2.word);
	RTMP_IO_READ32(pAd, TX_AGG_CNT3, &TxAggCnt3.word);
		RTMP_IO_READ32(pAd, TX_AGG_CNT4, &TxAggCnt4.word);
		RTMP_IO_READ32(pAd, TX_AGG_CNT5, &TxAggCnt5.word);
		RTMP_IO_READ32(pAd, TX_AGG_CNT6, &TxAggCnt6.word);
		RTMP_IO_READ32(pAd, TX_AGG_CNT7, &TxAggCnt7.word);
		pRalinkCounters->TxAggCount += TxAggCnt.field.AggTxCount;
		pRalinkCounters->TxNonAggCount += TxAggCnt.field.NonAggTxCount;
		pRalinkCounters->TxAgg1MPDUCount += TxAggCnt0.field.AggSize1Count;
		pRalinkCounters->TxAgg2MPDUCount += TxAggCnt0.field.AggSize2Count;
		
		pRalinkCounters->TxAgg3MPDUCount += TxAggCnt1.field.AggSize3Count;
		pRalinkCounters->TxAgg4MPDUCount += TxAggCnt1.field.AggSize4Count;
		pRalinkCounters->TxAgg5MPDUCount += TxAggCnt2.field.AggSize5Count;
		pRalinkCounters->TxAgg6MPDUCount += TxAggCnt2.field.AggSize6Count;
	
		pRalinkCounters->TxAgg7MPDUCount += TxAggCnt3.field.AggSize7Count;
		pRalinkCounters->TxAgg8MPDUCount += TxAggCnt3.field.AggSize8Count;
		pRalinkCounters->TxAgg9MPDUCount += TxAggCnt4.field.AggSize9Count;
		pRalinkCounters->TxAgg10MPDUCount += TxAggCnt4.field.AggSize10Count;

		pRalinkCounters->TxAgg11MPDUCount += TxAggCnt5.field.AggSize11Count;
		pRalinkCounters->TxAgg12MPDUCount += TxAggCnt5.field.AggSize12Count;
		pRalinkCounters->TxAgg13MPDUCount += TxAggCnt6.field.AggSize13Count;
		pRalinkCounters->TxAgg14MPDUCount += TxAggCnt6.field.AggSize14Count;

		pRalinkCounters->TxAgg15MPDUCount += TxAggCnt7.field.AggSize15Count;
		pRalinkCounters->TxAgg16MPDUCount += TxAggCnt7.field.AggSize16Count;

		/* Calculate the transmitted A-MPDU count*/
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += TxAggCnt0.field.AggSize1Count;
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt0.field.AggSize2Count >> 1);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt1.field.AggSize3Count / 3);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt1.field.AggSize4Count >> 2);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt2.field.AggSize5Count / 5);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt2.field.AggSize6Count / 6);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt3.field.AggSize7Count / 7);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt3.field.AggSize8Count >> 3);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt4.field.AggSize9Count / 9);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt4.field.AggSize10Count / 10);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt5.field.AggSize11Count / 11);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt5.field.AggSize12Count / 12);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt6.field.AggSize13Count / 13);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt6.field.AggSize14Count / 14);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt7.field.AggSize15Count / 15);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt7.field.AggSize16Count >> 4);	
	}
#endif /* STATS_COUNT_SUPPORT */			

#ifdef DBG_DIAGNOSE
	{
		RtmpDiagStruct	*pDiag;
		UCHAR			ArrayCurIdx, i;
		
		pDiag = &pAd->DiagStruct;
		ArrayCurIdx = pDiag->ArrayCurIdx;
		
		if (pDiag->inited == 0)
		{
			NdisZeroMemory(pDiag, sizeof(struct _RtmpDiagStrcut_));
			pDiag->ArrayStartIdx = pDiag->ArrayCurIdx = 0;
			pDiag->inited = 1;
		}
		else
		{
			/* Tx*/
			pDiag->TxFailCnt[ArrayCurIdx] = TxStaCnt0.field.TxFailCount;
			pDiag->TxAggCnt[ArrayCurIdx] = TxAggCnt.field.AggTxCount;
			pDiag->TxNonAggCnt[ArrayCurIdx] = TxAggCnt.field.NonAggTxCount;

			pDiag->TxAMPDUCnt[ArrayCurIdx][0] = TxAggCnt0.field.AggSize1Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][1] = TxAggCnt0.field.AggSize2Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][2] = TxAggCnt1.field.AggSize3Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][3] = TxAggCnt1.field.AggSize4Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][4] = TxAggCnt2.field.AggSize5Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][5] = TxAggCnt2.field.AggSize6Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][6] = TxAggCnt3.field.AggSize7Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][7] = TxAggCnt3.field.AggSize8Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][8] = TxAggCnt4.field.AggSize9Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][9] = TxAggCnt4.field.AggSize10Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][10] = TxAggCnt5.field.AggSize11Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][11] = TxAggCnt5.field.AggSize12Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][12] = TxAggCnt6.field.AggSize13Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][13] = TxAggCnt6.field.AggSize14Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][14] = TxAggCnt7.field.AggSize15Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][15] = TxAggCnt7.field.AggSize16Count;

			pDiag->RxCrcErrCnt[ArrayCurIdx] = RxStaCnt0.field.CrcErr;
			
			INC_RING_INDEX(pDiag->ArrayCurIdx,  DIAGNOSE_TIME);
			ArrayCurIdx = pDiag->ArrayCurIdx;
			for (i =0; i < 9; i++)
			{
				pDiag->TxDescCnt[ArrayCurIdx][i]= 0;
				pDiag->TxSWQueCnt[ArrayCurIdx][i] =0;
				pDiag->TxMcsCnt[ArrayCurIdx][i] = 0;
				pDiag->RxMcsCnt[ArrayCurIdx][i] = 0;
			}
			pDiag->TxDataCnt[ArrayCurIdx] = 0;
			pDiag->TxFailCnt[ArrayCurIdx] = 0;
			pDiag->RxDataCnt[ArrayCurIdx] = 0;
			pDiag->RxCrcErrCnt[ArrayCurIdx]  = 0;
/*			for (i = 9; i < 16; i++)*/
			for (i = 9; i < 24; i++) /* 3*3*/
			{
				pDiag->TxDescCnt[ArrayCurIdx][i] = 0;
				pDiag->TxMcsCnt[ArrayCurIdx][i] = 0;
				pDiag->RxMcsCnt[ArrayCurIdx][i] = 0;
}

			if (pDiag->ArrayCurIdx == pDiag->ArrayStartIdx)
				INC_RING_INDEX(pDiag->ArrayStartIdx,  DIAGNOSE_TIME);
		}
		
	}
#endif /* DBG_DIAGNOSE */


}

NDIS_STATUS NICLoadFirmware(
	IN PRTMP_ADAPTER pAd)
{
	/*
	1. For PCI:
        (1) Write SYS_CTRL bit16(HST_PM_SEL) to 1
        (2) Write 8051 firmware to RAM.
        (3) Write SYS_CTRL to 0.
        (4) Write SYS_CTRL bit0(MCU_RESET) to 1 to do MCU HW reset.

	2. For USB:
        (1)Get current firmware operation mode via VendorRequest(0x1, 0x11) command.
        (2) Write SYS_CTRL bit7(MCU_READY) to 0.
        (3) Write SYS_CTRL bit0(MCU_RESET) to 1 to do MCU reset to run 8051 on ROM.
        (4) Check MCU ready via SYS_CTRL bit7(MCU_READY).
        (5) Write 8051 firmware to RAM.
        (6) Write MAC 0x7014 to 0xffffffff.
        (7) Write MAC 0x701c to 0xffffffff.
        (8) Change 8051 from ROM to RAM site via VendorRequest(0x01, 0x8) command.
	*/

	NDIS_STATUS	 status = NDIS_STATUS_SUCCESS;
	if (pAd->chipOps.loadFirmware)
		status = pAd->chipOps.loadFirmware(pAd);

	return status;
}


/*
	========================================================================
	
	Routine Description:
		erase 8051 firmware image in MAC ASIC

	Arguments:
		Adapter						Pointer to our adapter

	IRQL = PASSIVE_LEVEL
		
	========================================================================
*/
VOID NICEraseFirmware(
	IN PRTMP_ADAPTER pAd)
{
	if (pAd->chipOps.eraseFirmware)
		pAd->chipOps.eraseFirmware(pAd);
	
}/* End of NICEraseFirmware */


/*
	========================================================================
	
	Routine Description:
		Load Tx rate switching parameters

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS         firmware image load ok
		NDIS_STATUS_FAILURE         image not found

	IRQL = PASSIVE_LEVEL

	Rate Table Format:
		1. (B0: Valid Item number) (B1:Initial item from zero)
		2. Item Number(Dec)      Mode(Hex)     Current MCS(Dec)    TrainUp(Dec)    TrainDown(Dec)
		
	========================================================================
*/
NDIS_STATUS NICLoadRateSwitchingParams(
	IN PRTMP_ADAPTER pAd)
{
	return NDIS_STATUS_SUCCESS;
}


/*
	========================================================================
	
	Routine Description:
		Compare two memory block

	Arguments:
		pSrc1		Pointer to first memory address
		pSrc2		Pointer to second memory address
		
	Return Value:
		0:			memory is equal
		1:			pSrc1 memory is larger
		2:			pSrc2 memory is larger

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
ULONG	RTMPCompareMemory(
	IN	PVOID	pSrc1,
	IN	PVOID	pSrc2,
	IN	ULONG	Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	ULONG	Index = 0;

	pMem1 = (PUCHAR) pSrc1;
	pMem2 = (PUCHAR) pSrc2;

	for (Index = 0; Index < Length; Index++)
	{
		if (pMem1[Index] > pMem2[Index])
			return (1);
		else if (pMem1[Index] < pMem2[Index])
			return (2);
	}

	/* Equal*/
	return (0);
}


/*
	========================================================================
	
	Routine Description:
		Zero out memory block

	Arguments:
		pSrc1		Pointer to memory address
		Length		Size

	Return Value:
		None
		
	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID	RTMPZeroMemory(
	IN	PVOID	pSrc,
	IN	ULONG	Length)
{
	PUCHAR	pMem;
	ULONG	Index = 0;

	pMem = (PUCHAR) pSrc;

	for (Index = 0; Index < Length; Index++)
	{
		pMem[Index] = 0x00;
	}
}


/*
	========================================================================
	
	Routine Description:
		Copy data from memory block 1 to memory block 2

	Arguments:
		pDest		Pointer to destination memory address
		pSrc		Pointer to source memory address
		Length		Copy size
		
	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID RTMPMoveMemory(
	OUT	PVOID	pDest,
	IN	PVOID	pSrc,
	IN	ULONG	Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	UINT	Index;

	ASSERT((Length==0) || (pDest && pSrc));

	pMem1 = (PUCHAR) pDest;
	pMem2 = (PUCHAR) pSrc;

	for (Index = 0; Index < Length; Index++)
	{
		pMem1[Index] = pMem2[Index];
	}
}

VOID UserCfgExit(
	IN RTMP_ADAPTER *pAd)
{
#ifdef DOT11_N_SUPPORT
	BATableExit(pAd);
#endif /* DOT11_N_SUPPORT */

	NdisFreeSpinLock(&pAd->MacTabLock);
}

/*
	========================================================================
	
	Routine Description:
		Initialize port configuration structure

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	UserCfgInit(
	IN	PRTMP_ADAPTER pAd)
{
	UINT i;
#ifdef CONFIG_AP_SUPPORT
	UINT j;
#endif /* CONFIG_AP_SUPPORT */
/*	EDCA_PARM DefaultEdcaParm;*/
    UINT key_index, bss_index;


	DBGPRINT(RT_DEBUG_TRACE, ("--> UserCfgInit\n"));    
	 
	pAd->IndicateMediaState = NdisMediaStateDisconnected; 
	
	/*  part I. intialize common configuration*/
	

	for(key_index=0; key_index<SHARE_KEY_NUM; key_index++)
	{
		for(bss_index = 0; bss_index < MAX_MBSSID_NUM(pAd) + MAX_P2P_NUM; bss_index++)
		{
			pAd->SharedKey[bss_index][key_index].KeyLen = 0;
			pAd->SharedKey[bss_index][key_index].CipherAlg = CIPHER_NONE;
		}
	}

	pAd->bLocalAdminMAC = FALSE;
	pAd->EepromAccess = FALSE;
	
	pAd->Antenna.word = 0; 
	pAd->CommonCfg.BBPCurrentBW = BW_20;

#ifdef RTMP_MAC_PCI
#ifdef LED_CONTROL_SUPPORT
	pAd->LedCntl.LedIndicatorStrength = 0;
#endif /* LED_CONTROL_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	pAd->RLnkCtrlOffset = 0;
	pAd->HostLnkCtrlOffset = 0;
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_PCI */

	pAd->bAutoTxAgcA = FALSE;			/* Default is OFF*/
	pAd->bAutoTxAgcG = FALSE;			/* Default is OFF*/
#ifdef RTMP_INTERNAL_TX_ALC
	pAd->TxPowerCtrl.bInternalTxALC = FALSE; /* Off by default*/
#endif /* RTMP_INTERNAL_TX_ALC */
	pAd->RfIcType = RFIC_2820;

	/* Init timer for reset complete event*/
	pAd->CommonCfg.CentralChannel = 1;
	pAd->bForcePrintTX = FALSE;
	pAd->bForcePrintRX = FALSE;
	pAd->bStaFifoTest = FALSE;
	pAd->bProtectionTest = FALSE;
	pAd->bHCCATest = FALSE;
	pAd->bGenOneHCCA = FALSE;
	pAd->CommonCfg.Dsifs = 10;      /* in units of usec */
	pAd->CommonCfg.TxPower = 100; /* mW*/
	pAd->CommonCfg.TxPowerPercentage = 0xffffffff; /* AUTO*/
	pAd->CommonCfg.TxPowerDefault = 0xffffffff; /* AUTO*/
	pAd->CommonCfg.TxPreamble = Rt802_11PreambleAuto; /* use Long preamble on TX by defaut*/
	pAd->CommonCfg.bUseZeroToDisableFragment = FALSE;
	pAd->CommonCfg.RtsThreshold = 2347;
	pAd->CommonCfg.FragmentThreshold = 2346;
	pAd->CommonCfg.UseBGProtection = 0;    /* 0: AUTO*/
	pAd->CommonCfg.bEnableTxBurst = TRUE; /* 0;    	*/
	pAd->CommonCfg.PhyMode = 0xff;     /* unknown*/
	pAd->CommonCfg.SavedPhyMode = 0xff;
	pAd->CommonCfg.BandState = UNKNOWN_BAND;
	pAd->CommonCfg.RadarDetect.CSPeriod = 10;
	pAd->CommonCfg.RadarDetect.CSCount = 0;
	pAd->CommonCfg.RadarDetect.RDMode = RD_NORMAL_MODE;
	pAd->CommonCfg.bDFSIndoor = 1;

#ifdef RTMP_RBUS_SUPPORT
	RTMP_SYS_IO_READ32(0xb000000c, &pAd->CommonCfg.CID);
	RTMP_SYS_IO_READ32(0xb0000000, &pAd->CommonCfg.CN);
#endif /* RTMP_RBUS_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#if defined(RTMP_RBUS_SUPPORT)|| defined(DFS_INTERRUPT_SUPPORT)
#ifdef RT2880
	RTMP_SYS_IO_READ32(0xa030000c, &pAd->CommonCfg.CID);
#endif

#ifdef DFS_SUPPORT
	/* TODO: shiang, check the R65 is specific for RT2880 only or also need to set this register in RT3052*/
	pAd->CommonCfg.R65 = 0x1d;
	pAd->CommonCfg.DFS_R66 = 0x60;

#ifdef DFS_HARDWARE_SUPPORT
	pAd->CommonCfg.DeltaDelay = 0x3;
	pAd->CommonCfg.ChEnable = 0xf;
	pAd->CommonCfg.RadarReEnable = 1;
	pAd->CommonCfg.PollTime = 0;
	pAd->CommonCfg.fcc_5_threshold = 1000;
	pAd->CommonCfg.fcc_5_idx = 0;
	pAd->CommonCfg.fcc_5_last_idx = 0;
#endif /* DFS_HARDWARE_SUPPORT */

	pAd->CommonCfg.McuRadarDebug = 0;

#ifdef DFS_HARDWARE_SUPPORT
	pAd->CommonCfg.DfsRssiHigh = -30;
	pAd->CommonCfg.DfsRssiLow = -90;
	pAd->CommonCfg.DFSWatchDogIsRunning=FALSE;
#ifdef DFS_2_SUPPORT
	pAd->CommonCfg.RadarEventExpire[3] = 0x99999999;
	pAd->CommonCfg.Symmetric_Round = 1;
	pAd->CommonCfg.VGA_Mask = 0; /*45;*/
	pAd->CommonCfg.Packet_End_Mask = 0; /*45;*/
	pAd->CommonCfg.Rx_PE_Mask = 0; /*45;*/
	pAd->CommonCfg.PollTime = 3;
#endif /* DFS_2_SUPPORT */
#endif /* DFS_HARDWARE_SUPPORT */
	pAd->CommonCfg.bDFSIndoor = 1;

#endif /* DFS_SUPPORT */
#endif /* defined(RTMP_RBUS_SUPPORT)|| defined(DFS_INTERRUPT_SUPPORT) */
#ifdef DFS_SUPPORT
#ifdef RTMP_RBUS_SUPPORT
#ifdef DFS_HARDWARE_SUPPORT
if ((pAd->MACVersion == 0x28720200) && (pAd->CommonCfg.CID == 0x200))
		pAd->CommonCfg.dfs_func = HARDWARE_DFS_V1;
	else
#endif /* DFS_HARDWARE_SUPPORT */
#endif /* RTMP_RBUS_SUPPORT */

		if (pAd->MACVersion <= 0x28720200)
		pAd->CommonCfg.dfs_func = SOFTWARE_DFS;
	else
		pAd->CommonCfg.dfs_func = HARDWARE_DFS_V2;
#endif /* DFS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CARRIER_DETECTION_SUPPORT
#ifdef TONE_RADAR_DETECT_SUPPORT
	pAd->CommonCfg.CarrierDetect.delta = CARRIER_DETECT_DELTA;
	pAd->CommonCfg.CarrierDetect.div_flag = CARRIER_DETECT_DIV_FLAG;
	pAd->CommonCfg.CarrierDetect.criteria = CARRIER_DETECT_CRITIRIA;
	pAd->CommonCfg.CarrierDetect.threshold = CARRIER_DETECT_THRESHOLD;
#endif /* TONE_RADAR_DETECT_SUPPORT */
#endif /* CARRIER_DETECTION_SUPPORT */

	pAd->CommonCfg.RadarDetect.ChMovingTime = 65;
#ifdef RTMP_RBUS_SUPPORT
	pAd->CommonCfg.RadarDetect.LongPulseRadarTh = 2;
	pAd->CommonCfg.RadarDetect.AvgRssiReq = -75;
#else /* original rt28xx source code*/
	pAd->CommonCfg.RadarDetect.LongPulseRadarTh = 3;
#endif /* RTMP_RBUS_SUPPORT */
	pAd->CommonCfg.bAPSDCapable = FALSE;
	pAd->CommonCfg.bNeedSendTriggerFrame = FALSE;
	pAd->CommonCfg.TriggerTimerCount = 0;
	pAd->CommonCfg.bAPSDForcePowerSave = FALSE;
	pAd->CommonCfg.bCountryFlag = FALSE;
	pAd->CommonCfg.TxStream = 0;
	pAd->CommonCfg.RxStream = 0;

	NdisZeroMemory(&pAd->BeaconTxWI, sizeof(pAd->BeaconTxWI));

#ifdef DOT11_N_SUPPORT
	NdisZeroMemory(&pAd->CommonCfg.HtCapability, sizeof(pAd->CommonCfg.HtCapability));
	pAd->HTCEnable = FALSE;
	pAd->bBroadComHT = FALSE;
	pAd->CommonCfg.bRdg = FALSE;
	
#ifdef DOT11N_DRAFT3
	pAd->CommonCfg.Dot11OBssScanPassiveDwell = dot11OBSSScanPassiveDwell;	/* Unit : TU. 5~1000*/
	pAd->CommonCfg.Dot11OBssScanActiveDwell = dot11OBSSScanActiveDwell;	/* Unit : TU. 10~1000*/
	pAd->CommonCfg.Dot11BssWidthTriggerScanInt = dot11BSSWidthTriggerScanInterval;	/* Unit : Second	*/
	pAd->CommonCfg.Dot11OBssScanPassiveTotalPerChannel = dot11OBSSScanPassiveTotalPerChannel;	/* Unit : TU. 200~10000*/
	pAd->CommonCfg.Dot11OBssScanActiveTotalPerChannel = dot11OBSSScanActiveTotalPerChannel;	/* Unit : TU. 20~10000*/
	pAd->CommonCfg.Dot11BssWidthChanTranDelayFactor = dot11BSSWidthChannelTransactionDelayFactor;
	pAd->CommonCfg.Dot11OBssScanActivityThre = dot11BSSScanActivityThreshold;	/* Unit : percentage*/
	pAd->CommonCfg.Dot11BssWidthChanTranDelay = (pAd->CommonCfg.Dot11BssWidthTriggerScanInt * pAd->CommonCfg.Dot11BssWidthChanTranDelayFactor);

	pAd->CommonCfg.bBssCoexEnable = TRUE; /* by default, we enable this feature, you can disable it via the profile or ioctl command*/
	pAd->CommonCfg.BssCoexApCntThr = 0; 
#endif  /* DOT11N_DRAFT3 */

	NdisZeroMemory(&pAd->CommonCfg.AddHTInfo, sizeof(pAd->CommonCfg.AddHTInfo));
	pAd->CommonCfg.BACapability.field.MMPSmode = MMPS_ENABLE;
	pAd->CommonCfg.BACapability.field.MpduDensity = 0;
	pAd->CommonCfg.BACapability.field.Policy = IMMED_BA;
	pAd->CommonCfg.BACapability.field.RxBAWinLimit = 64; /*32;*/
	pAd->CommonCfg.BACapability.field.TxBAWinLimit = 64; /*32;*/
	DBGPRINT(RT_DEBUG_TRACE, ("--> UserCfgInit. BACapability = 0x%x\n", pAd->CommonCfg.BACapability.word));    

	pAd->CommonCfg.BACapability.field.AutoBA = FALSE;	
	BATableInit(pAd, &pAd->BATable);

	pAd->CommonCfg.bExtChannelSwitchAnnouncement = 1;
	pAd->CommonCfg.bHTProtect = 1;
	pAd->CommonCfg.bMIMOPSEnable = TRUE;
#ifdef GREENAP_SUPPORT
	pAd->ApCfg.bGreenAPEnable=FALSE;
	pAd->ApCfg.bGreenAPActive = FALSE;
	pAd->ApCfg.GreenAPLevel= GREENAP_WITHOUT_ANY_STAS_CONNECT;
#endif /* GREENAP_SUPPORT */
	pAd->CommonCfg.bBADecline = FALSE;
	pAd->CommonCfg.bDisableReordering = FALSE;

	if (pAd->MACVersion == 0x28720200)
	{
		pAd->CommonCfg.TxBASize = 13; /*by Jerry recommend*/
	}else{
		pAd->CommonCfg.TxBASize = 7;
	}

	pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;
#endif /* DOT11_N_SUPPORT */

	/*pAd->CommonCfg.HTPhyMode.field.BW = BW_20;*/
	/*pAd->CommonCfg.HTPhyMode.field.MCS = MCS_AUTO;*/
	/*pAd->CommonCfg.HTPhyMode.field.ShortGI = GI_800;*/
	/*pAd->CommonCfg.HTPhyMode.field.STBC = STBC_NONE;*/
	pAd->CommonCfg.TxRate = RATE_6;
	
	pAd->CommonCfg.MlmeTransmit.field.MCS = MCS_RATE_6;
	pAd->CommonCfg.MlmeTransmit.field.BW = BW_20;
	pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_OFDM;

	pAd->CommonCfg.BeaconPeriod = 100;     /* in mSec*/

#ifdef RTMP_RBUS_SUPPORT
	pAd->CommonCfg.DebugFlags = 0;
#endif /* RTMP_RBUS_SUPPORT */

#ifdef WAPI_SUPPORT
	pAd->CommonCfg.wapi_usk_rekey_method = REKEY_METHOD_DISABLE;
	pAd->CommonCfg.wapi_msk_rekey_method = REKEY_METHOD_DISABLE;
	pAd->CommonCfg.wapi_msk_rekey_cnt = 0;
#endif /* WAPI_SUPPORT */

#ifdef VCORECAL_SUPPORT
	pAd->CommonCfg.VCORecalibrationThreshold = DEFAULT_VCO_RECALIBRATION_THRESHOLD;
	pAd->RefreshTssi = 1;
#endif /* VCORECAL_SUPPORT */

#ifdef MCAST_RATE_SPECIFIC
	pAd->CommonCfg.MCastPhyMode.word
	= pAd->MacTab.Content[MCAST_WCID].HTPhyMode.word;
#endif /* MCAST_RATE_SPECIFIC */

	/* WFA policy - disallow TH rate in WEP or TKIP cipher */
	pAd->CommonCfg.HT_DisallowTKIP = TRUE;

	
	/* part II. intialize STA specific configuration*/
	
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_DIRECT);
		RX_FILTER_CLEAR_FLAG(pAd, fRX_FILTER_ACCEPT_MULTICAST);
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_BROADCAST);
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_ALL_MULTICAST);

		pAd->StaCfg.Psm = PWR_ACTIVE;

		pAd->StaCfg.PairCipher = Ndis802_11EncryptionDisabled;
		pAd->StaCfg.GroupCipher = Ndis802_11EncryptionDisabled;
		pAd->StaCfg.bMixCipher = FALSE;	
		pAd->StaCfg.DefaultKeyId = 0;

		/* 802.1x port control*/
		pAd->StaCfg.PrivacyFilter = Ndis802_11PrivFilter8021xWEP;
		pAd->StaCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;
		pAd->StaCfg.LastMicErrorTime = 0;
		pAd->StaCfg.MicErrCnt        = 0;
		pAd->StaCfg.bBlockAssoc      = FALSE;
		pAd->StaCfg.WpaState         = SS_NOTUSE;

		pAd->CommonCfg.NdisRadioStateOff = FALSE;		/* New to support microsoft disable radio with OID command*/

		pAd->StaCfg.RssiTrigger = 0;
		NdisZeroMemory(&pAd->StaCfg.RssiSample, sizeof(RSSI_SAMPLE));
		pAd->StaCfg.RssiTriggerMode = RSSI_TRIGGERED_UPON_BELOW_THRESHOLD;
		pAd->StaCfg.AtimWin = 0;
		pAd->StaCfg.DefaultListenCount = 3;/*default listen count;*/
		pAd->StaCfg.BssType = BSS_INFRA;  /* BSS_INFRA or BSS_ADHOC or BSS_MONITOR*/
		pAd->StaCfg.bScanReqIsFromWebUI = FALSE;
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_WAKEUP_NOW);

		pAd->StaCfg.bAutoTxRateSwitch = TRUE;
		pAd->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
		pAd->StaCfg.bAutoConnectIfNoSSID = FALSE;
	}

#ifdef EXT_BUILD_CHANNEL_LIST
	pAd->StaCfg.IEEE80211dClientMode = Rt802_11_D_None;
#endif /* EXT_BUILD_CHANNEL_LIST */

#ifdef RTMP_MAC_PCI

pAd->brt30xxBanMcuCmd = FALSE;
pAd->StaCfg.PSControl.field.EnableNewPS=FALSE;

#ifdef PCIE_PS_SUPPORT
pAd->StaCfg.PSControl.field.EnableNewPS=TRUE;
pAd->b3090ESpecialChip = FALSE;
/*The value of PowerMode could be 1 or 3. Level 3 could save more power than Level 1. */
pAd->StaCfg.PSControl.field.rt30xxPowerMode=3;
pAd->StaCfg.PSControl.field.rt30xxForceASPMTest=0;
pAd->StaCfg.PSControl.field.rt30xxFollowHostASPM=1;
#endif /* PCIE_PS_SUPPORT */
#endif /* RTMP_MAC_PCI */
#endif /* CONFIG_STA_SUPPORT */

	/* global variables mXXXX used in MAC protocol state machines*/
	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_ADHOC_ON);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_INFRA_ON);

	/* PHY specification*/
	pAd->CommonCfg.PhyMode = PHY_11BG_MIXED;		/* default PHY mode*/
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);  /* CCK use LONG preamble*/

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* user desired power mode*/
		pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;
		pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;
		pAd->StaCfg.bWindowsACCAMEnable = FALSE;

		pAd->StaCfg.bHwRadio  = TRUE; /* Default Hardware Radio status is On*/
		pAd->StaCfg.bSwRadio  = TRUE; /* Default Software Radio status is On*/
		pAd->StaCfg.bRadio    = TRUE; /* bHwRadio && bSwRadio*/
		pAd->StaCfg.bHardwareRadio = FALSE;		/* Default is OFF*/
		pAd->StaCfg.bShowHiddenSSID = FALSE;		/* Default no show*/

		/* Nitro mode control*/
		pAd->StaCfg.bAutoReconnect = TRUE;

		/* Save the init time as last scan time, the system should do scan after 2 seconds.*/
		/* This patch is for driver wake up from standby mode, system will do scan right away.*/
		NdisGetSystemUpTime(&pAd->StaCfg.LastScanTime);
		if (pAd->StaCfg.LastScanTime > 10 * OS_HZ)
			pAd->StaCfg.LastScanTime -= (10 * OS_HZ);
		
		NdisZeroMemory(pAd->nickname, IW_ESSID_MAX_SIZE+1);
#ifdef PROFILE_STORE
		pAd->bWriteDat = FALSE;
#endif /* PROFILE_STORE */
#ifdef WPA_SUPPLICANT_SUPPORT
		pAd->StaCfg.IEEE8021X = FALSE;
		pAd->StaCfg.IEEE8021x_required_keys = FALSE;
		pAd->StaCfg.WpaSupplicantUP = WPA_SUPPLICANT_DISABLE;
		pAd->StaCfg.bRSN_IE_FromWpaSupplicant = FALSE;
		pAd->StaCfg.bLostAp = FALSE;
		pAd->StaCfg.pWpsProbeReqIe = NULL;
		pAd->StaCfg.WpsProbeReqIeLen = 0;
		pAd->StaCfg.pWpaAssocIe = NULL;
		pAd->StaCfg.WpaAssocIeLen = 0;
		pAd->StaCfg.WpaSupplicantScanCount = 0;
#endif /* WPA_SUPPLICANT_SUPPORT */

#ifdef WSC_STA_SUPPORT
		{
			INT					idx;
			PWSC_CTRL 			pWscControl;
#ifdef WSC_V2_SUPPORT
			PWSC_V2_INFO	pWscV2Info;
#endif /* WSC_V2_SUPPORT */

			/*
				WscControl cannot be zero here, because WscControl timers are initial in MLME Initialize 
				and MLME Initialize is called before UserCfgInit.

			*/
			pWscControl = &pAd->StaCfg.WscControl;
			pWscControl->WscConfMode = WSC_DISABLE;
			pWscControl->WscMode = WSC_PIN_MODE;
			pWscControl->WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
#ifdef WSC_V2_SUPPORT
			pWscControl->WscConfigMethods= 0x238C;
#else
			pWscControl->WscConfigMethods= 0x008C;
#endif /* WSC_V2_SUPPORT */
#ifdef P2P_SUPPORT
			pWscControl->WscConfigMethods |= 0x0100;
#endif /* P2P_SUPPORT */
			pWscControl->WscState = WSC_STATE_OFF;
			pWscControl->WscStatus = STATUS_WSC_NOTUSED;
			pWscControl->WscPinCode = 0;
			pWscControl->WscLastPinFromEnrollee = 0;
			pWscControl->WscEnrollee4digitPinCode = FALSE;
			pWscControl->WscEnrolleePinCode = 0;
			pWscControl->WscSelReg = 0;
			NdisZeroMemory(&pAd->StaCfg.WscControl.RegData, sizeof(WSC_REG_DATA));
			NdisZeroMemory(&pWscControl->WscProfile, sizeof(WSC_PROFILE));
			pWscControl->WscUseUPnP = 0;
			pWscControl->WscEnAssociateIE = TRUE;
			pWscControl->WscEnProbeReqIE = TRUE;
			pWscControl->RegData.ReComputePke = 1;
			pWscControl->lastId = 1;
			pWscControl->EntryIfIdx = BSS0;
			pWscControl->pAd = pAd;
			pWscControl->WscDriverAutoConnect = 0x02;
			pAd->WriteWscCfgToDatFile = 0xFF;
			pWscControl->WscRejectSamePinFromEnrollee = FALSE;
			pWscControl->WpsApBand = PREFERRED_WPS_AP_PHY_TYPE_AUTO_SELECTION;
			pWscControl->bCheckMultiByte = FALSE;
			pWscControl->bWscAutoTigeer = FALSE;
			/* Enrollee Nonce, first generate and save to Wsc Control Block*/
			for (idx = 0; idx < 16; idx++)
			{
				pWscControl->RegData.SelfNonce[idx] = RandomByte(pAd);
			}
			pWscControl->WscRxBufLen = 0;
			pWscControl->pWscRxBuf = NULL;
			os_alloc_mem(pAd, &pWscControl->pWscRxBuf, MGMT_DMA_BUFFER_SIZE);
			if (pWscControl->pWscRxBuf)
				NdisZeroMemory(pWscControl->pWscRxBuf, MGMT_DMA_BUFFER_SIZE);
			pWscControl->WscTxBufLen = 0;
			pWscControl->pWscTxBuf = NULL;
			os_alloc_mem(pAd, &pWscControl->pWscTxBuf, MGMT_DMA_BUFFER_SIZE);
			if (pWscControl->pWscTxBuf)
				NdisZeroMemory(pWscControl->pWscTxBuf, MGMT_DMA_BUFFER_SIZE);
			pWscControl->bWscFragment = FALSE;
			pWscControl->WscFragSize = 128;
			initList(&pWscControl->WscPeerList);

#ifdef WSC_V2_SUPPORT
			pWscV2Info = &pWscControl->WscV2Info;
			pWscV2Info->bWpsEnable = TRUE;
			pWscV2Info->ExtraTlv.TlvLen = 0;
			pWscV2Info->ExtraTlv.TlvTag = 0;
			pWscV2Info->ExtraTlv.pTlvData = NULL;
			pWscV2Info->ExtraTlv.TlvType = TLV_ASCII;
			pWscV2Info->bEnableWpsV2 = TRUE;
			pWscV2Info->bForceSetAP = FALSE;
#endif /* WSC_V2_SUPPORT */
		}
#endif /* WSC_STA_SUPPORT */
		NdisZeroMemory(pAd->StaCfg.ReplayCounter, 8);


		pAd->StaCfg.bAutoConnectByBssid = FALSE;
		pAd->StaCfg.BeaconLostTime = BEACON_LOST_TIME;
		NdisZeroMemory(pAd->StaCfg.WpaPassPhrase, 64);    
		pAd->StaCfg.WpaPassPhraseLen = 0;
		pAd->StaCfg.bAutoRoaming = FALSE;
		pAd->StaCfg.bForceTxBurst = FALSE;
		pAd->StaCfg.bNotFirstScan = FALSE;
		pAd->StaCfg.bImprovedScan = FALSE;
#ifdef DOT11_N_SUPPORT
		pAd->StaCfg.bAdhocN = TRUE;
#endif /* DOT11_N_SUPPORT */
		pAd->StaCfg.bFastConnect = FALSE;
		pAd->StaCfg.bAdhocCreator = FALSE;
	}
#endif /* CONFIG_STA_SUPPORT */

	/* Default for extra information is not valid*/
	pAd->ExtraInfo = EXTRA_INFO_CLEAR;
	
	/* Default Config change flag*/
	pAd->bConfigChanged = FALSE;

	/* */
	/* part III. AP configurations*/
	
#ifdef CONFIG_AP_SUPPORT
#ifndef P2P_APCLI_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* P2P_APCLI_SUPPORT */
	{
		/* Set MBSS Default Configurations*/
		pAd->ApCfg.BssidNum = MAX_MBSSID_NUM(pAd);
		for(j = BSS0; j < pAd->ApCfg.BssidNum; j++)
		{
			pAd->ApCfg.MBSSID[j].AuthMode = Ndis802_11AuthModeOpen;
			pAd->ApCfg.MBSSID[j].WepStatus = Ndis802_11EncryptionDisabled;
			pAd->ApCfg.MBSSID[j].GroupKeyWepStatus = Ndis802_11EncryptionDisabled;
			pAd->ApCfg.MBSSID[j].DefaultKeyId = 0;
			pAd->ApCfg.MBSSID[j].WpaMixPairCipher = MIX_CIPHER_NOTUSE;
			pAd->ApCfg.MBSSID[j].RekeyCountDown = 0;	/* it's used for WPA rekey */

#ifdef DOT1X_SUPPORT
			pAd->ApCfg.MBSSID[j].IEEE8021X = FALSE;
			pAd->ApCfg.MBSSID[j].PreAuth = FALSE;

			/* PMK cache setting*/
			pAd->ApCfg.MBSSID[j].PMKCachePeriod = (10 * 60 * OS_HZ); /* unit : tick(default: 10 minute)*/
			NdisZeroMemory(&pAd->ApCfg.MBSSID[j].PMKIDCache, sizeof(NDIS_AP_802_11_PMKID));

			/* dot1x related per BSS */
			pAd->ApCfg.MBSSID[j].radius_srv_num = 0;
			pAd->ApCfg.MBSSID[j].NasIdLen = 0;
#endif /* DOT1X_SUPPORT */

			/* VLAN related */
        		pAd->ApCfg.MBSSID[j].VLAN_VID = 0;

			/* Default MCS as AUTO*/
			pAd->ApCfg.MBSSID[j].bAutoTxRateSwitch = TRUE;
			pAd->ApCfg.MBSSID[j].DesiredTransmitSetting.field.MCS = MCS_AUTO;

			/* Default is zero. It means no limit.*/
			pAd->ApCfg.MBSSID[j].MaxStaNum = 0;
			pAd->ApCfg.MBSSID[j].StaCount = 0;
			
#ifdef WSC_AP_SUPPORT
			pAd->ApCfg.MBSSID[j].WscSecurityMode = 0xff;
			{
				PWSC_CTRL pWscControl;
				INT idx;
#ifdef WSC_V2_SUPPORT
				PWSC_V2_INFO	pWscV2Info;
#endif /* WSC_V2_SUPPORT */
				/*
					WscControl cannot be zero here, because WscControl timers are initial in MLME Initialize 
					and MLME Initialize is called before UserCfgInit.

				*/
				pWscControl = &pAd->ApCfg.MBSSID[j].WscControl;
				NdisZeroMemory(&pWscControl->RegData, sizeof(WSC_REG_DATA));
				NdisZeroMemory(&pAd->CommonCfg.WscStaPbcProbeInfo, sizeof(WSC_STA_PBC_PROBE_INFO));
				pWscControl->WscMode = 1;
				pWscControl->WscConfStatus = 1;
#ifdef WSC_V2_SUPPORT
				pWscControl->WscConfigMethods= 0x238C;
#else
				pWscControl->WscConfigMethods= 0x0084;
#endif /* WSC_V2_SUPPORT */
#ifdef P2P_SUPPORT
				pWscControl->WscConfigMethods |= 0x0108;
#endif /* P2P_SUPPORT */
				pWscControl->RegData.ReComputePke = 1;
				pWscControl->lastId = 1;
				/* pWscControl->EntryIfIdx = (MIN_NET_DEVICE_FOR_MBSSID | j); */
				pWscControl->pAd = pAd;
				pWscControl->WscRejectSamePinFromEnrollee = FALSE;
				pAd->CommonCfg.WscPBCOverlap = FALSE;
#ifdef P2P_SUPPORT
				/*
					Set defaule value of WscConfMode to be (WSC_REGISTRAR | WSC_ENROLLEE) for WiFi P2P.
				*/
				pWscControl->WscConfMode = (WSC_REGISTRAR | WSC_ENROLLEE);
#else /* P2P_SUPPORT */
				pWscControl->WscConfMode = 0;
#endif /* !P2P_SUPPORT */
				pWscControl->WscStatus = 0;
				pWscControl->WscState = 0;
				pWscControl->WscPinCode = 0;
				pWscControl->WscLastPinFromEnrollee = 0;
				pWscControl->WscEnrollee4digitPinCode = FALSE;
				pWscControl->WscEnrolleePinCode = 0;
				pWscControl->WscSelReg = 0;
				pWscControl->WscUseUPnP = 0;
				pWscControl->bWCNTest = FALSE;
				pWscControl->WscKeyASCII = 0; /* default, 0 (64 Hex) */
				
				/*
					Enrollee 192 random bytes for DH key generation
				*/
				for (idx = 0; idx < 192; idx++)
					pWscControl->RegData.EnrolleeRandom[idx] = RandomByte(pAd);

				/* Enrollee Nonce, first generate and save to Wsc Control Block*/
				for (idx = 0; idx < 16; idx++)
				{
					pWscControl->RegData.SelfNonce[idx] = RandomByte(pAd);
				}
				NdisZeroMemory(&pWscControl->WscDefaultSsid, sizeof(NDIS_802_11_SSID));
				NdisZeroMemory(&pWscControl->Wsc_Uuid_Str[0], UUID_LEN_STR);
				NdisZeroMemory(&pWscControl->Wsc_Uuid_E[0], UUID_LEN_HEX);
				pWscControl->bCheckMultiByte = FALSE;
				pWscControl->bWscAutoTigeer = FALSE;
				pWscControl->bWscFragment = FALSE;
				pWscControl->WscFragSize = 128;
				pWscControl->WscRxBufLen = 0;
				pWscControl->pWscRxBuf = NULL;
				os_alloc_mem(pAd, &pWscControl->pWscRxBuf, MGMT_DMA_BUFFER_SIZE);
				if (pWscControl->pWscRxBuf)
					NdisZeroMemory(pWscControl->pWscRxBuf, MGMT_DMA_BUFFER_SIZE);
				pWscControl->WscTxBufLen = 0;
				pWscControl->pWscTxBuf = NULL;
				os_alloc_mem(pAd, &pWscControl->pWscTxBuf, MGMT_DMA_BUFFER_SIZE);
				if (pWscControl->pWscTxBuf)
					NdisZeroMemory(pWscControl->pWscTxBuf, MGMT_DMA_BUFFER_SIZE);
				initList(&pWscControl->WscPeerList);
				pWscControl->PinAttackCount = 0;
				pWscControl->bSetupLock = FALSE;
#ifdef WSC_V2_SUPPORT
				pWscV2Info = &pWscControl->WscV2Info;
				pWscV2Info->bWpsEnable = TRUE;
				pWscV2Info->ExtraTlv.TlvLen = 0;
				pWscV2Info->ExtraTlv.TlvTag = 0;
				pWscV2Info->ExtraTlv.pTlvData = NULL;
				pWscV2Info->ExtraTlv.TlvType = TLV_ASCII;
				pWscV2Info->bEnableWpsV2 = TRUE;
#endif /* WSC_V2_SUPPORT */

			}
#endif /* WSC_AP_SUPPORT */


			for(i = 0; i < WLAN_MAX_NUM_OF_TIM; i++)
	        		pAd->ApCfg.MBSSID[j].TimBitmaps[i] = 0;
		}
		pAd->ApCfg.DtimCount  = 0;
		pAd->ApCfg.DtimPeriod = DEFAULT_DTIM_PERIOD;

		pAd->ApCfg.ErpIeContent = 0;

		pAd->ApCfg.StaIdleTimeout = MAC_TABLE_AGEOUT_TIME;

#ifdef IDS_SUPPORT
		/* Default disable IDS threshold and reset all IDS counters*/
		pAd->ApCfg.IdsEnable = FALSE;
		pAd->ApCfg.AuthFloodThreshold = 0;
		pAd->ApCfg.AssocReqFloodThreshold = 0;
		pAd->ApCfg.ReassocReqFloodThreshold = 0;
		pAd->ApCfg.ProbeReqFloodThreshold = 0;
		pAd->ApCfg.DisassocFloodThreshold = 0;
		pAd->ApCfg.DeauthFloodThreshold = 0;
		pAd->ApCfg.EapReqFloodThreshold = 0;
		RTMPClearAllIdsCounter(pAd);
#endif /* IDS_SUPPORT */

#ifdef WDS_SUPPORT
		APWdsInitialize(pAd);
#endif /* WDS_SUPPORT*/

#ifdef WSC_INCLUDED
		pAd->WriteWscCfgToDatFile = 0xFF;
		pAd->WriteWscCfgToAr9DatFile = FALSE;
#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_RBUS_SUPPORT
		/* Update PortCfg by WebUI */
		pAd->bWscDriverAutoUpdateCfg = FALSE;
#else
		pAd->bWscDriverAutoUpdateCfg = TRUE;
#endif
#endif /* CONFIG_AP_SUPPORT */
#endif /* WSC_INCLUDED */

#ifdef APCLI_SUPPORT
		for(j = 0; j < MAX_APCLI_NUM; j++) 
		{
			pAd->ApCfg.ApCliTab[j].AuthMode = Ndis802_11AuthModeOpen;
			pAd->ApCfg.ApCliTab[j].WepStatus = Ndis802_11WEPDisabled;
			pAd->ApCfg.ApCliTab[j].bAutoTxRateSwitch = TRUE;
			pAd->ApCfg.ApCliTab[j].DesiredTransmitSetting.field.MCS = MCS_AUTO;
		}
#endif /* APCLI_SUPPORT */
		pAd->ApCfg.EntryClientCount = 0;
	}
#endif /* CONFIG_AP_SUPPORT */


	
	/* part IV. others*/
	
	/* dynamic BBP R66:sensibity tuning to overcome background noise*/
	pAd->BbpTuning.bEnable                = TRUE;  
	pAd->BbpTuning.FalseCcaLowerThreshold = 100;
	pAd->BbpTuning.FalseCcaUpperThreshold = 512;
	pAd->BbpTuning.R66Delta               = 4;
	pAd->Mlme.bEnableAutoAntennaCheck = TRUE;
	
	
	/* Also initial R66CurrentValue, RTUSBResumeMsduTransmission might use this value.*/
	/* if not initial this value, the default value will be 0.*/
	
	pAd->BbpTuning.R66CurrentValue = 0x38;

	pAd->Bbp94 = BBPR94_DEFAULT;
	pAd->BbpForCCK = FALSE;
	
	/* Default is FALSE for test bit 1*/
	/*pAd->bTest1 = FALSE;*/
	
	/* initialize MAC table and allocate spin lock*/
	NdisZeroMemory(&pAd->MacTab, sizeof(MAC_TABLE));
	InitializeQueueHeader(&pAd->MacTab.McastPsQueue);
	NdisAllocateSpinLock(pAd, &pAd->MacTabLock);

	/*RTMPInitTimer(pAd, &pAd->RECBATimer, RECBATimerTimeout, pAd, TRUE);*/
	/*RTMPSetTimer(&pAd->RECBATimer, REORDER_EXEC_INTV);*/

#ifdef RALINK_ATE
	NdisZeroMemory(&pAd->ate, sizeof(ATE_INFO));
	pAd->ate.Mode = ATE_STOP;
#ifdef RT3350
	if (IS_RT3350(pAd))
		pAd->ate.PABias = 0;
#endif /* RT3350  */
	pAd->ate.TxCount = 1000;/* to sync with QA and to exceed TX_RING_SIZE ... */
	pAd->ate.TxDoneCount = 0;
	pAd->ate.RFFreqOffset = 0;
	pAd->ate.Payload = 0xA5;/* to be backward compatible */	
	pAd->ate.IPG = 200;/* 200 : sync with QA */	
	pAd->ate.TxLength = 1024;
	pAd->ate.TxWI.ShortGI = 0;/* LONG GI : 800 ns*/
	pAd->ate.TxWI.PHYMODE = MODE_CCK;
	pAd->ate.TxWI.MCS = 3;
	pAd->ate.TxWI.BW = BW_20;
	/* please do not change this default channel value */
	pAd->ate.Channel = 1;


#ifdef RTMP_MAC_PCI 
#endif /* RTMP_MAC_PCI */
	pAd->ate.QID = QID_AC_BE;

	pAd->ate.Addr1[0] = 0x00;
	pAd->ate.Addr1[1] = 0x11;
	pAd->ate.Addr1[2] = 0x22;
	pAd->ate.Addr1[3] = 0xAA;
	pAd->ate.Addr1[4] = 0xBB;
	pAd->ate.Addr1[5] = 0xCC;

	NdisMoveMemory(pAd->ate.Addr2, pAd->ate.Addr1, ETH_LENGTH_OF_ADDRESS);
	NdisMoveMemory(pAd->ate.Addr3, pAd->ate.Addr1, ETH_LENGTH_OF_ADDRESS);

	pAd->ate.bRxFER = 0;
	pAd->ate.bQAEnabled = FALSE;
	pAd->ate.bQATxStart = FALSE;
	pAd->ate.bQARxStart = FALSE;
	pAd->ate.bAutoTxAlc = FALSE;
#ifdef RTMP_MAC_PCI 
#ifndef RTMP_RBUS_SUPPORT
	pAd->ate.bFWLoading = FALSE;
#endif /* RTMP_RBUS_SUPPORT */
#endif /* RTMP_MAC_PCI */


#ifdef RALINK_QA
	pAd->ate.TxStatus = 0;
	RtmpOsTaskPidInit(&pAd->ate.AtePid);
/*	pAd->ate.AtePid = THREAD_PID_INIT_VALUE;*/
#endif /* RALINK_QA */
#endif /* RALINK_ATE */


	pAd->CommonCfg.bWiFiTest = FALSE;
#ifdef RTMP_MAC_PCI
    pAd->bPCIclkOff = FALSE;
#endif /* RTMP_MAC_PCI */

#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.EntryLifeCheck = MAC_ENTRY_LIFE_CHECK_CNT;

#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
	RTMP_SET_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif /* PCIE_PS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */


#if defined(AP_SCAN_SUPPORT) || defined(CONFIG_STA_SUPPORT)
	for (i = 0; i < MAX_LEN_OF_BSS_TABLE; i++) 
	{
		PBSS_ENTRY	pBssEntry = &pAd->ScanTab.BssEntry[i];
		
		if (pAd->ProbeRespIE[i].pIe)
			pBssEntry->pVarIeFromProbRsp = pAd->ProbeRespIE[i].pIe;
		else
			pBssEntry->pVarIeFromProbRsp = NULL;
	}
#endif /* defined(AP_SCAN_SUPPORT) || defined(CONFIG_STA_SUPPORT) */


#ifdef WSC_INCLUDED
	NdisZeroMemory(&pAd->CommonCfg.WscStaPbcProbeInfo, sizeof(WSC_STA_PBC_PROBE_INFO));
	pAd->CommonCfg.WscPBCOverlap = FALSE;
#endif /* WSC_INCLUDED */


#ifdef P2P_SUPPORT
	P2pCfgInit(pAd);
#endif /* P2P_SUPPORT */

#ifdef RT3352
	if ((pAd->CommonCfg.CID & 0x0000000f) < 0x00000006)
		pAd->BBPResetCtl.AsicCheckEn = 1;
#endif /* RT3352 */

	DBGPRINT(RT_DEBUG_TRACE, ("<-- UserCfgInit\n"));
}

/* IRQL = PASSIVE_LEVEL*/
UCHAR BtoH(STRING ch)
{
	if (ch >= '0' && ch <= '9') return (ch - '0');        /* Handle numerals*/
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  /* Handle capitol hex digits*/
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  /* Handle small hex digits*/
	return(255);
}


/*  FUNCTION: AtoH(char *, UCHAR *, int)*/

/*  PURPOSE:  Converts ascii string to network order hex*/

/*  PARAMETERS:*/
/*    src    - pointer to input ascii string*/
/*    dest   - pointer to output hex*/
/*    destlen - size of dest*/

/*  COMMENTS:*/

/*    2 ascii bytes make a hex byte so must put 1st ascii byte of pair*/
/*    into upper nibble and 2nd ascii byte of pair into lower nibble.*/

/* IRQL = PASSIVE_LEVEL*/

void AtoH(PSTRING src, PUCHAR dest, int destlen)
{
	PSTRING srcptr;
	PUCHAR destTemp;

	srcptr = src;	
	destTemp = (PUCHAR) dest; 

	while(destlen--)
	{
		*destTemp = BtoH(*srcptr++) << 4;    /* Put 1st ascii byte in upper nibble.*/
		*destTemp += BtoH(*srcptr++);      /* Add 2nd ascii byte to above.*/
		destTemp++;
	}
}


/*+++Mark by shiang, not use now, need to remove after confirm*/
/*---Mark by shiang, not use now, need to remove after confirm*/


/*
========================================================================
Routine Description:
	Add a timer to the timer list.

Arguments:
	pAd				- WLAN control block pointer
	pRsc			- the OS resource

Return Value:
	None

Note:
========================================================================
*/
VOID	RTMP_TimerListAdd(
	IN	PRTMP_ADAPTER			pAd,
	IN	VOID					*pRsc)
{
	LIST_HEADER *pRscList = &pAd->RscTimerCreateList;
	LIST_RESOURCE_OBJ_ENTRY *pObj;


	/* try to find old entry */
	pObj = (LIST_RESOURCE_OBJ_ENTRY *)(pRscList->pHead);
	while(1)
	{
		if (pObj == NULL)
			break;
		if ((ULONG)(pObj->pRscObj) == (ULONG)pRsc)
			return; /* exists */
		pObj = pObj->pNext;
	}

	/* allocate a timer record entry */
	os_alloc_mem(NULL, (UCHAR **)&(pObj), sizeof(LIST_RESOURCE_OBJ_ENTRY));
	if (pObj == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: alloc timer obj fail!\n", __FUNCTION__));
		return;
	}
	else
	{
		pObj->pRscObj = pRsc;
		insertTailList(pRscList, (LIST_ENTRY *)pObj);
		DBGPRINT(RT_DEBUG_TRACE, ("%s: add timer obj %lx!\n", __FUNCTION__, (ULONG)pRsc));
	}
}


/*
========================================================================
Routine Description:
	Cancel all timers in the timer list.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID	RTMP_TimerListRelease(
	IN	PRTMP_ADAPTER			pAd)
{
	LIST_HEADER *pRscList = &pAd->RscTimerCreateList;
	LIST_RESOURCE_OBJ_ENTRY *pObj, *pObjOld;
	BOOLEAN Cancel;


	/* try to find old entry */
	pObj = (LIST_RESOURCE_OBJ_ENTRY *)(pRscList->pHead);
	while(1)
	{
		if (pObj == NULL)
			break;
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Cancel timer obj %lx!\n", __FUNCTION__, (ULONG)(pObj->pRscObj)));
		RTMPReleaseTimer(pObj->pRscObj, &Cancel);
		pObjOld = pObj;
		pObj = pObj->pNext;
		os_free_mem(NULL, pObjOld);
	}

	/* reset TimerList */
	initList(&pAd->RscTimerCreateList);
}


/*
	========================================================================
	
	Routine Description:
		Init timer objects

	Arguments:
		pAd			Pointer to our adapter
		pTimer				Timer structure
		pTimerFunc			Function to execute when timer expired
		Repeat				Ture for period timer

	Return Value:
		None

	Note:
		
	========================================================================
*/
VOID	RTMPInitTimer(
	IN	PRTMP_ADAPTER			pAd,
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	PVOID					pTimerFunc,
	IN	PVOID					pData,
	IN	BOOLEAN					Repeat)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	RTMP_TimerListAdd(pAd, pTimer);

	/* Set Valid to TRUE for later used.*/
	/* It will crash if we cancel a timer or set a timer */
	/* that we haven't initialize before.*/
	/* */
	pTimer->Valid      = TRUE;
	
	pTimer->PeriodicType = Repeat;
	pTimer->State      = FALSE;
	pTimer->cookie = (ULONG) pData;
	pTimer->pAd = pAd;

	RTMP_OS_Init_Timer(pAd, &pTimer->TimerObj,	pTimerFunc, (PVOID) pTimer, &pAd->RscTimerMemList);	
	DBGPRINT(RT_DEBUG_TRACE,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================
	
	Routine Description:
		Init timer objects

	Arguments:
		pTimer				Timer structure
		Value				Timer value in milliseconds

	Return Value:
		None

	Note:
		To use this routine, must call RTMPInitTimer before.
		
	========================================================================
*/
VOID	RTMPSetTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		RTMP_ADAPTER *pAd;
		
		pAd = (RTMP_ADAPTER *)pTimer->pAd;
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		{
			DBGPRINT_ERR(("RTMPSetTimer failed, Halt in Progress!\n"));
			RTMP_SEM_UNLOCK(&TimerSemLock);
			return;
		}
		
		pTimer->TimerValue = Value;
		pTimer->State      = FALSE;
		if (pTimer->PeriodicType == TRUE)
		{
			pTimer->Repeat = TRUE;
			RTMP_SetPeriodicTimer(&pTimer->TimerObj, Value);
		}
		else
		{
			pTimer->Repeat = FALSE;
			RTMP_OS_Add_Timer(&pTimer->TimerObj, Value);
		}

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT_ERR(("RTMPSetTimer failed, Timer hasn't been initialize!\n"));
	}
	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================
	
	Routine Description:
		Init timer objects

	Arguments:
		pTimer				Timer structure
		Value				Timer value in milliseconds

	Return Value:
		None

	Note:
		To use this routine, must call RTMPInitTimer before.
		
	========================================================================
*/
VOID	RTMPModTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value)
{
	BOOLEAN	Cancel;


	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		pTimer->TimerValue = Value;
		pTimer->State      = FALSE;
		if (pTimer->PeriodicType == TRUE)
		{
			RTMP_SEM_UNLOCK(&TimerSemLock);
			RTMPCancelTimer(pTimer, &Cancel);
			RTMPSetTimer(pTimer, Value);
		}
		else
		{
			RTMP_OS_Mod_Timer(&pTimer->TimerObj, Value);
			RTMP_SEM_UNLOCK(&TimerSemLock);
		}
		DBGPRINT(RT_DEBUG_TRACE,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT_ERR(("RTMPModTimer failed, Timer hasn't been initialize!\n"));
		RTMP_SEM_UNLOCK(&TimerSemLock);
	}
}


/*
	========================================================================
	
	Routine Description:
		Cancel timer objects

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		1.) To use this routine, must call RTMPInitTimer before.
		2.) Reset NIC to initial state AS IS system boot up time.
		
	========================================================================
*/
VOID	RTMPCancelTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	OUT	BOOLEAN					*pCancelled)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		if (pTimer->State == FALSE)
			pTimer->Repeat = FALSE;
		
		RTMP_OS_Del_Timer(&pTimer->TimerObj, pCancelled);
		
		if (*pCancelled == TRUE)
			pTimer->State = TRUE;

#ifdef RTMP_TIMER_TASK_SUPPORT
		/* We need to go-through the TimerQ to findout this timer handler and remove it if */
		/*		it's still waiting for execution.*/
		RtmpTimerQRemove(pTimer->pAd, pTimer);
#endif /* RTMP_TIMER_TASK_SUPPORT */

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT(RT_DEBUG_INFO,("RTMPCancelTimer failed, Timer hasn't been initialize!\n"));
	}

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


VOID	RTMPReleaseTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	OUT	BOOLEAN					*pCancelled)
{
	RTMP_SEM_LOCK(&TimerSemLock);

	if (pTimer->Valid)
	{
		if (pTimer->State == FALSE)
			pTimer->Repeat = FALSE;
		
		RTMP_OS_Del_Timer(&pTimer->TimerObj, pCancelled);
		
		if (*pCancelled == TRUE)
			pTimer->State = TRUE;

#ifdef RTMP_TIMER_TASK_SUPPORT
		/* We need to go-through the TimerQ to findout this timer handler and remove it if */
		/*		it's still waiting for execution.*/
		RtmpTimerQRemove(pTimer->pAd, pTimer);
#endif /* RTMP_TIMER_TASK_SUPPORT */

		/* release timer */
		RTMP_OS_Release_Timer(&pTimer->TimerObj);

		pTimer->Valid = FALSE;

		DBGPRINT(RT_DEBUG_INFO,("%s: %lx\n",__FUNCTION__, (ULONG)pTimer));
	}
	else
	{
		DBGPRINT(RT_DEBUG_INFO,("RTMPReleasefailed, Timer hasn't been initialize!\n"));
	}

	RTMP_SEM_UNLOCK(&TimerSemLock);
}


/*
	========================================================================
	
	Routine Description:
		Enable RX 

	Arguments:
		pAd						Pointer to our adapter

	Return Value:
		None

	IRQL <= DISPATCH_LEVEL
	
	Note:
		Before Enable RX, make sure you have enabled Interrupt.
	========================================================================
*/
VOID RTMPEnableRxTx(
	IN PRTMP_ADAPTER	pAd)
{
/*	WPDMA_GLO_CFG_STRUC	GloCfg;*/
/*	ULONG	i = 0;*/
	UINT32 rx_filter_flag;

	DBGPRINT(RT_DEBUG_TRACE, ("==> RTMPEnableRxTx\n"));

	/* Enable Rx DMA.*/
	RT28XXDMAEnable(pAd);

	/* enable RX of MAC block*/
	if (pAd->OpMode == OPMODE_AP)
	{
		rx_filter_flag = APNORMAL;

#ifdef CONFIG_AP_SUPPORT
#ifdef IDS_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			if (pAd->ApCfg.IdsEnable)
				rx_filter_flag &= (~0x4);	/* Don't drop those not-U2M frames*/
		}
#endif /* IDS_SUPPORT */			
#endif /* CONFIG_AP_SUPPORT */

		RTMP_IO_WRITE32(pAd, RX_FILTR_CFG, rx_filter_flag);     /* enable RX of DMA block*/
	}
#ifdef CONFIG_STA_SUPPORT
	else
	{
#ifdef XLINK_SUPPORT
		if (pAd->StaCfg.PSPXlink)
			rx_filter_flag = PSPXLINK;
		else
#endif /* XLINK_SUPPORT */	
			rx_filter_flag = STANORMAL;     /* Staion not drop control frame will fail WiFi Certification.*/
		RTMP_IO_WRITE32(pAd, RX_FILTR_CFG, rx_filter_flag);
	}
#endif /* CONFIG_STA_SUPPORT */
	
	{
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0xc);
	}
	DBGPRINT(RT_DEBUG_TRACE, ("<== RTMPEnableRxTx\n"));	
}


/*+++Add by shiang, move from os/linux/rt_main_dev.c*/
void CfgInitHook(PRTMP_ADAPTER pAd)
{
	/*pAd->bBroadComHT = TRUE;*/
}

/*---Add by shiang, move from os/linux/rt_main_dev.c*/


static INT RtmpChipOpsRegister(
	IN RTMP_ADAPTER *pAd,
	IN INT			infType)
{
	RTMP_CHIP_OP	*pChipOps = &pAd->chipOps;
	int status;
	
	memset(pChipOps, 0, sizeof(RTMP_CHIP_OP));

	/* set eeprom related hook functions */
	status = RtmpChipOpsEepromHook(pAd, infType);

	/* set mcu related hook functions */
	switch(infType)
	{
#ifdef RTMP_PCI_SUPPORT
		case RTMP_DEV_INF_PCI:
		case RTMP_DEV_INF_PCIE:
			pChipOps->loadFirmware = RtmpAsicLoadFirmware;
			pChipOps->eraseFirmware = RtmpAsicEraseFirmware;
			pChipOps->sendCommandToMcu = RtmpAsicSendCommandToMcu;
			break;
#endif /* RTMP_PCI_SUPPORT */


#ifdef RTMP_RBUS_SUPPORT
		case RTMP_DEV_INF_RBUS:
			pChipOps->sendCommandToMcu = RtmpAsicSendCommandToSwMcu;
			break;
#endif /*RTMP_RBUS_SUPPORT */
		default:
			break;
	}

	return status;
}


INT RtmpRaDevCtrlInit(
	IN	VOID			*pAdSrc,
	IN	RTMP_INF_TYPE	infType)
{
	PRTMP_ADAPTER	pAd = (PRTMP_ADAPTER)pAdSrc;
	/*VOID	*handle;*/

	/* Assign the interface type. We need use it when do register/EEPROM access.*/
	pAd->infType = infType;

#ifdef CONFIG_STA_SUPPORT
	pAd->OpMode = OPMODE_STA;
	DBGPRINT(RT_DEBUG_TRACE, ("STA Driver version-%s\n", STA_DRIVER_VERSION));
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
	pAd->OpMode = OPMODE_AP;
	DBGPRINT(RT_DEBUG_TRACE, ("AP Driver version-%s\n", AP_DRIVER_VERSION));
#endif /* CONFIG_AP_SUPPORT */

#ifdef P2P_SUPPORT
	pAd->OpMode = OPMODE_STA;
#endif /* P2P_SUPPORT */


	RtmpChipOpsRegister(pAd, infType);

#ifdef MULTIPLE_CARD_SUPPORT
{
	extern BOOLEAN RTMP_CardInfoRead(PRTMP_ADAPTER pAd);

	/* find its profile path*/
	pAd->MC_RowID = -1; /* use default profile path*/
	RTMP_CardInfoRead(pAd);

	if (pAd->MC_RowID == -1)
#ifdef CONFIG_AP_SUPPORT
		strcpy(pAd->MC_FileName, AP_PROFILE_PATH);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		strcpy(pAd->MC_FileName, STA_PROFILE_PATH);
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("MC> ROW = %d, PATH = %s\n", pAd->MC_RowID, pAd->MC_FileName));
}
#endif /* MULTIPLE_CARD_SUPPORT */

	return 0;
}


BOOLEAN RtmpRaDevCtrlExit(IN VOID *pAdSrc)
{
	PRTMP_ADAPTER	pAd = (PRTMP_ADAPTER)pAdSrc;
	INT index;
	
#ifdef MULTIPLE_CARD_SUPPORT
extern UINT8  MC_CardUsed[MAX_NUM_OF_MULTIPLE_CARD];

	if ((pAd->MC_RowID >= 0) && (pAd->MC_RowID <= MAX_NUM_OF_MULTIPLE_CARD))
		MC_CardUsed[pAd->MC_RowID] = 0; /* not clear MAC address*/
#endif /* MULTIPLE_CARD_SUPPORT */


	/*
		Free ProbeRespIE Table
	*/
	for (index = 0; index < MAX_LEN_OF_BSS_TABLE; index++) 
	{
		if (pAd->ProbeRespIE[index].pIe)
			os_free_mem(pAd, pAd->ProbeRespIE[index].pIe);
	}

#ifdef RESOURCE_PRE_ALLOC
	RTMPFreeTxRxRingMemory(pAd);
#endif /* RESOURCE_PRE_ALLOC */


	RTMPFreeAdapter(pAd);

	return TRUE;
}


#ifdef CONFIG_AP_SUPPORT
#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
VOID RTMP_11N_D3_TimerInit(
	IN PRTMP_ADAPTER pAd)
{
	RTMPInitTimer(pAd, &pAd->CommonCfg.Bss2040CoexistTimer, GET_TIMER_FUNCTION(Bss2040CoexistTimeOut), pAd, FALSE);
}
#endif /* DOT11N_DRAFT3 */
#endif /* DOT11_N_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


#ifdef VENDOR_FEATURE3_SUPPORT
VOID RTMP_IO_WRITE32(
	PRTMP_ADAPTER pAd,
	UINT32 Offset,
	UINT32 Value)
{
	_RTMP_IO_WRITE32(pAd, Offset, Value);
}

VOID RTMP_BBP_IO_READ8_BY_REG_ID(
	PRTMP_ADAPTER pAd,
	UINT32 Offset,
	UINT8 *pValue)
{
	_RTMP_BBP_IO_READ8_BY_REG_ID(pAd, Offset, pValue);
}

VOID RTMP_BBP_IO_READ8(
	PRTMP_ADAPTER pAd,
	UCHAR Offset,
	UINT8 *pValue,
	BOOLEAN FlgValidMCR)
{
	_RTMP_BBP_IO_READ8(pAd, Offset, pValue, FlgValidMCR);
}

VOID RTMP_BBP_IO_WRITE8_BY_REG_ID(
	PRTMP_ADAPTER pAd,
	UINT32 Offset,
	UINT8 Value)
{
	_RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, Offset, Value);
}

VOID RTMP_BBP_IO_WRITE8(
	PRTMP_ADAPTER pAd,
	UCHAR Offset,
	UINT8 Value,
	BOOLEAN FlgValidMCR)
{
	_RTMP_BBP_IO_WRITE8(pAd, Offset, Value, FlgValidMCR);
}
#endif /* VENDOR_FEATURE3_SUPPORT */


#ifdef RTMP_MAC_PCI
VOID CMDHandler(                                                                                                                                                
    IN PRTMP_ADAPTER pAd)                                                                                                                                       
{                                                                                                                                                               
	PCmdQElmt		cmdqelmt;                                                                                                                                       
	PUCHAR			pData;                                                                                                                                          
	NDIS_STATUS		NdisStatus = NDIS_STATUS_SUCCESS;                                                                                                               
/*	ULONG			Now = 0;*/
/*	NTSTATUS		ntStatus;*/
/*	unsigned long	IrqFlags;*/
	
	while (pAd && pAd->CmdQ.size > 0)	
	{                                                                                                                                                           
		NdisStatus = NDIS_STATUS_SUCCESS;
		                                                                                                                      
		NdisAcquireSpinLock(&pAd->CmdQLock);
		RTThreadDequeueCmd(&pAd->CmdQ, &cmdqelmt);
		NdisReleaseSpinLock(&pAd->CmdQLock);
		                                                                                                        
		if (cmdqelmt == NULL)                                                                                                                                   
			break; 
			                                                                                                                                             
		pData = cmdqelmt->buffer;                                      
		                                                                                         
		if(!(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) || RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)))
		{
			switch (cmdqelmt->command)
			{
#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_PCI_SUPPORT
				case CMDTHREAD_CHAN_RESCAN:
					DBGPRINT(RT_DEBUG_TRACE, ("cmd> Re-scan channel! \n"));

					pAd->CommonCfg.Channel = AP_AUTO_CH_SEL(pAd, TRUE);

#ifdef DOT11_N_SUPPORT
					/* If phymode > PHY_11ABGN_MIXED and BW=40 check extension channel, after select channel  */
					N_ChannelCheck(pAd);
#endif /* DOT11_N_SUPPORT */

					DBGPRINT(RT_DEBUG_TRACE, ("cmd> Switch to %d! \n", pAd->CommonCfg.Channel));
					APStop(pAd);
					APStartUp(pAd);

#ifdef AP_QLOAD_SUPPORT
					QBSS_LoadAlarmResume(pAd);
#endif /* AP_QLOAD_SUPPORT */
					break;
#endif /* RTMP_PCI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

				case CMDTHREAD_REG_HINT:
#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
					RT_CFG80211_CRDA_REG_HINT(pAd, pData, cmdqelmt->bufferlength);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */
					break;

				case CMDTHREAD_REG_HINT_11D:
#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
					RT_CFG80211_CRDA_REG_HINT11D(pAd, pData, cmdqelmt->bufferlength);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */
					break;

				case CMDTHREAD_SCAN_END:
#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
					RT_CFG80211_SCAN_END(pAd, FALSE);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */
					break;

				case CMDTHREAD_CONNECT_RESULT_INFORM:
#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
					RT_CFG80211_CONN_RESULT_INFORM(pAd,
												pAd->MlmeAux.Bssid,
												pData, cmdqelmt->bufferlength,
												pData, cmdqelmt->bufferlength,
												1);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */
					break;

				default:
					DBGPRINT(RT_DEBUG_ERROR, ("--> Control Thread !! ERROR !! Unknown(cmdqelmt->command=0x%x) !! \n", cmdqelmt->command));
					break;
			}
		}

		if (cmdqelmt->CmdFromNdis == TRUE)
		{
			if (cmdqelmt->buffer != NULL)
				os_free_mem(pAd, cmdqelmt->buffer);
			os_free_mem(pAd, cmdqelmt);
		}
		else
		{
			if ((cmdqelmt->buffer != NULL) && (cmdqelmt->bufferlength != 0))
				os_free_mem(pAd, cmdqelmt->buffer);
			os_free_mem(pAd, cmdqelmt);
		}
	}	/* end of while */
}
#endif /* RTMP_MAC_PCI */

