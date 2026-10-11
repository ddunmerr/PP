#include "BmpIO.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>

#pragma pack(push, 1)
struct BMPFileHeader
{
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
};
struct BMPInfoHeader
{
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};
#pragma pack(pop)

Image LoadBMP(const std::string& path)
{
	std::ifstream in(path, std::ios::binary);
	if (!in)
		throw std::runtime_error("Не удалось открыть входной файл: " + path);

	BMPFileHeader fh{};
	BMPInfoHeader ih{};
	in.read(reinterpret_cast<char*>(&fh), sizeof(fh));
	in.read(reinterpret_cast<char*>(&ih), sizeof(ih));
	if (!in)
		throw std::runtime_error("Файл слишком короткий или повреждён: " + path);

	if (fh.bfType != 0x4D42)
		throw std::runtime_error("Это не BMP-файл: " + path);
	if (ih.biSize < 40)
		throw std::runtime_error("Неподдерживаемый заголовок BMP");
	if (ih.biCompression != 0)
		throw std::runtime_error("Поддерживается только BI_RGB (без сжатия)");
	if (ih.biBitCount != 24 && ih.biBitCount != 32)
		throw std::runtime_error("Поддерживаются только 24- и 32-битные BMP");

	const int w = ih.biWidth;
	const int h = std::abs(ih.biHeight);
	if (w <= 0 || h <= 0)
		throw std::runtime_error("Некорректные размеры изображения");
	const bool topDown = ih.biHeight < 0;

	Image img(w, h);

	const int bytesPerPixel = ih.biBitCount / 8;
	const int rowSize = ((w * bytesPerPixel + 3) / 4) * 4; // выравнивание строки до 4
	std::vector<uint8_t> row(static_cast<size_t>(rowSize));

	in.seekg(fh.bfOffBits, std::ios::beg);
	if (!in)
		throw std::runtime_error("Не удалось перейти к пиксельным данным");

	for (int y = 0; y < h; ++y)
	{
		in.read(reinterpret_cast<char*>(row.data()), rowSize);
		if (!in)
			throw std::runtime_error("Неожиданный конец файла при чтении пикселей");

		// BMP хранит строки снизу вверх (если biHeight > 0)
		const int dstY = topDown ? y : (h - 1 - y);
		for (int x = 0; x < w; ++x)
		{
			const uint8_t* src = row.data() + static_cast<size_t>(x) * bytesPerPixel;
			// BMP хранит BGR
			uint8_t b = src[0], g = src[1], r = src[2];
			uint8_t* d = img.at(x, dstY);
			d[0] = r;
			d[1] = g;
			d[2] = b;
		}
	}
	return img;
}

void SaveBMP(const std::string& path, const Image& img)
{
	const int w = img.width, h = img.height;
	const int rowSize = ((w * 3 + 3) / 4) * 4;
	const uint32_t pixelDataSize = static_cast<uint32_t>(rowSize) * h;
	const uint32_t fileSize = 14 + 40 + pixelDataSize;

	BMPFileHeader fh{};
	fh.bfType = 0x4D42;
	fh.bfSize = fileSize;
	fh.bfOffBits = 14 + 40;

	BMPInfoHeader ih{};
	ih.biSize = 40;
	ih.biWidth = w;
	ih.biHeight = h;
	ih.biPlanes = 1;
	ih.biBitCount = 24;
	ih.biCompression = 0;
	ih.biSizeImage = pixelDataSize;

	std::ofstream out(path, std::ios::binary);
	if (!out)
		throw std::runtime_error("Не удалось открыть выходной файл: " + path);

	out.write(reinterpret_cast<const char*>(&fh), sizeof(fh));
	out.write(reinterpret_cast<const char*>(&ih), sizeof(ih));

	std::vector<uint8_t> row(static_cast<size_t>(rowSize), 0);
	for (int y = h - 1; y >= 0; --y)
	{ // снизу вверх
		for (int x = 0; x < w; ++x)
		{
			const uint8_t* s = img.at(x, y);
			uint8_t* d = row.data() + static_cast<size_t>(x) * 3;
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0]; // R,G,B -> B,G,R
		}
		out.write(reinterpret_cast<const char*>(row.data()), rowSize);
	}
	if (!out)
		throw std::runtime_error("Ошибка записи в файл: " + path);
}
