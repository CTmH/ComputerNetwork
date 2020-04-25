#define WIN32
#define HAVE_REMOTE
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")

//#include "remote-ext.h"
#include "pcap.h"
//#include <string>
//#include <iostream>
using namespace std;

/* 6�ֽڵ�MAC��ַ */
typedef struct mac_address {
	u_char byte1; //��ַ��һ���ֽ� 8λ
	u_char byte2;
	u_char byte3;
	u_char byte4;
	u_char byte5;
	u_char byte6;
}mac_address;

/* 14�ֽڵ���̫��֡ͷ */
typedef struct mac_header {
	mac_address dmac;	//Ŀ��mac��ַ 6�ֽ�
	mac_address smac;	//Դmac��ַ 6�ֽ�  
	u_short type;	//���� 2�ֽ�
	//string
}mac_header;

/* 4�ֽڵ���̫��֡β */
typedef struct mac_tail {
	u_int fcs;	//4�ֽ�
}mac_tail;

/* ���1500�ֽ�����Ϊ46�ֽڵ�ip���ݰ� */
typedef struct ip_data {
	u_int byte1;	//4�ֽ�
	u_int byte2;
	u_int byte3;
	u_int byte4;
	u_int byte5;
	u_int byte6;
	u_int byte7;	
	u_int byte8;
	u_int byte9;
	u_int byte10;
	u_int byte11;
	u_int byte12;
}ip_data;

/* 4�ֽڵ�IP��ַ */
typedef struct ip_address {
	u_char byte1; //��ַ��һ���ֽ� 8λ
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 ���ݰ��ײ� */
typedef struct ip_header {
	u_char  ver_ihl;        // �汾 (4 bits) + �ײ����� (4 bits) ��1�ֽ�
	u_char  tos;            // ��������(Type of service) 
	u_short tlen;           // �ܳ�(Total length)  16λ ��λһ���ֽ� 2�ֽ�
	u_short identification; // ��ʶ(Identification)
	u_short flags_fo;       // ��־λ(Flags) (3 bits) + ��ƫ����(Fragment offset) (13 bits)
	u_char  ttl;            // ���ʱ��(Time to live) 8λ
	u_char  proto;          // Э��(Protocol) 8λ
	u_short crc;            // �ײ�У���(Header checksum) 16λ
	ip_address  saddr;      // Դ��ַ(Source address) 32λ 4�ֽ�
	ip_address  daddr;      // Ŀ�ĵ�ַ(Destination address) 32λ
	u_int   op_pad;         // ѡ�������(Option + Padding)	 32 λ 4�ֽ�
}ip_header;

/* UDP �ײ�*/
typedef struct udp_header {
	u_short sport;          // Դ�˿�(Source port) 2�ֽ�
	u_short dport;          // Ŀ�Ķ˿�(Destination port) 2�ֽ�
	u_short len;            // UDP���ݰ�����(Datagram length) 2�ֽ�
	u_short crc;            // У���(Checksum) 2�ֽ�
}udp_header;

/* 10�ֽڵ�udp���ݰ� */
typedef struct udp_data {
	u_int byte1;	//4�ֽ�
	u_int byte2;
	u_int byte3;
	u_int byte4;
	u_int byte5;
	u_int byte6;
	u_int byte7;
	u_int byte8;
	u_int byte9;
	u_int byte10;
}udp_data;

/* TCP �ײ�*/
typedef struct tcp_header {
	u_short sport;          // Դ�˿�(Source port) 2�ֽ� 16bit
	u_short dport;          // Ŀ�Ķ˿�(Destination port) 2�ֽ�
	u_int shunxu;			//  4�ֽ�
	u_int queren;			//  4�ֽ�
	u_char len;				// ƫ��+���� 10λ ��8λ ����һ���ֽ�
	u_char control;			// ���� 6λ��һ���ֽ�
	u_short window;			//���� 2�ֽ�
	u_short crc;			//У��� 2�ֽ�
	u_short jinji;			//����ָ�� 2�ֽ�
	u_int   op_pad;         // ѡ�������(Option + Padding)	4�ֽ� 32λ
}tcp_header;

/* 10�ֽڵ�tcp���ݰ� */
typedef struct tcp_data {
	u_int byte1;	//4�ֽ�
	u_int byte2;
	u_int byte3;
	u_int byte4;
	u_int byte5;
	u_int byte6;
	u_int byte7;
	u_int byte8;
	u_int byte9;
	u_int byte10;
}tcp_data;


/* UDPר�õ�DHCP���ĸ�ʽ*/
typedef struct dhcp {
	u_char op;
	u_char htype;
	u_char hlen;
	u_char hops;
	u_int  xid;
	u_short secs;
	u_short flags;
	u_int ciaddr;
	u_int yiaddr;
	u_int siaddr;
	u_int giaddr;
}dhcp;

/* �ص�����ԭ�� */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);


/* �ص����������յ�ÿһ�����ݰ�ʱ�ᱻlibpcap������ */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	ip_header *ih;
	udp_header *uh;
	u_int ip_len,ip_tlen,udp_len,tcp_len;
	u_short sport, dport, sport2, dport2;
	time_t local_tv_sec;

	/* ��ʱ���ת���ɿ�ʶ��ĸ�ʽ */
	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* ��ӡ���ݰ���ʱ����ͳ��� */
	printf("%s.%.6d len:%d\n ", timestr, header->ts.tv_usec, header->len);

	/* �����̫��֡ͷ����λ�� */
	mac_header *mh;
	mh = (mac_header*)pkt_data;

	/* ���IP���ݰ�ͷ����λ�� */
	ih = (ip_header *)(pkt_data + 14); //��̫��ͷ������

	/* ���UDP�ײ���λ�� */
	ip_len = (ih->ver_ihl & 0xf) * 4;
	uh = (udp_header *)((u_char*)ih + ip_len);

	/* ���TCP�ײ���λ�� */
	tcp_header *th;
	th = (tcp_header *)((u_char*)ih + ip_len);

	/* ���ip���ݰ���λ�� */
	ip_data *id;
	id = (ip_data *)((u_char*)ih + ip_len);

	/* �����̫��֡β����λ�� */
	mac_tail *mt;
	ip_tlen = (ih->tlen & 0xf) * 1;
	mt = (mac_tail*)((u_char*)ih + ip_tlen);

	/* ���dhcp��λ�� */
	dhcp *d;
	udp_len = (uh->len & 0xf) * 1;
	d = (dhcp*)((u_char*)uh + udp_len);

	/* ���udp���ݰ���λ�� */
	udp_data *ud;
	ud = (udp_data *)((u_char*)uh + udp_len);

	/* ���tcp���ݰ���λ�� */
	tcp_data *td;
	tcp_len = (th->len & 0xf) * 4;
	td = (tcp_data *)((u_char*)uh + tcp_len);

	/* �������ֽ�����ת���������ֽ����� */
	sport = ntohs(uh->sport);
	dport = ntohs(uh->dport);
	sport2 = ntohs(th->sport);
	dport2 = ntohs(th->dport);

	/* ��ӡMAC�Ӳ�ṹ */
	printf("��ӡMAC�Ӳ�ṹ\n");
	//cout << "֡ͷ" << mh<<" IP���ݰ�"<<ih<<"֡β"<<mt<<endl;
	printf("֡ͷ  DMAC:%d%d%d%d%d%d SMAC:%d%d%d%d%d%d type:%hd",
		mh->dmac.byte1, mh->dmac.byte2, mh->dmac.byte3, mh->dmac.byte4, mh->dmac.byte5, mh->dmac.byte6,
		mh->smac.byte1, mh->smac.byte2, mh->smac.byte3, mh->smac.byte4, mh->smac.byte5, mh->smac.byte6,
		mh->type);
	printf("\n");
	printf("ip���ݰ���ǰ10���ֽ�:%d%d%hd%hd%hd%d%d",
		ih->ver_ihl,
		ih->tos,
		ih->tlen,
		ih->identification,
		ih->flags_fo,
		ih->ttl,
		ih->proto);
	printf("\n");
	printf("֡β FCS:%d",
		mt->fcs);
	printf("\n\n");

	/* ��ӡ�����ṹ */
	printf("��ӡ�����ṹ\n");
	printf("ip�ײ�:%d �汾�ź��ײ�����:%d ��������:%d Э��:%d Դip��ַ:%d.%d.%d.%d Ŀ��ip��ַ:%d.%d.%d.%d",
		ih,
		ih->ver_ihl,
		ih->tos,
		ih->proto,
		ih->saddr.byte1,
		ih->saddr.byte2,
		ih->saddr.byte3,
		ih->saddr.byte4,
		ih->daddr.byte1,
		ih->daddr.byte2,
		ih->daddr.byte3,
		ih->daddr.byte4);
	printf("\n");
	printf("ǰ46���ֽڵ����ݰ�:%d%d%d%d%d%d%d%d%d%d%d%d",
		id->byte1,
		id->byte2, 
		id->byte3, 
		id->byte4, 
		id->byte5, 
		id->byte6, 
		id->byte7,
		id->byte8,
		id->byte9,
		id->byte10,
		id->byte11,
		id->byte12);
	printf("\n\n");

	/* ��ӡ�����ṹ */
	printf("��ӡ�����ṹ\n");
	printf("udp�ײ�:%d Դ�˿�:%d Ŀ��˿�:%d",
		uh,
		sport,
		dport);
	printf("\n");
	printf("udp���ݰ���ǰ10���ֽ�:%d%d%d%d%d%d%d%d%d%d",
		ud->byte1,
		ud->byte2,
		ud->byte3,
		ud->byte4,
		ud->byte5,
		ud->byte6,
		ud->byte7,
		ud->byte8,
		ud->byte9,
		ud->byte10);
	printf("\n");
	printf("tcp�ײ�:%d Դ�˿�:%d Ŀ��˿�:%d ���:%d ȷ�Ϻ�:%d",
		th,
		sport2,
		dport2,
		th->shunxu,
		th->queren);
	printf("\n");
	printf("tcp���ݰ���ǰ10���ֽ�:%d%d%d%d%d%d%d%d%d%d",
		td->byte1,
		td->byte2,
		td->byte3,
		td->byte4,
		td->byte5,
		td->byte6,
		td->byte7,
		td->byte8,
		td->byte9,
		td->byte10);
	printf("\n\n");


	/* ��ӡӦ�ò�ṹ */
	printf("��ӡӦ�ò�ṹ\n");
	printf("DHCP:%d ��������:%d �ͻ��˵�MAC��ַ����:%d �ͻ��˵�IP��ַ:%d ������������ͻ��˵�IP��ַ:%d",
		d,
		d->op,
		d->htype,
		d->ciaddr,
		d->yiaddr);
	printf("\n\n");
	/*
	printf("DHCP:%d ��������:%d �ͻ��˵�MAC��ַ����:%d �ͻ��˵�IP��ַ:%d ������������ͻ��˵�IP��ַ:%d",
		d,
		d->op,
		d->htype,
		d->ciaddr,
		d->yiaddr);
	printf("\n\n");
	//tcp��ĳ��Э�� http��ftp���ĳ��Ȳ�ȷ����dns̫���ӡ������޷���������ֶε�ֵ
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



