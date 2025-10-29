
#include "cprocessing.h"
#include "utils.h"

#include "currency_manager.h"

static int coins = 0;

void add_coins(int amount) {
	coins += amount;
}

void deduct_coins(int amount) {
	coins -= amount;
	if (coins < 0)
		coins = 0;
}

void set_coins(int amount) {
	coins = amount;
	if (coins < 0)
		coins = 0;
}