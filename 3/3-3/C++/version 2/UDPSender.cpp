#include <stdio.h>
#include <Winsock2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
using namespace std;
#pragma warning(disable:4996)

class UDPSender
{
private:
	SOCKET Sock;
	SOCKADDR_IN receiverAddress;
	int receiverPort = 8888; //���ն˶˿ں�

	int dataLen = 20; //����֡����
	int sendFrameLen = 40; //����֡����
	int receiveFrameLen = 1; //����֡����
	int serialPos = 0; //���к�λ��
	int dataStartPos = 1; //����֡��ʼλ��
	int validDataLenPos = 21; //��Ч���ݳ��ȵ�λ��
	int crcStartPos = 22; //16λCRCУ�������ʼλ��
	int crcLen = 16; //CRC������ĳ���
	int isEndPos = 38; //�ļ��Ƿ�����ϵı�ʶλ��

	int nextFrameToSend = 0;
	long seq = 0; //����֡�ı��
	long filterSeq = 0; //����֡�ı��
	int filterError = 10; //ÿ10֡1֡����
	int filterLost = 10; //ÿ10֡1֡��ʧ
	int firstError = 3; //��һ��������֡�ı��
	int firstLost = 8;  //��һ����ʧ����֡�ı��

	//���ִ���ģʽ�Ĵ���
	const int right = 0; 
	const int error = 1;
	const int lost = 2;

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

	void Print(int method)
	{
		printTime();
		cout << "next_frame_to_send: " << nextFrameToSend << endl;
		cout << "seq: " << seq << endl;
		if (method == right)
		{
			cout << "Simulate sending right." << endl;
		}
		else if (method == error)
		{
			cout << "Simulate sending wrong." << endl;
		}
		else if (method == lost)
		{
			cout << "Simulate sending lost." << endl;
		}
		cout << endl;
	}

	//�жϳ�ʱ������Ƿ��յ�ȷ��֡
	bool waitForEvent()
	{
		char *receiveFrame = new char[receiveFrameLen];
		int len = sizeof(SOCKADDR);
		int a = recvfrom(Sock, receiveFrame, 1024, 0, (SOCKADDR*)&receiverAddress, &len);
		if (a <= 0)
		{
			printTime();
			cout << "Reveiving ack overtime, be ready to resend." << endl << endl;
			return false;
		}
		printTime();
		cout << "Received ack, ack is: " << receiveFrame[0] << endl << endl;
		return true;
	}

	void Send()
	{
		//�����׽��ֿ�
		WSADATA WSAdata;
		WSAStartup(MAKEWORD(2, 2), &WSAdata);

		//�󶨶˿�
		Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		receiverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		receiverAddress.sin_family = AF_INET;
		receiverAddress.sin_port = htons(receiverPort);
		int len = sizeof(SOCKADDR);

		//����recvfrom�Ľ��ճ�ʱΪ3��
		timeval tv_out;
		tv_out.tv_sec = 3000;
		tv_out.tv_usec = 0;
		setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_out, sizeof(timeval));

		ifstream in("SendText.txt");
		char *data = new char[dataLen];

		bool flag = true;
		int pos = dataLen;
		while (true)
		{
			for (int i = 0; i < dataLen; i++)
			{
				data[i] = '\a';
			}

			in.read(data, dataLen); 

			if (in.eof())//����Ѷ����ļ�����Ҫʶ��������ַ�����
			{
				if (flag == false)//���һ���������ݵ�֡�յ�ȷ��֡������û�����ݵ�֡���˳�ѭ��
				{
					char *sendFrame = new char[sendFrameLen];
					sendFrame[isEndPos] = '1';
					sendto(Sock, sendFrame, strlen(sendFrame), 0, (SOCKADDR*)&receiverAddress, len);
					cout << "Send the file finished." << endl;
					break;
				}
				pos = 0;
				while (data[pos] != '\a')
				{
					pos++;
					continue;
				}
				flag = false;
			}
			seq++;

			//����CRCУ����
			string s = getBinaryString(string(data).substr(0, pos));
			string crcStr = getCRCString(s);

			//Ϊ����֡��ֵ
			char *sendFrame = new char[sendFrameLen];
			sendFrame[serialPos] = nextFrameToSend + '0';
			for (int i = 0; i < dataLen; i++)
			{
				sendFrame[i + dataStartPos] = data[i];
			}
			sendFrame[validDataLenPos] = pos;
			for (int i = 0; i < crcLen; i++)
			{
				sendFrame[i + crcStartPos] = crcStr[i];
			}
			sendFrame[isEndPos] = '0';

			bool mark = false;
			while (mark == false)
			{
				//ģ�⴫�����
				if ((filterSeq - firstError) % filterError == 0)
				{
					char pre = sendFrame[crcStartPos];
					sendFrame[crcStartPos] = '1' - pre + '0';
					sendto(Sock, sendFrame, strlen(sendFrame), 0, (SOCKADDR*)&receiverAddress, len);
					Print(error);
					sendFrame[crcStartPos] = pre;
					filterSeq++;
				}
				//ģ�⴫�䶪ʧ
				else if ((filterSeq - firstLost) % filterLost == 0)
				{
					Print(lost);
					filterSeq++;
				}
				//ģ�⴫����ȷ
				else
				{
					sendto(Sock, sendFrame, strlen(sendFrame), 0, (SOCKADDR*)&receiverAddress, len);
					Print(right);
					filterSeq++;
				}
				// ���ڴ����ٶ�
				Sleep(1000);

				mark = waitForEvent();
				if (mark == true) 
				{
					nextFrameToSend = (nextFrameToSend + 1) % 2;
				}
			}
		}
		in.close();
		closesocket(Sock);
		WSACleanup();
	}

};

int main()
{
	//��Ҫ�ȴ򿪽��նˣ�����recefrom������
	string confirm;
	cout << "Please open UDPReceiver.exe and input yes!" << endl;
	cin >> confirm;
	if (confirm.compare("yes"))
	{
		return 0;
	}
	cout << endl;
	cout << "Be ready to send file..." << endl;
	UDPSender operation;
	operation.Send();
	system("pause");
}