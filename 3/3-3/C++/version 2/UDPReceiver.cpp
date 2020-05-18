#include <stdio.h>
#include <Winsock2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
using namespace std;
#pragma warning(disable:4996)

class UDPReceiver
{
private:
	SOCKET Sock;
	SOCKADDR_IN senderAddress;
	SOCKADDR_IN receiverAddress;
	int receiverPort = 8888; //���ն˶˿ں�

	int dataLen = 20; //����֡����
	int sendFrameLen = 1; //����֡����
	int receiveFrameLen = 40; //����֡����
	int serialPos = 0; //���к�λ��
	int dataStartPos = 1; //����֡��ʵλ��
	int validDataLenPos = 21; //��Ч���ݳ��ȵ�λ��
	int crcStartPos = 22; //16ΪCRCУ�������ʼλ��
	int crcLen = 16; //CRC������ĳ���
	int isEndPos = 38; //�ļ��Ƿ�����ϵı�ʶλ��
	int frameExpected = 0; 

public:
	//��õ����ַ��İ�λ��������
	string getSingleBinaryString(int a)
	{
		char s1[10];
		_itoa_s(a, s1, 2);
		string s2(s1);
		while (s2.length() < 8)
		{
			s2 = "0" + s2;
		}
		return s2;
	}

	//����ַ�����ÿ���ַ��İ�λ����������϶��ɵĶ������ַ���
	string getBinaryString(string source)
	{
		string s = "";
		for (int i = 0; i < source.length(); i++)
		{
			s += getSingleBinaryString(int(source[i]));
		}
		return s;
	}

	//��������
	string getRemainderStr(string dividendStr, string divisorStr)
	{
		int dividendLen = dividendStr.length();
		int divisorLen = divisorStr.length();
		for (int i = 0; i < divisorLen - 1; i++)
		{
			dividendStr += "0";
		}
		for (int i = 0; i < dividendLen; i++)
		{
			if (dividendStr[i] == '1')
			{
				dividendStr[i] = '0';
				for (int j = 1; j < divisorLen; j++)
				{
					if (dividendStr[i + j] == divisorStr[j])
					{
						dividendStr[i + j] = '0';
					}
					else
					{
						dividendStr[i + j] = '1';
					}
				}
			}
		}
		string remainderStr = dividendStr.substr(dividendLen, divisorLen);
		return remainderStr;
	}

	string getCRCString(string s)
	{
		string gxStr = "10001000000100001";
		return getRemainderStr(s, gxStr);
	}

	void printTime()
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		printf("Current time: %4d-%02d-%02d %02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	}

	void Receive()
	{
		//�����׽��ֿ�
		WSADATA WSAdata;
		WSAStartup(MAKEWORD(2, 2), &WSAdata);

		//����UDPͨ�ŵ��������
		Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		receiverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		receiverAddress.sin_family = AF_INET;
		receiverAddress.sin_port = htons(receiverPort);
		bind(Sock, (SOCKADDR*)&receiverAddress, sizeof(SOCKADDR));
		int len = sizeof(SOCKADDR);

		ofstream out("ReceiveText.txt");
		while (true)
		{
			char *receiveFrame = new char[receiveFrameLen];
			memset(receiveFrame, 0, sizeof(receiveFrame));

			recvfrom(Sock, receiveFrame, 1024, 0, (SOCKADDR*)&senderAddress, &len);
			if (receiveFrame[isEndPos] == '1') //����յ�û�����ݵ�֡����ʾ�ļ�ȫ��������ϣ��˳�ѭ��
			{
				cout << "Receive the file finished." << endl;
				break;
			}
			int serial = receiveFrame[serialPos] - '0';
			int validDataLen = receiveFrame[validDataLenPos];

			//��������
			string dataStr = getBinaryString(string(receiveFrame).substr(dataStartPos, validDataLen));
			string CRC = string(receiveFrame).substr(crcStartPos, crcLen);
			string crcStr = getCRCString(dataStr + CRC);

			if (atoi(crcStr.c_str()) == 0)//������ȷ
			{
				printTime();
				cout << "frame_expected: " << frameExpected << endl;
				cout << "Data of the frame is right, serial is: " << serial << endl;

				char *sendFrame = new char[sendFrameLen];
				sendFrame[0] = 1 - frameExpected + '0';
				sendto(Sock, sendFrame, strlen(sendFrame), 0, (SOCKADDR*)&senderAddress, len);
				cout << "Sending ack, ack is: " << (1 - frameExpected) << endl << endl;

				if (serial - 0 == frameExpected)
				{
					char *dataFrame = new char[validDataLen];
					for (int i = 0; i < validDataLen; i++)
					{
						dataFrame[i] = receiveFrame[dataStartPos + i];
					}
					out.write(dataFrame, validDataLen);
					frameExpected = (frameExpected + 1) % 2;
				}
			}
			else //���ݳ���
			{
				printTime();
				cout << "Data of the frame is wrong, doesn't send ack." << endl << endl;
			}
		}
		out.close();
		closesocket(Sock);
		WSACleanup();
	}

};

int main()
{
	cout << "Be ready to receive file..." << endl;
	UDPReceiver operation;
	operation.Receive();
	system("pause");	
}