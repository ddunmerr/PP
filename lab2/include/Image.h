#pragma once
#include <cstdint>
#include <vector>

// Простое RGB-изображение. Храним по 3 байта на пиксель в порядке R,G,B.
struct Image
{
	int width = 0;
	int height = 0;
	std::vector<uint8_t> pixels; // size = width * height * 3

	Image() = default;
	Image(int w, int h)
		: width(w)
		, height(h)
		, pixels(static_cast<size_t>(w) * h * 3, 0)
	{
	}

	uint8_t* at(int x, int y)
	{
		return pixels.data() + (static_cast<size_t>(y) * width + x) * 3;
	}
	const uint8_t* at(int x, int y) const
	{
		return pixels.data() + (static_cast<size_t>(y) * width + x) * 3;
	}
};
