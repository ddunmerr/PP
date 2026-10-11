#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

std::mutex cout_mutex;

void Worker(int index)
{
	std::lock_guard lock(cout_mutex);
	std::cout << "Поток " << index << " выполняет свою работу\n";
}

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	int threadCount = 0;

	if (argc != 2)
	{
		std::cerr << "Ошибка, для запуска программы используйте: main.exe <количество_потоков>" << std::endl;
		return 1;
	}

	try
	{
		threadCount = std::stoi(argv[1]);
		if (threadCount <= 0)
		{
			std::cerr << "Ошибка, количество потоков должно быть положительным\n";
			return 1;
		}
	}
	catch (const std::invalid_argument&)
	{
		std::cerr << "Ошибка, невалидный аргумент" << std::endl;
		return 1;
	}
	catch (const std::out_of_range&)
	{
		std::cerr << "Ошибка, слишком большое число" << std::endl;
		return 1;
	}
	std::vector<std::jthread> threads;
	threads.reserve(threadCount);
	for (int i = 1; i <= threadCount; ++i)
	{
		threads.emplace_back(Worker, i);
	}
	return 0;
}
