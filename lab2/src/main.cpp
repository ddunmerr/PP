#include "include/blur.h"
#include "include/image.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

namespace
{

void Usage(const char* argv0)
{
	std::fprintf(stderr,
		"usage: %s <input.bmp> <output.bmp> <threads> [radius]\n"
		"  threads : 1..1024\n"
		"  radius  : 0..1024 (default 1)\n",
		argv0);
}

bool ParseInt(const char* s, int lo, int hi, int& out)
{
	if (!s || !*s)
		return false;
	char* end = nullptr;
	errno = 0;
	long v = std::strtol(s, &end, 10);
	if (errno != 0 || *end != '\0')
		return false;
	if (v < lo || v > hi)
		return false;
	out = static_cast<int>(v);
	return true;
}

} // namespace

int main(int argc, char** argv)
{
	if (argc < 4 || argc > 5)
	{
		Usage(argv[0]);
		return 1;
	}

	int threadCount = 0;
	if (!ParseInt(argv[3], 1, 1024, threadCount))
	{
		std::fprintf(stderr, "error: invalid <threads> '%s' (expected 1..1024)\n", argv[3]);
		return 1;
	}

	int radius = 1;
	if (argc == 5)
	{
		if (!ParseInt(argv[4], 0, 1024, radius))
		{
			std::fprintf(stderr, "error: invalid [radius] '%s' (expected 0..1024)\n", argv[4]);
			return 1;
		}
	}

	Image src;
	std::string err;
	if (!LoadImage(argv[1], src, err))
	{
		std::fprintf(stderr, "error: %s\n", err.c_str());
		return 1;
	}

	Image dst = MakeEmpty(src.width, src.height);
	auto work = MakeWork(src.width, src.height, threadCount);

	// Замер: только обработка. Чтение и запись файла — вне замера.
	const auto t0 = std::chrono::steady_clock::now();
	{
		std::vector<std::jthread> threads;
		threads.reserve(threadCount);
		for (int i = 0; k < threadCount; ++i)
		{
			threads.emplace_back(
				Worker,
				std::cref(src),
				std::ref(dst),
				std::move(work[i]),
				radius);
		}
	}
	const auto t1 = std::chrono::steady_clock::now();

	const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

	// Одна CSV-строка в stdout. Всё остальное — в stderr.
	std::printf("%d,%d,%u,%lld\n",
		threadCount,
		radius,
		std::thread::hardware_concurrency(),
		static_cast<long long>(ms));

	if (!SaveImage(argv[2], dst, err))
	{
		std::fprintf(stderr, "error: %s\n", err.c_str());
		return 1;
	}

	return 0;
}
