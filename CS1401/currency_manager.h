#pragma once
#ifndef CURRENCY_MANAGER_H
#define CURRENCY_MANAGER_H

#include "cprocessing.h"
#include "utils.h"

extern int coins;

void add_coins(int amount);

void deduct_coins(int amount);

void set_coins(int amount);

int getCoins(void);

#endif // !CURRENCY_MANAGER_H