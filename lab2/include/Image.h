#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Простое RGB-изображение. Хранится как width*height*3 байт, RGB, без padding.
struct Image
{
	int width = 0;
	int height = 0;
	std::vector<std::uint8_t> data; // size == width * height * 3

	std::uint8_t* Pixel(int x, int y);
	const std::uint8_t* Pixel(int x, int y) const;
};

// Возвращает false и пишет причину в err. Не бросает.
bool LoadImage(const std::string& path, Image& out, std::string& err);

// Возвращает false и пишет причину в err. Не бросает.
bool SaveImage(const std::string& path, const Image& img, std::string& err);

// Пустое изображение того же размера (нули).
Image MakeEmpty(int width, int height);
