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
     if(head == NULL){ // make sure not empty
        return;
     }
     displayRoomLegendRec(head->next); // do it recursivly so it goes from last to first
     char m = head->monster!= NULL ? 'V' : 'X'; // check if theres a monster
     char i = head->item!= NULL ? 'V' : 'X';  // check if theres an item
     printf("ID %d: [M:%c] [I:%c]\n", head->id,m,i);

}
static void displayRoomLegend(GameState* g){
    printf("=== ROOM LEGEND ===\n");
    displayRoomLegendRec(g->rooms); 
    printf("===================\n");
}


void freeMonster(void* data){
    Monster *mon = (Monster*) data; // cast to Monster
    free(mon->name);
    free(mon);
}

int compareMonsters(void* a, void* b){
    Monster *mon1 = (Monster*) a; // cast both
    Monster *mon2 = (Monster*) b;
    if (strcmp(mon1->name,mon2->name) != 0)
    {
        return strcmp(mon1->name,mon2->name); // first check name
    }
    if (mon1->attack != mon2->attack){
        return (mon1-> attack) - (mon2-> attack); // then attack
    }
    if(mon1->maxHp != mon2-> maxHp){
        return (mon1->maxHp) - (mon2->maxHp); // then hp
    }
    return (mon1->type) - (mon2->type); //lastly hp
}
const char* getMonsterType(MonsterType type) // get string value from MonsterType
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
    Monster *mon = (Monster*) data; // cast
    printf("[%s] Type: %s, Attack: %d, HP: %d\n",mon->name,getMonsterType(mon->type),mon->attack, mon->maxHp);
}

void freeItem(void* data){
    Item *it = (Item*) data; // cast
    free(it->name);
    free(it);
}

int compareItems(void* a, void* b){
    Item *it1 = (Item*) a; // cast both
    Item *it2 = (Item*) b;
    if (strcmp(it1->name,it2->name) != 0) //first check name
    {
        return strcmp(it1->name,it2->name);
    }
    if (it1->value != it2->value){ // then check value
        return (it1-> value) - (it2-> value);
    }
    return (it1->type) - (it2->type); //lastly check type
}
const char* getItemType(ItemType item){ // translate ItemType to string
    if (item == SWORD){
        return "SWORD";
    }else if(item == ARMOR){
        return "ARMOR";
    }
    return "No name";
}
Room* findRoomWithId(GameState* g, int id){ // return room with given id
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
            vert = -1; // up
            break;
        case 1: 
            vert = 1; // down
            break;
        case 2:
            hori = -1;  //left
            break;
        case 3:
            hori = 1; // right
            break;
        default: 
            return NULL;
    }

    Room *current = g->rooms;
    while(current != NULL){
        if (current->x == x + hori && current->y == y + vert){ // check if found
            return current;
            break;
        }
        current = current->next;
    }
    return NULL;
}



void printItem(void* data){
    Item *it = (Item*)data; //cast
    printf("[%s] %s - Value: %d\n", getItemType(it->type),it->name, it->value);
}
void addRoom(GameState* g){
    Room* room = (Room*) malloc(sizeof(Room)); // alloc new room
    room->monster = NULL; // init values accordingly
    room->item = NULL;
    room->visited = 0;
    room->next = NULL;
    int x = 0;
    int y = 0;
    if (g->roomCount !=0) // if its not first
    {
        displayMap(g);
        displayRoomLegend(g);
        int attachTo = getInt("Attach to room ID: ");
        int dir = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
        Room *check = findRoomWithId(g,attachTo); 
        if(findRoomWithCordsAndDir(g,check->x,check->y,dir) != NULL){ // check if a room exits in given location
            printf("Room exists there\n");
            free(room);
            return;
        }
        x = giveDesX(check->x,dir); // set new cords
        y = giveDesY(check->y,dir);
    }
    room -> id = g -> roomCount;
    g-> roomCount += 1; // scared to do ++.. :(
    room -> x = x;
    room -> y = y;

    int addmon = getInt("Add monster? (1=Yes, 0=No): ");
    if(addmon == 1){
        Monster* mon = (Monster*) malloc(sizeof(Monster)); //mem alloc new monster
        mon ->name = getString("Monster name: "); // init mon accordingly
        mon->type = getInt("Type (0-4): ");
        mon-> maxHp = getInt("HP: ");
        mon->hp = mon-> maxHp;
        mon->attack = getInt("Attack: ");
        room->monster = mon;
    }
    int additem = getInt("Add item? (1=Yes, 0=No): ");
    if (additem == 1){
        Item* item = (Item*) malloc(sizeof(Item)); // mem alloc item
        item->name = getString("Item name: "); //init item accordingly
        item->type = getInt("Type (0=Armor, 1=Sword): ");
        item->value = getInt("Value: ");
        room -> item = item;
    }
    if(g-> rooms == NULL){ //first room
        g-> rooms = room;
    }else{
        Room *current = g->rooms;
        while(current -> next != NULL) current = current -> next; // add room to last place in linked list
        current -> next = room;
    }
    printf("Created room %d at (%d,%d)\n",room->id,room->x,room->y);
}
void initPlayer(GameState* g){
    if(g->rooms == NULL){
        printf("Create rooms first\n");
        return;
    }
    Player *pl = (Player*) malloc(sizeof(Player)); // mem new player
    pl->baseAttack = g ->configBaseAttack; // init accordingly
    pl->maxHp = g->configMaxHp;
    pl -> hp = g-> configMaxHp;
    pl->bag = createBST(compareItems,printItem,freeItem); // make sure to initilize its BST
    pl->defeatedMonsters = createBST(compareMonsters,printMonster,freeMonster); // make sure to initilize its BST
    pl->currentRoom = g->rooms;

    g->player = pl;
}

int gameWon(GameState* g){
    int counter = 0;
    Room *current = g->rooms;
    while(current != NULL){
        if(current->monster != NULL){ // a living monster was found :-0
            return 0;
        }
        if(current->visited){
            counter++;
        }
        current = current-> next;
    }
    if (counter == g->roomCount){ // check if all rooms were visited
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
    if(g->player->currentRoom->monster != NULL){ // check if a monster exists
        printf("Kill monster first\n");
        return;
    }
    int dir = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
    Room *moveto = findRoomWithCordsAndDir(g,g->player->currentRoom->x,g->player->currentRoom->y,dir); // get the des room
    if(moveto == NULL){
        printf("No room there\n"); 
    }else{
        g->player->currentRoom = moveto;
        g->player->currentRoom->visited = 1; // make sure to mark as visited asap
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
    while(pl->hp > 0 && mon ->hp > 0){ // as long as both are alive
        if(turn){
            mon->hp -= pl->baseAttack;
            if (mon->hp < 0) mon->hp = 0;// make sure no minus in print
            printf("You deal %d damage. Monster HP: %d\n",pl->baseAttack,mon->hp);
            turn = 0;
        }else{
            pl->hp -= mon->attack;
            if (pl->hp < 0) pl->hp = 0; // make suren no minus in print
            printf("Monster deals %d damage. Your HP: %d\n",mon->attack,pl->hp);
            turn = 1;
        }
    }
    if (pl->hp > 0){ // monster dead
        printf("Monster defeated!\n");
        // notice that bstInsert returns another bst pointer (not void)
        pl->defeatedMonsters->root = bstInsert(pl->defeatedMonsters->root, mon, pl->defeatedMonsters->compare);

        pl->currentRoom->monster = NULL;
    }
    else{
        printf("--- YOU DIED ---\n"); // :'(
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
    if(bstFind(pl->bag->root,it,compareItems) != NULL){ // check if there isnt a nother item the same
        printf("Duplicate item.\n");
        return;
    }else{
        pl->bag->root = bstInsert(pl->bag->root, it, pl->bag->compare); // notice that bstInsert gives new pointer
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
            if (gameWon(g)) return; // check for win immidiatly after
            break;
        case 2:
             playFight(g);
             if (gameWon(g)) return; // check for win immidiatly after
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
    bstFree(g->player->bag->root,freeItem); // free bag bst
    bstFree(g->player->defeatedMonsters->root,freeMonster); // free def bst
    free(g->player->bag); // free bag wrapper
    free(g->player->defeatedMonsters); // free def wrapper
    free(g->player); // free player
    Room *current = g->rooms;
    while(current != NULL){
        Room *next = current->next; // used as some sort of a temp
        if (current->item) freeItem(current->item); // free the item in the room
        if (current->monster) freeMonster(current->monster);// free the monster in the room :D
        free(current); // free room
        current = next;
    }

    
    
}










