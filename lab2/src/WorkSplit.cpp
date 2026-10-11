#include "WorkSplit.h"
#include <algorithm>

// Вариант 1: горизонтальные полосы.
// Разбиение делаем так, чтобы:
//   * покрыть все строки 0..height-1 ровно один раз;
//   * корректно работать, если threadCount > height (часть потоков получит пустой
//     набор прямоугольников — это нормально, они просто ничего не сделают).
std::vector<std::vector<Rect>> MakeWork(int width, int height, int threadCount)
{
	std::vector<std::vector<Rect>> work(threadCount);

	if (threadCount <= 0 || width <= 0 || height <= 0)
		return work;

	// Базовое число строк на поток и остаток, который раздаём по одной строке.
	const int base = height / threadCount;
	const int rem = height % threadCount;

	int y = 0;
	for (int t = 0; t < threadCount; ++t)
	{
		// Первые 'rem' потоков получают на одну строку больше.
		const int rows = base + (t < rem ? 1 : 0);
		if (rows == 0)
		{
			// Поток без работы (например, потоков больше, чем строк).
			continue;
		}
		const int y0 = y;
		const int y1 = y + rows;
		y = y1;
		work[t].push_back(Rect{ 0, y0, width, y1 });
	}
	return work;
}
