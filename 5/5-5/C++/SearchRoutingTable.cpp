#include <stdio.h>
#include <string>
#include <iostream>
#define n 4
using namespace std;

class SearchRoutingTable
{
private:
	int IPAddress[n][4]; //IP��ַ�Ķ����������ֶ�
	int subnetMask[n][4]; //��������Ķ����������ֶ�
	string nextHop[n] = { "Interface0", "Interface1", "Router1", "Router2" }; //��һ��
	int IPSubAddress[n][4] = { { 135, 46, 56, 0 }, { 136, 46, 60, 0 }, { 192, 53, 40, 0 }, { 0, 0, 0, 0 } }; //IP��ַ��ʮ���������ֶ�
	int prefix[n] = { 22, 22, 23, 0 }; //ǰ׺

	string subIPAddress[4]; //Ŀ��IP��ַ���ַ����ֶ�

public:
	string getBinaryString(int a)
	{
		char s[10];
		_itoa_s(a, s, 2);
		return string(s);
	}

	void Initialize()
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				IPAddress[i][j] = atoi(getBinaryString(IPSubAddress[i][j]).c_str());
			}
			string s = "";
			for (int j = 0; j < prefix[i]; j++)
			{
				s += "1";
			}
			for (int j = prefix[i]; j < 32; j++)
			{
				s += "0";
			}
			for (int j = 0; j < 4; j++)
			{
				subnetMask[i][j] = atoi(s.substr(j * 8, 8).c_str());
			}
		}
	}

	void printRoutingTable()
	{
		cout << "Routing Table: " << endl;
		printf("IPAddress/Prefix\tNextHop\n");
		for (int i = 0; i < n; i++)
		{
			if (i == n - 1)
			{
				printf("0/0\t\t\t");
			}
			else
			{
				printf("%d", IPSubAddress[i][0]);
				for (int j = 1; j < 4; j++)
				{
					printf(".%d", IPSubAddress[i][j]);
				}
				printf("/%d\t\t", prefix[i]);
			}
			cout << nextHop[i] << endl;
		}
		cout << endl;
	}

	//��ֵ��subIPAddress����
	void Split(string s)
	{
		int num = 0;
		int pos1 = 0;
		int pos2 = 0;
		while (true)
		{
			pos2 = s.find(".", pos1);
			if (pos2 == s.npos)
			{
				break;
			}
			subIPAddress[num] = s.substr(pos1, pos2 - pos1);
			pos1 = pos2 + 1;
			num++;
		}
		subIPAddress[num] = s.substr(pos1);
	}

	void Search(string s)
	{
		Split(s);
		int maxPrefixPos = n;
		int maxPrefixLen = -1;
		for (int i = 0; i < n; i++)
		{
			bool flag = true;
			for (int j = 0; j < 4; j++)
			{
				int value1 = strtol(to_string(subnetMask[i][j]).c_str(), NULL, 2);
				int value2 = strtol(subIPAddress[j].c_str(), NULL, 10);
				int value3 = strtol(to_string(IPAddress[i][j]).c_str(), NULL, 2);
				if ((value1 & value2) != value3)
				{
					flag = false;
					break;
				}
			}
			if (flag == false)
			{
				printf("Searching line %d, not matched.\n", i);
			}
			else if (flag == true)
			{
				printf("Searching line %d, matched successfully.\n", i);
				if (prefix[i] > maxPrefixLen)
				{
					maxPrefixLen = prefix[i];
					maxPrefixPos = i;
				}
			}
		}
		cout << "Next hop: " << nextHop[maxPrefixPos] << endl;
	}

};

int main()
{
	cout << "Please input Destination IPAddress of datagram as format 'num1.num2.num3.num4' such as '192.53.41.128'(no ''):" << endl;
	string s;
	cin >> s;
	cout << endl;
	SearchRoutingTable operation;
	operation.Initialize();
	operation.printRoutingTable();
	operation.Search(s);
	system("pause");
}