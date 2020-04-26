import java.util.Scanner;

public class BitStuffing {
	private String flagString = "01111110";
	private String insertString = "11111";

	public String Send() {
		System.out.print("����������͵�������Ϣ�����Ʊ��ش�: ");
		Scanner sc = new Scanner(System.in);
		String dataStr = sc.nextLine();
		sc.close();

		String sendFrame = flagString + dataStr + flagString;
		int flagLen = flagString.length();
		int sendFrameLen = sendFrame.length();
		System.out.println("֡��ʼ��־��" + sendFrame.substring(0, flagLen));
		System.out.println("֡������Ϣ��" + sendFrame.substring(flagLen, sendFrameLen - flagLen));
		System.out.println("֡������־��" + sendFrame.substring(sendFrameLen - flagLen));

		int insertLen = insertString.length();
		for (int i = flagLen; i < (sendFrame.length() - flagLen); i++) {
			if (sendFrame.substring(i, i + insertLen).equals(insertString)) {
				sendFrame = sendFrame.substring(0, i + insertLen) + "0" + sendFrame.substring(i + insertLen);
				i = i + insertLen;
			}
		}

		System.out.println("��������ķ���֡��" + sendFrame);
		System.out.println();
		return sendFrame;
	}

	public void Receive(String receiveFrame) {
		int flagLen = flagString.length();
		int insertLen = insertString.length();
		for (int i = flagLen; i < (receiveFrame.length() - flagLen); i++) {
			if (receiveFrame.substring(i, i + insertLen).equals(insertString)) {
				receiveFrame = receiveFrame.subSequence(0, i + insertLen) + receiveFrame.substring(i + insertLen + 1);
				i = i + insertLen - 1;
			}
		}
		System.out.println("����ɾ����Ľ���֡��" + receiveFrame);
	}

	public static void main(String[] args) {
		BitStuffing operation = new BitStuffing();
		String frameStr = operation.Send();
		operation.Receive(frameStr);
	}

}
