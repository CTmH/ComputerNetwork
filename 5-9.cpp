#define WIN32
#define HAVE_REMOTE
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "packet.lib")


//#include "remote-ext.h"
#include "pcap.h"
//#include "stdafx.h"
//#include <string>
//#include <iostream>

#define ETH_ARP         0x0806  //��̫��֡���ͱ�ʾ�������ݵ����ͣ�����ARP�����Ӧ����˵�����ֶε�ֵΪx0806
#define ARP_HARDWARE    1  //Ӳ�������ֶ�ֵΪ��ʾ��̫����ַ
#define ETH_IP          0x0800  //Э�������ֶα�ʾҪӳ���Э���ַ����ֵΪx0800��ʾIP��ַ
#define ARP_REQUEST     1   //ARP����
#define ARP_RESPONSE       2      //ARPӦ��
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;


/* 14�ֽڵ���̫��֡ͷ */
typedef struct mac_header {
	u_char dmac[6];	//Ŀ��mac��ַ 6�ֽ�
	u_char smac[6];	//Դmac��ַ 6�ֽ�  
	u_short type;	//���� 2�ֽ�
	//string
}mac_header;

typedef struct ip_address {
	u_char byte1; //��ַ��һ���ֽ� 8λ
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;


/*28�ֽڵ� ARP֡�ṹ */
struct arp_header
{
	unsigned short hdType;   //Ӳ������
	unsigned short proType;   //Э������
	unsigned char hdSize;   //Ӳ����ַ����
	unsigned char proSize;   //Э���ַ����
	unsigned short op;   //�������ͣ�ARP����1����ARPӦ��2����RARP����3����RARPӦ��4����
	u_char smac[6];   //ԴMAC��ַ
	ip_address sip;   //ԴIP��ַ
	u_char dmac[6];   //Ŀ��MAC��ַ
	ip_address dip;   //Ŀ��IP��ַ
};

/* ARP�Ļ����� */
struct arp_buffer
{
	u_char dmac[6];   //Ŀ��MAC��ַ
	u_char dip1;   //Ŀ��IP��ַ
	u_char dip2;
	u_char dip3;
	u_char dip4;
};

//��������arp���İ����ܳ���42�ֽ�
struct ArpPacket {
	mac_header ed;
	arp_header ah;
};

arp_buffer *ab[10] ;



/* �ص�����ԭ�� */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);


/* �ص����������յ�ÿһ�����ݰ�ʱ�ᱻlibpcap������ */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	u_int ip_len,ip_tlen,udp_len,tcp_len;
	u_short sport, dport, sport2, dport2;
	time_t local_tv_sec;
	pcap_t *adhandle;

	/* ��ʱ���ת���ɿ�ʶ��ĸ�ʽ */
	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* ��ӡ���ݰ���ʱ����ͳ��� */
	printf("%s.%.6d len:%d\n ", timestr, header->ts.tv_usec, header->len);

	/* �����̫��֡ͷ����λ�� */
	mac_header *mh;
	mh = (mac_header*)pkt_data;

	unsigned char sendbuf[42]; //arp���ṹ��С��42���ֽ�
	unsigned char mac[6] = { 0x00,0x11,0x22,0x33,0x44,0x55 };
	unsigned char ip[4] = { 0x01,0x02,0x03,0x04 };
	mac_header *eh;
	arp_header *ah;
	
	//arp_buffer *ab[10];
	ah = (struct arp_header*)(pkt_data + 14);
//	ab[0]->dip.byte1 = ah->dip.byte1;
	int flag = 0;
	int i, j;
	*ab = new arp_buffer[10];
	//ab[0]->dip1 = 1;
	/*
	for (j = 0; j < 10; j++)
	{
		if (flag <= 10)
		{
			flag++;
			ab[j]->dip1 = ah->dip.byte1;
			ab[j]->dip2 = ah->dip.byte2;
			ab[j]->dip3 = ah->dip.byte3;
			ab[j]->dip4 = ah->dip.byte4;
		
			for( i = 0; i < 6; i++)
			{
				ab[j]->dmac[i] = ah->dmac[i];
			}
		}
		else
		{
			j = 0;
			flag++;
			ab[j]->dip1 = ah->dip.byte1;
			ab[j]->dip2 = ah->dip.byte2;
			ab[j]->dip3 = ah->dip.byte3;
			ab[j]->dip4 = ah->dip.byte4;
			for (i = 0; i < 6; i++)
			{
				ab[j]->dmac[i] = ah->dmac[i];
			}
		}
	}
	*/



	printf("ARP���Ͱ�����\n ");
	printf("Դ��̫����ַ:%02x:%02x:%02x:%02x:%02x:%02x\n", *ah->smac, *(ah->smac + 1), *(ah->smac + 2), *(ah->smac + 3), *(ah->smac + 4), *(ah->smac + 5));
	u_char *mac_string;
	//printf("ԴIP��ַ:%s\n", (source_ip_address));
	printf("ԴIP��ַ��%d.%d.%d.%d\n",(ah->sip.byte1),(ah->sip.byte2),(ah->sip.byte3),(ah->sip.byte4));
	printf("Ŀ��MAC��ַ��ff-ff-ff-ff-ff-ff\n ");
	printf("Ŀ��IP��ַ��%d.%d.%d.%d\n\n", (ah->dip.byte1), (ah->dip.byte2), (ah->dip.byte3), (ah->dip.byte4));

	printf("ARP��Ӧ������\n ");
	printf("Ŀ��IP��ַ��%d.%d.%d.%d\n", (ah->dip.byte1), (ah->dip.byte2), (ah->dip.byte3), (ah->dip.byte4));
	mac_string = ah->dmac;
	printf("Ŀ��IP��ַ��Ӧ����̫����ַ:%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
	//printf("Ŀ��IP��ַ:%s\n", (destination_ip_address));

	printf("ARP��ǰ����������\n ");
	printf("Ŀ��IP��ַ��%d.%d.%d.%d\n", (ah->dip.byte1), (ah->dip.byte2), (ah->dip.byte3), (ah->dip.byte4));
	mac_string = ah->dmac;
	printf("Ŀ��IP��ַ��Ӧ����̫����ַ:%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
	/*
	for (i = 0; i < 10; i++)
	{
		printf("Ŀ��IP��ַ��%d.%d.%d.%d\n", (ab[i]->dip1), (ab[i]->dip2), (ab[i]->dip3), (ab[i]->dip4));
		mac_string = ab[i]->dmac;
		printf("��Ӧ����̫����ַ:%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
		//printf("Ŀ��IP��ַ:%s\n", (destination_ip_address));
	}
	*/

	printf("\n\n");


	//����һ��ARP����
	memset(sendbuf, 0, sizeof(sendbuf));   //ARP����
	memcpy(sendbuf, &eh, sizeof(eh));
	memcpy(sendbuf + sizeof(eh), &ah, sizeof(ah));
	//pcap_sendpacket(adhandle, sendbuf, 42);
	/*
	//������ͳɹ�
	if (pcap_sendpacket(adhandle, sendbuf, 42) == 0) {
		printf("\nPacketSend succeed\n");
	}
	else {
		printf("PacketSendPacket in getmine Error: %d\n", GetLastError());
	}
	*/
	
}

int main()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i = 0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "ip and udp or ip and tcp";
	struct bpf_program fcode;

	/* ����豸�б� */
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* ��ӡ�б� */
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}

	printf("Enter the interface number (1-%d):", i);
	scanf("%d", &inum);

	if (inum < 1 || inum > i)
	{
		printf("\nInterface number out of range.\n");
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* ��ת����ѡ�豸 */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* �������� */
	if ((adhandle = pcap_open(d->name,  // �豸��
		65536,     // Ҫ��׽�����ݰ��Ĳ��� 
				   // 65535��֤�ܲ��񵽲�ͬ������·���ϵ�ÿ�����ݰ���ȫ������
		PCAP_OPENFLAG_PROMISCUOUS,         // ����ģʽ
		1000,      // ��ȡ��ʱʱ��
		NULL,      // Զ�̻�����֤
		errbuf     // ���󻺳��
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n");
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* ���������·�㣬Ϊ�˼򵥣�����ֻ������̫�� */
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
		/* ��ýӿڵ�һ����ַ������ */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* ����ӿ�û�е�ַ����ô���Ǽ���һ��C������� */
		netmask = 0xffffff;


	//���������
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		return -1;
	}

	//���ù�����
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* �ͷ��豸�б� */
	pcap_freealldevs(alldevs);

	/* ��ʼ��׽ */
	pcap_loop(adhandle, 0, packet_handler, NULL);

	system("pause");
	return 0;
}



