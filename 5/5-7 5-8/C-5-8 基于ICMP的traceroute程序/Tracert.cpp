#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <IPHlpApi.h>
//���Ӿ�̬���ӿ�ws2_32.lib
#pragma comment(lib,"ws2_32.lib")
//����3���������͵�ָ��
typedef HANDLE(WINAPI *lpIcmpCreateFile)(VOID);
typedef BOOL(WINAPI *lpIcmpCloseHandle)(HANDLE  IcmpHandle);
typedef DWORD(WINAPI *lpIcmpSendEcho)(
	HANDLE                   IcmpHandle,
	IPAddr                   DestinationAddress,
	LPVOID                   RequestData,
	WORD                     RequestSize,
	PIP_OPTION_INFORMATION   RequestOptions,
	LPVOID                   ReplyBuffer,
	DWORD                    ReplySize,
	DWORD                    Timeout
	);
int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: %s destIP\n", argv[0]);
		exit(-1);
	}
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("WSAStartup failed.\n");
		exit(-1);
	}
	//ת��IP��ַ������
	unsigned long ip = inet_addr(argv[1]);
	if (ip == INADDR_NONE) {
		//�û����������������
		hostent* pHost = gethostbyname(argv[1]);
		//��������޷�����
		if (pHost == NULL) {
			printf("Invalid IP or domain name: %s\n", argv[1]);
			exit(-1);
		}
		//ȡ�����ĵ�һ��IP��ַ
		ip = *(unsigned long*)pHost->h_addr_list[0];
		printf("trace route to %s(%s)\n\n", argv[1], inet_ntoa(*(in_addr*)&ip));
	}
	else {
		printf("trace route to %s\n\n", argv[1]);
	}
	//����ICMP.DLL��̬��
	HMODULE hIcmpDll = LoadLibrary("icmp.dll");
	if (hIcmpDll == NULL) {
		printf("fail to load icmp.dll\n");
		exit(-1);
	}
	//����3������ָ��
	lpIcmpCreateFile IcmpCreateFile;
	lpIcmpCloseHandle IcmpCloseHandle;
	lpIcmpSendEcho IcmpSendEcho;
	//��ICMP.DLL�л�ȡ����ĺ�����ڵ�ַ
	IcmpCreateFile = (lpIcmpCreateFile)GetProcAddress(hIcmpDll, "IcmpCreateFile");
	IcmpCloseHandle = (lpIcmpCloseHandle)GetProcAddress(hIcmpDll, "IcmpCloseHandle");
	IcmpSendEcho = (lpIcmpSendEcho)GetProcAddress(hIcmpDll, "IcmpSendEcho");
	//��ICMP���
	HANDLE hIcmp;
	if ((hIcmp = IcmpCreateFile()) == INVALID_HANDLE_VALUE) {
		printf("\tUnable to open ICMP file.\n");
		exit(-1);
	}
	//����IP��ͷ��TTLֵ
	IP_OPTION_INFORMATION IpOption;
	ZeroMemory(&IpOption, sizeof(IP_OPTION_INFORMATION));
	IpOption.Ttl = 1;
	//����Ҫ���͵�����
	char SendData[32];
	memset(SendData, '0', sizeof(SendData));
	//���ý��ջ�����
	char ReplyBuffer[sizeof(ICMP_ECHO_REPLY) + 32];
	PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
	BOOL bLoop = TRUE;
	int iMaxHop = 30;
	while (bLoop && iMaxHop--) {
		printf("%2d: ", IpOption.Ttl);
		//����ICMP��������
		if (IcmpSendEcho(hIcmp, (IPAddr)ip, SendData, sizeof(SendData), &IpOption,
			ReplyBuffer, sizeof(ReplyBuffer), 3000) != 0) {
			if (pEchoReply->RoundTripTime == 0) {
				printf("\t<1ms");
			}
			else {
				printf("\t%dms", pEchoReply->RoundTripTime);
			}
			printf("\t%s\n", inet_ntoa(*(in_addr*)&(pEchoReply->Address)));
			//�ж��Ƿ����·��·��̽��
			if ((unsigned long)pEchoReply->Address == ip) {
				printf("\nTrace complete.\n");
				bLoop = FALSE;
			}
		}
		else {
			printf("\t*\tRequest time out.\n");
		}
		IpOption.Ttl++;
	}
	IcmpCloseHandle(hIcmp);
	FreeLibrary(hIcmpDll);
	WSACleanup();
	system("pause");
	return 0;
}