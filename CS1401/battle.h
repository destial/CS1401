#pragma once
#ifndef BATTLE_H
#define BATTLE_H

typedef enum PokemonSpecies {
	MISSING_NO,
	CHARMANDER = 4
} PokemonSpecies;

typedef enum TypeInfo {
	T_TYPELESS,
	T_FIRE,
	T_WATER,
	T_GRASS,
	T_ELECTRIC,
	T_ROCK,
	T_GROUND,
	T_FIGHTING,
	T_DARK,
	T_STEEL,
	T_FAIRY,
	T_FLYING,
	T_NORMAL,
	T_DRAGON,
	T_GHOST,
	T_POISON,
	T_PSYCHIC,
	T_ICE,
	T_BUG,
	TOTAL_TYPES,
} PokemonType;

typedef enum MoveType {
	M_STATUS,
	M_PHYSICAL,
	M_SPECIAL
} MoveType;

typedef struct MoveInfo {
	float dmg;
	float accuracy;
	char* name;
	PokemonType type;
	MoveType move_type;
} Move;

typedef struct PokemonInfo {
	int level;
	unsigned char base_hp;
	unsigned char base_attack;
	unsigned char base_defense;
	unsigned char base_spattack;
	unsigned char base_spdefense;
	unsigned char base_speed;

	Move moves[4];
	PokemonType types[2];
	PokemonSpecies species;
} Pokemon;

typedef struct TurnInfo {
	Pokemon* attacker, *defender;
	Move* move_used;
} Turn;

typedef struct BattleStruct {
	Pokemon player, opponent;
	Turn turns[30];
	int turn_length;
} BattleInfo;

void battle_init(void);
void battle_update(void);
void battle_end(void);

#endif