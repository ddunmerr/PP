#include "Blur.h"
#include "BmpIO.h"
#include "Image.h"
#include "WorkSplit.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

static void PrintUsage(const char* exe)
{
	std::cerr << "Использование: " << exe
			  << " <input.bmp> <output.bmp> <threads> [radius]\n"
			  << "  threads — целое > 0, radius — целое >= 0 (по умолчанию 1)\n";
}

int main(int argc, char** argv)
{
	if (argc < 4 || argc > 5)
	{
		PrintUsage(argv[0]);
		return 1;
	}

	const std::string inPath = argv[1];
	const std::string outPath = argv[2];

	int threadCount = 0;
	int radius = 1;

	try
	{
		threadCount = std::stoi(argv[3]);
		if (argc == 5)
			radius = std::stoi(argv[4]);
	}
	catch (const std::exception&)
	{
		std::cerr << "Ошибка: threads и radius должны быть целыми числами.\n";
		return 1;
	}

	if (threadCount <= 0)
	{
		std::cerr << "Ошибка: threads должно быть > 0.\n";
		return 1;
	}
	if (radius < 0)
	{
		std::cerr << "Ошибка: radius должен быть >= 0.\n";
		return 1;
	}

	Image src;
	try
	{
		src = LoadBMP(inPath);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Ошибка чтения: " << e.what() << "\n";
		return 1;
	}

	Image dst(src.width, src.height); // пустое изображение того же размера

	auto work = MakeWork(src.width, src.height, threadCount);

	// Замер только обработки: от запуска первого потока до завершения последнего.
	const auto start = std::chrono::steady_clock::now();
	{
		std::vector<std::jthread> threads;
		threads.reserve(threadCount);
		for (int k = 0; k < threadCount; ++k)
		{
			threads.emplace_back(Worker,
				std::cref(src),
				std::ref(dst),
				std::move(work[k]),
				radius);
		}
	} // деструкторы jthread делают join
	const auto finish = std::chrono::steady_clock::now();

	const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

	try
	{
		SaveBMP(outPath, dst);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Ошибка записи: " << e.what() << "\n";
		return 1;
	}

	// Единственная строка в stdout: threads,radius,hardware_concurrency,ms
	std::cout << threadCount << ','
			  << radius << ','
			  << std::thread::hardware_concurrency() << ','
			  << ms << '\n';

	return 0;
}
