#pragma once

#include <modes.h>
#include <aes.h>
#include <filters.h>

#include <iostream>
#include <string>
#include <immintrin.h>	// _rdrand32_step

char* generate_key(char* buff, size_t size);