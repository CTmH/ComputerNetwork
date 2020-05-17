import java.util.Arrays;

public class CRC {
	private String gxStr = "10001000000100001";

	public String GetRemainderStr(String dividendStr, String divisorStr) {
		int dividendLen = dividendStr.length();
		int divisorLen = divisorStr.length();
		for (int i = 0; i < divisorLen - 1; i++) {
			dividendStr += "0";
		}
		char str1[] = dividendStr.toCharArray();
		char str2[] = divisorStr.toCharArray();
		for (int i = 0; i < dividendLen; i++) {
			if (str1[i] == '1') {
				str1[i] = '0';
				for (int j = 1; j < divisorLen; j++) {
					if (str1[i + j] == str2[j]) {
						str1[i + j] = '0';
					} else {
						str1[i + j] = '1';
					}
				}
			}
		}
		String remainderStr = Arrays.toString(str1).replaceAll(", ", "").substring(dividendLen + 1,
				dividendLen + divisorLen);
		return remainderStr;
	}

	public String Send() {
		String dataStr = "01100000000000111110000000000110";
		System.out.println("�����͵�������Ϣ�����Ʊ��ش�Ϊ��" + dataStr);
		System.out.println("CRC-CCITT��Ӧ�Ķ����Ʊ��ش�Ϊ��" + gxStr);

		String remainderStr = GetRemainderStr(dataStr, gxStr);

		String crcStr = remainderStr;
		String sendFrameStr = dataStr + remainderStr;
		
		System.out.println("���ɵ�CRC-CodeΪ: " + crcStr);
		System.out.println("��У��͵ķ���֡Ϊ: " + sendFrameStr);
		System.out.println();
		return sendFrameStr;
	}

	public void Receive(String sendFrameStr) {
		int sendFrameLen = sendFrameStr.length();
		int gxLen = gxStr.length();

		String dataStr = sendFrameStr.substring(0, sendFrameLen - gxLen + 1);
		String crcStr = sendFrameStr.substring(sendFrameLen - gxLen + 1);
		System.out.println("���յ�������Ϣ�����Ʊ��ش�Ϊ��" + dataStr);
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
		String frameStr = operation.Send();
		operation.Receive(frameStr);
	}

}

