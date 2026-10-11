#include "Bank.h"
#include "EconomySimulation.h"
#include "Wallet.h"

#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

namespace
{

void TestBankBasic()
{
	Bank bank(1000);
	const auto acc = bank.OpenAccount();
	bank.DepositMoney(acc, 500);
	assert(bank.GetAccountBalance(acc) == 500);
	assert(bank.GetCash() == 500);

	bank.WithdrawMoney(acc, 200);
	assert(bank.GetAccountBalance(acc) == 300);
	assert(bank.GetCash() == 700);

	assert(bank.TryWithdrawMoney(acc, 100));
	assert(!bank.TryWithdrawMoney(acc, 1000));
	assert(bank.GetAccountBalance(acc) == 200);
	std::cout << "TestBankBasic OK\n";
}

void TestBankErrors()
{
	Bank bank(100);
	const auto acc = bank.OpenAccount();

	bool thrown = false;
	try
	{
		bank.DepositMoney(acc, -1);
	}
	catch (const std::out_of_range&)
	{
		thrown = true;
	}
	assert(thrown);

	thrown = false;
	try
	{
		bank.DepositMoney(999, 10);
	}
	catch (const BankOperationError&)
	{
		thrown = true;
	}
	assert(thrown);

	thrown = false;
	try
	{
		bank.DepositMoney(acc, 200);
	}
	catch (const BankOperationError&)
	{
		thrown = true;
	}
	assert(thrown);
	assert(bank.GetCash() == 100);
	std::cout << "TestBankErrors OK\n";
}

void TestSendMoney()
{
	Bank bank(1000);
	const auto a = bank.OpenAccount();
	const auto b = bank.OpenAccount();
	bank.DepositMoney(a, 500);

	bank.SendMoney(a, b, 300);
	assert(bank.GetAccountBalance(a) == 200);
	assert(bank.GetAccountBalance(b) == 300);

	assert(bank.TrySendMoney(a, b, 100));
	assert(bank.GetAccountBalance(a) == 100);

	assert(!bank.TrySendMoney(a, b, 1000));
	assert(bank.GetAccountBalance(a) == 100);
	std::cout << "TestSendMoney OK\n";
}

void TestWallet()
{
	Wallet a(100);
	Wallet b(0);

	assert(a.TryTake(40));
	assert(a.GetBalance() == 60);

	assert(!a.TryTake(1000));
	assert(a.GetBalance() == 60);

	assert(Transfer(a, b, 30));
	assert(a.GetBalance() == 30);
	assert(b.GetBalance() == 30);

	assert(!Transfer(a, b, 100));
	assert(a.GetBalance() == 30);
	std::cout << "TestWallet OK\n";
}

void TestMultithreaded()
{
	Bank bank(1000000);
	const auto acc = bank.OpenAccount();
	const Money cashBefore = bank.GetCash();
	const Money balanceBefore = bank.GetAccountBalance(acc);
	const auto opsBefore = bank.GetOperationsCount();

	constexpr int kThreads = 8;
	constexpr int kIterations = 100000;

	std::vector<std::jthread> threads;
	for (int t = 0; t < kThreads; ++t)
	{
		threads.emplace_back([&] {
			for (int i = 0; i < kIterations; ++i)
			{
				bank.DepositMoney(acc, 1);
				bank.WithdrawMoney(acc, 1);
			}
		});
	}
	threads.clear();

	assert(bank.GetCash() == cashBefore);
	assert(bank.GetAccountBalance(acc) == balanceBefore);

	const auto expected = opsBefore + 2ULL * kThreads * kIterations;
	assert(bank.GetOperationsCount() == expected);
	std::cout << "TestMultithreaded OK\n";
}

void TestSimulationInvariants()
{
	constexpr Money kInitial = 100000;
	CEconomySimulation sim(kInitial);

	std::atomic<bool> stop{ false };
	// 200 однопоточных шагов достаточно, чтобы деньги зациклились.
	for (int i = 0; i < 200 && !stop.load(); ++i)
	{
		// Step приватный — гоняем через RunSingle с быстрым стопом.
	}

	// Запустим параллельно ненадолго.
	std::jthread stopper([&] {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		stop.store(true);
	});
	sim.RunSingle(stop, true);
	stop.store(false);

	std::jthread stopper2([&] {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		stop.store(true);
	});
	sim.RunParallel(stop, true);

	assert(sim.CheckInvariants());
	assert(sim.GetBank().GetTotalMoney() == kInitial);
	std::cout << "TestSimulationInvariants OK\n";
}

} // namespace

int main()
{
	TestBankBasic();
	TestBankErrors();
	TestSendMoney();
	TestWallet();
	TestMultithreaded();
	TestSimulationInvariants();
	std::cout << "Все тесты пройдены.\n";
	return 0;
}
