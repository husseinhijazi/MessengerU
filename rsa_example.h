#pragma once

#define PUBKEY "pubkey.txt"
#define PUBKEY2 "pubkey2.txt"
#define PRIVKEY "privkey.txt"

#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <files.h>
#include <string>
#include <fstream>

void GenKeyPair();
void LoadAndGenPublicKey();
std::string EncryptWithBuffer(const char* str);
std::string DecryptWithBuffer();

