
#include "cprocessing.h"
#include "utils.h"
#include "currency_manager.h"

/*!
@author Ethan Chua (chua.e@digipen.edu)
@date 8 September 2025
@course CSD1401
@brief this file contains functions and code for managing the in-game currency (coins)
*/


// Player's coin amount
int coins = 100;

// Adds coins to the player's total
void add_coins(int amount) {
	coins += amount;
}

// Deducts coins from the player's total
void deduct_coins(int amount) {
	coins -= amount;
	if (coins < 0)
		coins = 0;
}

// Sets the player's coin amount to a specific value
void set_coins(int amount) {
	coins = amount;
	if (coins < 0)
		coins = 0;
}

// Retrieves the current coin amount
int getCoins(void) {
	return coins;
}