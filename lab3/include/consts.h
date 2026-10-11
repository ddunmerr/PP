#pragma once

#include <string>

// Имена акторов
const std::string DIRECTOR = "Директор";
const std::string FATHER = "Отец";
const std::string MOTHER = "Мать";
const std::string CHILD = "Ребёнок";
const std::string SELLER = "Продавец";

// Логи Директора
const std::string PAID_SALARY = "выплатил зарплату Отцу: ";
const std::string COULDNT_PAY_SALARY = "не смог выплатить зарплату - недостаточно средств";

// Логи Отца
const std::string SENT_MONEY_TO_MOTHER = "перевёл Матери: ";
const std::string COULDNT_SEND_MONEY_TO_MOTHER = "не смог перевести Матери - недостаточно средств";
const std::string WITHDREW_FOR_ELECTRICITY = "снял на оплату электричества: ";
const std::string WITHDREW_FOR_CHILD = "снял наличные для Ребёнка: ";

// Логи Матери
const std::string BOUGHT_GROCERIES = "купила продукты у Продавца: ";
const std::string COULDNT_BUY_GROCERIES = "не смогла купить продукты - недостаточно средств";

// Логи Ребёнка
const std::string RECEIVED_ALLOWANCE = "получил карманные деньги: ";
const std::string SPENT_CASH_AT_SELLER = "потратил у Продавца: ";
const std::string NO_CASH_TO_SPEND = "нет наличных, чтобы потратить";

// Логи Продавца
const std::string DEPOSITED_CASH = "положил выручку на счёт: ";
const std::string PAID_ELECTRICITY = "оплатил электричество Директору: ";
const std::string COULDNT_PAY_ELECTRICITY = "не смог оплатить электричество - недостаточно средств";

// Ошибки
const std::string ERROR_CANT_DEPOSIT = "не удалось положить наличные - в банке недостаточно средств";
const std::string ERROR_CANT_BE_NEGATIVE = " не может быть отрицательной";
const std::string ERROR_INITIAL_CASH_CANT_BE_NEGATIVE = "Начальная наличность не может быть отрицательной";
const std::string ERROR_ACCOUNT_DOESNT_EXIST = "Счёт не существует ";
const std::string ERROR_INSUFFICIENT_FUNDS_ON_ACCOUNT = "Недостаточно средств на счёте ";
const std::string ERROR_NOT_ENOUGH_CASH_ON_CIRCULATION = "В обороте недостаточно наличных";

// CLI
const std::string USAGE = "usage: task3 --mode <single|parallel> --seconds <N> [--quiet]";
