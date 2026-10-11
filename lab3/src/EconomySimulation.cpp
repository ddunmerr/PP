#include "EconomySimulation.h"

#include <iostream>
#include <thread>
#include <vector>

CEconomySimulation::CEconomySimulation(Money initialCash)
	: m_bank(initialCash)
	, m_initialTotalMoney(initialCash)
{
	Initialize();
}

void CEconomySimulation::Initialize()
{
	const Money salary = 500;

	m_directorId = m_bank.OpenAccount();
	m_fatherId = m_bank.OpenAccount();
	m_motherId = m_bank.OpenAccount();
	m_sellerId = m_bank.OpenAccount();

	m_director = std::make_unique<CDirector>(m_bank, m_directorId, m_fatherId, salary);
	m_father = std::make_unique<CFather>(m_bank, m_fatherId, m_motherId, m_directorId, salary);
	m_mother = std::make_unique<CMother>(m_bank, m_motherId, m_sellerId);
	m_seller = std::make_unique<CSeller>(m_bank, m_sellerId, m_directorId);
	m_child = std::make_unique<CChild>(m_bank);

	// Все наличные банка — в кошелёк Директора. Теперь m_cash == sum(wallets).
	m_director->GetWallet().Put(m_bank.GetCash());

	// Часть кладём на счёт Директора — через кошелёк, чтобы инвариант держался.
	const Money directorInitial = 4000;
	if (m_director->GetWallet().TryTake(directorInitial))
	{
		m_bank.DepositMoney(m_directorId, directorInitial);
	}
}

void CEconomySimulation::Step(int step, std::atomic<bool>& stop, bool quiet)
{
	if (stop.load(std::memory_order_relaxed))
		return;

	m_director->Act(step, stop, quiet);

	Money childCash = m_father->Act(step, stop, quiet);
	if (childCash > 0)
	{
		Transfer(m_father->GetWallet(), m_child->GetWallet(), childCash);
	}

	Money motherSpent = m_mother->Act(step, stop, quiet);
	if (motherSpent > 0)
	{
		Transfer(m_mother->GetWallet(), m_seller->GetWallet(), motherSpent);
	}

	Money childSpent = m_child->Act(step, stop, quiet);
	if (childSpent > 0)
	{
		Transfer(m_child->GetWallet(), m_seller->GetWallet(), childSpent);
	}

	m_seller->Act(step, stop, quiet);
}

void CEconomySimulation::RunSingle(std::atomic<bool>& stop, bool quiet)
{
	int step = 1;
	while (!stop.load(std::memory_order_relaxed))
	{
		Step(step++, stop, quiet);
	}
}

void CEconomySimulation::RunParallel(std::atomic<bool>& stop, bool quiet)
{
	std::vector<std::jthread> threads;

	threads.emplace_back([&] {
		int s = 1;
		while (!stop.load(std::memory_order_relaxed))
		{
			m_director->Act(s++, stop, quiet);
		}
	});
	threads.emplace_back([&] {
		int s = 1;
		while (!stop.load(std::memory_order_relaxed))
		{
			Money childCash = m_father->Act(s++, stop, quiet);
			if (childCash > 0)
			{
				Transfer(m_father->GetWallet(), m_child->GetWallet(), childCash);
			}
		}
	});
	threads.emplace_back([&] {
		int s = 1;
		while (!stop.load(std::memory_order_relaxed))
		{
			Money spent = m_mother->Act(s++, stop, quiet);
			if (spent > 0)
			{
				Transfer(m_mother->GetWallet(), m_seller->GetWallet(), spent);
			}
		}
	});
	threads.emplace_back([&] {
		int s = 1;
		while (!stop.load(std::memory_order_relaxed))
		{
			Money spent = m_child->Act(s++, stop, quiet);
			if (spent > 0)
			{
				Transfer(m_child->GetWallet(), m_seller->GetWallet(), spent);
			}
		}
	});
	threads.emplace_back([&] {
		int s = 1;
		while (!stop.load(std::memory_order_relaxed))
		{
			m_seller->Act(s++, stop, quiet);
		}
	});
}

Money CEconomySimulation::TotalWallets() const
{
	return m_director->GetCash() + m_father->GetCash() + m_mother->GetCash() + m_child->GetCash() + m_seller->GetCash();
}

bool CEconomySimulation::CheckInvariants() const
{
	const Money wallets = TotalWallets();
	const Money bankCash = m_bank.GetCash();
	const Money accounts = m_bank.GetTotalMoney() - bankCash;

	bool ok = true;
	if (wallets != bankCash)
	{
		std::cerr << "FAIL: сумма кошельков (" << wallets
				  << ") != GetCash() (" << bankCash << ")\n";
		ok = false;
	}
	if (bankCash + accounts != m_initialTotalMoney)
	{
		std::cerr << "FAIL: GetCash() + балансы (" << (bankCash + accounts)
				  << ") != начальная сумма (" << m_initialTotalMoney << ")\n";
		ok = false;
	}
	return ok;
}

void CEconomySimulation::PrintFinalReport() const
{
	std::cout << "\n=== Итог ===\n";

	auto printWithAccount = [](const CActor& actor) {
		std::cout << actor.GetName()
				  << ": счёт=" << actor.GetAccountBalance()
				  << ", наличные=" << actor.GetCash() << "\n";
	};
	auto printWithout = [](const CActor& actor) {
		std::cout << actor.GetName()
				  << ": наличные=" << actor.GetCash() << " (без счёта)\n";
	};

	printWithAccount(*m_father);
	printWithAccount(*m_mother);
	printWithout(*m_child);
	printWithAccount(*m_seller);
	printWithAccount(*m_director);

	const Money bankCash = m_bank.GetCash();
	const Money totalOnAccounts = m_bank.GetTotalMoney() - bankCash;

	std::cout << "\nНаличные в банке: " << bankCash << "\n";
	std::cout << "Всего на счетах: " << totalOnAccounts << "\n";
	std::cout << "Наличные у акторов: " << TotalWallets() << "\n";
	std::cout << "Итого в системе (наличные + счета): "
			  << (bankCash + totalOnAccounts) << "\n";
	std::cout << "Ожидалось: " << m_initialTotalMoney << "\n";

	if (CheckInvariants())
	{
		std::cout << "Инварианты соблюдены.\n";
	}
	else
	{
		std::cout << "Инварианты нарушены!\n";
	}
}
