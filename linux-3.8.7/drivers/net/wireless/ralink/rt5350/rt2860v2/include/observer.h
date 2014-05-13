#ifndef OBS_HEADER
#define OBS_HEADER

#include "rt_config.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#ifdef OBS_USE_NETLINK

/* Multicast Communication between Kernel and Applications */
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

static struct sock *nl_sk   = NULL;
static struct sk_buff *skb  = NULL;
static struct nlmsghdr *nlh = NULL;
#define NETLINK_OBSERVER    16
#define OBS_NETLINK_DEL_MODE 1
#define OBS_NETLINK_ADD_MODE 2
#define OBS_MAX_PAYLOAD		1024  /* maximum payload size*/
#endif

/* Buff for mac addr */
#define OBS_MAC_ADDR_BUFF	0xF

/* Debuging */
#define OBS_DBG				1

#define OBS_GIVE_ME_MEM(x) printk(KERN_INFO "Observer: malloc memory for struct ...\n"); x = kmalloc(OBS_MAC_ADDR_BUFF, GFP_ATOMIC)


/* Prototypes */
static void obs_init(void);
static void obs_procfs_init(void);
#ifdef OBS_USE_NETLINK
static void obs_netlink_init(void);
static void obs_nl_data_ready(struct sock *sk, int len);
static void obs_netlink_update(int mode, char *mac);
static void obs_netlink_broadcast(void);
#endif
static void obs_stop(void);
static int obs_read_func(char *page, char **start, off_t off, int count, int *eof, void *data);
static void obs_write_del_mac(char* mac);
static void obs_write_add_mac(char* mac);
static void obs_check_and_add_mac(char *mac);
static void obs_check_and_del_mac(char *mac);


static struct proc_dir_entry *obs_dir		= NULL; /* /proc/observer */
static struct proc_dir_entry *obs_add_mac	= NULL; /* /proc/observer/add_mac */
static struct proc_dir_entry *obs_del_mac	= NULL; /* /proc/observer/del_mac */


static int obs_read_func(char *page, char **start,
                            off_t off, int count, 
                            int *eof, void *data)
{
	
	int offset = 0;
	char * message = "";
	strcpy(page + offset, message);
	offset += strlen(message);
	memcpy(page + offset, data, strlen(data));
	offset += strlen(data);
	page[offset] = '\n';
	offset += 1;
	return offset;
}


#ifdef OBS_USE_NETLINK
static void obs_nl_data_ready (struct sock *sk, int len){
 // wake_up_interruptible(sk->sleep);
}
#endif
static void obs_procfs_init(void){
	
	printk(KERN_INFO "Observer procfs init.\n");
	
	obs_dir = proc_mkdir("observer", NULL);
	if(obs_dir == NULL){
		printk(KERN_ERR "Observer: error while creating control directory!\n");
		return;
	}
    
	obs_add_mac = create_proc_entry("add_mac", 0644, obs_dir);
    if(obs_add_mac == NULL) {
		printk(KERN_ERR "Observer: error while creating control file for add_mac!\n");
		remove_proc_entry("observer", obs_dir);
		return;
	}
	
	obs_del_mac = create_proc_entry("del_mac", 0644, obs_dir);
	if(obs_del_mac == NULL) {
		printk(KERN_ERR "Observer: error while creating control file for del_mac!\n");
		remove_proc_entry("add_mac", NULL);
		remove_proc_entry("observer", obs_dir);
		return;
	}
	
	/* Malloc mem */
	OBS_GIVE_ME_MEM(obs_add_mac->data);
	OBS_GIVE_ME_MEM(obs_del_mac->data);
	
	if(obs_del_mac->data == NULL){
		printk(KERN_ERR "Observer: need more memory for del_mac!\n");
		obs_stop();
		return;
	}
	
	if(obs_add_mac->data == NULL){
		printk(KERN_ERR "Observer: need more memory for add_mac!\n");
		obs_stop();
		return;
	}
	
	/* Init default value */
	strcpy(obs_add_mac->data, "null");
	strcpy(obs_del_mac->data, "null");
	
	/* Callback func */
	obs_add_mac->read_proc = obs_read_func;
	obs_del_mac->read_proc = obs_read_func;
	
	/* Owner */
	obs_add_mac->owner	= THIS_MODULE;
	obs_del_mac->owner	= THIS_MODULE;
	obs_dir->owner		= THIS_MODULE;
}
#ifdef OBS_USE_NETLINK
static void obs_netlink_init(void){
	
	printk(KERN_INFO "Observer netlink init.\n");
	nl_sk = netlink_kernel_create(NETLINK_OBSERVER, 0, obs_nl_data_ready, THIS_MODULE);
//	nl_sk = netlink_kernel_create(6, NETLINK_OBSERVER, 0, obs_nl_data_ready, NULL, THIS_MODULE);
	if(nl_sk == NULL){
		printk(KERN_ERR "%s: error while creating socket!\n", __FUNCTION__);
		return;
	}
	skb = alloc_skb(NLMSG_SPACE(OBS_MAX_PAYLOAD), GFP_KERNEL);
	if(skb == NULL){
		printk(KERN_ERR "%s: mem error!\n", __FUNCTION__);
		return;
	}
	nlh = (struct nlmsghdr *)skb->data;
	nlh->nlmsg_len = NLMSG_SPACE(OBS_MAX_PAYLOAD);
	nlh->nlmsg_pid = 0;  /* from kernel */
	nlh->nlmsg_flags = 0;
	strcpy(NLMSG_DATA(nlh), "NULL");
}

static void obs_netlink_update(int mode, char *mac){
	
	if(nlh == NULL){
		printk("%s: cannot update mac entry, netlink socket is down.\n", __FUNCTION__);
		return;
	}
	memset(NLMSG_DATA(nlh), 0, sizeof(NLMSG_DATA(nlh)));
	switch(mode){
		case OBS_NETLINK_DEL_MODE:
			strcpy(NLMSG_DATA(nlh), "DEL#");
			break;
		case OBS_NETLINK_ADD_MODE:
			strcpy(NLMSG_DATA(nlh), "ADD#");
			break;
	}
	strcpy(NLMSG_DATA(nlh), mac);
	obs_netlink_broadcast();
}

static void obs_netlink_broadcast(void){
	if(nl_sk == NULL || skb == NULL){
		printk(KERN_ERR "%s: netlink socket is down.\n", __FUNCTION__);
		return;
	}
	#ifdef OBS_DBG
	printk(KERN_INFO "%s: sending update...\n", __FUNCTION__);
	#endif
	netlink_broadcast(nl_sk, skb, 0, 1, GFP_KERNEL);
}
#endif

static void obs_init(void){
	
	obs_procfs_init();
	#ifdef OBS_USE_NETLINK
	obs_netlink_init();
	#endif
	
	/* Ready for fun! */
	printk(KERN_INFO "Observer: ready.\n");
}

static void obs_write_del_mac(char* mac){
	if(obs_del_mac == NULL){
		printk(KERN_ERR "Observer: error: file is null.\n");
		return;
	}
	if(obs_del_mac->data == NULL){
		printk(KERN_ERR "Observer: error: data is null.\n");
		return;		
	}
	if(mac == NULL){
		printk(KERN_ERR "Observer: error: mac is null.\n");
		return;		
	}
	if(strlen(mac) > OBS_MAC_ADDR_BUFF){
		printk(KERN_ERR "Observer: detected buffer overflow. MAC: %s. Length: %d\n",
			mac, strlen(mac));
		return;
	}
	
	#ifdef OBS_DELETE_DUB
	if (strncmp(obs_del_mac->data, mac, strlen(obs_del_mac->data)) == 0){
		#ifdef OBS_DELETE_DUB_DBG
		printk(KERN_INFO "Observer: found dub in del: %s\n", mac);
		#endif
		return;
	}
	#endif
	#ifdef OBS_DBG
	printk(KERN_INFO "Observer: given mac for del: %s\n", mac);
	#endif
	memset(obs_del_mac->data, 0, OBS_MAC_ADDR_BUFF);
	strcpy(obs_del_mac->data, mac);
	#ifdef OBS_USE_NETLINK
	obs_netlink_update(OBS_NETLINK_DEL_MODE, mac);
	#endif
}

static void obs_write_add_mac(char* mac){
	if(obs_add_mac == NULL){
		printk(KERN_ERR "Observer: error: file is null.\n");
		return;
	}
	if(obs_add_mac->data == NULL){
		printk(KERN_ERR "Observer: error: data is null.\n");
		return;		
	}
	if(mac == NULL){
		printk(KERN_ERR "Observer: error: mac is null.\n");
		return;		
	}
	if(strlen(mac) > OBS_MAC_ADDR_BUFF){
		printk(KERN_ERR "Observer: detected buffer overflow. MAC: %s. Length: %d\n",
			mac, strlen(mac));
		return;
	}
	
	#ifdef OBS_DELETE_DUB
	if (strncmp(obs_add_mac->data, mac, strlen(obs_add_mac->data)) == 0){
		#ifdef OBS_DELETE_DUB_DBG
		printk(KERN_INFO "Observer: found dub in add: %s\n", mac);
		#endif
		return;
	}
	#endif
	#ifdef OBS_DBG
	printk(KERN_INFO "Observer: given mac for add: %s\n", mac);
	#endif
	memset(obs_add_mac->data, 0, OBS_MAC_ADDR_BUFF);
	strcpy(obs_add_mac->data, mac);
	#ifdef OBS_USE_NETLINK
	obs_netlink_update(OBS_NETLINK_ADD_MODE, mac);
	#endif
}

static void obs_check_and_add_mac(char *mac){
	#ifdef OBS_USE_PROC
	if(obs_add_mac == NULL || obs_add_mac->data == NULL){
		obs_init();
		if(obs_add_mac == NULL || obs_add_mac->data == NULL){
			printk(KERN_ERR "Observer: error while init!\n");
			return;
		}
	}
	obs_write_add_mac(mac);
	#else
	return;
	#endif
}


static void obs_check_and_del_mac(char *mac){
	#ifdef OBS_USE_PROC
	if(obs_del_mac == NULL || obs_del_mac->data == NULL){
		obs_init();
		if(obs_del_mac == NULL || obs_del_mac->data == NULL){
			printk(KERN_ERR "Observer: error while init!\n");
			return;
		}
	}
	obs_write_del_mac(mac);
	#else
	return;
	#endif
}

static void obs_stop(void){
	printk(KERN_INFO "Observer: stoping ...\n");
	if(obs_add_mac->data != NULL && obs_del_mac->data != NULL){
		kfree(obs_add_mac->data);	
		kfree(obs_del_mac->data);
	}
	remove_proc_entry("observer", obs_dir);
	remove_proc_entry("add_mac", NULL);
	remove_proc_entry("del_mac", NULL);
	obs_dir = NULL;
	obs_add_mac = NULL;
	obs_del_mac = NULL;
}

#endif
