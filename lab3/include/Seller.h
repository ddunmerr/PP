#pragma once

#include "Actor.h"

// Продавец. Кладёт выручку на счёт и платит за электричество Директору.
class CSeller : public CActor
{
public:
	CSeller(Bank& bank, AccountId accountId, AccountId directorAccountId);

	Money Act(int step, std::atomic<bool>& stop, bool quiet) override;

private:
	AccountId m_directorAccountId;
	Money m_electricityPayment = 150;
};
