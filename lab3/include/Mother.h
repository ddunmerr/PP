#pragma once

#include "Actor.h"

// Мать. Получает наличные от Отца и покупает продукты у Продавца.
class CMother : public CActor
{
public:
	CMother(Bank& bank, AccountId accountId, AccountId sellerAccountId);

	// Возвращает сумму, которую нужно перевести Продавцу в этом ходу.
	Money Act(int step, std::atomic<bool>& stop, bool quiet) override;

private:
	AccountId m_sellerAccountId;
	Money m_groceryBudget = 200;
};
