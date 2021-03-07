

#include "rsa_example.h"


class rsa_example {

	CryptoPP::AutoSeededRandomPool rng;
	// This example generate key pair (private and public) and then saves them to disk in base64 format
	
	bool isFileExist(const char* name) {
		std::ifstream f(name);
		return f.good();
	}

	void GenKeyPair()
	{
		// InvertibleRSAFunction is used directly only because the private key
		// won't actually be used to perform any cryptographic operation;
		// otherwise, an appropriate typedef'ed type from rsa.h would have been used.
		

		CryptoPP::InvertibleRSAFunction privkey;
		privkey.Initialize(rng, 1024);

		// With the current version of Crypto++, MessageEnd() needs to be called
		// explicitly because Base64Encoder doesn't flush its buffer on destruction.
		CryptoPP::Base64Encoder privkeysink(new CryptoPP::FileSink(PRIVKEY));
		privkey.DEREncode(privkeysink);
		privkeysink.MessageEnd();

		// Suppose we want to store the public key separately,
		// possibly because we will be sending the public key to a third party.
		CryptoPP::RSAFunction pubkey(privkey);

		CryptoPP::Base64Encoder pubkeysink(new CryptoPP::FileSink(PUBKEY));
		pubkey.DEREncode(pubkeysink);
		pubkeysink.MessageEnd();
	}

	// This example loads a private key, and generates it's public counterpart
	void LoadAndGenPublicKey()
	{
		CryptoPP::ByteQueue bytes;
		CryptoPP::FileSource file(PRIVKEY, true, new CryptoPP::Base64Decoder);
		file.TransferTo(bytes);
		bytes.MessageEnd();
		CryptoPP::RSA::PrivateKey privateKey;
		privateKey.Load(bytes);

		CryptoPP::RSAFunction pubkey(privateKey);
		CryptoPP::Base64Encoder pubkeysink(new CryptoPP::FileSink(PUBKEY2));
		pubkey.DEREncode(pubkeysink);
		pubkeysink.MessageEnd();

		

	}

public:

	// this example loads a public key into a buffer. then uses that buffer to encrypt a message
	std::string EncryptWithBuffer(const char* message,unsigned int size)
	{
		// plain text message

		std::string ciphertext;
		std::string plaintext;
		for (unsigned int i = 0; i < size;i++) {
			plaintext.push_back( message[i]);
		}

		//Read public key
		CryptoPP::ByteQueue bytes;
		CryptoPP::FileSource file(PUBKEY, true, new CryptoPP::Base64Decoder);
		file.TransferTo(bytes);
		bytes.MessageEnd();
		CryptoPP::RSA::PublicKey pubKey;
		pubKey.Load(bytes);


		// Save the key to an unsigned char buffer.
		// Note on key size: We use 160 character here becuase our key is 1024 bits in size (the actual public key contains more data then just "the key")
		// In a real world scenario we would have used different key sizes and thus, using a dynamic buffer (vector, or even an std::string)
		static const size_t KEYSIZE = 160;
		CryptoPP::byte buf[KEYSIZE];
		CryptoPP::ArraySink as(buf, KEYSIZE);
		pubKey.Save(as);


		// copy the buffer to a different variable
		CryptoPP::byte buf2[KEYSIZE];
		memcpy(buf2, buf, KEYSIZE);
		CryptoPP::ArraySource as2(buf2, KEYSIZE, true);

		CryptoPP::RSA::PublicKey pubKey2;
		pubKey2.Load(as2);


		// encrypt (using public key)
		
		CryptoPP::RSAES_OAEP_SHA_Encryptor e(pubKey2);
		CryptoPP::StringSource ss(plaintext, true, new CryptoPP::PK_EncryptorFilter(rng, e, new CryptoPP::StringSink(ciphertext)));

		return ciphertext;
	}

	std::string DecryptWithBuffer(const unsigned char* encrypted, unsigned int size)
	{
		//Read private key
		{

			CryptoPP::ByteQueue bytes;
			CryptoPP::FileSource file(PRIVKEY, true, new CryptoPP::Base64Decoder);
			file.TransferTo(bytes);
			bytes.MessageEnd();
			CryptoPP::RSA::PrivateKey privateKey;
			privateKey.Load(bytes);

			// decrypt
			std::string decrypted;
			CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey);
			CryptoPP::StringSource ss(encrypted,size, true, new CryptoPP::PK_DecryptorFilter(rng, d, new CryptoPP::StringSink(decrypted)));
			
			return decrypted;
		}
	}

	rsa_example()
	{
		if (!(isFileExist(PUBKEY)&&isFileExist(PRIVKEY))) {

			GenKeyPair();

			LoadAndGenPublicKey();
		}
	}
};