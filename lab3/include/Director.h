#pragma once

#include "Actor.h"

// Директор электростанции. Платит зарплату Отцу.
class CDirector : public CActor
{
public:
    CDirector(Bank& bank, AccountId accountId, AccountId fatherAccountId, Money salary);

    Money Act(int step, std::atomic<bool>& stop, bool quiet) override;

private:
    AccountId m_fatherAccountId;
    Money m_salary;
};
