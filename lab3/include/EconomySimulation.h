#pragma once

#include "Bank.h"
#include "Child.h"
#include "Director.h"
#include "Father.h"
#include "Mother.h"
#include "Seller.h"
#include "Wallet.h"
#include "consts.h"

#include <atomic>
#include <memory>
#include <string>

class CEconomySimulation
{
public:
	CEconomySimulation(Money initialCash);
	~CEconomySimulation() = default;

	CEconomySimulation(const CEconomySimulation&) = delete;
	CEconomySimulation& operator=(const CEconomySimulation&) = delete;

	void RunSingle(std::atomic<bool>& stop, bool quiet);
	void RunParallel(std::atomic<bool>& stop, bool quiet);

	Bank& GetBank() { return m_bank; }
	Money InitialTotal() const { return m_initialTotalMoney; }
	Money TotalWallets() const;

	bool CheckInvariants() const;
	void PrintFinalReport() const;

private:
	void Initialize();
	void Step(int step, std::atomic<bool>& stop, bool quiet);

	Bank m_bank;
	Money m_initialTotalMoney;

	AccountId m_directorId = 0;
	AccountId m_fatherId = 0;
	AccountId m_motherId = 0;
	AccountId m_sellerId = 0;

	std::unique_ptr<CDirector> m_director;
	std::unique_ptr<CFather> m_father;
	std::unique_ptr<CMother> m_mother;
	std::unique_ptr<CSeller> m_seller;
	std::unique_ptr<CChild> m_child;
};
