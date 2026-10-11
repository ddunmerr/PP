#pragma once

#include "Bank.h"

#include <mutex>

// Кошелёк — наличные на руках у актора. Защищён своим мьютексом,
// потому что Отец кладёт деньги в кошелёк Ребёнка, а Ребёнок в это же
// время тратит из него.
class Wallet
{
public:
    explicit Wallet(Money initial = 0);

    bool TryTake(Money amount);
    void Put(Money amount);
    Money GetBalance() const;

private:
    mutable std::mutex m_mutex;
    Money m_balance;

    friend bool Transfer(Wallet& from, Wallet& to, Money amount);
};

// Перевод между кошельками. Оба мьютекса берутся одним std::scoped_lock,
// который сам разрешает возможные дедлоки.
bool Transfer(Wallet& from, Wallet& to, Money amount);
