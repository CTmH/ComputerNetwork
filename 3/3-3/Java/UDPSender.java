import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.Arrays;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.TimeUnit;

public class UDPSender {
	private int senderPort = 9999;
	private int receiverPort = 8888;
	DatagramSocket ds;

	private int dataLen = 20;
	private int sendFrameLen = 25;
	private int receiveFrameLen = 1;
	private int serialPos = 0;
	private int dataStartPos = 1;
	private int crcStartPos = 21;
	private int crcEndPos = 22;
	private int isEndPos = 23;

	private int nextFrameToSend = 0;
	private long seq = 0;
	private long filterSeq = 0;
	private int filterError = 10;
	private int filterLost = 10;
	private int firstError = 3;
	private int firstLost = 8;

	final private int right = 0;
	final private int error = 1;
	final private int lost = 2;

	public String getBinaryString(byte b[]) {
		String str = "";
		for (int i = 0; i < b.length; i++) {
			str += Integer.toBinaryString((b[i] & 0xFF) + 0x100).substring(1);
		}
		return str;
	}

	public String getRemainderStr(String dividendStr, String divisorStr) {
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

	public String getCRCString(String s) {
		String gxStr = "10001000000100001";
		return getRemainderStr(s, gxStr);
	}

	public boolean waitForEvent() throws Exception {
		boolean flag = true;
		try {
			byte receiveFrame[] = new byte[receiveFrameLen];
			DatagramPacket dp = new DatagramPacket(receiveFrame, receiveFrame.length);

			try {
				ds.receive(dp);
			} catch (SocketTimeoutException e) {
				Date t = new Date();
				SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
				System.out.println("Current time: " + df.format(t));
				System.out.println("Reveiving ack overtime, be ready to resend.");
				flag = false;
			}

			if (flag == true) {
				Date t = new Date();
				SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
				System.out.println("Current time: " + df.format(t));
				System.out.println("Received ack, ack is: " + receiveFrame[0]);
			}
			System.out.println();

		} catch (SocketException e) {
			e.printStackTrace();
		}
		return flag;
	}

	public void Print(int method) {
		Date t = new Date();
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		System.out.println("Current time: " + df.format(t));

		System.out.println("next_frame_to_send: " + nextFrameToSend);
		System.out.println("seq: " + seq);
		if (method == right) {
			System.out.println("Simulate sending right.");
		} else if (method == error) {
			System.out.println("Simulate sending wrong.");
		} else if (method == lost) {
			System.out.println("Simulate sending lost.");
		}
		System.out.println();
	}

	public void Send() throws Exception {
		try {
			ds = new DatagramSocket(senderPort);
			ds.setSoTimeout(3000);
			InetAddress address = InetAddress.getByName(null);

			InputStream is = new FileInputStream(new File("SendText.txt"));
			int flag = 0;
			byte[] data = new byte[dataLen];
			while (true) {
				flag = is.read(data);
				if (flag == -1) {
					byte[] sendFrame = new byte[sendFrameLen];
					sendFrame[isEndPos] = 1;
					DatagramPacket dp = new DatagramPacket(sendFrame, sendFrame.length, address, receiverPort);
					ds.send(dp);
					System.out.println("Send the file finished.");
					break;
				}

				seq++;

				byte[] realData = new byte[flag];
				System.arraycopy(data, 0, realData, 0, flag);
				String binaryStr = getBinaryString(realData);
				String crcStr = getCRCString(binaryStr);
				
				byte[] sendFrame = new byte[sendFrameLen];
				sendFrame[serialPos] = (byte) nextFrameToSend;

				for (int i = 0; i < flag; i++) {
					sendFrame[i + dataStartPos] = data[i];
				}
				for (int i = flag; i < dataLen; i++) {
					sendFrame[i + dataStartPos] = 0;
				}

				sendFrame[crcStartPos] = (byte) Integer.parseInt(crcStr.substring(0, 8), 2);
				sendFrame[crcEndPos] = (byte) Integer.parseInt(crcStr.substring(8, 16), 2);

				sendFrame[isEndPos] = 0;

				boolean mark = false;
				while (mark == false) {
					if ((filterSeq - firstError) % filterError == 0) {
						byte pre = sendFrame[crcEndPos - 1];
						sendFrame[crcEndPos - 1] = (byte) ((pre + 1) % 128);
						DatagramPacket dp = new DatagramPacket(sendFrame, sendFrame.length, address, receiverPort);
						ds.send(dp);
						Print(error);
						sendFrame[crcEndPos - 1] = pre;
						filterSeq++;
					} else if ((filterSeq - firstLost) % filterLost == 0) {
						Print(lost);
						filterSeq++;
					} else {
						DatagramPacket dp = new DatagramPacket(sendFrame, sendFrame.length, address, receiverPort);
						ds.send(dp);
						Print(right);
						filterSeq++;
					}

					// ���ڴ����ٶ�
					TimeUnit.MILLISECONDS.sleep(1000);

					mark = waitForEvent();
					if (mark == true) {
						nextFrameToSend = (nextFrameToSend + 1) % 2;
					}
				}

			}
			is.close();
			ds.close();
		} catch (SocketException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	public static void main(String[] args) throws Exception {
		System.out.println("Be ready to send file...");
		UDPSender operation = new UDPSender();
		operation.Send();
	}

}