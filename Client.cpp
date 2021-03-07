
#include "Client.h"

class Client {

	// Definition of variables and constants

	std::string serverInfo;
	std::string meInfo;
	Protocol* protocol;
	u_short SERVERPORT;
	std::string SERVERHOST;
	
	//the exit method it's make a new file to tell the main function to stop

	void exitClient() {
		try {
			std::cout << "See You Later ..." << std::endl;
			stringToFile(EXIT,"EXIT");
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

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

	//reading the file buffer to a string

	std::string fileToString(const char* fileName) {
		std::ifstream in(fileName);
		if (in.good()) {
			std::stringstream ss;
			ss << in.rdbuf();
			return ss.str();
		}
		return "";
	}

	//writing from buffer to file

	void stringToFile(const char* fileName, const char* str) {

		std::ofstream out(fileName);
		if (out.good()) {
			out << str;
			out.close();

		}
		else {
			std::cout << "Can't Write To File" << std::endl;
		}

	}

	//getting the port to listen on it by the socket

	unsigned long getPort(const char* serverInfo) {
		unsigned long val = 0;
		if (serverInfo) {
			try {
				auto port = splitString(serverInfo, ':').at(1);
				val = std::stoul(port);
			}
			catch (std::exception& e) {
				std::cout << "Can't get The PORT" << std::endl;
			}
		}

		return val;
	}

	//getting the host to listen on it by the socket

	std::string getHost(const char* serverInfo) {
		std::string host = "";
		if (serverInfo) {
			try {
				host = splitString(serverInfo, ':').at(0);
			}
			catch (std::exception& e) {
				std::cout << "Can't get The HOST" << std::endl;
			}
		}

		return host;
	}

	//checks if the port to listen is legal one

	bool isLegalPort() {
		return SERVERPORT < 65536 && SERVERPORT > 0;
	}

	//checks if the host to listen is a local host

	bool isLegalHost() {
		return SERVERHOST == LOCALHOST;
	}

	public:

		//Constructor

		Client() {
		
			serverInfo = fileToString(SERVERFILENAMEINFO);
			SERVERHOST = getHost(serverInfo.c_str());
			SERVERPORT = getPort(serverInfo.c_str());
			if (isLegalHost() && isLegalPort()) {
				protocol = new Protocol(SERVERHOST, SERVERPORT);
			}
			else {
				std::cout << "ERROR in HOST OR PORT VALUE" << std::endl;
				exitClient();
			}
			
		}

		//Destructor

		~Client() {
			serverInfo.clear();
			SERVERHOST.clear();
			SERVERPORT = -1;
			delete protocol;
		}

		//main menu thats get from the client all the choosens

		void runClient() {

			int in;

			std::cout << "MessageU client at your service." << std::endl;
			std::cout << "1) Register" << std::endl;
			std::cout << "2) Request for clients list" << std::endl;
			std::cout << "3) Request for public key" << std::endl;
			std::cout << "4) Request for waiting messages" << std::endl;
			std::cout << "5) Send a text message" << std::endl;
			std::cout << "50) Send a File" << std::endl;
			std::cout << "51) Send a request for symmetric key" << std::endl;
			std::cout << "52) Send your summetric key" << std::endl;
			std::cout << "0) Exit client" << std::endl;

			std::cin >> in;
			std::cin.ignore();

			//joining the threads to prevent the cout to be confused
			//handling the client choosen activity

			switch (in)
			{
			case 1:
				try {
					std::thread t(&Protocol::Register,protocol);
					t.join();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				break;
			case 2:
				try {
					std::thread t(&Protocol::requestForClientsList, protocol);
					t.join();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				break;
			case 3:
				try {
					std::thread t(&Protocol::requestForPublicKey, protocol);
					t.join();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				break;
			case 4:
				try {
					std::thread t(&Protocol::requestForWaitingMessages, protocol);
					t.join();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				break;
			case 5:
				try {
					std::thread t(&Protocol::sendAMessage, protocol);
					t.join();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				break;
			case 50:
				try {
					std::thread t(&Protocol::sendAFileMessage, protocol);
					t.join();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				break;
			case 51:
				try {
					std::thread t(&Protocol::sendARequestForSymmetricKey, protocol);
					t.join();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				break;
			case 52:
				try {
					std::thread t(&Protocol::sendYourSummetricKey, protocol);
					t.join();
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				break;
			case 0:
				exitClient();
				break;
			default:
				std::cout << "Invalid choosen !!!!! , try again" << std::endl;
				break;
			}

		}
};

