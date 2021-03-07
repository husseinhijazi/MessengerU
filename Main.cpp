
#include "Client.cpp"

//checks if the file is exist

bool isFileExist(const char* name) {
	std::ifstream f(name);
	return f.good();
}

//Main method and main loop

int main() {
	
	if (isFileExist(EXIT)) {
		std::remove(EXIT);
	}

	Client* client = new Client();


	do{
		client->runClient();
	} while (!isFileExist(EXIT));

	delete client;
	

	return 0;
}