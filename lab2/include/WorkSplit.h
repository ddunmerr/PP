#pragma once
#include "blur.h"
#include <vector>

// Вариант 1: горизонтальные полосы.
// Матрица делится на threadCount горизонтальных полос; остаток распределяется
// по одной строке первым потокам, чтобы каждый пиксель обрабатывался ровно раз.
std::vector<std::vector<Rect>> MakeWork(int width, int height, int threadCount);
