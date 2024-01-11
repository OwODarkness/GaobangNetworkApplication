/**
* ������ͻ���
*/
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<graphics.h>
#include <memory>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "12000"
#define DEFAULT_ADDRESS "LAPTOP-BLAV0CC4"
#define DEFAULT_BUFSIZE 512

/**
* ����
* 
*/
class Chess {
public:
	Chess(int x, int y) :coor_x(x), coor_y(y), radius(20) {};
	
	//��ȡ����
	std::pair<int, int> getCoor() const;
	//��ȡ�뾶
	int getRadius() const { return radius; };


private:
	int coor_x;//������
	int coor_y;//������
	int radius;//�뾶
};
std::pair<int, int> Chess::getCoor() const
{
	return std::pair<int, int>(coor_x, coor_y);
}

class ChessBoard {
public:
	ChessBoard();
	//��������
	void generateChessBoard();
	//������ʾ���
	void updatePromptText(const char contains[])
	{
		int left = width - textBlockWidth + textBlockMargin;
		int top = height - textBlockHeight + textBlockMargin;
		int right = width - textBlockMargin;
		int bottom = height - textBlockMargin;
		clearrectangle(left, top, right, bottom);
		setcolor(BLACK);
		rectangle(left, top, right, bottom);
		settextcolor(BLACK);
		outtextxy(left+5, top+5, (TCHAR*)contains);
	}

	int getStep() const { return step; }

	//��������
	bool placeChess(const Chess& chess, int flag)
	{
		int x = chess.getCoor().first;
		int y = chess.getCoor().second;
		if (x >= 1 && x <= lineMaxNumber && y >= 1 && y <= lineMaxNumber)
		{
			if (position[x][y] == 0)
			{
				position[x][y] = flag;
				COLORREF chessColor = flag == 1 ? WHITE : BLACK;
				setfillcolor(chessColor);
				fillcircle(x * step, y * step, chess.getRadius());
				return true;
			}
		}
		return false;
	}

private:
	int position[DEFAULT_BUFSIZE][DEFAULT_BUFSIZE];
	int height;//���̳���
	int width;//���̿��
	int step;//���
	int lineMaxNumber;//���������ߵ�����15*15
	int textBlockWidth;//�ı�����
	int textBlockHeight;//�ı��򳤶�
	int textBlockMargin;//�ı�����߾�

};

ChessBoard::ChessBoard() :
	height(640), width(800), step(40), lineMaxNumber(15), textBlockWidth(200), textBlockHeight(200), textBlockMargin(20)
{
	memset(position, 0, sizeof(position));
}

void ChessBoard::generateChessBoard()
{
	initgraph(width, height, SHOWCONSOLE);//����һ������
	setbkcolor(YELLOW);//���ñ�����ɫ
	cleardevice();//����
	setlinecolor(BLACK);
	for (int i = 1; i <= lineMaxNumber; i++)  // �����ߺ�����
	{
		line(i * step, 1 * step, i * step, lineMaxNumber * step);
		line(1 * step, i * step, lineMaxNumber * step, i * step);
	}
}

void formatCoordinate(int& x, int& y, char s[])
{
	std::cout << "(" << s << ")" << std::endl;
	if (strlen(s) < 4)
	{
		return;
	}
	x = (s[0] - '0') * 10 + (s[1] - '0');
	y = (s[2] - '0') * 10 + (s[3] - '0');
}


char* formatString(int x, int y)
{
	char* temp = (char*)malloc(128 * sizeof(char));
	//��ʼ���ַ���
	memset(temp, '\0', sizeof(char) * 128);
	temp[0] = x / 10 % 10 + '0';
	temp[1] = x % 10 + '0';
	temp[2] = y / 10 % 10 + '0';
	temp[3] = y % 10 + '0';
	return temp;
}

int main()
{
	//Init Winsock
	WSADATA wsadata;

	int iResult;
	int iSendResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed " << iResult << std::endl;
		return 1;
	}

	struct addrinfo* result, *ptr, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(DEFAULT_ADDRESS, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout << "getaddrinfo failed with error " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	SOCKET ConnectSocket = INVALID_SOCKET;
	//�����׽���
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 0);
		if (ConnectSocket == INVALID_SOCKET)
		{
			std::cout << "socket failed with error " << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			std::cout << "trying connect ... " << std::endl;
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		std::cout << "Connect failed with error " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	else
	{
		std::cout << "connect successfully ... " << std::endl;
	}
	//���շ������ķ�������ƥ����һ�����
	int flag = 0;
	char recvbuf[DEFAULT_BUFSIZE] = "";
	char sendbuf[DEFAULT_BUFSIZE] = "";
	iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFSIZE, 0);
	if (iResult < 0)
	{
		std::cout << "recevie failed with error " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	else if (iResult == 0)
	{
		std::cout << "Connection closed" << std::endl;
	}
	else
	{
		std::shared_ptr<ChessBoard> MyChessBoard = std::make_shared<ChessBoard>();
		MyChessBoard->generateChessBoard();
		//BeginBatchDraw();
		std::cout << recvbuf << std::endl;
		
		//��ҵ�ǰ�Ǻ��廹�ǰ���
		if (strcmp(recvbuf, "White") == 0)
		{
			flag = 1;
		}
		else if(strcmp(recvbuf, "Black") == 0)
		{
			flag = -1;
			MyChessBoard->updatePromptText("ƥ����");
		}
		iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFSIZE, 0);
		if (iResult < 0)
		{
			std::cout << "recevie failed with error " << WSAGetLastError() << std::endl;
		}
		else if (iResult == 0)
		{
			std::cout << "Connection closed" << std::endl;
		}
		else
		{
			std::cout << recvbuf << std::endl;
			MyChessBoard->updatePromptText(recvbuf);

			bool bIsGameover = false;//��־��Ϸ�Ƿ����
			bool bCanPlace = flag == -1 ? true : false;//��ǰ�Ƿ��ܰ���
			
			while (!bIsGameover)
			{
				if (bCanPlace)
				{

					MOUSEMSG m;
					m = GetMouseMsg();
					int x_sim;
					int y_sim;
					switch (m.uMsg)
					{
					case WM_LBUTTONDOWN:
					{
						int step = MyChessBoard->getStep();
						x_sim = (int)((double)m.x / step + 0.5);
						y_sim = (int)((double)m.y / step + 0.5);
						
						sprintf_s(sendbuf, "%s", formatString(x_sim, y_sim));
						std::cout << "send:" << sendbuf << std::endl;
						iSendResult = send(ConnectSocket, sendbuf, strlen(sendbuf), 0);
						if (iSendResult == SOCKET_ERROR)
						{
							std::cout << "send chess message failed with error" << WSAGetLastError() << std::endl;
							closesocket(ConnectSocket);
							WSACleanup();
							return 1;
						}
						else
						{
							iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFSIZE, 0);
							recvbuf[iResult] = '\0';
							if (iResult <= 0)
							{
								std::cout << "recevie failed with error " << iResult << std::endl;
								closesocket(ConnectSocket);
								WSACleanup();
								return 1;
							}
							else
							{
								//���յ��������԰ڷŸ����ӵ�ͬ����Ϣ����������

								if (strcmp(recvbuf, "TRUE")==0 || strcmp(recvbuf, "WINNER") == 0)
								{
									std::cout << x_sim << "," << y_sim << std::endl;
									Chess chess(x_sim, y_sim);
									if (MyChessBoard->placeChess(chess, flag))
									{
										bCanPlace = false;
										MyChessBoard->updatePromptText("�ȴ��Է�����");
									}
									if (strcmp(recvbuf, "WINNER") == 0)
									{
										MyChessBoard->updatePromptText("ʤ��");
										Sleep(2000);
										bIsGameover = true;
									}
								}
							}
						}
						break;
					}
					default:
						break;
					}
				}
				//�ȴ�������ȷ���Է����������
				else
				{
					iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFSIZE, 0);
					if (iResult < 0)
					{
						std::cout << "recevie failed with error " << WSAGetLastError() << std::endl;
						closesocket(ConnectSocket);
						WSACleanup();
						return 1;
					}
					else if (iResult == 0)
					{
						std::cout << "Connect closed" << std::endl;
						closesocket(ConnectSocket);
						WSACleanup();
						return 1;
					}
					//������յ����ݣ�����ݽ�����ж��Ǽ������廹����Ϸ����
					else
					{
						recvbuf[iResult] = '\0';
						std::cout << "Receive Bytes:" << iResult << "�Է�������" << recvbuf << std::endl;
						int coor_x;
						int coor_y;
						formatCoordinate(coor_x, coor_y, recvbuf);
						Chess chess(coor_x, coor_y);
						MyChessBoard->placeChess(chess, flag * -1);
						//��֪ͨʧ��
						if(recvbuf[iResult-1] == 'F')
						{
							MyChessBoard->updatePromptText("ʧ��");
							Sleep(2000);
							bIsGameover = true;
						}
						//��������
						else
						{
							MyChessBoard->updatePromptText("���������");
							bCanPlace = true;
						}
					}

				}
				EndBatchDraw();//˫�����ͼ
			}
			getchar();
			closegraph();
		}
		
	}
	
	// cleanup 
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;

}


