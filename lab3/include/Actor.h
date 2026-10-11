#pragma once

#include "Bank.h"
#include "Wallet.h"
#include "consts.h"

#include <atomic>
#include <iostream>
#include <memory>
#include <string>

class CActor
{
public:
    CActor(const std::string& name, Bank& bank);
    CActor(const std::string& name, Bank& bank, AccountId accountId);
    virtual ~CActor() = default;

    // Один ход. stop — общий флаг остановки, quiet — режим без печати.
    virtual Money Act(int step, std::atomic<bool>& stop, bool quiet) = 0;

    [[nodiscard]] std::string GetName() const;
    [[nodiscard]] AccountId GetAccountId() const;
    [[nodiscard]] bool HasAccount() const;
    [[nodiscard]] Money GetCash() const;
    [[nodiscard]] Money GetAccountBalance() const;
    [[nodiscard]] Wallet& GetWallet();
    [[nodiscard]] const Wallet& GetWallet() const;

    void Log(const std::string& message, bool quiet) const;

protected:
    std::string m_name;
    Bank& m_bank;
    AccountId m_accountId = 0;
    std::unique_ptr<Wallet> m_wallet;
};
