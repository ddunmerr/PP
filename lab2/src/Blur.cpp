#include "Blur.h"
#include <algorithm>

// За границей картинки берём ближайший пиксель (clamp).
static inline int ClampI(int value, int lo, int hi)
{
	if (value < lo)
		return lo;
	if (value > hi)
		return hi;
	return value;
}

// Box blur радиусом r: значение пикселя — среднее по квадрату (2r+1)x(2r+1).
// Для простоты и корректности используем прямой проход (O(N * (2r+1)^2)).
void BlurRect(const Image& src, Image& dst, Rect rect, int radius)
{
	const int W = src.width;
	const int H = src.height;

	// Ограничиваем прямоугольник размерами картинки
	rect.x0 = std::clamp(rect.x0, 0, W);
	rect.x1 = std::clamp(rect.x1, 0, W);
	rect.y0 = std::clamp(rect.y0, 0, H);
	rect.y1 = std::clamp(rect.y1, 0, H);

	if (rect.x0 >= rect.x1 || rect.y0 >= rect.y1)
		return;

	const long long area = static_cast<long long>(2 * radius + 1) * (2 * radius + 1);

	for (int y = rect.y0; y < rect.y1; ++y)
	{
		for (int x = rect.x0; x < rect.x1; ++x)
		{
			long long sumR = 0, sumG = 0, sumB = 0;
			for (int dy = -radius; dy <= radius; ++dy)
			{
				const int sy = ClampI(y + dy, 0, H - 1);
				const uint8_t* row = src.at(0, sy);
				for (int dx = -radius; dx <= radius; ++dx)
				{
					const int sx = ClampI(x + dx, 0, W - 1);
					const uint8_t* p = row + static_cast<size_t>(sx) * 3;
					sumR += p[0];
					sumG += p[1];
					sumB += p[2];
				}
			}
			uint8_t* out = dst.at(x, y);
			out[0] = static_cast<uint8_t>(sumR / area);
			out[1] = static_cast<uint8_t>(sumG / area);
			out[2] = static_cast<uint8_t>(sumB / area);
		}
	}
}

void Worker(const Image& src, Image& dst, std::vector<Rect> rects, int radius)
{
	for (const Rect& r : rects)
	{
		BlurRect(src, dst, r, radius);
	}
}
