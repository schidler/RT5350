#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include <linux/wireless.h>

#if WIRELESS_EXT <= 11
#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE 0x8BE0
#endif
#define SIOCIWFIRSTPRIV SIOCDEVPRIVATE
#endif


#define RTPRIV_IOCTL_GSITESURVEY (SIOCIWFIRSTPRIV + 0x0D)

struct __ap_node {
	char ssid[32];
	char bssid[20];
	char security[10];
	char mode[10];
	char signal[4];
	char channel[3];
};

void tr_msg(char *msg, struct __ap_node *node){
  char buff[255];
  char *pch;
  int i = 0;
  memset(buff, 0, 255);
  strncpy(buff, msg, 255);
  pch = strtok (buff, "#");
  while (pch != NULL){
	switch(i++){
		case 0: { strncpy(node->ssid,     pch, 32);	break; }
		case 1: { strncpy(node->bssid,    pch, 32);	break; }
		case 2: { strncpy(node->channel,  pch,  4);	break; }
		case 3: { strncpy(node->signal,   pch,  3);	break; }
		case 4: { strncpy(node->mode,     pch, 10); break; }
		case 5: { strncpy(node->security, pch, 10);	break; }
	}
    pch = strtok (NULL, "#");
  }
}


void notify(struct __ap_node *node){
/*	pid_t pid;

		node->ssid,		node->bssid,
		node->channel,  	node->signal,
		node->mode,		node->security

	printf("%s\t%s\t%s\t%s\t%s\t%s\n",node->ssid,		node->bssid,
					node->channel,  	node->signal,
					node->mode,		node->security);
*/
return;
}


int main(){
	char data[4096], buff[256];
	int fd, ret, i;
	char *pch;
	struct iwreq wrq;
	struct __ap_node nodes[255];
	
	ret = system("iwpriv ra0 set SiteSurvey=1");
	if(ret != 0){
		perror("iwpriv");
		return 1;
	}
	//printf("Searching ...\n");
	sleep(5);
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0){
		perror("socket");
		return 1;
	}

	memset(nodes,	0, sizeof(nodes));
	memset(buff,	0, 255);
	memset(data,	0, 4096);
	
	strcpy(wrq.ifr_ifrn.ifrn_name, "ra0");
	wrq.u.data.length = 4096;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;
	ret = ioctl(fd, RTPRIV_IOCTL_GSITESURVEY, &wrq);
	if(ret != 0)
	{
		perror("ioctl");
		return 1;
	}
	if(wrq.u.data.length <= 0){
		return 1;
	}
	i = 0;
	pch = strtok (wrq.u.data.pointer, "\n");
	while (pch != NULL){
		if(strstr(pch, "#") != NULL){
			tr_msg(pch, &(nodes[i]));
			i++;
		}
		pch = strtok (NULL, "\n");
	}
	i--;
	for(;i>=0;i--){
		printf("SSID: %s, BSSID: %s, Signal: %s\n", nodes[i].ssid, nodes[i].bssid, nodes[i].signal);
		notify(&(nodes[i]));
	}
	return 0;
}
