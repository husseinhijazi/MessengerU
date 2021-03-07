#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
#include <iostream>
#include <vector>
#include <exception>
#include <algorithm>
#include <sstream>
#include <fstream>

#include <thread>

#include "rsa_example.cpp"
#include "aes_example.cpp"

#define DEFAULTSTRINGSIZE 256
#define VERSION 2
#define REGREQ 100
#define USRSREQ 101
#define PBKREQ 102
#define SMREQ 103
#define GWMREQ 104
#define REGRES "1000"
#define USRSRES "1001"
#define PBKRES "1002"
#define SMRES "1003"
#define GWMRES "1004"
#define SERVERERROR "9000"
#define MSGTYPE1 "1"
#define MSGTYPE2 "2"
#define MSGTYPE3 "3"
#define MSGTYPE4 "4"
#define PROTOCOLSEPARATION '/'
#define PROTOCOLINNERSEPARATION ','
#define SERVERFILENAMEINFO "server.info"
#define MEFILENAMEINFO "me.info"
#define TEMPCLIID "tempCliID.txt"
#define BUFFERSIZE 4096
#define IDSIZE 129




struct MsgData {
	std::string froCli;
	std::string id;
	std::string msgType;
	std::string cSize;
	unsigned char* msgContent;
};


struct UsersData {
	std::string id;
	std::string name;
	std::string pKey;
	aes_example* sKey;
};


struct Title {
	char* ClientID;
	unsigned int Version : 8;
};

struct Header {
	unsigned int Code : 8;
	unsigned int PayLoadSize : 32;
};

struct DATA {
	std::string payLoad;
};

struct Req {
	Title* title;
	Header* header;
	DATA* data;
};



