

#include "Protocol.h"



class Protocol {

	// Definition of variables and constants

	Req* req;
	rsa_example* rsa;
	std::string SERVERHOST;
	unsigned short PORT;
	std::vector <UsersData*> usersList;

	//it's splits the string to a list of string according to the character that between them

	std::vector<std::string> splitString(const char* data, char ch) {
		std::stringstream test(data);
		std::string segment;
		std::vector<std::string> seglist;

		while (std::getline(test, segment, ch))
		{
			seglist.push_back(segment);

		}
		return seglist;
	}

	//writing from buffer to file

	void stringToFile(const char* fileName, const char* str) {

		std::ofstream out(fileName);
		if (out.good()) {
			out << str;
			out.close();

		}

	}

	//reading the file buffer to a string
	
	std::string fileToString(const char* fileName) {
		std::ifstream in(fileName);
		std::stringstream ss;
		ss << in.rdbuf();
		return ss.str();
	}

	//checks if the file is exist

	bool isFileExist(const char* name) {
		std::ifstream f(name);
		return f.good();
	}

	//the main method that connect to server by sending data and getting data

	char* sendToServer() {
		try {

			//definning the socket,sockaddr,wsadata.... 

			SOCKET sock = INVALID_SOCKET;
			struct sockaddr_in sa;
			WSADATA wsd;
			if (WSAStartup(MAKEWORD(2, 2), &wsd) != NO_ERROR)
			{
				std::cout << "WSAStartup failed! " << std::endl;
			}
			else {

				//accordding to protocol seperate beetwen the data

				char* recvData = new char[BUFFERSIZE];
				std::string buf(req->title->ClientID);
				buf += PROTOCOLSEPARATION;
				buf += std::to_string(req->title->Version);
				buf += PROTOCOLSEPARATION;
				buf += std::to_string(req->header->Code);
				buf += PROTOCOLSEPARATION;
				buf += std::to_string(req->header->PayLoadSize);
				buf += PROTOCOLSEPARATION;
				buf += req->data->payLoad;

				std::string sendBuf(buf);

				//send buffer most be less or equal to BUFFERSIZE

				if (sendBuf.size() > BUFFERSIZE) {
					WSACleanup();
					delete[]recvData;
					std::cout << "Sending Buffer Is Greater Than The System Can Handle" << std::endl;
					return nullptr;
				}

				//create socket

				sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (sock == INVALID_SOCKET) {
					std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
					WSACleanup();
					delete[]recvData;
					return nullptr;
				}

				//filling the socket address by the right port and host to listen

				sa.sin_addr.s_addr = inet_addr(SERVERHOST.c_str());
				sa.sin_port = htons(PORT);
				sa.sin_family = AF_INET;

				//connecting to the server

				int res = connect(sock, (struct sockaddr*)&sa, sizeof(sa));
				if (res == SOCKET_ERROR) {
					closesocket(sock);
					std::cout << "Unable to connect to server: " << WSAGetLastError() << std::endl;
					WSACleanup();
					delete[]recvData;
					return nullptr;
				}

				//sending to server

				res = send(sock, sendBuf.c_str(), sendBuf.size() + 1, 0);
				if (res == SOCKET_ERROR) {
					closesocket(sock);
					std::cout << "send failed : " << WSAGetLastError() << std::endl;
					WSACleanup();
					delete[]recvData;
					return nullptr;
				}
				
				//shutdown the sending 
				
				res = shutdown(sock, SD_SEND);

				if (res == SOCKET_ERROR) {
					std::cout << "shutdown failed: " << WSAGetLastError() << std::endl;
					closesocket(sock);
					WSACleanup();
					delete[]recvData;
					return nullptr;
				}
				auto bytesNum = 0;
				
				//reciving the data from server

				do {
					res = recv(sock, recvData, BUFFERSIZE, 0);
					if (res > 0) {
						bytesNum += res;

					}
					else {//continue to get the while argument and it will stop there
						if (res == 0) {
							continue;
						}
						else {
							std::cout << "recv failed: " << WSAGetLastError() << std::endl;
							closesocket(sock);
							WSACleanup();
							delete[]recvData;
							return nullptr;
						}
					}

				} while (res > 0);

				//checks if the data that recived less or equal to BUFFERSIZE

				if (bytesNum > BUFFERSIZE) {
					closesocket(sock);
					WSACleanup();
					delete[]recvData;
					return nullptr;
				}
				
				//closing the socket and clean up

				closesocket(sock);
				WSACleanup();

				//return the recived data

				return recvData;
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			std::cout << "Server Problem" << std::endl;
		}
		return nullptr;
	}

	//checks if the responsed is ERROR one or not

	bool isErrorResponed(const char* data) {
		try {
			if (data) {
				if (getCode(data).compare(SERVERERROR) == 0) {
					return true;
				}
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return false;
	}

	//getting the user from users list on the ram by id

	UsersData* getUserByID(const char* id) {
		UsersData* user = nullptr;
		try {
			for (unsigned int i = 0; i < usersList.size(); i++) {
				if (usersList.at(i)->id.compare(id) == 0) {
					user = usersList.at(i);
					return user;
				}
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return nullptr;
	}

	//getting the user from users list on the ram by name

	UsersData* getUserByName(const char* name) {
		UsersData* user = nullptr;
		try {
			for (unsigned int i = 0; i < usersList.size(); i++) {
				if (usersList.at(i)->name.compare(name) == 0) {
					user = usersList.at(i);
					return user;
				}
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return nullptr;
	}

	//adding public key to users list on the ram by id

	void addPKeyToUsersList(const char* cliID, const char* pKey) {
		
		try {
			auto ptr = getUserByID(cliID);
			if (ptr != nullptr) {
				ptr->pKey = pKey;
			}
			else {
				std::cout << "can't add Public Key to Users Data!!" << std::endl;
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

	}

	//adding symmetric key to users list on the ram by id

	void addSKeyToUsersList(const char* cliID, aes_example* sKey) {

		try {
			auto ptr = getUserByID(cliID);
			if (ptr != nullptr) {
				ptr->sKey = sKey;
			}
			else {
				std::cout << "can't add Symmetric Key to Users Data!!" << std::endl;
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

	}

	//getting the id from user list by name

	std::string getIdFromUsersList(const char* userName) {
		
		std::string str = "";
		try {
			auto ptr = getUserByName(userName);
			if(ptr != nullptr)
				str = ptr->id;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return str;
	}

	//getting the name from user list by id
	std::string getNameFromUsersList(const char* Id) {
		
		std::string str = "";
		try {
			auto ptr = getUserByID(Id);
			if(ptr != nullptr)
				str = ptr->name;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return str;
	}

	//getting the public key from user list by id

	std::string getPkeyFromUsersList(const char* Id) {
		
		std::string str = "";
		try {
			auto ptr = getUserByID(Id);
			if(ptr != nullptr)
				str = ptr->pKey;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return str;
	}

	//getting the symmetric key from user list by id

	aes_example* getsKeyFromUsersList(const char* Id) {
		
		aes_example* str = nullptr;
		try {
			auto ptr = getUserByID(Id);
			if(ptr != nullptr)
				str = ptr->sKey;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return str;
	}

	//inserting to the users list on the ram a new client

	void insertIntoUsersList(const char* name, const char* id, aes_example* sKey = nullptr , const char* pKey = "") {
		try {
			auto userI = getUserByID(id);
			auto userN = getUserByName(name);
			if (userI == nullptr && userN == nullptr) {
				UsersData* temp = new UsersData;
				temp->id = id;
				temp->name = name;
				temp->pKey = pKey;
				temp->sKey = sKey;
				usersList.push_back(temp);
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	//printting the users list 

	void printUsersList() {
		try {
			if (usersList.empty()) {
				std::cout << "Users List Is Empty!!!! " << std::endl;
			}
			else {
				for (unsigned int i = 0; i < usersList.size(); i++) {
					std::cout << "NAME : " << usersList.at(i)->name << "	ID : " << usersList.at(i)->id << std::endl;
				}
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	//getting the version from data that had recived from server

	std::string getVersion(const char* data) {
		std::string str = "";
		try {

			std::vector <std::string> splitingRespones = splitString(data, PROTOCOLSEPARATION);
			str = splitingRespones.at(0);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return nullptr;
		}
		return str;
	}

	//getting the code from data that had recived from server

	std::string getCode(const char* data) {
		std::string str = "";
		try {

			std::vector <std::string> splitingRespones = splitString(data, PROTOCOLSEPARATION);
			str = splitingRespones.at(1);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return nullptr;
		}
		return str;
	}

	//getting the pay load size from data that had recived from server

	std::string getPayLoadSize(const char* data) {
		std::string str;
		try {

			std::vector <std::string> splitingRespones = splitString(data, PROTOCOLSEPARATION);
			str = splitingRespones.at(2);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return nullptr;
		}
		return str;
	}

	//getting the pay load from data that had recived from server

	char* getPayLoad(const char* data) {
		
		try {
			auto payS = strtoull(getPayLoadSize(data).c_str(), nullptr, 10);
			char* payL = new char[payS+1];
			const char* ptr = data;
			int i = 0;

			while (true) {
				ptr++;
				if (*ptr == PROTOCOLSEPARATION) {
					i++;
				}
				if (i > 2) {
					ptr++;
					break;
				}
			}

			std::copy(ptr, ptr + payS+1, payL);
			return payL;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			
		}
		return nullptr;
	}

	//getting the from client id from data that had recived from server

	std::string getFromClient(const char* data) {
		std::string str;
		try {

			std::vector <std::string> splitingRespones = splitString(data, PROTOCOLINNERSEPARATION);
			str = splitingRespones.at(0);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return nullptr;
		}
		return str;
	}

	//getting the msg id from data that had recived from server

	std::string getMsgID(const char* data) {
		std::string str;
		try {

			std::vector <std::string> splitingRespones = splitString(data, PROTOCOLINNERSEPARATION);
			str = splitingRespones.at(1);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return nullptr;
		}
		return str;
	}

	//getting the msg type from data that had recived from server

	std::string getMsgType(const char* data) {
		std::string str;
		try {

			std::vector <std::string> splitingRespones = splitString(data, PROTOCOLINNERSEPARATION);
			str = splitingRespones.at(2);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return nullptr;
		}
		return str;
	}

	//getting the msg size from data that had recived from server

	std::string getContentSize(const char* data) {
		std::string str;
		try {

			std::vector <std::string> splitingRespones = splitString(data, PROTOCOLINNERSEPARATION);
			str = splitingRespones.at(3);
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			return nullptr;
		}
		return str;
	}

	//getting the msg from data that had recived from server

	unsigned char* getMsgContent(const char* data , unsigned int size) {

		try {
			auto cS = size;
			
			unsigned char* msgC = new unsigned char[cS+1];
			const char* ptr = data;
			int i = 0;
			while (!(*ptr == '\0' && *(ptr + 1) == 'e' && *(ptr + 2) == 'n' && *(ptr + 3) == 'd')) {
				ptr++;
				if (*ptr == PROTOCOLINNERSEPARATION) {
					i++;
				}
				if (i > 3) {
					ptr++;
					break;
				}
			}

			std::copy(ptr, ptr + cS + 1, msgC);
			return msgC;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;

		}
		return nullptr;
	}

	//getting the all msgs from data that had recived from server

	std::vector<MsgData*> getMsgs(const char* data,unsigned int size) {
		
		std::vector<MsgData*> msgs;
			try {
				
				const char* left = data;
				const char* right = data;
				unsigned int i = 0;
				while (i < size + 1 ) {
					
					if (*right == 's' && *(right + 1) == 't' && *(right + 2) == 'a' && *(right + 3) == 'r' && *(right + 4) == 't') {
						right += 5;
						i += 5;
						left = right;
						while (!(*right == '\0' && *(right + 1) == 'e' && *(right + 2) == 'n' && *(right + 3) == 'd')) {
							right++;
							i++;
						}
						char* msg = new char[right-left];
						std::copy(left,right,msg);
						right += 3;
						MsgData* newMsg = new MsgData;
						newMsg->froCli = getFromClient(msg);
						newMsg->id = getMsgID(msg);
						newMsg->msgType = getMsgType(msg);
						newMsg->cSize = getContentSize(msg);
						newMsg->msgContent = getMsgContent(msg,std::stoul(newMsg->cSize));
						msgs.push_back(newMsg);
					}
					right++;
					i++;
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
		return msgs;
	}

	//handling by registration choseen by checking the user name if its valid , if the server success to make the registeration.....

	void _Register() {
		try {
			//checking if the client had registered not to do it again

			if (isFileExist(MEFILENAMEINFO)) {
				std::cerr << "You Have Registered Once You Cant Do It Again!!!!!" << std::endl;
				std::cout << fileToString(MEFILENAMEINFO) << std::endl;
			}
			else {
				//get the user name from std input

				std::string userName;
				std::cout << "Please enter a user name : \n";
				std::getline(std::cin, userName);
				if (userName.size() > DEFAULTSTRINGSIZE || userName.empty()) {
					std::cerr << "incorect user name length!!!!" << std::endl;
				}
				else {

					//gets the public key and the user name and send it to the server 

					std::string pKey(fileToString(PUBKEY2));
					std::string payload = userName + PROTOCOLINNERSEPARATION + pKey;

					req->title->Version = VERSION;
					req->header->Code = REGREQ;
					req->data->payLoad = payload;
					req->header->PayLoadSize = payload.size();

					std::string recvData(sendToServer());

					//check if the server success to register and write to a file the uuid and name

					if (!recvData.empty()) {

						if (isErrorResponed(recvData.c_str()) || getCode(recvData.c_str()).compare(REGRES) != 0) {
							std::cerr << "You Have Registered Once You Cant Do It Again!!!!!" << std::endl;
						}
						else {
							
							std::string fileData(userName);
							std::string cliID(getPayLoad(recvData.c_str()));
							fileData = fileData + "\n" + cliID;

							stringToFile(MEFILENAMEINFO, fileData.c_str());
							
							//update the client id on the ram

							std::size_t length = cliID.copy(req->title->ClientID, cliID.size());
							req->title->ClientID[length] = '\0';

							
							std::cout << "thanks for registering" << std::endl;
						}

					}
					else {
						printErr();
					}
				}
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	//handling by users list request choseen by checking if the server success to make the get the response and getting the data from data base.....

	void _requestForClientsList() {
		try {
			req->title->Version = VERSION;
			req->header->Code = USRSREQ;
			req->data->payLoad = "";
			req->header->PayLoadSize = req->data->payLoad.size();

			std::string recvData(sendToServer());

			if (!recvData.empty()) {

				if (isErrorResponed(recvData.c_str()) || getCode(recvData.c_str()).compare(USRSRES) != 0) {
					printErr();
				}
				else {

					try {
						auto payLoad = getPayLoad(recvData.c_str());
						
						//if there a new client it will be add to the users list on ram

						auto splits = splitString(payLoad, PROTOCOLINNERSEPARATION);
						std::string cliID, Name;
						for (unsigned int i = 0; i < splits.size() - 1; i += 2) {
							cliID = splits.at(i);
							Name = splits.at(i + 1);
							if (getUserByName(Name.c_str()) == nullptr && getUserByID(cliID.c_str()) == nullptr) {
								insertIntoUsersList(Name.c_str(), cliID.c_str());
							}

						}
					}
					catch (std::exception& e) {
						std::cout << e.what() << std::endl;
					}

				}
			}
			else {
				printErr();
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	//handling by public key request choseen by checking if the server success to get data from data base , if there a client by this name.....

	void _requestForPublicKey() {
		try {
			//refreshing the users list by request users from server

			_requestForClientsList();
			std::string userName;
			std::cout << "Enter User Name" << std::endl;
			std::getline(std::cin, userName);
			std::string payload = userName;
			auto user = getUserByName(userName.c_str());

			//checking if the user name is on the ram

			if (user == nullptr || user->pKey.empty()) {
				req->title->Version = VERSION;
				req->header->Code = PBKREQ;
				req->header->PayLoadSize = payload.size();
				req->data->payLoad = payload.c_str();

				std::string recvData(sendToServer());
				
				if (!recvData.empty()) {

					if (isErrorResponed(recvData.c_str()) || getCode(recvData.c_str()).compare(PBKRES) != 0) {
						printErr();
						stringToFile(PUBKEY, "ERROR");
						stringToFile(TEMPCLIID, "ERROR");
					}
					else {
						try {

							//writing the public key and client id on files and update the users list on ram

							payload = getPayLoad(recvData.c_str());
							auto vec = splitString(payload.c_str(), PROTOCOLINNERSEPARATION);
							auto cliID = vec.at(0);
							auto pKey = vec.at(1);
							for (unsigned int i = 2; i < vec.size(); i++) {
								pKey += PROTOCOLINNERSEPARATION + vec.at(i);
							}
							if (user != nullptr) {
								addPKeyToUsersList(cliID.c_str(), pKey.c_str());
							}
							else {
								std::cout << "Try To Request For Clints List" << std::endl;
							}
							stringToFile(PUBKEY, pKey.c_str());
							stringToFile(TEMPCLIID, cliID.c_str());

						}
						catch (std::exception& e) {
							std::cout << e.what() << std::endl;
						}
					}
				}
				else {
					printErr();
					stringToFile(PUBKEY, "ERROR");
					stringToFile(TEMPCLIID, "ERROR");
				}
			}
			else {
				try {

					auto cliID = getIdFromUsersList(userName.c_str());
					auto pKey = getPkeyFromUsersList(cliID.c_str());
					stringToFile(PUBKEY, pKey.c_str());
					stringToFile(TEMPCLIID, cliID.c_str());
				}
				catch (std::exception& e) {
					printErr();
					stringToFile(PUBKEY, "ERROR");
					stringToFile(TEMPCLIID, "ERROR");
				}
			}
		}
		catch (std::exception& e) {
			printErr();
			stringToFile(PUBKEY, "ERROR");
			stringToFile(TEMPCLIID, "ERROR");
		}
	}

	//handling by getting the messages request choseen by checking if the server success to make the get the response and getting the data from data base.....

	void _requestForWaitingMessages() {
		try {
			std::string payload = "";
			req->title->Version = VERSION;
			req->header->Code = GWMREQ;
			req->header->PayLoadSize = payload.size();
			req->data->payLoad = payload.c_str();

			auto recvData = sendToServer();

			if (recvData) {


				if (isErrorResponed(recvData) || getCode(recvData).compare(GWMRES) != 0) {
					printErr();
				}
				else {
					_requestForClientsList();

					try {

						auto payS = strtoul(getPayLoadSize(recvData).c_str(),nullptr,10);
						auto payL = getPayLoad(recvData);
						
						auto msgs = getMsgs(payL,payS);
						
						//make arragment to the msgs from the server and handling according to the protocol msgs type

						if (!msgs.empty()) {
							for (unsigned int i = 0; i < msgs.size(); i++) {
								auto froCli = getUserByID(msgs.at(i)->froCli.c_str());
								if (froCli == nullptr) {
									printErr();
									std::cout << "Try To Request For Clients List" << std::endl;
									
								}
								else {
									auto size = std::stoul(msgs.at(i)->cSize);
									
									auto msgType = std::stoul(msgs.at(i)->msgType);
									auto msgContent = msgs.at(i)->msgContent;
									if (msgType == 1) {
										std::cout << "From : " << froCli->name << std::endl;
										std::cout << "Content : " << rsa->DecryptWithBuffer(msgContent,size) << std::endl;
									}
									else {
										
										if (msgType == 2) {
											auto decrypt = rsa->DecryptWithBuffer(msgContent, size);
											if (froCli->sKey == nullptr)
												froCli->sKey = new aes_example();
											froCli->sKey->setKey((unsigned char*)decrypt.c_str(), decrypt.size());
											std::cout << "From : " << froCli->name << std::endl;
											std::cout << "Content : " << "Gets The Symmetric Key" << std::endl;
										}
										else {
											auto decrypt= froCli->sKey->DecryptWithBuffer((char*)msgContent, size);
											if (msgType == 3) {
												std::cout << "From : " << froCli->name << std::endl;
												std::cout << "Content : " << decrypt << std::endl;
											}
											else {
												if (msgType == 4) {
													auto filePath = froCli->name + "%TMP%.txt";

													stringToFile(filePath.c_str(),decrypt.c_str());
													std::cout << filePath << std::endl;
												}
												else {
													printErr();
												}
											}
										}
									}
								}
							}
						}
						else {
							std::cout << "There Is No Msgs" << std::endl;
						}
					}
					catch (std::exception& e) {
						std::cout << e.what() << std::endl;
					}
				}
			}
			else {
				printErr();
			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	//handling by sending msg request choseen by checking if the server success to get the data and update data base.....
	void _sendAMessage() {
		try {
			//updating the users list on the ram
			//getting the public key and the cliID from the server if they had changed and update on the ram
			_requestForClientsList();
			_requestForPublicKey();

			std::string cliID = fileToString(TEMPCLIID);
			std::string pubKey = fileToString(PUBKEY);
			if (cliID.compare("ERROR") == 0 || pubKey.compare("ERROR") == 0) {
				
			}
			else {
				try {
					//checking if can be encrypt the message

					auto sKey = getsKeyFromUsersList(cliID.c_str());
					if (sKey == nullptr) {
						
						std::cout << "Can't Encrypt The Message!!!!" << std::endl;
					}
					else {
					std::string mType = MSGTYPE3;
					std::string cSize;
					std::string mContent;
					std::cout << "Enter Your Message" << std::endl;
					std::getline(std::cin, mContent);
						mContent = sKey->EncryptWithBuffer(mContent.c_str());
						cSize = std::to_string(mContent.size());
						std::string payload = cliID + PROTOCOLINNERSEPARATION + mType + PROTOCOLINNERSEPARATION + cSize + PROTOCOLINNERSEPARATION + mContent;
						req->title->Version = VERSION;
						req->header->Code = SMREQ;
						req->header->PayLoadSize = payload.size();
						req->data->payLoad = payload;


						std::string recvData(sendToServer());

						if (!recvData.empty()) {


							if (isErrorResponed(recvData.c_str()) || getCode(recvData.c_str()).compare(SMRES) != 0) {
								printErr();
							}
							else {
								std::cout << "Your Msg is sent succesfully" << std::endl;
							}

						}
						else {
							printErr();
						}
					}
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}

			}
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	//handling by sending file request choseen by checking if the server success to get the data and update data base.....

	void _sendAFileMessage() {

		//updating the users list on the ram
		//getting the public key and the cliID from the server if they had changed and update on the ram
		_requestForClientsList();
		_requestForPublicKey();

		std::string cliID = fileToString(TEMPCLIID);
		std::string pubKey = fileToString(PUBKEY);
		if (cliID.compare("ERROR") == 0 || pubKey.compare("ERROR") == 0) {
			
		}
		else {
			try {
				//checking if can be encrypt the message

				auto sKey = getsKeyFromUsersList(cliID.c_str());
				if (sKey == nullptr) {
					
					std::cout << "Can't Encrypt The Message!!!!" << std::endl;
				}
				else {
					std::string mType = MSGTYPE4;
					std::string cSize;
					std::string mContent;
					std::string filePath;
					std::cout << "Enter Your File Path" << std::endl;
					std::getline(std::cin, filePath);

					//checking the filepath is allowed the get it

					if (!isFileExist(filePath.c_str()) || filePath.compare(PRIVKEY) == 0 || filePath.compare(PUBKEY) == 0 || filePath.compare(PUBKEY2) == 0
						|| filePath.compare(SERVERFILENAMEINFO) == 0 || filePath.compare(MEFILENAMEINFO) == 0 || filePath.compare(TEMPCLIID) == 0) {
						std::cout << "file not found" << std::endl;
					}
					else {
						auto fileContent = fileToString(filePath.c_str());
						mContent = sKey->EncryptWithBuffer(fileContent.c_str());
						cSize = std::to_string(mContent.size());

						std::string payload = cliID + PROTOCOLINNERSEPARATION + mType + PROTOCOLINNERSEPARATION + cSize + PROTOCOLINNERSEPARATION + mContent;

						req->title->Version = VERSION;
						req->header->Code = SMREQ;
						req->header->PayLoadSize = payload.size();
						req->data->payLoad = payload;


						std::string recvData(sendToServer());

						if (!recvData.empty()) {


							if (isErrorResponed(recvData.c_str()) || getCode(recvData.c_str()).compare(SMRES) != 0) {
								printErr();
							}

						}
						else {
							printErr();
						}
					}
				}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}

		}

	}

	//handling by sending symmetric key request choseen by checking if the server success to get the data and update data base.....

	void _sendARequestForSymmetricKey() {
		//updating the users list on the ram
		//getting the public key and the cliID from the server if they had changed and update on the ram
		_requestForClientsList();
		_requestForPublicKey();

		std::string cliID = fileToString("tempCliID.txt");
		std::string pubKey = fileToString(PUBKEY);
		if (cliID.compare("ERROR") == 0 || pubKey.compare("ERROR") == 0) {
			printErr();
		}
		else {
			try {

					std::string mType = MSGTYPE1;
					std::string temp = "Request For Symmetric Key";

					//encrypt the msg by RSA reciving msg public key

					std::string mContent = rsa->EncryptWithBuffer(temp.c_str(),temp.size());
					auto cSize = std::to_string(mContent.size());

					std::string payload = cliID + PROTOCOLINNERSEPARATION + mType + PROTOCOLINNERSEPARATION + cSize + PROTOCOLINNERSEPARATION + mContent;

					req->title->Version = VERSION;
					req->header->Code = SMREQ;
					req->header->PayLoadSize = payload.size();
					req->data->payLoad = payload;


					std::string recvData(sendToServer());

					if (!recvData.empty()) {


						if (isErrorResponed(recvData.c_str()) || getCode(recvData.c_str()).compare(SMRES) != 0) {
							printErr();
						}

					}
					else {
						printErr();
					}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}

		}
	}

	//handling by sending symmetric key msg choseen by checking if the server success to get the data and update data base.....

	void _sendYourSummetricKey() {
		//updating the users list on the ram
		//getting the public key and the cliID from the server if they had changed and update on the ram
		_requestForClientsList();
		_requestForPublicKey();
		std::string cliID = fileToString("tempCliID.txt");

		std::string pubKey = fileToString(PUBKEY);
		if (cliID.compare("ERROR") == 0 || pubKey.compare("ERROR") == 0) {
			printErr();
		}
		else {
			try {

					std::string mType = MSGTYPE2;
					std::string cSize;
					std::string mContent;

					//make a new symmetric key and update the users list on ram

					addSKeyToUsersList(cliID.c_str(),new aes_example());
					auto user = getUserByID(cliID.c_str());
					auto sKey = user->sKey->generateKey();

					//encrypt the msg by RSA reciving msg public key

					mContent = rsa->EncryptWithBuffer(sKey,CryptoPP::AES::DEFAULT_KEYLENGTH);
					cSize = std::to_string(mContent.size());

					std::string payload = cliID + PROTOCOLINNERSEPARATION + mType + PROTOCOLINNERSEPARATION + cSize + PROTOCOLINNERSEPARATION + mContent;

					req->title->Version = VERSION;
					req->header->Code = SMREQ;
					req->header->PayLoadSize = payload.size();
					req->data->payLoad = payload;


					std::string recvData(sendToServer());

					if (!recvData.empty()) {


						if (isErrorResponed(recvData.c_str()) || getCode(recvData.c_str()).compare(SMRES) != 0) {
							printErr();
						}

					}
					else {
						printErr();
					}
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}

		}
	}


	void printErr() {
		std::cout << "server responded with an error" << std::endl;
	}

public:

	//constructor

	Protocol(std::string host, unsigned short port) {

		SERVERHOST = host;
		PORT = port;
		
		req = new Req;
		req->title = new Title;
		req->header = new Header;
		req->data = new DATA;

		rsa = new rsa_example();
		req->title->ClientID = new char[IDSIZE];
		memset(req->title->ClientID, 0x00, IDSIZE);
		if (isFileExist(MEFILENAMEINFO)) {
			std::string cliId = fileToString(MEFILENAMEINFO);
			std::string userName;
			try {
				std::vector <std::string> splitingRespones = splitString(cliId.c_str(), '\n');
				cliId = splitingRespones.at(1);
				userName = splitingRespones.at(0);
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}

			std::size_t length = cliId.copy(req->title->ClientID, cliId.size());
			req->title->ClientID[length] = '\0';

			std::cout << "Your User Name Is : " << userName << std::endl;
			std::cout << "Your ID Is : " << req->title->ClientID << std::endl;
			std::cout << "Please Dont Try To Register " << std::endl;
			std::thread t(&Protocol::_requestForClientsList,this);
			t.join();
		}


	}

	//destructor

	~Protocol() {

		delete rsa;
		delete req->title->ClientID;
		req->title->Version = -1;
		delete req->title;
		req->data->payLoad.clear();
		delete req->data;
		req->header->Code = -1;
		req->header->PayLoadSize = -1;
		delete req->header;
		delete req;
		usersList.clear();

	}

	//public methods that the client can use this methods only

	void Register() {
		_Register();

	}

	void requestForClientsList() {
		_requestForClientsList();
		printUsersList();
	}

	void requestForPublicKey() {
		_requestForPublicKey();
	}


	void requestForWaitingMessages() {
		_requestForWaitingMessages();
	}


	void sendAMessage() {
		_sendAMessage();
	}

	void sendAFileMessage() {
		_sendAFileMessage();
	}


	void sendARequestForSymmetricKey() {
		_sendARequestForSymmetricKey();
	}


	void sendYourSummetricKey() {
		_sendYourSummetricKey();
	}

};