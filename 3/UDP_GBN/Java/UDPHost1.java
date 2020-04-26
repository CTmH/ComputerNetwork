import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Arrays;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.TimeUnit;

public class UDPHost1 implements Runnable {
	private static int sendPort = 8888;
	private static int recePort = 9999;
	private static DatagramSocket ds;
	private static InetAddress address;
	private static InputStream is;
	private static OutputStream os;

	private static int infoLen = 20;
	private int frameLen = 28;
	private int kindPos = 0;
	private byte hasData = 1;
	private byte notHasData = 0;
	private int seqPos = 1;
	private int ackPos = 2;
	private byte notHasAck = 127;
	private int infoFlagPos = 3;
	private byte lastInfoFrame = 0;
	private byte notLastInfoFrame = 1;
	private int infoStartPos = 4;
	private int crcStartPos = 24;
	private int crcLastPos = 25;

	private static int MAX_SEQ = 7;
	private static byte buffer[][] = new byte[MAX_SEQ + 1][infoLen + 2];
	private static int nbuffered = 0;
	private static int number[] = new int[MAX_SEQ + 1];

	private static int nextFrameToSend = 0;
	private static int ackExpected = 0;
	private static int frameExpected = 0;
	private static int rank = 0;
	private int filterRank = 0;
	private int filterError = 10;
	private int filterLost = 10;
	private int firstError = 3;
	private int firstLost = 8;

	final private int right = 0;
	final private int error = 1;
	final private int lost = 2;

	private static boolean needToResend = false;
	private static boolean waitForResend = false;
	private static boolean receiveInfoFinished = false;
	private static boolean lastSendAck = false;
	private static boolean receiveFirstInfo = false;

	private static int readFileFlag = 0;

	public int Inc(int k) {
		if (k < MAX_SEQ) {
			return k + 1;
		} else {
			return 0;
		}
	}

	public boolean Between(int a, int b, int c) {
		if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a))) {
			return true;
		} else {
			return false;
		}
	}

	public String getBinaryString(byte b[]) {
		String str = "";
		for (int i = 0; i < b.length; i++) {
			str += Integer.toBinaryString((b[i] & 0xFF) + 0x100).substring(1);
		}
		return str;
	}

	public String getRemainderString(String dividendStr, String divisorStr) {
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

	public String getCRCString(byte info[], int index) {
		byte realInfo[] = new byte[index];
		System.arraycopy(info, 0, realInfo, 0, index);
		String binaryStr = getBinaryString(realInfo);
		String gxStr = "10001000000100001";
		return getRemainderString(binaryStr, gxStr);
	}

	public boolean isInfoRight(byte frame[]) {
		int index = infoStartPos;
		for (; index < infoStartPos + infoLen; index++) {
			if (frame[index] == 0) {
				break;
			}
		}
		byte info[] = new byte[index - infoStartPos + 2];
		System.arraycopy(frame, infoStartPos, info, 0, index - infoStartPos);
		System.arraycopy(frame, crcStartPos, info, index - infoStartPos, 2);
		String crcStr = getCRCString(info, info.length);
		if (Integer.parseInt(crcStr, 2) == 0) {
			return true;
		} else {
			return false;
		}
	}

	public boolean fetchNewInfo() throws Exception {
		byte info[] = new byte[infoLen];
		if (readFileFlag != -1) {
			readFileFlag = is.read(info);
			if (readFileFlag == -1) {
				return false;
			}
			for (int i = readFileFlag; i < infoLen; i++) {
				info[i] = 0;
			}
			String crcStr = getCRCString(info, readFileFlag);
			System.arraycopy(info, 0, buffer[ackExpected], 0, infoLen);
			buffer[ackExpected][infoLen] = (byte) Integer.parseInt(crcStr.substring(0, 8), 2);
			buffer[ackExpected][infoLen + 1] = (byte) Integer.parseInt(crcStr.substring(8, 16), 2);
			number[ackExpected] = rank;
			rank++;
			return true;
		}
		return false;
	}

	@Override
	public void run() {

		// TODO Auto-generated method stub

		byte info[] = new byte[infoLen];

		for (rank = 0; rank < MAX_SEQ + 1; rank++) {
			try {
				readFileFlag = is.read(info);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			if (readFileFlag == -1) {
				break;
			}
			for (int i = readFileFlag; i < infoLen; i++) {
				info[i] = 0;
			}
			String crcStr = getCRCString(info, readFileFlag);
			System.arraycopy(info, 0, buffer[rank], 0, infoLen);
			buffer[rank][infoLen] = (byte) Integer.parseInt(crcStr.substring(0, 8), 2);
			buffer[rank][infoLen + 1] = (byte) Integer.parseInt(crcStr.substring(8, 16), 2);
			number[rank] = rank;
		}
		nbuffered = rank;

		byte receFrame[] = new byte[frameLen];
		while (true) {
			try {
				DatagramPacket dp = new DatagramPacket(receFrame, receFrame.length);
				ds.receive(dp);
				if (receFrame[kindPos] == notHasData) {
					SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
					System.out.println("Current time: " + df.format(new Date()));
					System.out.println("Receive a frame, only has ack, the ack is: " + receFrame[ackPos]);
					System.out.println();
					while (Between(ackExpected, receFrame[ackPos], nextFrameToSend)) {
						nbuffered = nbuffered - 1;
						try {
							if (fetchNewInfo() == true) {
								nbuffered = nbuffered + 1;
							}
						} catch (Exception e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						ackExpected = Inc(ackExpected);
						if (nbuffered == 0) {
							System.out.println("SendFile was sended completed.");
							System.out.println();
						}
					}
					if (nbuffered == 0 && receiveInfoFinished == true) {
						break;
					}
				} else if (receFrame[kindPos] == hasData) {
					if (receFrame[ackPos] != notHasAck) {
						while (Between(ackExpected, receFrame[ackPos], nextFrameToSend)) {
							nbuffered = nbuffered - 1;
							try {
								if (fetchNewInfo() == true) {
									nbuffered = nbuffered + 1;
								}
							} catch (Exception e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							ackExpected = Inc(ackExpected);
							if (nbuffered == 0) {
								System.out.println("SendFile was sended completed.");
								System.out.println();
							}
						}
					}
					if (receFrame[seqPos] == frameExpected && isInfoRight(receFrame) == true) {
						os.write(receFrame, infoStartPos, infoLen);
						SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
						System.out.println("Current time: " + df.format(new Date()));
						if (receFrame[ackPos] == notHasAck) {
							System.out.println(
									"Receive a frame, both seq and info are right, doesn't has ack, seq | frame_expected are: "
											+ receFrame[seqPos] + " | " + frameExpected);
						} else {
							System.out.println(
									"Receive a frame, both seq and info are right, seq | ack | frame_expected are: "
											+ receFrame[seqPos] + " | " + receFrame[ackPos] + " | " + frameExpected);
						}
						System.out.println();

						frameExpected = Inc(frameExpected);
						waitForResend = false;
						receiveFirstInfo = true;

						if (receFrame[infoFlagPos] == lastInfoFrame) {
							System.out.println("ReceiveFile was received completed.");
							System.out.println();
							receiveInfoFinished = true;
							if (nbuffered == 0) {
								break;
							}
						}

					} else {
						if (waitForResend == true) {
							continue;
						}
						SimpleDateFormat df1 = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
						System.out.println("Current time: " + df1.format(new Date()));
						if (receFrame[seqPos] != frameExpected && isInfoRight(receFrame) != true) {
							System.out.println("Receive a frame, both seq and info are wrong, wait for resend!");
						} else if (receFrame[seqPos] != frameExpected) {
							System.out.println("Receive a frame, seq is wrong, wait for resend!");
						} else if (isInfoRight(receFrame) != true) {
							System.out.println("Receive a frame, info is wrong, wait for resend!");
						}
						System.out.println();
						waitForResend = true;
					}
				}

			} catch (Exception e) {
				SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
				System.out.println("Current time: " + df.format(new Date()));
				System.out.println("Receiving is overtime, need to resend!");
				System.out.println();
				needToResend = true;
			}

		}

	}

	public void fillAndSend(int method) throws Exception {
		byte sendFrame[] = new byte[frameLen];

		sendFrame[kindPos] = hasData;
		sendFrame[seqPos] = (byte) nextFrameToSend;
		if (lastSendAck == false && receiveFirstInfo == true) {
			sendFrame[ackPos] = (byte) ((frameExpected + MAX_SEQ) % (MAX_SEQ + 1));
			if (receiveInfoFinished == true) {
				lastSendAck = true;
			}
		} else {
			sendFrame[ackPos] = notHasAck;
		}

		if (readFileFlag == -1 && nbuffered == 1) {
			sendFrame[infoFlagPos] = lastInfoFrame;
		} else {
			sendFrame[infoFlagPos] = notLastInfoFrame;
		}
		System.arraycopy(buffer[nextFrameToSend], 0, sendFrame, infoStartPos, infoLen + 2);

		// System.out.println("nbuffered: " + nbuffered);

		if (method == error) {
			sendFrame[crcLastPos] = (byte) ((sendFrame[crcLastPos] + 1) % 128);
			SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			System.out.println("Current time: " + df.format(new Date()));
			System.out.println("ack_expected | next_frame_to_send | frame_expected are: " + ackExpected + " | "
					+ nextFrameToSend + " | " + frameExpected);
			if (sendFrame[ackPos] != notHasAck) {
				System.out.println("Stimulate sending wrongly, the number and ack of the frame are: "
						+ number[nextFrameToSend] + " | " + sendFrame[ackPos]);
			} else {
				System.out.println("Stimulate sending wrongly, doesn't has ack, the number of the frame is: "
						+ number[nextFrameToSend]);
			}
			System.out.println();
			DatagramPacket dp = new DatagramPacket(sendFrame, sendFrame.length, address, recePort);
			ds.send(dp);
		} else if (method == lost) {
			SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			System.out.println("Current time: " + df.format(new Date()));
			System.out.println("ack_expected | next_frame_to_send | frame_expected are: " + ackExpected + " | "
					+ nextFrameToSend + " | " + frameExpected);
			if (sendFrame[ackPos] != notHasAck) {
				System.out.println("Stimulate sending lost, the number and ack of the frame are: "
						+ number[nextFrameToSend] + " | " + sendFrame[ackPos]);
			} else {
				System.out.println("Stimulate sending lost, doesn't has ack, the number of the frame is: "
						+ number[nextFrameToSend]);
			}
			System.out.println();
		} else if (method == right) {
			SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			System.out.println("Current time: " + df.format(new Date()));
			System.out.println("ack_expected | next_frame_to_send | frame_expected are: " + ackExpected + " | "
					+ nextFrameToSend + " | " + frameExpected);
			if (sendFrame[ackPos] != notHasAck) {
				System.out.println("Stimulate sending rightly, the number and ack of the frame are: "
						+ number[nextFrameToSend] + " | " + sendFrame[ackPos]);
			} else {
				System.out.println("Stimulate sending rightly, doesn't has ack, the number of the frame is: "
						+ number[nextFrameToSend]);
			}
			System.out.println();
			DatagramPacket dp = new DatagramPacket(sendFrame, sendFrame.length, address, recePort);
			ds.send(dp);
		}

	}

	public void Send() throws Exception {

		while (nbuffered != 0) {
			if (waitForResend == true) {
				continue;
			}
			if (needToResend == true) {
				needToResend = false;
				nextFrameToSend = ackExpected;
				for (int i = 1; i <= nbuffered; i++) {
					if ((filterRank - firstError) % filterError == 0) {
						fillAndSend(error);
					} else if ((filterRank - firstLost) % filterLost == 0) {
						fillAndSend(lost);
					} else {
						fillAndSend(right);
					}
					filterRank++;
					nextFrameToSend = Inc(nextFrameToSend);
					TimeUnit.MILLISECONDS.sleep(500);
					if (needToResend == true) {
						i = 1;
						needToResend = false;
						nextFrameToSend = ackExpected;
					}
				}
				continue;
			}

			if ((filterRank - firstError) % filterError == 0) {
				fillAndSend(error);
			} else if ((filterRank - firstLost) % filterLost == 0) {
				fillAndSend(lost);
			} else {
				fillAndSend(right);
			}
			filterRank++;
			nextFrameToSend = Inc(nextFrameToSend);
			TimeUnit.MILLISECONDS.sleep(500);

		}

		byte sendFrame[] = new byte[frameLen];

		while (lastSendAck == false) {
			sendFrame[kindPos] = notHasData;
			sendFrame[ackPos] = (byte) ((frameExpected + MAX_SEQ) % (MAX_SEQ + 1));
			SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			System.out.println("Current time: " + df.format(new Date()));
			System.out.println("Send a frame, only has ack, ack is: " + sendFrame[ackPos]);
			System.out.println();
			DatagramPacket dp = new DatagramPacket(sendFrame, sendFrame.length, address, recePort);
			ds.send(dp);
			TimeUnit.MILLISECONDS.sleep(1000);
			if (receiveInfoFinished == true) {
				lastSendAck = true;
			}
		}

	}

	public static void main(String[] args) throws Exception {
		try {
			is = new FileInputStream(new File("D:\\desktop\\sendText1.txt"));
			os = new FileOutputStream(new File("D:\\desktop\\receiveText2.txt"));
			ds = new DatagramSocket(sendPort);
			ds.setSoTimeout(2000);
			address = InetAddress.getByName(null);
		} catch (Exception e) {
			e.printStackTrace();
		}

		UDPHost1 send = new UDPHost1();
		UDPHost1 receive = new UDPHost1();
		new Thread(receive).start();
		TimeUnit.MILLISECONDS.sleep(1000);
		send.Send();

		try {
			is.close();
			os.close();
			ds.close();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

}
