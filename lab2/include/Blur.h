#pragma once

#include "image.h"
#include <vector>

// Прямоугольник пикселей: [x0, x1) × [y0, y1)
struct Rect
{
	int x0, y0, x1, y1;
};

// Размывает пиксели rc: читает из src, пишет в dst.
// radius — радиус box blur (окно (2r+1)×(2r+1)).
void BlurRect(const Image& src, Image& dst, Rect rc, int radius);

// Для каждого потока — список его прямоугольников.
// Вариант 1: N горизонтальных полос.
std::vector<std::vector<Rect>> MakeWork(int width, int height, int threadCount);

// Функция потока: прогоняет все свои прямоугольники.
void Worker(const Image& src, Image& dst, std::vector<Rect> rects, int radius);
