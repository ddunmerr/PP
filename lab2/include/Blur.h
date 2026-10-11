#pragma once
#include "Image.h"

// Прямоугольник пикселей [x0, x1) x [y0, y1)
struct Rect
{
	int x0, y0, x1, y1;
};

// Box blur. Читает из src, пишет в dst только пиксели прямоугольника rc.
void BlurRect(const Image& src, Image& dst, Rect rc, int radius);

// Функция для std::jthread
void Worker(const Image& src, Image& dst, std::vector<Rect> rects, int radius);
