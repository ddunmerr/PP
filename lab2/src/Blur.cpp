#include "include/blur.h"

#include <algorithm>

namespace
{

inline int Clamp(int v, int lo, int hi)
{
	if (v < lo)
		return lo;
	if (v > hi)
		return hi;
	return v;
}

} // namespace

void BlurRect(const Image& src, Image& dst, Rect rc, int radius)
{
	const int W = src.width;
	const int H = src.height;

	const int window = 2 * radius + 1;
	const int area = window * window;

	for (int y = rc.y0; y < rc.y1; ++y)
	{
		for (int x = rc.x0; x < rc.x1; ++x)
		{
			int sumR = 0, sumG = 0, sumB = 0;

			for (int dy = -radius; dy <= radius; ++dy)
			{
				const int yy = Clamp(y + dy, 0, H - 1);
				for (int dx = -radius; dx <= radius; ++dx)
				{
					const int xx = Clamp(x + dx, 0, W - 1);
					const std::uint8_t* srcPx = src.Pixel(xx, yy);
					sumR += srcPx[0];
					sumG += srcPx[1];
					sumB += srcPx[2];
				}
			}

			std::uint8_t* dstPx = dst.Pixel(x, y);
			dstPx[0] = static_cast<std::uint8_t>(sumR / area);
			dstPx[1] = static_cast<std::uint8_t>(sumG / area);
			dstPx[2] = static_cast<std::uint8_t>(sumB / area);
		}
	}
}

std::vector<std::vector<Rect>> MakeWork(int width, int height, int threadCount)
{
	std::vector<std::vector<Rect>> work(threadCount);

	for (int i = 0; i < threadCount; ++i)
	{
		// (k*h)/N … ((k+1)*h)/N — без потерь остатка, без наложений.
		int y0 = (i * height) / threadCount;
		int y1 = ((i + 1) * height) / threadCount;

		Rect r{ 0, y0, width, y1 };
		work[i].push_back(r);
	}
	return work;
}

void Worker(const Image& src, Image& dst, std::vector<Rect> rects, int radius)
{
	for (const Rect& rc : rects)
	{
		BlurRect(src, dst, rc, radius);
	}
}
