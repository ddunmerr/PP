#include "Wallet.h"

Wallet::Wallet(Money initial) : m_balance(initial) {
    if (initial < 0) {
        throw std::out_of_range("wallet balance cannot be negative");
    }
}

bool Wallet::TryTake(Money amount) {
    if (amount < 0) {
        throw std::out_of_range("amount cannot be negative");
    }
    std::lock_guard lock(m_mutex);
    if (m_balance < amount) {
        return false;
    }
    m_balance -= amount;
    return true;
}

void Wallet::Put(Money amount) {
    if (amount < 0) {
        throw std::out_of_range("amount cannot be negative");
    }
    std::lock_guard lock(m_mutex);
    m_balance += amount;
}

Money Wallet::GetBalance() const {
    std::lock_guard lock(m_mutex);
    return m_balance;
}

bool Transfer(Wallet& from, Wallet& to, Money amount) {
    if (amount < 0) {
        throw std::out_of_range("amount cannot be negative");
    }
    if (&from == &to) {
        return amount == 0;
    }
    std::scoped_lock lock(from.m_mutex, to.m_mutex);
    if (from.m_balance < amount) {
        return false;
    }
    from.m_balance -= amount;
    to.m_balance += amount;
    return true;
}
