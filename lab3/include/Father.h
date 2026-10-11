#pragma once

#include "Actor.h"
#include "Director.h"

// Отец. Переводит Матери, снимает наличные на электричество и детям.
class CFather : public CActor
{
public:
	CFather(Bank& bank, AccountId accountId, AccountId motherAccountId,
		AccountId directorAccountId, Money salary);
	// Возвращает сумму наличных, которую нужно передать Ребёнку.
	Money Act(int step, std::atomic<bool>& stop, bool quiet) override;

private:
	AccountId m_motherAccountId;
	AccountId m_directorAccountId;
	Money m_salary;
	Money m_electricityBill;
	Money m_homeAllowance;
	Money m_childAllowance;
};
