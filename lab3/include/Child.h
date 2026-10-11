#pragma once

#include "Actor.h"

// Ребёнок. Получает наличные и тратит их у Продавца.
class CChild : public CActor
{
public:
	explicit CChild(Bank& bank);

	// Возвращает сумму, которую нужно перевести Продавцу в этом ходу.
	Money Act(int step, std::atomic<bool>& stop, bool quiet) override;

private:
	Money m_spendingAmount = 10;
};
