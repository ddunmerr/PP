#pragma once
#include "image.h"
#include <string>

// Чтение несжатого BMP (24 или 32 бита, BI_RGB). Бросает std::runtime_error.
Image LoadBMP(const std::string& path);

// Запись 24-битного BMP. Бросает std::runtime_error.
void SaveBMP(const std::string& path, const Image& img);
