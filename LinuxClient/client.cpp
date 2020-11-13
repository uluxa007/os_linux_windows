#include "client.h"
#include <cstring>


std::vector<Client> clientList;
bool isIP(char* buffer);

int StartClient(int argc,char *argv[])
{
    int sock;
    const char *server_IP = "192.168.79.79";
    int server_PORT = 27015;
    if (argc == 3)
    {
        if(!isIP(argv[1]))
        { 
            std::cout << "Something wrong with patameters\n";
            return 1;
        }
        for(int i=0;argv[2][i]!='\0';i++)
        {
            if(!isdigit(argv[2][i]))
            {
                std::cout << "Something wrong with patameters\n";
                return 1;
            }
        }
        

        server_IP = argv[1];
        server_PORT = atoi(argv[2]);
	if(server_PORT>65535)
	{
            std::cout << "Something wrong with patameters\n";
            return 1;
        }
    }
    else
    {
        std::cout << "Something wrong with patameters\n";
        return 1;
    }

    //return 0;

    std::string action2;
    std::string action;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Error: unable init socket\n";
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_PORT);
    addr.sin_addr.s_addr = inet_addr(server_IP);

    std::cout << "Connecting to server " << server_IP << "  " << server_PORT << std::endl;
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        std::cerr << "Error: failed connect to server\n";
        exit(2);
    }
    std::cout << "\x1b[32mConnected\x1b[0m\n";
    std::vector<std::string> commands;
    MyCommandType command;
    std::string str, substring;
    bool error;
    int size, clientList_size;
    int answer;
    while (1)
    {
        std::cout << "--------------------------------\n";
        //system("clear");
        std::cout << "Info:\n"
                  << "reg <username>  - зарегистрировать себя в списке пользователей\n"
                  << "list            - просмотреть список зарегистрированных пользователей\n"
                  << "quit            - отключиться и выйти\n";
        error = false;
        commands.clear();
        std::getline(std::cin, str);
        int pointer;
        char *buffer;
        while ((pointer = str.find(' ')) != std::string::npos)
        {
            substring = str.substr(0, pointer);
            if (substring.length() > 0)
                commands.push_back(substring);
            str.erase(0, pointer + 1);
        }
        if (str.length() > 0)
            commands.push_back(str);

        if (commands.size() > 2)
        {
            std::cout << "\x1b[31mWrong command\x1b[0m\n";
        }
        else if (commands.size() == 1)
        {
            if (commands[0] == "list")
            {
                Client client;
                command = LIST;
                send(sock, (char *)&command, sizeof(MyCommandType), 0);
                std::cout << "\x1b[32mCommand \"list\" sended to server\x1b[0m\n";
                if (recv(sock, NULL, 1, MSG_PEEK | MSG_DONTWAIT) != 0)
                {
                    recv(sock, (char *)&clientList_size, sizeof(int), 0);
                    std::cout << "ClientList size: " << clientList_size << std::endl;
                    clientList.clear();
                    int _PORT;
                    if (clientList_size > 0)
                        std::cout << "ClientList:\n";
                    else
                        std::cout << "ClientList is empty\n";
                    for (int i = 0; i < clientList_size; i++)
                    {
                        //Name
                        recv(sock, (char *)&size, sizeof(size), 0);
                        buffer = new char[size + 1];
                        buffer[size] = '\0';
                        recv(sock, (char *)buffer, size, 0);
                        client.name = buffer;
                        //IP
                        recv(sock, (char *)&size, sizeof(size), 0);
                        buffer = new char[size + 1];
                        buffer[size] = '\0';
                        recv(sock, (char *)buffer, size, 0);
                        client.IP = buffer;
                        //PORT
                        recv(sock, (char *)&_PORT, sizeof(int), 0);
                        client.PORT = _PORT;
                        clientList.push_back(client);
                    }

                    for (int i = 0; i < clientList.size(); i++)
                    {
                        std::cout << clientList[i].name << "  " << clientList[i].IP << "  " << clientList[i].PORT << std::endl;
                    }
                }
                else
                {
                    std::cout << "\x1b[31mConnection lost\x1b[0m\n";
                    break;
                }
            }
            else if (commands[0] == "quit")
            {
                //SEND QUIT
                command = QUIT;
                send(sock, (char *)&command, sizeof(MyCommandType), 0);
                std::cout << "\x1b[32mCommand \"quit\" sended to server\x1b[0m\n";
                if (recv(sock, NULL, 1, MSG_PEEK | MSG_DONTWAIT) != 0)
                {
                    recv(sock, (char *)&answer, sizeof(answer), 0);
                    if (answer == 0)
                    {
                        std::cout << "\x1b[32m'0' received from server. Disconnecting...\x1b[0m\n";
                        break;
                    }
                }
                else
                {
                    std::cout << "\x1b[31mConnection lost\x1b[0m\n";
                    break;
                }
            }
            /*
            else if (commands[0] == "clearlist")
            {
                command = CLEARLIST;
                send(sock, (char *)&command, sizeof(MyCommandType), 0);
                std::cout << "\x1b[32mCommand \"clearlist\" sended to server\x1b[0m\n";
                if (recv(sock, NULL, 1, MSG_PEEK | MSG_DONTWAIT) != 0)
                {
                    recv(sock, (char *)&answer, sizeof(answer), 0);
                    if (answer == 0)
                    {
                        std::cout << "\x1b[32m'0' received from server. List cleaned\x1b[0m\n";
                        break;
                    }
                }
                else
                {
                    std::cout << "\x1b[31mConnection lost\x1b[0m\n";
                    break;
                }
            }*/
            else
            {
                std::cout << "\x1b[31mWrong command\x1b[0m\n";
            }
        }
        else if (commands.size() == 2)
        {
            if (commands[0] == "reg")
            {
                command = REG;
                send(sock, (char *)&command, sizeof(MyCommandType), 0);
                if (recv(sock, NULL, 1, MSG_PEEK | MSG_DONTWAIT) != 0)
                {

                    size = commands[1].size();
                    send(sock, (char *)&size, sizeof(size), 0);
                    send(sock, commands[1].c_str(), size, 0);
                    std::cout << "\x1b[32mCommand \"reg " << commands[1] << "\" sended to server\x1b[0m\n";
                    recv(sock, (char *)&answer, sizeof(answer), 0);
                    if (answer == 1)
                        std::cout << "Client with this name already exist;\n";
                    else
                        std::cout << "Client added\n";
                }
                else
                {
                    std::cout << "\x1b[31mConnection lost\x1b[0m\n";
                    break;
                }
            }
            else
            {
                std::cout << "\x1b[31mWrong command\x1b[0m\n";
            }
        }
    }
    std::cout << "\x1b[32mDisconnected\x1b[0m\n";

    return 0;
}

bool isIP(char* buffer)
{
	int IP[4];
	int CountIp = 0;
	char* NotInt;
	int current;


	char* b = buffer;
	bool isIp = true;

	while (strstr(b, "."))
	{
		char* k = new char[100];
		memset(k, 0, 100);

		strncpy(k, b, strlen(b) - strlen(strstr(b, ".")));
		b = strstr(b, ".");
		b++;

		current = strtol(k, &NotInt, 10);
		if (!*NotInt)
		{
			if ((current >= 0 && current < 256) && CountIp < 3)
			{
				IP[CountIp] = current;
				CountIp++;
			}
			else isIp = false;
		}
		else isIp = false;

		delete[] k;

		if (!isIp) break;
	}

	current = strtol(b, &NotInt, 10);
	if (!*NotInt && isIp)
	{
		if (current >= 0 && current < 256)
		{
			IP[CountIp] = current;
		}
		else isIp = false;
	}
	else isIp = false;

	return isIp;
}