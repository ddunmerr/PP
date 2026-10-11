#include "Director.h"

CDirector::CDirector(Bank& bank, AccountId accountId, AccountId fatherAccountId, Money salary)
    : CActor(DIRECTOR, bank, accountId)
    , m_fatherAccountId(fatherAccountId)
    , m_salary(salary)
{
}

Money CDirector::Act(int step, std::atomic<bool>& stop, bool quiet) {
    if (stop.load(std::memory_order_relaxed)) return 0;

    if (m_bank.TrySendMoney(m_accountId, m_fatherAccountId, m_salary)) {
        Log(PAID_SALARY + std::to_string(m_salary), quiet);
        return m_salary;
    }
    Log(COULDNT_PAY_SALARY, quiet);
    return 0;
}
