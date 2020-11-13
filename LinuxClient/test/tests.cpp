#include "../client.h"
#include "gtest/gtest.h"
#include <cstring>

struct ArgumentTestCase : public testing::Test{
	int size;
	std::string APP_NAME;
	std::string IP;
	std::string PORT;
	char** arguments;
};

TEST_F(ArgumentTestCase, Wrong_Ip_Wrong_Port){
	size = 3;
	APP_NAME = "Client";
	IP = "WrongIP";
	PORT = "WrongPORT";
	arguments = new char*[size];
	//Arg 0
	arguments[0] = new char[APP_NAME.size()];
	std::strcpy(arguments[0],APP_NAME.c_str());
	//IP
	arguments[1] = new char[IP.size()];
	std::strcpy(arguments[1],IP.c_str());
	//PORT
	arguments[2] = new char[PORT.size()];
	std::strcpy(arguments[2],PORT.c_str());
	
	std::cout << "Input Parameters:  " << arguments[1] << " " << arguments[2] << std::endl;

	ASSERT_EQ(1,StartClient(size,arguments));
}

TEST_F(ArgumentTestCase, Wrong_Ip_Good_Port){
	size = 3;
	APP_NAME = "Client";
	IP = "WrongIP";
	PORT = "27015";
	arguments = new char*[size];
	//Arg 0
	arguments[0] = new char[APP_NAME.size()];
	std::strcpy(arguments[0],APP_NAME.c_str());
	//IP
	arguments[1] = new char[IP.size()];
	std::strcpy(arguments[1],IP.c_str());
	//PORT
	arguments[2] = new char[PORT.size()];
	std::strcpy(arguments[2],PORT.c_str());
	
	std::cout  << "Input Parameters: " << arguments[1] << " " << arguments[2] << std::endl;

	ASSERT_EQ(1,StartClient(size,arguments));
}

TEST_F(ArgumentTestCase, Good_Ip_Wrong_Port){
	size = 3;
	APP_NAME = "Client";
	IP = "192.168.0.0";
	PORT = "WrongPORT";
	arguments = new char*[size];
	//Arg 0
	arguments[0] = new char[APP_NAME.size()];
	std::strcpy(arguments[0],APP_NAME.c_str());
	//IP
	arguments[1] = new char[IP.size()];
	std::strcpy(arguments[1],IP.c_str());
	//PORT
	arguments[2] = new char[PORT.size()];
	std::strcpy(arguments[2],PORT.c_str());
	
	std::cout << "Input Parameters: " << arguments[1] << " " << arguments[2] << std::endl;
	ASSERT_EQ(1,StartClient(size,arguments));
}

TEST_F(ArgumentTestCase, Only_One_Parameter){
	size = 2;
	APP_NAME = "Client";
	IP = "192.168.0.0";
	arguments = new char*[size];
	//Arg 0
	arguments[0] = new char[APP_NAME.size()];
	std::strcpy(arguments[0],APP_NAME.c_str());
	//IP
	arguments[1] = new char[IP.size()];
	std::strcpy(arguments[1],IP.c_str());
	
	std::cout << "Input Parameters: " << arguments[1] << std::endl;
	ASSERT_EQ(1,StartClient(size,arguments));
}

TEST_F(ArgumentTestCase, No_Parameters){
	size = 1;
	APP_NAME = "Client";
	arguments = new char*[size];
	//Arg 0
	arguments[0] = new char[APP_NAME.size()];
	std::strcpy(arguments[0],APP_NAME.c_str());
	
	ASSERT_EQ(1,StartClient(size,arguments));
}

TEST_F(ArgumentTestCase, Good_Ip_TooLarge_Port){
	size = 3;
	APP_NAME = "Client";
	IP = "192.168.0.0";
	PORT = "65536";
	arguments = new char*[size];
	//Arg 0
	arguments[0] = new char[APP_NAME.size()];
	std::strcpy(arguments[0],APP_NAME.c_str());
	//IP
	arguments[1] = new char[IP.size()];
	std::strcpy(arguments[1],IP.c_str());
	//PORT
	arguments[2] = new char[PORT.size()];
	std::strcpy(arguments[2],PORT.c_str());
	
	std::cout << "Input Parameters: " << arguments[1] << " " << arguments[2] << std::endl;
	ASSERT_EQ(1,StartClient(size,arguments));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
