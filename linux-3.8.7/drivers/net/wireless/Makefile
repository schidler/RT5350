#
# Makefile for the Linux Wireless network device drivers.
#

obj-$(CONFIG_IPW2100) += ipw2x00/
obj-$(CONFIG_IPW2200) += ipw2x00/

obj-$(CONFIG_HERMES)		+= orinoco/

obj-$(CONFIG_AIRO)		+= airo.o
obj-$(CONFIG_AIRO_CS)		+= airo_cs.o airo.o

obj-$(CONFIG_ATMEL)             += atmel.o
obj-$(CONFIG_PCI_ATMEL)         += atmel_pci.o 
obj-$(CONFIG_PCMCIA_ATMEL)      += atmel_cs.o

obj-$(CONFIG_AT76C50X_USB)      += at76c50x-usb.o

obj-$(CONFIG_PRISM54)		+= prism54/

obj-$(CONFIG_HOSTAP)		+= hostap/
obj-$(CONFIG_B43)		+= b43/
obj-$(CONFIG_B43LEGACY)		+= b43legacy/
obj-$(CONFIG_ZD1211RW)		+= zd1211rw/
obj-$(CONFIG_RTL8180)		+= rtl818x/
obj-$(CONFIG_RTL8187)		+= rtl818x/
obj-$(CONFIG_RTLWIFI)		+= rtlwifi/

# 16-bit wireless PCMCIA client drivers
obj-$(CONFIG_PCMCIA_RAYCS)	+= ray_cs.o
obj-$(CONFIG_PCMCIA_WL3501)	+= wl3501_cs.o

obj-$(CONFIG_USB_NET_RNDIS_WLAN)	+= rndis_wlan.o

obj-$(CONFIG_USB_ZD1201)	+= zd1201.o
obj-$(CONFIG_LIBERTAS)		+= libertas/

obj-$(CONFIG_LIBERTAS_THINFIRM)	+= libertas_tf/

obj-$(CONFIG_ADM8211)	+= adm8211.o

obj-$(CONFIG_MWL8K)	+= mwl8k.o

obj-$(CONFIG_IWLWIFI)	+= iwlwifi/
obj-$(CONFIG_IWLEGACY)	+= iwlegacy/
obj-$(CONFIG_RT2X00)	+= rt2x00/

obj-$(CONFIG_P54_COMMON)	+= p54/

obj-$(CONFIG_ATH_CARDS)		+= ath/

obj-$(CONFIG_MAC80211_HWSIM)	+= mac80211_hwsim.o

obj-$(CONFIG_WL_TI)	+= ti/

obj-$(CONFIG_MWIFIEX)	+= mwifiex/

obj-$(CONFIG_BRCMFMAC)	+= brcm80211/
obj-$(CONFIG_BRCMSMAC)	+= brcm80211/

ifdef CONFIG_RALINK_RT3052
obj-$(CONFIG_RT2860V2_AP)       += ralink/rt305x/rt2860v2_ap/
obj-$(CONFIG_RT2860V2_STA)      += ralink/rt305x/rt2860v2_sta/
endif
ifdef CONFIG_RALINK_RT3352
obj-$(CONFIG_RT2860V2_AP)       += ralink/rt305x/rt2860v2_ap/
obj-$(CONFIG_RT2860V2_STA)      += ralink/rt305x/rt2860v2_sta/
endif
ifdef CONFIG_RALINK_RT3883
obj-$(CONFIG_RT2860V2_AP)       += ralink/rt3883/rt2860v2_ap/
obj-$(CONFIG_RT2860V2_STA)      += ralink/rt3883/rt2860v2_sta/
endif
ifdef CONFIG_RALINK_RT5350
obj-$(CONFIG_RT2860V2_AP)       += ralink/rt5350/rt2860v2_ap/
obj-$(CONFIG_RT2860V2_STA)      += ralink/rt5350/rt2860v2_sta/
endif
obj-$(CONFIG_RT3090_AP)         += ralink/rt3090/rt3090_ap/
obj-$(CONFIG_RTPCI_AP)			+= ralink/rt5392/rt2860v2_ap/
