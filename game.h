#ifndef GAME_H
#define GAME_H

#include "bst.h"

typedef enum { ARMOR, SWORD } ItemType;
typedef enum { PHANTOM, SPIDER, DEMON, GOLEM, COBRA } MonsterType;

typedef struct Item {
    char* name;
    ItemType type;
    int value;
} Item;

typedef struct Monster {
    char* name;
    MonsterType type;
    int hp;
    int maxHp;
    int attack;
} Monster;

typedef struct Room {
    int id;
    int x, y;
    int visited;
    Monster* monster;
    Item* item;
    struct Room* next;
} Room;

typedef struct Player {
    int hp;
    int maxHp;
    int baseAttack;
    BST* bag;
    BST* defeatedMonsters;
    Room* currentRoom;
} Player;

typedef struct {
    Room* rooms;
    Player* player;
    int roomCount;
    int configMaxHp;
    int configBaseAttack;
} GameState;

// Monster functions
void freeMonster(void* data);
int compareMonsters(void* a, void* b);
void printMonster(void* data);

// Item functions
void freeItem(void* data);
int compareItems(void* a, void* b);
void printItem(void* data);

// Game functions
void addRoom(GameState* g);
void initPlayer(GameState* g);
void playMove(GameState* g);
void playFight(GameState* g);
void playPickUp(GameState* g);
void playBag(GameState* g);
void playDefeated(GameState* g);
void playQuit(GameState* g);
void playGame(GameState* g);
void freeGame(GameState* g);

const char* getMonsterType(MonsterType type);
const char* getItemType(ItemType type);

Room* findRoom(GameState* g, int id);
Room* findRoomWithCordsAndDir(GameState* g, int x,int y,int dir);

int gameWon(GameState* g);
void printRoom(GameState* g,Room* room);

#endif
