#include "winsock2.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable :4996)

#define FOREGROUND_WHITE FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED

struct Client {
	Client() {
		name = "";
		IP = "0.0.0.0";
		PORT = 0;
	}
	Client(std::string name, std::string IP, int PORT)
	{
		this->name = name;
		this->IP = IP;
		this->PORT = PORT;
	}
	std::string name = "";
	std::string IP = "0.0.0.0";
	int PORT = 0;
};

enum MyCommandType {
	REG,
	LIST,
	QUIT,
	CLEARLIST,
	STOPSERVER
};

struct ClientSocketData {
	ClientSocketData(SOCKET socket, SOCKADDR_IN sockaddr_in) {
		this->socket = socket;
		this->sockaddr_in = sockaddr_in;
	}
	SOCKET socket;
	SOCKADDR_IN sockaddr_in;
};

DWORD WINAPI ClientHandler(LPVOID);
DWORD WINAPI ConnectionsHandler(LPVOID);
int AddressSelect(int, char**, in_addr* addr);
bool LoadClientListFromFile();
bool SaveClientListToFile();

HANDLE stdHandle;

std::vector<Client> ClientList;
std::vector<SOCKET> Connections;
std::vector<HANDLE> Threads;
bool stopConnecting = false;


int main(int argc, char* argv[])
{
	if(!LoadClientListFromFile())
		std::cout << "Cant open client list file. Is it exist?. I create a new one.\n";

	short _PORT;
	if (argc == 2)
	{
		int index = 0;
		while (argv[1][index] != '\0')
		{
			if (!isdigit(argv[1][index]))
			{
				std::cout << "Something wrong with parameters\n";
				return 1;
			}
			++index;
		}
		_PORT = atoi(argv[1]);
	}
	else
	{
		std::cout << "Something wrong with parameters\n";
		return 1;
	}

	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	
	
	
	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	struct in_addr addr;
	if (AddressSelect(argc, argv, &addr) != 0)
	{
		std::cout << "Something wrong with detecting IP\n";
		return 1;
	}
	// Create a socket.
	SOCKET m_socket;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Bind the socket.
	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr = addr;
	service.sin_port = htons(_PORT);

	if (bind(m_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind() failed.\n");
		closesocket(m_socket);
		return 1;
	}

	// Listen on the socket.
	if (listen(m_socket, 1) == SOCKET_ERROR)
		printf("Error listening on socket.\n");

	HANDLE connThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConnectionsHandler, (LPVOID)m_socket, 0, NULL);

	int menu = 1;
	while (menu > 0)
	{
		std::cin >> menu;
	}


	WSACleanup();
	closesocket(m_socket);
	for (int i = 0; i < Connections.size(); i++)
		closesocket(Connections[i]);
	for (int i = 0; i < Threads.size(); i++)
		WaitForSingleObject(Threads[i],INFINITE);
	stopConnecting = true;
	WaitForSingleObject(connThread, INFINITE);
	SaveClientListToFile();
	ExitProcess(EXIT_SUCCESS);
}

DWORD WINAPI ClientHandler(LPVOID socket)
{
	MyCommandType command;
	ClientSocketData clientSockData = *((ClientSocketData*)socket);
	SetConsoleTextAttribute(stdHandle, FOREGROUND_GREEN);
	std::cout << "Client " << inet_ntoa(clientSockData.sockaddr_in.sin_addr) << "  " << clientSockData.sockaddr_in.sin_port << " connected.------------------------------------------------\n";
	SetConsoleTextAttribute(stdHandle, FOREGROUND_WHITE);
	int size;
	char* buffer;
	int exist, quitRequest = 0;
	Client client;
	while (recv(clientSockData.socket, (char*)&command, sizeof(MyCommandType), 0) > 0)
	{
		switch (command)
		{
		case REG:
		{
			SetConsoleTextAttribute(stdHandle, FOREGROUND_RED | FOREGROUND_GREEN);
			std::cout << "command \"reg ";
			//Size
			recv(clientSockData.socket, (char*)&size, sizeof(size), 0);
			buffer = new char[size + 1];
			//Data
			recv(clientSockData.socket, buffer, size, 0);
			buffer[size] = '\0';
			std::cout << buffer << "\" recieved from client " << inet_ntoa(clientSockData.sockaddr_in.sin_addr) << "  " << clientSockData.sockaddr_in.sin_port << std::endl;
			client = Client(buffer, inet_ntoa(clientSockData.sockaddr_in.sin_addr), clientSockData.sockaddr_in.sin_port);
			exist = 0;
			//Finding client in list
			for (int i = 0; i < ClientList.size(); i++)
			{
				if (ClientList[i].name == client.name)
				{
					exist = 1;
					break;
				}
			}
			if (exist == 0)
			{
				ClientList.push_back(client);
				std::cout << "Client \"" << client.name << "  " << client.IP << "  " << client.PORT << "\" registered\n";
			}
			else
			{
				std::cout << "Client \"" << client.name << "  " << client.IP << "  " << client.PORT << "\" already exist ";
				SetConsoleTextAttribute(stdHandle, FOREGROUND_RED);
				std::cout << "(Not added)\n";
			}
			send(clientSockData.socket, (char*)&exist, sizeof(exist), 0);
			SetConsoleTextAttribute(stdHandle, FOREGROUND_WHITE);
			break;
		}
		case LIST:
		{
			SetConsoleTextAttribute(stdHandle, FOREGROUND_BLUE | FOREGROUND_RED);
			std::cout << "command \"list\" ";
			std::cout << " recieved from client " << inet_ntoa(clientSockData.sockaddr_in.sin_addr) << "  " << clientSockData.sockaddr_in.sin_port << std::endl;
			size = ClientList.size();
			send(clientSockData.socket, (char*)&size, sizeof(size), 0);
			for (int i = 0; i < ClientList.size(); i++)
			{
				//Name
				size = ClientList[i].name.size();
				send(clientSockData.socket, (char*)&size, sizeof(size), 0);
				send(clientSockData.socket, ClientList[i].name.c_str(), size, 0);
				//IP
				size = ClientList[i].IP.size();
				send(clientSockData.socket, (char*)&size, sizeof(size), 0);
				send(clientSockData.socket, ClientList[i].IP.c_str(), size, 0);
				//PORT
				send(clientSockData.socket, (char*)&ClientList[i].PORT, sizeof(int), 0);
			}
			std::cout << "List of clients sended to client " << inet_ntoa(clientSockData.sockaddr_in.sin_addr) << "  " << clientSockData.sockaddr_in.sin_port << std::endl;
			SetConsoleTextAttribute(stdHandle, FOREGROUND_WHITE);
			break;
		}
		case QUIT:
		{
			SetConsoleTextAttribute(stdHandle, FOREGROUND_BLUE | FOREGROUND_GREEN);
			std::cout << "command \"quit\" ";
			std::cout << " recieved from client " << inet_ntoa(clientSockData.sockaddr_in.sin_addr) << "  " << clientSockData.sockaddr_in.sin_port << std::endl;
			send(clientSockData.socket, (char*)&quitRequest, sizeof(quitRequest), 0);
			std::cout << "0 sended to client " << inet_ntoa(clientSockData.sockaddr_in.sin_addr) << "  " << clientSockData.sockaddr_in.sin_port << std::endl;
			SetConsoleTextAttribute(stdHandle, FOREGROUND_WHITE);
			break;
		}
		default:
		{
			std::cout << "Broken command\n";
			break;
		}
		}
		Sleep(100);
		//break;
	}
	Connections.erase(std::remove(Connections.begin(), Connections.end(), clientSockData.socket), Connections.end());
	closesocket(clientSockData.socket);
	SetConsoleTextAttribute(stdHandle, FOREGROUND_RED);
	std::cout << "Client " << inet_ntoa(clientSockData.sockaddr_in.sin_addr) << "  " << clientSockData.sockaddr_in.sin_port << " disconnected.---------------------------------------------\n";
	SetConsoleTextAttribute(stdHandle, FOREGROUND_WHITE);
	ExitThread(EXIT_SUCCESS);
}

DWORD WINAPI ConnectionsHandler(LPVOID sock)
{
	SOCKET m_socket = (SOCKET)sock;
	// Accept connections.
	SOCKET AcceptSocket;
	SOCKADDR_IN client_sockaddr;
	int sss = sizeof(client_sockaddr);
	ZeroMemory(&client_sockaddr, sizeof(client_sockaddr));
	printf("Waiting for a client to connect...\n");
	HANDLE clientThread;
	while (!stopConnecting)
	{
		AcceptSocket = accept(m_socket, (SOCKADDR*)&client_sockaddr, &sss);

		if (AcceptSocket != SOCKET_ERROR)
		{
			Connections.push_back(AcceptSocket);
			ClientSocketData clientSockData = ClientSocketData(AcceptSocket, client_sockaddr);
			clientThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)&clientSockData, 0, NULL);
			Threads.push_back(clientThread);
		}
		
	}
	std::cout << "Connections thread exit\n";
	ExitThread(EXIT_SUCCESS);
}

int AddressSelect(int, char**, in_addr* addr)
{
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		std::cerr << "Error " << WSAGetLastError() << std::endl;
		return 1;
	}
	std::cout << "Host name is " << ac << "." << std::endl;

	struct hostent* phe = gethostbyname(ac);
	if (phe == 0) {
		std::cerr << "Bad host lookup." << std::endl;
		return 1;
	}
	int i = 0;
	struct in_addr tmp;
	for (; phe->h_addr_list[i] != 0; ++i) {
		memcpy(&tmp, phe->h_addr_list[i], sizeof(struct in_addr));
		std::cout << i + 1 << ". Address " << ": " << inet_ntoa(tmp) << std::endl;
	}
	int index = 0;
	if (i > 1)
	{
		std::cout << "Insert number of IP Address to use: ";
		std::cin >> index;
		if (!isdigit(index) && index > i) return 1;
		memcpy(addr, phe->h_addr_list[index - 1], sizeof(struct in_addr));
	}
	else *addr = tmp;
	return 0;
}

bool LoadClientListFromFile()
{
	std::fstream file("ClientList.mybin", std::ios::in | std::ios::binary);
	if (!file.is_open())
		return false;

	Client client;
	ClientList.clear();
	size_t size;
	char* buffer;
	if (file.is_open())
	{
		while (file.read((char*)&size, sizeof(size)))
		{
			//Name
			if (file.fail())return false;
			buffer = new char[size];
			file.read(buffer, size);
			client.name = buffer;
			if (file.fail())return false;
			//IP
			file.read((char*)&size, sizeof(size));
			if (file.fail())return false;
			buffer = new char[size];
			file.read(buffer, size);
			client.IP = buffer;
			//PORT
			file.read((char*)&client.PORT, sizeof(int));
			if (file.fail())return false;

			ClientList.push_back(client);
		}
	}
	else return false;
	return true;
	return true;
}


bool SaveClientListToFile()
{
	std::ofstream file("ClientList.mybin", std::ios::out | std::ios::binary);
	size_t size;
	if (file.is_open())
	{
		for (int i = 0; i < ClientList.size(); i++)
		{
			//Name
			size = ClientList[i].name.length() + 1;
			file.write((char*)&size, sizeof(size));
			file.write((char*)ClientList[i].name.c_str(), size);
			//IP
			size = ClientList[i].IP.length() + 1;
			file.write((char*)&size, sizeof(size));
			file.write((char*)ClientList[i].IP.c_str(), size);
			//PORT
			file.write((char*)&ClientList[i].PORT, sizeof(int));
		}
	}
	else return false;
	file.close();
	return true;
}