#include "Child.h"

CChild::CChild(Bank& bank)
	: CActor(CHILD, bank)
{
}

Money CChild::Act(int step, std::atomic<bool>& stop, bool quiet)
{
	if (stop.load(std::memory_order_relaxed))
		return 0;

	if (m_wallet->GetBalance() < m_spendingAmount)
	{
		Log(NO_CASH_TO_SPEND, quiet);
		return 0;
	}
	Log(SPENT_CASH_AT_SELLER + std::to_string(m_spendingAmount), quiet);
	return m_spendingAmount;
}
