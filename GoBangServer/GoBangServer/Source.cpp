/**
* �����������
*/
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<graphics.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "12000"
#define DEFAULT_BUFSIZE  512
#define MAX_CLIENTNUMBER 2

class Rule {
public:
	Rule();
	//�жϷ����Ƿ�Ϸ�
	bool IsValid(int x, int y, int flag);
	/**
	* �жϵ�ǰ�Ƿ���������
	* param x ���ӵ�x����
	* param y ���ӵ�y����
	* param flag ���ӵ���ɫ 1��ʾ���� 2��ʾ����
	*/
	bool IsGameOver(int x, int y, int flag) const;

	void printChessBoard() const;
private:
	int position[DEFAULT_BUFSIZE][DEFAULT_BUFSIZE];
	int height;//���̳���
	int width;//���̿��
	int lineMaxNumber;//���������ߵ�����15*15
};

Rule::Rule():height(640), width(800), lineMaxNumber(15)
{
	memset(position, 0, sizeof(position));
}

bool Rule::IsValid(int x, int y, int flag) 
{
	if (x >= 1 && x <= lineMaxNumber && y >= 1 && y <= lineMaxNumber)
	{
		if (position[x][y] == 0)
		{
			position[x][y] = flag;
			return true;
		}
	}
	return false;
}
bool Rule::IsGameOver(int x, int y, int flag) const
{
	//�ж������������Ƿ�����������
	for (int i = 1; i <= lineMaxNumber - 4; i++)
	{
		bool horizontalCond = true;
		for (int j = 0; j <= 4; j++)
		{
			horizontalCond &= (position[x][i+j] == flag);
		}
		if (horizontalCond)
		{
			return true;
		}
	}
	//�ж������������Ƿ�����������
	for (int i = 1; i <= lineMaxNumber - 4; i++)
	{
		bool verticalCond = true;
		for (int j = 0; j <= 4; j++)
		{
			verticalCond &= (position[i+j][y] == flag);
		}
		if (verticalCond)
		{
			return true;
		}
	}
	//�ж����Խ���
	int step1 = y - x;
	if (step1>=0)
	{
		//y = x + step1 �� xΪ�Ա���

		for (int i = 1; i <= lineMaxNumber - (step1) - 4; i++)
		{
			bool Condition1 = true;
			for (int j = 0; j <= 4; j++)
			{
				Condition1 &= (position[(i + j)][(i + j) + step1] == flag);
			}
			if (Condition1)
			{
				return true;
			}
		}
	}
	else
	{
		//y = x + step1 �� yΪ�Ա���

		for (int i = 1; i <= lineMaxNumber + step1-4; i++)
		{
			bool Condition2 = true;
			for (int j = 0; j <= 4; j++)
			{
				Condition2 &= (position[(i + j) - step1][i + j] == flag);
			}
			if (Condition2)
			{
				return true;
			}
		}
	}
	//�жϸ��Խ���
	int step = x + y;
	if (step <= 1 + lineMaxNumber)
	{
		//y = -x + step yΪ�Ա���
		for (int i=1; i <= -1 + step - 4; i++)
		{
			bool Condition3 = true;
			for (int j = 0; j <= 4; j++)
			{
				Condition3 &= (position[step - (i + j)][i + j] == flag);
			}
			if (Condition3)
			{
				return true;
			}
		}
	}
	else
	{
		//y = -x + step yΪ�Ա���
		for (int i =step - lineMaxNumber; i<= lineMaxNumber-4; i++)
		{
			bool Condition4 = true;
			for (int j = 0; j <= 4; j++)
			{
				Condition4 &= position[step - (i + j)][i + j];
			}
			if (Condition4)
			{
				return true;
			}
		}
	}

	return false;
}
void Rule::printChessBoard() const
{
	for (int i = 1; i <= lineMaxNumber; i++)
	{
		for (int j = 1; j <= lineMaxNumber; j++)
		{
			if (position[i][j] == 1)
			{
				std::cout << "��" << " ";
			}
			else if (position[i][j] == -1)
			{
				std::cout << "��" << " ";
			}
			else 
			{
				std::cout << "-" << " ";
			}

		}
		std::cout << std::endl;
	}
}
//��ר���ַ���ת��Ϊ����
void formatCoordinate(int& x, int& y, char s[])
{

	if (strlen(s) < 4)
	{
		return;
	}
	x = (s[0] - '0') * 10 + (s[1] - '0');
	y = (s[2] - '0') * 10 + (s[3] - '0');

}


int main(void)
{
	//��ʼ��WinSock
	WSADATA wsadata;
	int iResult;
	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult!=0)
	{
		std::cout << "WSAStartup failed " << iResult << std::endl;
		return 1;
	}

	struct addrinfo* result = nullptr, hints;
	//define properties of the address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout << "getaddrinfo failed with error " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	//Create Socket
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		std::cout << "Create socket failed with error" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	//Bind TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Socket Bind failed with error " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	//Setup Listen
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Socket Listen failed with error " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	//wait for the client
	SOCKET ClientSocket[3]  ={INVALID_SOCKET};

	char sendBuf[DEFAULT_BUFSIZE] = "";
	char receiveBuf[DEFAULT_BUFSIZE] = "";
	int iSendResult;
	int currentClientCount = 0;
	while (true)
	{
		if (currentClientCount < MAX_CLIENTNUMBER)
		{
			std::cout << "�ȴ��ͻ�������...\n" << std::endl;
			SOCKADDR_IN clientAddr;
			int sockaddr_in_size = sizeof(SOCKADDR_IN);
			ClientSocket[currentClientCount] = accept(ListenSocket, (sockaddr*)&clientAddr, &sockaddr_in_size);
			if (ClientSocket[currentClientCount] == INVALID_SOCKET) {
				printf("accept failed: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "accept from" << inet_ntoa(clientAddr.sin_addr) << " connect successfully" << std::endl;
			if (currentClientCount == 0)
			{
				sprintf_s(sendBuf, "%s", "Black");
			}
			else if(currentClientCount == 1)
			{
				sprintf_s(sendBuf, "%s", "White");
			}

			iSendResult = send(ClientSocket[currentClientCount], sendBuf, strlen(sendBuf), 0);
			if (iSendResult == SOCKET_ERROR)
			{
					std::cout << "Send failed with error " << WSAGetLastError() << std::endl;
					closesocket(ClientSocket[currentClientCount]);
			}
			else
			{
				std::cout << "�ͻ���" << currentClientCount+1 << "��" << sendBuf << std::endl;
				currentClientCount++;
			}
			Sleep(200);

		}
		//ƥ�䵽����ʱ��ʼ��Ϸ,
		if (currentClientCount == MAX_CLIENTNUMBER)
		{
			//֪ͨ˫����ʼ��Ϸ��������һ������ʧ������˫���˳���Ϸ
			sprintf_s(sendBuf, "%s", "���Ǻڷ�,���������");
			iSendResult = send(ClientSocket[0], sendBuf, strlen(sendBuf), 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket[0]);
				closesocket(ClientSocket[1]);
				currentClientCount = 0;
			}


			sprintf_s(sendBuf, "%s", "���ǰ׷�����ȴ�");
			iSendResult = send(ClientSocket[1], sendBuf, strlen(sendBuf), 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket[0]);
				closesocket(ClientSocket[1]);
				currentClientCount = 0;
			}

			std::shared_ptr<Rule> ChessRule = std::make_shared<Rule>();
			int flag = -1;
			int chessCount = 0;
			//��˫����ʼ����
			while (true)
			{
				int ClientIndex = flag == -1 ? 0 : 1;

				std::cout << "���ڽ��տͻ���" << ClientIndex+1  << "������" << std::endl;
				iResult = recv(ClientSocket[ClientIndex], receiveBuf, DEFAULT_BUFSIZE, 0);
				if (iResult <= 0)
				{
					std::cout << "receive from client failed" << std::endl;
					closesocket(ClientSocket[0]);
					closesocket(ClientSocket[1]);
					currentClientCount = 0;
					break;
				}
				else
				{


					//���յ��ͻ���1���������Ϣ����֤��������ĺϷ��Բ�֪ͨ�ͻ���2����
					int coor_x;
					int coor_y;
					formatCoordinate(coor_x, coor_y, receiveBuf);
					//����Ϸ���֪ͨ˫����������
					if (ChessRule->IsValid(coor_y, coor_x, flag))
					{
						std::cout << "receive correct message:" << receiveBuf << std::endl;
						chessCount++;
						bool bGameOver = false;
						if (chessCount >= 9)
						{
							bGameOver = ChessRule->IsGameOver(coor_y, coor_x, flag);
							std::cout << "Client" << ClientIndex+1 << ((bGameOver) ? "ʤ��":"δ�ֳ�ʤ��") << std::endl;
						}
						//�����֪ͨ����һ���ͻ���
						if (bGameOver)
						{
							receiveBuf[iResult] = 'F';//�Է�����
							receiveBuf[iResult+1] = '\0';
						}
						else
						{
							receiveBuf[iResult] = 'T';//������δ����
							receiveBuf[iResult+1] = '\0';
						}
						iSendResult = send(ClientSocket[(ClientIndex+1)%2], receiveBuf, strlen(receiveBuf), 0);
						if (iSendResult == SOCKET_ERROR)
						{
							std::cout << "send failed with" << WSAGetLastError() << std::endl;
							closesocket(ClientSocket[0]);
							closesocket(ClientSocket[1]);
							currentClientCount = 0;
							break;
						}
						if (bGameOver)
						{
							sprintf_s(sendBuf, "WINNER");
						}
						else
						{
							sprintf_s(sendBuf, "TRUE");
						}

						iSendResult = send(ClientSocket[ClientIndex], sendBuf,strlen(sendBuf), 0);
						if (iSendResult == SOCKET_ERROR)
						{
							std::cout << "send failed with" << WSAGetLastError() << std::endl;
							closesocket(ClientSocket[0]);
							closesocket(ClientSocket[1]);
							currentClientCount = 0;
							break;
						}
						else
						{
							std::cout << "Client" << ClientIndex + 1 << "����Ϸ�" << std::endl;
						}
						flag = flag * -1;

					}
					//������Ϸ�,��֪ͨ�ͻ���2���µ��
					else
					{
						sprintf_s(sendBuf, "FALSE");
						iSendResult = send(ClientSocket[ClientIndex], sendBuf, strlen(sendBuf), 0);
						if (iSendResult == SOCKET_ERROR)
						{
							std::cout << "send failed with" << WSAGetLastError() << std::endl;
							closesocket(ClientSocket[0]);
							closesocket(ClientSocket[1]);
							currentClientCount = 0;
							break;
						}
					}
				}

			}
		}
	}
	freeaddrinfo(result);
	WSACleanup();
	return 0;
}