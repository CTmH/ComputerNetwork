import java.util.Scanner;
import java.lang.Math;

public class CRC {
	private String gxStr = "1101";

	public String GetRemainderStr(String dividendStr, String divisorStr) {
		int dividendLen = dividendStr.length();
		int divisorLen = divisorStr.length();
		long dividend = Long.parseLong(dividendStr, 2);
		long divisor = Long.parseLong(divisorStr, 2);
		dividend = dividend << (divisorLen - 1);
		divisor = divisor << (dividendLen - 1);
		int sumLen = dividendLen + divisorLen - 1;
		long flag = (long) Math.pow(2, sumLen - 1);

		for (int i = 0; i < dividendLen; i++) {
			// �жϲ�����֡���λΪ1������
			if ((dividend & flag) != 0) {
				dividend = dividend ^ divisor;
				divisor = divisor >> 1;
			} else {
				divisor = divisor >> 1;
			}
			// flag���λ��1����
			flag = flag >> 1;
		}

		String remainderStr = Long.toBinaryString(dividend);
		while (remainderStr.length() < (divisorLen - 1)) {
			remainderStr = "0" + remainderStr;
		}
		return remainderStr;
	}

	public String Send() {
		System.out.print("����������͵�������Ϣ�����Ʊ��ش�: ");
		Scanner sc = new Scanner(System.in);
		String dataStr = sc.nextLine();
		sc.close();
		System.out.println("�����͵�������Ϣ�����Ʊ��ش�Ϊ��" + dataStr);
		System.out.println("CRC-CCITT��Ӧ�Ķ����Ʊ��ش�Ϊ��" + gxStr);

		String remainderStr = GetRemainderStr(dataStr, gxStr);

		long remainder = Long.parseLong(remainderStr, 2);
		long data = Long.parseLong(dataStr, 2);
		int dataLen = dataStr.length();
		int gxLen = gxStr.length();
		data = data << (gxLen - 1);
		System.out.println(Long.toBinaryString(data));
		String sendFrame = Long.toBinaryString(data ^ remainder);
		String crcStr = sendFrame.substring(dataLen);
		System.out.println("���ɵ�CRC-CodeΪ: " + crcStr);
		System.out.println("��У��͵ķ���֡Ϊ: " + sendFrame);
		System.out.println();
		return sendFrame;
	}

	public void Receive(String sendFrameStr) {
		System.out.println("���յ�������Ϣ�����Ʊ��ش�Ϊ��" + sendFrameStr);
		int sendFrameLen = sendFrameStr.length();
		int gxLen = gxStr.length();
		String crcStr = sendFrameStr.substring(sendFrameLen - gxLen + 1);
		System.out.println("���ɵ�CRC-CodeΪ: " + crcStr);
		String remainderStr = GetRemainderStr(sendFrameStr, gxStr);
		long remainder = Long.parseLong(remainderStr, 2);
		System.out.println("����Ϊ: " + remainder);
		if (remainder == 0) {
			System.out.println("У��ɹ�");
		} else {
			System.out.println("У�����");
		}
	}

	public static void main(String[] args) {
		CRC operation = new CRC();
		String FrameStr = operation.Send();
		operation.Receive(FrameStr);
	}

}
