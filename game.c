#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "utils.h"

// Map display functions
static void displayMap(GameState* g) {
    if (!g->rooms) return;
    
    // Find bounds
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->x < minX) minX = r->x;
        if (r->x > maxX) maxX = r->x;
        if (r->y < minY) minY = r->y;
        if (r->y > maxY) maxY = r->y;
    }
    
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    
    // Create grid
    int** grid = malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        grid[i] = malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) grid[i][j] = -1;
    }
    
    for (Room* r = g->rooms; r; r = r->next)
        grid[r->y - minY][r->x - minX] = r->id;
    
    printf("=== SPATIAL MAP ===\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] != -1) printf("[%2d]", grid[i][j]);
            else printf("    ");
        }
        printf("\n");
    }
    
    for (int i = 0; i < height; i++) free(grid[i]);
    free(grid);
}

static void displayRoomLegendRec(Room* head){
     if(head == NULL){
        return;
     }
     displayRoomLegendRec(head->next);
     char m = head->monster!= NULL ? 'V' : 'X'; 
     char i = head->item!= NULL ? 'V' : 'X'; 
     printf("ID %d: [M:%c] [I:%c]\n", head->id,m,i);

}
static void displayRoomLegend(GameState* g){
    printf("=== ROOM LEGEND ===\n");
    displayRoomLegendRec(g->rooms); 
    printf("===================\n");
}


void freeMonster(void* data){
    Monster *mon = (Monster*) data;
    free(mon->name);
    free(mon);
}

int compareMonsters(void* a, void* b){
    Monster *mon1 = (Monster*) a;
    Monster *mon2 = (Monster*) b;
    if (strcmp(mon1->name,mon2->name) != 0)
    {
        return strcmp(mon1->name,mon2->name);
    }
    if (mon1->attack != mon2->attack){
        return (mon1-> attack) - (mon2-> attack);
    }
    if(mon1->maxHp != mon2-> maxHp){
        return (mon1->maxHp) - (mon2->maxHp);
    }
    return (mon1->type) - (mon2->type); 
}
const char* getMonsterType(MonsterType type)
{
    switch(type){
    case PHANTOM: return "Phantom";
    case SPIDER: return "Spider";
    case DEMON:  return "Demon";
    case GOLEM:  return "Golem";
    case COBRA:  return "Cobra";
    default:     return "No Name";
    }
}
void printMonster(void* data){
    Monster *mon = (Monster*) data;
    printf("[%s] Type: %s, Attack: %d, HP: %d\n",mon->name,getMonsterType(mon->type),mon->attack, mon->maxHp);
}

void freeItem(void* data){
    Item *it = (Item*) data;
    free(it->name);
    free(it);
}

int compareItems(void* a, void* b){
    Item *it1 = (Item*) a;
    Item *it2 = (Item*) b;
    if (strcmp(it1->name,it2->name) != 0)
    {
        return strcmp(it1->name,it2->name);
    }
    if (it1->value != it2->value){
        return (it1-> value) - (it2-> value);
    }
    return (it1->type) - (it2->type); 
}
const char* getItemType(ItemType item){
    if (item == SWORD){
        return "SWORD";
    }else if(item == ARMOR){
        return "ARMOR";
    }
    return "No name";
}
Room* findRoomWithId(GameState* g, int id){
    Room *current = g->rooms;
    while(current != NULL){
        if (current->id == id){
            return current;
            break;
        }
        current = current->next;
    }
    return NULL;
    
}
Room* findRoomWithCordsAndDir(GameState* g, int x,int y,int dir){
    int vert = 0;
    int hori = 0; 
    switch(dir) {
        case 0: 
            vert = -1;
            break;
        case 1: 
            vert = 1;
            break;
        case 2:
            hori = -1;
            break;
        case 3:
            hori = 1;
            break;
        default: 
            return NULL;
    }

    Room *current = g->rooms;
    while(current != NULL){
        if (current->x == x + hori && current->y == y + vert){
            return current;
            break;
        }
        current = current->next;
    }
    return NULL;
}



void printItem(void* data){
    Item *it = (Item*)data;
    printf("[%s] %s - Value: %d\n", getItemType(it->type),it->name, it->value);
}
void addRoom(GameState* g){
    Room* room = (Room*) malloc(sizeof(Room));
    room->monster = NULL;
    room->item = NULL;
    room->visited = 0;
    room->next = NULL;
    int x = 0;
    int y = 0;
    if (g->roomCount !=0)
    {
        displayMap(g);
        displayRoomLegend(g);
        int attachTo = getInt("Attach to room ID: ");
        int dir = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
        Room *check = findRoomWithId(g,attachTo);
        if(findRoomWithCordsAndDir(g,check->x,check->y,dir) != NULL){
            printf("Room exists there\n");
            free(room);
            return;
        }
        x = giveDesX(check->x,dir);
        y = giveDesY(check->y,dir);
    }
    room -> id = g -> roomCount;
    g-> roomCount += 1; // scared to do ++.. :(
    room -> x = x;
    room -> y = y;

    int addmon = getInt("Add monster? (1=Yes, 0=No): ");
    if(addmon == 1){
        Monster* mon = (Monster*) malloc(sizeof(Monster));
        mon ->name = getString("Monster name: ");
        mon->type = getInt("Type (0-4): ");
        mon-> maxHp = getInt("HP: ");
        mon->hp = mon-> maxHp;
        mon->attack = getInt("Attack: ");
        room->monster = mon;
    }
    int additem = getInt("Add item? (1=Yes, 0=No): ");
    if (additem == 1){
        Item* item = (Item*) malloc(sizeof(Item));
        item->name = getString("Item name: ");
        item->type = getInt("Type (0=Armor, 1=Sword): ");
        item->value = getInt("Value: ");
        room -> item = item;
    }
    if(g-> rooms == NULL){
        g-> rooms = room;
    }else{
        Room *current = g->rooms;
        while(current -> next != NULL) current = current -> next;
        current -> next = room;
    }
    printf("Created room %d at (%d,%d)\n",room->id,room->x,room->y);
}
void initPlayer(GameState* g){
    if(g->rooms == NULL){
        printf("Create rooms first\n");
        return;
    }
    Player *pl = (Player*) malloc(sizeof(Player));
    pl->baseAttack = g ->configBaseAttack;
    pl->maxHp = g->configMaxHp;
    pl -> hp = g-> configMaxHp;
    pl->bag = createBST(compareItems,printItem,freeItem);
    pl->defeatedMonsters = createBST(compareMonsters,printMonster,freeMonster);
    pl->currentRoom = g->rooms;

    g->player = pl;
}

int gameWon(GameState* g){
    int counter = 0;
    Room *current = g->rooms;
    while(current != NULL){
        if(current->monster != NULL){
            return 0;
        }
        if(current->visited){
            counter++;
        }
        current = current-> next;
    }
    if (counter == g->roomCount){
        printf("***************************************\n");
        printf("VICTORY!\n");
        printf(" All rooms explored. All monsters defeated.\n");
        printf("***************************************\n");
        return 1;
    }
    return 0;
}
void printRoom(GameState* g,Room* room){
    printf("--- Room %d ---\n",room->id);
    if(room-> monster != NULL){
        printf("Monster: %s (HP:%d)\n",room->monster->name,room->monster->maxHp);
    }
    if(room-> item != NULL){
        printf("Item: %s\n",room->item->name);
    }
    printf("HP: %d/%d\n",g->player->hp,g->player->maxHp);
}

void playMove(GameState* g){
    if(g->player->currentRoom->monster != NULL){
        printf("Kill monster first\n");
        return;
    }
    int dir = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
    Room *moveto = findRoomWithCordsAndDir(g,g->player->currentRoom->x,g->player->currentRoom->y,dir);
    if(moveto == NULL){
        printf("No room there\n");
    }else{
        g->player->currentRoom = moveto;
        g->player->currentRoom->visited = 1;
    }
}

void playFight(GameState* g){
    if(g->player->currentRoom->monster == NULL){
        printf("No monster\n");
        return;
    }
    int turn = 1;
    Monster *mon = g->player->currentRoom->monster;
    Player *pl = g->player;
    while(pl->hp > 0 && mon ->hp > 0){
        if(turn){
            mon->hp -= pl->baseAttack;
            if (mon->hp < 0) mon->hp = 0;
            printf("You deal %d damage. Monster HP: %d\n",pl->baseAttack,mon->hp);
            turn = 0;
        }else{
            pl->hp -= mon->attack;
            if (pl->hp < 0) pl->hp = 0;
            printf("Monster deals %d damage. Your HP: %d\n",mon->attack,pl->hp);
            turn = 1;
        }
    }
    if (pl->hp > 0){
        printf("Monster defeated!\n");
        pl->defeatedMonsters->root = bstInsert(pl->defeatedMonsters->root, mon, pl->defeatedMonsters->compare);

        pl->currentRoom->monster = NULL;
    }
    else{
        printf("--- YOU DIED ---\n");
    }
}
void playPickUp(GameState* g){
    Player *pl = g->player;
    if (pl->currentRoom ->monster != NULL){
        printf("Kill monster first\n");
        return;
    }else if(pl->currentRoom->item == NULL){
        printf("No item here\n");
        return;
    }
    Item *it = pl->currentRoom->item;
    if(bstFind(pl->bag->root,it,compareItems) != NULL){
        printf("Duplicate item.\n");
        return;
    }else{
        pl->bag->root = bstInsert(pl->bag->root, it, pl->bag->compare);
        printf("Picked up %s\n",it->name);
        pl->currentRoom->item = NULL;
        return;
    }
}

void playBag(GameState* g){
    printf("=== INVENTORY ===\n");
    int chose = getInt("1.Preorder 2.Inorder 3.Postorder\n");
    switch (chose)
    {
    case 1:
        bstPreorder(g->player->bag->root,printItem);
        break;
    
    case 2:
        bstInorder(g->player->bag->root,printItem);
        break;      
    
    case 3:
        bstPostorder(g->player->bag->root,printItem);
        break;
    default:
        break;
    }
}
void playDefeated(GameState* g){
    printf("=== DEFEATED MONSTERS ===\n");
    int chose = getInt("1.Preorder 2.Inorder 3.Postorder\n");
    switch (chose)
    {
    case 1:
        bstPreorder(g->player->defeatedMonsters->root,printMonster);
        break;
    
    case 2:
        bstInorder(g->player->defeatedMonsters->root,printMonster);
        break;      
    
    case 3:
        bstPostorder(g->player->defeatedMonsters->root,printMonster);
        break;
    default:
        break;
    }
}

void playGame(GameState* g){
    if(g->player == NULL){
        printf("Init player first\n");
        return;
    }
    g->player->currentRoom->visited = 1; // make sure room is visited
    while(1){
        displayMap(g);
        displayRoomLegend(g);
        printRoom(g,g->player->currentRoom);
        g->player->currentRoom->visited = 1;// make sure
        int play = getInt("1.Move 2.Fight 3.Pickup 4.Bag 5.Defeated 6.Quit\n");
        switch (play)
        {
        case 1:
            playMove(g);
            if (gameWon(g)) return;
            break;
        case 2:
             playFight(g);
             if (gameWon(g)) return;
             break;
        case 3:
             playPickUp(g);
             break;
        case 4:
             playBag(g);
             break;
        case 5:
             playDefeated(g);
             break;
        case 6:
             return;
             break;
        default:
            return;
            break;
        }
        if(g->player->hp <= 0){
            break;
        }
    }
}
void freeGame(GameState* g){
    bstFree(g->player->bag->root,freeItem);
    bstFree(g->player->defeatedMonsters->root,freeMonster);
    free(g->player->bag);
    free(g->player->defeatedMonsters);
    free(g->player);
    Room *current = g->rooms;
    while(current != NULL){
        Room *next = current->next;
        if (current->item) freeItem(current->item);
        if (current->monster) freeMonster(current->monster);
        free(current);
        current = next;
    }
    
}










