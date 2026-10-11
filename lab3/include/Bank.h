#pragma once

#include "consts.h"

#include <atomic>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

using AccountId = unsigned long long;
using Money = long long;

class BankOperationError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class Bank
{
public:
    explicit Bank(Money cash);

    Bank(const Bank&) = delete;
    Bank& operator=(const Bank&) = delete;

    AccountId OpenAccount();
    Money CloseAccount(AccountId account);

    void DepositMoney(AccountId account, Money amount);      // наличные -> счёт
    void WithdrawMoney(AccountId account, Money amount);     // счёт -> наличные
    bool TryWithdrawMoney(AccountId account, Money amount);  // false, если не хватает

    void SendMoney(AccountId src, AccountId dst, Money amount);
    bool TrySendMoney(AccountId src, AccountId dst, Money amount);

    Money GetAccountBalance(AccountId account) const;
    Money GetCash() const;
    Money GetTotalMoney() const;
    unsigned long long GetOperationsCount() const;

private:
    // Внутренние методы без счётчика — вызываются из уже посчитанных операций.
    void DepositLocked(AccountId account, Money amount);
    void WithdrawLocked(AccountId account, Money amount);
    bool TryWithdrawLocked(AccountId account, Money amount);

    void ValidateAccountExists(AccountId account) const;
    void ValidateNotNegative(Money amount, const std::string& paramName) const;

    // mutable — чтобы захватывать мьютекс и увеличивать счётчик в const-методах.
    mutable std::mutex m_mutex;
    Money m_cash;
    AccountId m_nextAccountId = 1;
    std::unordered_map<AccountId, Money> m_accounts;

    mutable std::atomic<unsigned long long> m_operations{0};
};
