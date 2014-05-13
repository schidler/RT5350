#!/bin/sh

__nvram_write(){
echo "nvram_set $1 $2"
nvram_set $1 "$2"
}

#getting parameter
PARAM=`echo $1 | awk -F "?" '{ print($1) }' | cut -d'/' -f2`
PARAM=`echo $PARAM | awk -F " " '{ printf($1) }'` 		#protetion of some browsers stupidity
echo "-----> PARAM1: $PARAM"

    if [ "$PARAM" = "" ]; then
	PARAM=`echo $1 | awk -F " " '{ print($1) }' | cut -d'/' -f2`
	echo "---------------#######_-----------> PARAM2: $PARAM"
    fi

########################### SETTING BRIDGE CONFIGURATION ##############################
if [ "set_bridge.html" = "$PARAM" ]; then

IP=`echo $1 | awk -F "&" '{ print($1) }' | cut -d'=' -f2`
echo "IP address:     $IP"
MASK=`echo $1 | awk -F "&" '{ print($2) }' | cut -d'=' -f2`
echo "Netmask:        $MASK"
GATEWAY=`echo $1 | awk -F "&" '{ print($3) }' | cut -d'=' -f2`
echo "Gateway:        $GATEWAY"
DNS1=`echo $1 | awk -F "&" '{ print($4) }' | cut -d'=' -f2`
echo "DNS1:           $DNS1"
DNS2=`echo $1 | awk -F "&" '{ print($5) }' | cut -d'=' -f2`
echo "DNS2:           $DNS2"

if [ "$IP" != "" ]; then
    if [ "$MASK" != "" ]; then
	if [ "$GATEWAY" != "" ]; then
	    if [ "$DNS1" != "" ]; then
		if [ "$DNS2" != "" ]; then
		    __nvram_write wan_ipaddr $IP
		    __nvram_write wan_netmask $MASK
		    __nvram_write wan_gateway $GATEWAY
		    __nvram_write wan_primary_dns $DNS1
		    __nvram_write wan_secondary_dns $DNS2
		fi
	    fi
	fi
    fi
fi

fi

########################### SETTING WLAN CONFIGURATION ##############################
if [ "set_wifi.html" = "$PARAM" ]; then
    SSID=`echo $1 | awk -F "&" '{ print($1) }' | cut -d'=' -f2`
    echo "SSID:     $SSID"
    CHANNEL=`echo $1 | awk -F "&" '{ print($2) }' | cut -d'=' -f2`
    echo "Channel:  $CHANNEL"
    if [ "$SSID" != "" ]; then
	if [ "$CHANNEL" != "" ]; then
	    __nvram_write SSID1 $SSID
	    __nvram_write Channel $CHANNEL

	    iwpriv ra0 set SSID=$SSID
	    iwpriv ra0 set Channel=$CHANNEL
	fi
    fi
fi

########################### SETTING DHCP CONFIGURATION ##############################
if [ "set_dhcp.html" = "$PARAM" ]; then

	DHCP_START=`echo $1 | awk -F "&" '{ print($1) }' | cut -d'=' -f2`
	DHCP_END=`echo $1 | awk -F "&" '{ print($2) }' | cut -d'=' -f2`
	DHCP_DOMAIN=`echo $1 | awk -F "&" '{ print($3) }' | cut -d'=' -f2`
	DHCP_GATEWAY=`echo $1 | awk -F "&" '{ print($4) }' | cut -d'=' -f2`

echo $DHCP_START
echo $DHCP_END
echo $DHCP_DOMAIN
echo $DHCP_GATEWAY

if [ "$DHCP_START" != "" ]; then
    if [ "$DHCP_END" != "" ]; then
	if [ "$DHCP_DOMAIN" != "" ]; then
            if [ "$DHCP_END" != "" ]; then
		if [ "$DHCP_GATEWAY" != "" ]; then

			__nvram_write dhcpStart $DHCP_START
			__nvram_write dhcpEnd $DHCP_END
			__nvram_write dhcpDomain $DHCP_DOMAIN
			__nvram_write dhcpGateway $DHCP_GATEWAY

		fi
	    fi
	fi
    fi
fi

fi

########################### SETTING MJPG CONFIGURATION ##############################
if [ "set_streamer.html" = "$PARAM" ]; then
    IS_MJPG_ON=$(echo $1 | grep 'mjpg_enabled')

    if [ "x" != "x$IS_MJPG_ON" ]; then
	MJPG_ON=1
	MJPG_ADDR=`echo $1 | awk -F "&" '{ print($2) }' | cut -d'=' -f2`
	MJPG_PORT=`echo $1 | awk -F "&" '{ print($3) }' | cut -d'=' -f2`
	MJPG_RSN=`echo $1 | awk -F "&" '{ print($4) }' | cut -d'=' -f2`
    else
	MJPG_ADDR=`echo $1 | awk -F "&" '{ print($1) }' | cut -d'=' -f2`
	MJPG_PORT=`echo $1 | awk -F "&" '{ print($2) }' | cut -d'=' -f2`
	MJPG_RSN=`echo $1 | awk -F "&" '{ print($3) }' | cut -d'=' -f2`
    fi

    if [ "$MJPG_ADDR" != "" ]; then
	if [ "$MJPG_PORT" != "" ]; then

	    if [ "$MJPG_ON" != "" ]; then
		__nvram_write STREAM_ENABLED 1
	    else
		__nvram_write STREAM_ENABLED 0
	    fi

	__nvram_write STREAM_IP $MJPG_ADDR
	__nvram_write STREAM_PORT $MJPG_PORT
	__nvram_write STREAM_RSN $MJPG_RSN

	fi
    fi

fi

########################### UPDATING ROUTER CONFIGURATION ###########################
if [ "update_config.html" = "$PARAM" ]; then
#if [[ "$PARAM" =~ 'update_config.html' ]]; then

echo "Getting configs.."

IP=$(nvram_get wan_ipaddr)
NETMASK=$(nvram_get wan_netmask)
GATEWAY=$(nvram_get wan_gateway)
DNS1=$(nvram_get wan_primary_dns)
DNS2=$(nvram_get wan_secondary_dns)
SSID=$(nvram_get SSID1)
CHANNEL=$(nvram_get Channel)

STATE=`state`

TOTAL_MEM=`echo $STATE | awk -F " " '{ print($6) }'`
FREE_MEM=`echo $STATE | awk -F " " '{ print($9) }'`
CPU_LOAD=`echo $STATE | awk -F " " '{ print($12) }'`

WAN_MAC_ADDR=$(cat /sys/class/net/br0/address)
WLAN_MAC_ADDR=$(cat /sys/class/net/ra0/address)

UPTIME=`uptime | awk -F" " '{print($3" "$4)}' | sed 's/.$//'`

echo "Concatenating parameters.."

TEXT="<center><input type=\"button\" onClick=\"add_info_hide_show();\" value=\"Show/Hide\"><div id=\"add_info_div\" style=\"display:none\"><table name=\"deleteable_settings\" border=\"1\" bgcolor=\"D0D0D0\" ><th colspan=\"2\">Network configuration</th><tr><td>IP<td>$IP<tr><td>Netmask:<td>$NETMASK<tr><td>Gateway:<td>$GATEWAY<tr><td>Primary DNS:<td>$DNS1<tr><td>Secondary DNS:<td>$DNS2<tr><tr><tr><td>SSID:<td>$SSID<tr><td>Channel<td>$CHANNEL<tr><th colspan=\"2\">Hardware condition</th><tr><td>CPU load:<td>$CPU_LOAD<tr><td>Total memory:<td>$TOTAL_MEM<tr><td>Free memory:<td>$FREE_MEM<tr><td>WAN mac address:<td>$WAN_MAC_ADDR<tr><td>WLAN mac address:<td>$WLAN_MAC_ADDR<tr><td>Uptime:<td>$UPTIME<tr><\/table><\/div><\/center>"

echo "Configuring index.html.."

    sed -i /"deleteable_settings"/d /web/index.html
    sed -i "/CURRENTSETTINGS/a $TEXT" /web/index.html

echo "Done!"
fi

########################### UPDATING CONNECTION MODE CONFIGURATION ###########################
if [ "dhcp_switch.html" = "$PARAM" ]; then

    IS_DHCP_ON=$(echo $1 | grep 'dhcp_on')

    if [ "x" != "x$IS_DHCP_ON" ]; then
	__nvram_write wanConnectionMode DHCP
    else
	__nvram_write wanConnectionMode STATIC
    fi
fi

########################### SETTING THE WIRELESS ENCRYPT CONFIGURATION ###########################
if [ "set_encrypt.html" = "$PARAM" ]; then

    ENCRYPT_T=`echo $1 | awk -F "&" '{ print($1) }' | cut -d'=' -f2`
    PASSWORD=`echo $1 | awk -F "&" '{ print($2) }' | cut -d'=' -f2 | cut -d' ' -f1`

echo "PASSWORD: $PASSWORD"

    ENC_SSID=$(nvram_get SSID1)

    echo $ENCRYPT_T
    echo $PASSWORD


# Opened WiFi Network

	if [ "$ENCRYPT_T" = "OPEN" ]; then
	    echo "Setting 'OPEN' network!"
	    __nvram_write AuthMode OPEN
	    __nvram_write EncryptType NONE
	iwpriv ra0 set AuthMode=OPEN
	iwpriv ra0 set EncrypType=NONE
	iwpriv ra0 set IEEE8021X=0
	iwpriv ra0 set SSID=$ENC_SSID
	fi

#WPAPSK WiFi Network

	if [ "$ENCRYPT_T" = "WPAPSK" ]; then
	    echo "Setting 'WPAPSK' network!"
	    __nvram_write AuthMode WPAPSK
	    __nvram_write EncryptType TKIP

	iwpriv ra0 set AuthMode=WPAPSK
	iwpriv ra0 set EncrypType=TKIP
	iwpriv ra0 set IEEE8021X=0
	iwpriv ra0 set SSID=$ENC_SSID
	iwpriv ra0 set WPAPSK=$PASSWORD
	iwpriv ra0 set DefaultKeyID=2
	iwpriv ra0 set SSID=$ENC_SSID
	fi

__nvram_write Password $PASSWORD

fi