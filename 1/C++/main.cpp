#include <iostream>
#include "WzSerialPort.h"
#include <string.h>
using namespace std;

void sendDemo()
{
	WzSerialPort w;
	char COM1[10];
	int Parity;
	cout << "*****�����봮��*****" << endl;
	cin >> COM1;
	cout << "*****������У��λ*****" << endl;
	cin >> Parity;
	if (w.open(COM1, 9600, Parity, 8, 1))
	{
		for (int i = 0;i < 10;i++)
		{
			w.send("helloworld", 10);
		}
		cout << "send demo finished...";
	}
	else
	{
		cout << "*****���ڴ�ʧ��*****"<< endl;
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
	if (w.open(COM2, 9600, Parity, 8, 1))
	{
		char buf[1024];
		while (true)
		{
			memset(buf, 0, 1024);
			w.receive(buf, 1024);
			cout << buf;
		}
	}
	else {
		cout << "*****���ڴ�ʧ��*****" << endl;
	}
}

int main(int argumentCount, const char* argumentValues[])
{
	// ����COM1�Ѿ�������һ���������Ӻ���
	// ���� demo
	//sendDemo();

	// ���� demo
	receiveDemo();

	getchar();
	return 0;
}