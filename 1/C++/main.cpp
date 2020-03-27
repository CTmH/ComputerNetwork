#include <iostream>
#include "WzSerialPort.h"
#include <string.h>

using namespace std;

char parity[10];

char* pari(int Parity) {
	memset(parity, 0, sizeof(parity));
	while (true) {
		switch (Parity) {
		case 0:
			strcpy_s(parity, "��У��");
			return parity;
			break;
		case 1:
			strcpy_s(parity, "��У��");
			return parity;
			break;
		case 2:
			strcpy_s(parity, "żУ��");
			return parity;
			break;
		default:
			cout << "����������" << endl;
			break;
		}
	}
}

void send(WzSerialPort z) {
	char message[1024];
	while (true) {
		cout << "*****��������Ҫ������ַ���*****" << endl;
		memset(message, 0, sizeof(message));
		cin >> message;
		if (strcmp("stop", message) == 0) {
			z.send(message, 1024);
			return;
		}
		z.send(message, 1024);
	}
}

void receive(WzSerialPort z) {
	char message[1024];
	while (true) {
		memset(message, 0, sizeof(message));
		z.receive(message, 1024);
		cout << message << endl;
		if (strcmp(message, "stop") == 0) {
			cout << "*****���ͽ���*****" << endl;
			return;
		}
	}
}

void sendDemo()
{
	WzSerialPort w;
	char COM1[10];
	int Parity;
	cout << "*****�����봮��*****" << endl;
	cin >> COM1;
	cout << "*****������У��λ*****" << endl;
	cin >> Parity;
	strcpy_s(parity, pari(Parity));
	if (w.open(COM1, 9600, Parity, 8, 1))
	{
		cout << "*****" << "���Ͷ˴�������: " << COM1 << " У�鷽ʽ: " << parity << " ������: " << 9600 << " bps ����λ: " << 8 << " ֹͣλ: " << 1 << "*****" << endl;
		send(w);
	}
	else
	{
		cout << "*****���ڴ�ʧ��*****" << endl;
	}
}

void receiveDemo()
{
	WzSerialPort w;
	char COM2[10];
	int Parity;
	cout << "*****�����봮��*****" << endl;
	cin >> COM2;
	cout << "*****������У��λ*****" << endl;
	cin >> Parity;
	strcpy_s(parity, pari(Parity));
	if (w.open(COM2, 9600, Parity, 8, 1))
	{
		cout << "*****" << "���ն˴�������: " << COM2 << " У�鷽ʽ: " << parity << " ������: " << 9600 << " bps ����λ: " << 8 << " ֹͣλ: " << 1 << "*****" << endl;
		receive(w);
	}
	else {
		cout << "*****���ڴ�ʧ��*****" << endl;
	}
}

int main(int argumentCount, const char* argumentValues[])
{
	// ����COM1�Ѿ�������һ���������Ӻ���
	// ���� demo
	sendDemo();

	// ���� demo
	// receiveDemo();

	getchar();
	return 0;
}