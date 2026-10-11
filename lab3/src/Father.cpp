#include "Father.h"

CFather::CFather(Bank& bank, AccountId accountId, AccountId motherAccountId,
	AccountId directorAccountId, Money salary)
	: CActor(FATHER, bank, accountId)
	, m_motherAccountId(motherAccountId)
	, m_directorAccountId(directorAccountId)
	, m_salary(salary)
	, m_electricityBill(salary / 4)
	, m_homeAllowance(salary / 3)
	, m_childAllowance(salary / 10)
{
}

Money CFather::Act(int step, std::atomic<bool>& stop, bool quiet)
{
	if (stop.load(std::memory_order_relaxed))
		return 0;

	if (m_bank.TrySendMoney(m_accountId, m_motherAccountId, m_homeAllowance))
	{
		Log(SENT_MONEY_TO_MOTHER + std::to_string(m_homeAllowance), quiet);
	}
	else
	{
		Log(COULDNT_SEND_MONEY_TO_MOTHER, quiet);
	}

	if (m_bank.TrySendMoney(m_accountId, m_directorAccountId, m_electricityBill))
	{
		Log(PAID_ELECTRICITY + std::to_string(m_electricityBill), quiet);
	}
	else
	{
		Log(COULDNT_PAY_ELECTRICITY, quiet);
	}

	// Снимаем наличные для Ребёнка — только через кошелёк.
	Money childCash = m_childAllowance;
	if (m_bank.TryWithdrawMoney(m_accountId, childCash))
	{
		m_wallet->Put(childCash);
		Log(WITHDREW_FOR_CHILD + std::to_string(childCash), quiet);
		return childCash;
	}
	return 0;
}
