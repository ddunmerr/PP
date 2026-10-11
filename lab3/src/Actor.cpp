#include "Actor.h"

CActor::CActor(const std::string& name, Bank& bank)
    : m_name(name)
    , m_bank(bank)
    , m_wallet(std::make_unique<Wallet>(0))
{
}

CActor::CActor(const std::string& name, Bank& bank, AccountId accountId)
    : m_name(name)
    , m_bank(bank)
    , m_accountId(accountId)
    , m_wallet(std::make_unique<Wallet>(0))
{
}

std::string CActor::GetName() const { return m_name; }
AccountId CActor::GetAccountId() const { return m_accountId; }
bool CActor::HasAccount() const { return m_accountId != 0; }

Money CActor::GetCash() const { return m_wallet->GetBalance(); }

Money CActor::GetAccountBalance() const {
    if (HasAccount()) {
        return m_bank.GetAccountBalance(m_accountId);
    }
    return 0;
}

Wallet& CActor::GetWallet() { return *m_wallet; }
const Wallet& CActor::GetWallet() const { return *m_wallet; }

void CActor::Log(const std::string& message, bool quiet) const {
    if (!quiet) {
        std::cout << "[" << m_name << "] " << message << std::endl;
    }
}
