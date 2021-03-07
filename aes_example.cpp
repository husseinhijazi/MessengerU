

#include "aes_example.h"

class aes_example {



	CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];

	char* generate_key(char* buff, size_t size)
	{
		try {
			for (size_t i = 0; i < size; i += 4) {
				_rdrand32_step(reinterpret_cast<unsigned int*>(&buff[i]));
			}
			return buff;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return nullptr;
	}

	// size must be a 4 bytes align.
public:
	char* generateKey() {
		return generate_key(reinterpret_cast<char*>(key), CryptoPP::AES::DEFAULT_KEYLENGTH);
	}

	void setKey(const unsigned char* theKey, unsigned int size) {
		try {
			for (size_t i = 0; i < size; i++) {
				key[i] = theKey[i];
			}
		}
		catch (std::exception& e) {
			std::cout << "can't set the AES-KEY" << std::endl;
		}
	}


	std::string EncryptWithBuffer(const char* str) {
		// create cipher text
		std::string plaintext;
		std::string ciphertext;
		plaintext = str;
		CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

		CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
		stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
		stfEncryptor.MessageEnd();


		
		return ciphertext;
	}

	std::string DecryptWithBuffer(const char* ciphertext,unsigned int size ) {

		// decrypt
		std::string decryptedtext;
		CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

		CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
		stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext), size );
		stfDecryptor.MessageEnd();


		return decryptedtext;
	}

	aes_example()
	{
		

		// Key and IV setup
		// AES encryption uses a secret key of  a variable length (128-bit, 196-bit or 256-bit).
		// This key is secretly exchanged between two parties before communication begins.
		//



		memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
		memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);
	}
};
