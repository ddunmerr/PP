#include "EconomySimulation.h"

#include <Windows.h>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
namespace
{
std::atomic<bool> g_stop{ false };

void OnSignal(int)
{
	g_stop.store(true, std::memory_order_relaxed);
}
} // namespace

int main(int argc, char** argv)
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	std::string mode;
	int seconds = 0;
	bool quiet = false;

	for (int i = 1; i < argc; ++i)
	{
		const std::string arg = argv[i];
		if (arg == "--mode" && i + 1 < argc)
		{
			mode = argv[++i];
		}
		else if (arg == "--seconds" && i + 1 < argc)
		{
			seconds = std::atoi(argv[++i]);
		}
		else if (arg == "--quiet")
		{
			quiet = true;
		}
		else
		{
			std::cerr << USAGE << "\n";
			return 1;
		}
	}

	if ((mode != "single" && mode != "parallel") || seconds <= 0)
	{
		std::cerr << USAGE << "\n";
		return 1;
	}

	std::signal(SIGINT, OnSignal);
	std::signal(SIGTERM, OnSignal);

	constexpr Money kInitialCash = 100000;
	CEconomySimulation sim(kInitialCash);

	const auto start = std::chrono::steady_clock::now();

	std::jthread stopper([&] {
		const auto deadline = start + std::chrono::seconds(seconds);
		while (!g_stop.load(std::memory_order_relaxed) && std::chrono::steady_clock::now() < deadline)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		g_stop.store(true, std::memory_order_relaxed);
	});

	if (mode == "single")
	{
		sim.RunSingle(g_stop, quiet);
	}
	else
	{
		sim.RunParallel(g_stop, quiet);
	}

	stopper.join();

	const auto elapsed = std::chrono::duration<double>(
		std::chrono::steady_clock::now() - start)
							 .count();

	const auto ops = sim.GetBank().GetOperationsCount();
	std::cout << "Операций: " << ops << "\n";
	std::cout << "Операций в секунду: "
			  << static_cast<unsigned long long>(ops / elapsed) << "\n";

	sim.PrintFinalReport();
	return 0;
}
