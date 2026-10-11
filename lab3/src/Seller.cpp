#include "Seller.h"

CSeller::CSeller(Bank& bank, AccountId accountId, AccountId directorAccountId)
	: CActor(SELLER, bank, accountId)
	, m_directorAccountId(directorAccountId)
{
}

Money CSeller::Act(int step, std::atomic<bool>& stop, bool quiet)
{
	if (stop.load(std::memory_order_relaxed))
		return 0;

	// 1. Всю наличность из кошелька кладём на счёт.
	const Money cash = m_wallet->GetBalance();
	if (cash > 0 && m_wallet->TryTake(cash))
	{
		try
		{
			m_bank.DepositMoney(m_accountId, cash);
			Log(DEPOSITED_CASH + std::to_string(cash), quiet);
		}
		catch (const BankOperationError&)
		{
			m_wallet->Put(cash); // вернуть, если не получилось
			Log(ERROR_CANT_DEPOSIT, quiet);
		}
	}

	// 2. Платим за электричество Директору со счёта.
	if (m_bank.TrySendMoney(m_accountId, m_directorAccountId, m_electricityPayment))
	{
		Log(PAID_ELECTRICITY + std::to_string(m_electricityPayment), quiet);
	}
	else
	{
		Log(COULDNT_PAY_ELECTRICITY, quiet);
	}

	return 0;
}
