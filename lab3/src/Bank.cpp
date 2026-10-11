#include "Bank.h"

namespace {
// Проверка, что сумма не отрицательная.
void CheckAmount(Money amount) {
    if (amount < 0) {
        throw std::out_of_range("amount cannot be negative");
    }
}
}  // namespace

Bank::Bank(Money cash) : m_cash(cash) {
    if (cash < 0) {
        throw std::out_of_range(ERROR_INITIAL_CASH_CANT_BE_NEGATIVE);
    }
}

void Bank::ValidateAccountExists(AccountId account) const {
    if (m_accounts.find(account) == m_accounts.end()) {
        throw BankOperationError(ERROR_ACCOUNT_DOESNT_EXIST + std::to_string(account));
    }
}

void Bank::ValidateNotNegative(Money amount, const std::string& paramName) const {
    if (amount < 0) {
        throw std::out_of_range(paramName + ERROR_CANT_BE_NEGATIVE);
    }
}

AccountId Bank::OpenAccount() {
    std::lock_guard lock(m_mutex);
    m_operations.fetch_add(1, std::memory_order_relaxed);

    const AccountId id = m_nextAccountId++;
    m_accounts.emplace(id, 0);
    return id;
}

Money Bank::CloseAccount(AccountId account) {
    std::lock_guard lock(m_mutex);
    m_operations.fetch_add(1, std::memory_order_relaxed);

    const auto it = m_accounts.find(account);
    if (it == m_accounts.end()) {
        throw BankOperationError(ERROR_ACCOUNT_DOESNT_EXIST + std::to_string(account));
    }

    const Money balance = it->second;
    m_accounts.erase(it);
    m_cash += balance;
    return balance;
}

void Bank::DepositMoney(AccountId account, Money amount) {
    std::lock_guard lock(m_mutex);
    m_operations.fetch_add(1, std::memory_order_relaxed);
    DepositLocked(account, amount);
}

void Bank::DepositLocked(AccountId account, Money amount) {
    CheckAmount(amount);

    const auto it = m_accounts.find(account);
    if (it == m_accounts.end()) {
        throw BankOperationError(ERROR_ACCOUNT_DOESNT_EXIST + std::to_string(account));
    }
    if (m_cash < amount) {
        throw BankOperationError(ERROR_NOT_ENOUGH_CASH_ON_CIRCULATION);
    }
    m_cash -= amount;
    it->second += amount;
}

void Bank::WithdrawMoney(AccountId account, Money amount) {
    std::lock_guard lock(m_mutex);
    m_operations.fetch_add(1, std::memory_order_relaxed);
    WithdrawLocked(account, amount);
}

void Bank::WithdrawLocked(AccountId account, Money amount) {
    CheckAmount(amount);

    const auto it = m_accounts.find(account);
    if (it == m_accounts.end()) {
        throw BankOperationError(ERROR_ACCOUNT_DOESNT_EXIST + std::to_string(account));
    }
    if (it->second < amount) {
        throw BankOperationError(ERROR_INSUFFICIENT_FUNDS_ON_ACCOUNT + std::to_string(account));
    }
    it->second -= amount;
    m_cash += amount;
}

bool Bank::TryWithdrawMoney(AccountId account, Money amount) {
    std::lock_guard lock(m_mutex);
    m_operations.fetch_add(1, std::memory_order_relaxed);
    return TryWithdrawLocked(account, amount);
}

bool Bank::TryWithdrawLocked(AccountId account, Money amount) {
    CheckAmount(amount);

    const auto it = m_accounts.find(account);
    if (it == m_accounts.end()) {
        throw BankOperationError(ERROR_ACCOUNT_DOESNT_EXIST + std::to_string(account));
    }
    if (it->second < amount) {
        return false;
    }
    it->second -= amount;
    m_cash += amount;
    return true;
}

void Bank::SendMoney(AccountId src, AccountId dst, Money amount) {
    std::lock_guard lock(m_mutex);
    m_operations.fetch_add(1, std::memory_order_relaxed);

    CheckAmount(amount);
    ValidateAccountExists(src);
    ValidateAccountExists(dst);

    if (m_accounts[src] < amount) {
        throw BankOperationError(ERROR_INSUFFICIENT_FUNDS_ON_ACCOUNT + std::to_string(src));
    }
    m_accounts[src] -= amount;
    m_accounts[dst] += amount;
}

bool Bank::TrySendMoney(AccountId src, AccountId dst, Money amount) {
    std::lock_guard lock(m_mutex);
    m_operations.fetch_add(1, std::memory_order_relaxed);

    CheckAmount(amount);
    ValidateAccountExists(src);
    ValidateAccountExists(dst);

    if (m_accounts[src] < amount) {
        return false;
    }
    m_accounts[src] -= amount;
    m_accounts[dst] += amount;
    return true;
}

Money Bank::GetAccountBalance(AccountId account) const {
    std::lock_guard lock(m_mutex);
    m_operations.fetch_add(1, std::memory_order_relaxed);

    const auto it = m_accounts.find(account);
    if (it == m_accounts.end()) {
        throw BankOperationError(ERROR_ACCOUNT_DOESNT_EXIST + std::to_string(account));
    }
    return it->second;
}

Money Bank::GetCash() const {
    std::lock_guard lock(m_mutex);
    return m_cash;
}

Money Bank::GetTotalMoney() const {
    std::lock_guard lock(m_mutex);
    Money total = m_cash;
    for (const auto& [id, balance] : m_accounts) {
        total += balance;
    }
    return total;
}

unsigned long long Bank::GetOperationsCount() const {
    return m_operations.load(std::memory_order_relaxed);
}
