#!/bin/sh


[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

source /sbin/conf

RESOLV_CONF=$RESOLV
[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
	deconfig)
		/sbin/ifconfig $interface 0.0.0.0
		;;

	renew|bound)
		__log "Settings ip $ip on $interface with $NETMASK ..."
		/sbin/ifconfig $interface $ip $BROADCAST $NETMASK
		
		if [ -n "$router" ] ; then
			if [ "$interface" == "$CLI_IFACE" ]; then
				for i in $router ; do
					__log "Adding route $i via $CLI_IFACE to cache ..."
#					echo "$i" >> "/tmp/$CLI_IFACE.gw"
					route delete default 
					route add default gw "$i"
				done
				exit 0; # Exit here
			else
				__log "deleting routers ..."
				ip r del default via $(____get_gateway_addr) dev $(____get_gateway_dev)
				for i in $router ; do
					__log "Adding route $i ..."
					ip r add default via $i dev $interface
				done
			fi
		fi
		
		echo -n > $RESOLV_CONF
		[ -n "$domain" ] && echo search $domain >> $RESOLV_CONF
		for i in $dns ; do
			__log "Adding DNS $i to $RESOLV_CONF ..."
			echo nameserver $i >> $RESOLV_CONF
		done
		;;
esac																																																																												
exit 0

