#include "Mother.h"

CMother::CMother(Bank& bank, AccountId accountId, AccountId sellerAccountId)
	: CActor(MOTHER, bank, accountId)
	, m_sellerAccountId(sellerAccountId)
{
}

Money CMother::Act(int step, std::atomic<bool>& stop, bool quiet)
{
	if (stop.load(std::memory_order_relaxed))
		return 0;

	if (m_wallet->GetBalance() < m_groceryBudget)
	{
		const Money need = m_groceryBudget - m_wallet->GetBalance();
		if (m_bank.TryWithdrawMoney(m_accountId, need))
		{
			m_wallet->Put(need);
		}
	}

	if (m_wallet->GetBalance() < m_groceryBudget)
	{
		Log(COULDNT_BUY_GROCERIES, quiet);
		return 0;
	}
	Log(BOUGHT_GROCERIES + std::to_string(m_groceryBudget), quiet);
	return m_groceryBudget;
}
