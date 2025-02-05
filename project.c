#include<stdio.h>
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#define MAX 1000
#define USERS "users.txt"
#define SCORES "scores.txt"
#define WALL '|'
#define FLOOR '.'
#define DOOR '+'
#define CORRIDOR '#'
#define PILLAR 'O'
#define WINDOW '='
#define EMPTY ' '
#define TRAP '^'
#define STAIRCASE '<'
#define HEIGHT 50
#define WIDTH 100

int visited[WIDTH][HEIGHT];
int showFullMap = 0;

int color=0;
int floors=1;

void init_colors() {
    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
}

typedef struct {
    int x, y;     
    int width, height; 
    int doorX, doorY; 
    int trapX, trapY;
    int windowX, windowY;
    int pillarX, pillarY;
    int stairX, stairY;
    int sdoorX, sdoorY;
    bool sdoorfound;
} Room;
 
 typedef struct {
    int x;
    int y;
    bool active; 
} Trap;

#define MAX_TRAPS 100
Trap traps[MAX_TRAPS];
int trapCounter = 0;


typedef struct {
    char username[MAX];
    int total_score;
    int gold;
    int games_completed;
    time_t first_game_time;
    int x;
    int y;
    int health;
} Player;

typedef struct {
    char username[MAX];
    char password[MAX];
    char email[MAX];
} User;


int is_username_taken(const char* username) {
    FILE *file = fopen(USERS, "r");
    if (!file) {
        return 0;
    }

    User stored_user;
    while (fscanf(file, "%s %s %s", stored_user.username, stored_user.password, stored_user.email) != EOF) {
        if (strcmp(stored_user.username, username) == 0) {
            fclose(file);
            return 1; 
        }
    }
    fclose(file);
    return 0; 
}

void save_user(const char* username, const char* password, const char* email) {
    FILE *file = fopen(USERS, "a");
    if (!file) {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "%s %s %s\n", username, password, email);
    fclose(file);
}

void create_user(){
    clear();
    move(0, 0);
    printw("Create User Menu");
    
    move(2, 0);
    printw("ENTER USERNAME: ");
    User new_user;
    scanw("%s", new_user.username);
    printw("\n");
    while(is_username_taken(new_user.username)==1){
        printw("USERNAME IS ALREADY TAKEN; PLEASE CHOOSE ANOTHER USERNAME.\n");
        scanw("%s", new_user.username);
    }
    
    printw("ENTER YOUR EMAIL: ");
    scanw("%s", new_user.email);
    while(true){
        int dot[MAX];
        int j=0;
        int a=0;
        int counta=0;
        for(int i=strlen(new_user.email)-1; i>=0; i--){
            if(new_user.email[i]=='.'){
                dot[j++]=i;
            }
            if(new_user.email[i]=='@'){
                a=i;
                counta++;
            }

        }
        if(counta!=1){
            printw("INVALID EMAIL ADDRESS. PLEASE ENTER ANOTHER EMAIL ADDRESS.\n");
            scanw("%s", new_user.email);
            continue;
        }
        else if(dot[0]>=3 && dot[0]<=strlen(new_user.email)-2 && dot[0]-a>=1 && a>=1 && a<=strlen(new_user.email)-4){
            break;
        }
        else {
            printw("INVALID EMAIL ADDRESS. PLEASE ENTER ANOTHER EMAIL ADDRESS.\n");
            scanw("%s", new_user.email);
            continue;
        }
    }
    printw("\n");
    
    printw("ENTER PASSWORD: ");
    scanw("%s", new_user.password);
    while(strlen(new_user.password)<7){
        printw("YOUR PASSWORD MUST INCLUDE AT LEAST 7 CHARACTERS. PLEASE ENTER ANOTHER PASSWORD.\n");
        scanw("%s", new_user.password);
    }
    printw("\n");
    while(true){
        int isnum=0;
        int islowercase=0;
        int isuppercase=0;
        for(int i=0; i<strlen(new_user.password); i++){
            if(new_user.password[i]>='0' && new_user.password[i]<='9'){
                isnum++;
            }
            else if(new_user.password[i]>='a' && new_user.password[i]<='z'){
                islowercase++;
            }
            else if(new_user.password[i]>='A' && new_user.password[i]<='Z'){
                isuppercase++;
            }
        }
        if(islowercase==0){
            printw("YOUR PASSWORD MUST HAVE A LOWERCASE CHARACTER. PLEASE ENTER ANOTHER PASSWORD.\n");
            scanw("%s", new_user.password);
        }
        else if(isuppercase==0){
            printw("YOUR PASSWORD MUST HAVE AN UPPERCASE CHARACTER. PLEASE ENTER ANOTHER PASSWORD.\n");
            scanw("%s", new_user.password);
        }
        else if(isnum==0){
            printw("YOUR PASSWORD MUST HAVE A NUMBER. PLEASE ENTER ANOTHER PASSWORD.\n");
            scanw("%s", new_user.password);
        }
        else if(isnum>0 && isuppercase>0 && islowercase>0){
            break;
        }
    }
    save_user(new_user.username, new_user.password, new_user.email);
    
    
    
}
int is_valid_user(const char* username, const char* password) {
    FILE *file = fopen(USERS, "r");

    User stored_user;
    while (fscanf(file, "%s %s %s", stored_user.username, stored_user.password, stored_user.email) != EOF) {
        if (strcmp(stored_user.username, username) == 0 && strcmp(stored_user.password, password) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}
void login_user(char user_name[MAX], Player * newplayer){
    clear();
    printw("Login Menu\n");
    char username[MAX];
    char password[MAX];
    printw("ENTER USERNAME(or type guest to continue as a guest): \n");
    scanw("%s", username);
    if(strcmp(username, "guest")==0){
        printw("YOU HAVE LOGGED IN AS A GUEST. PRESS ANY KEY TO RETURN TO THE MAIN MENU.\n");
        getch();
        return;
    }
    printw("ENTER PASSWORD: \n");
    scanw("%s", password);
    while(is_valid_user(username, password)==0){
        printw("INVALID USERNAME OR PASSWORD. TRY AGAIN!\n");
        printw("ENTER USERNAME(or type guest to continue as a guest): \n");
        scanf("%s", username);
        if(strcmp(username, "guest")==0){
        printw("YOU HAVE LOGGED IN AS A GUEST. PRESS ANY KEY TO RETURN TO THE MAIN MENU.\n");
        getch();
        return;
        }
        printw("ENTER PASSWORD: \n");
        scanf("%s", password);
    }
    printw("WELCOME, %s! PRESS ANY KEY TO RETURN TO THE MAIN MENU.\n", username);
    strcpy(user_name, username);
    strcpy(newplayer->username, username);
    getch();
    return;
}

void difficulty(char settingChoices[4]){
    char level;
    clear();
    printw("How difficult would you like the game to be? \nEnter 1, 2, 3 or 4. \n1 is the easiest level!\n");
    scanw("%c", &level);
    settingChoices[1]=level;
    return;
}

void choose_color(){
    clear();
    int colornum;
    printw("Choose the color you want for the player.\n");
    printw("1. Magenta\n");
    printw("2. Green\n");
    printw("3. Red\n");
    printw("4. Yellow\n");
    printw("Enter your choice:\n");
    printw("(Defult color is white.)\n");
    scanw("%d", &colornum);
    color=colornum;
}

void Settings(char settingChoices[4]){
    int choice;
    
    do{
    clear();
    printw("1. Choose Color\n");
    printw("2. Choose Difficulty Level\n");
    printw("3. Choose Music\n");
    printw("4. Back to Pre-Game Menu\n");
    printw("Enter Your Choice: \n");
    scanw("%d", &choice);

    switch (choice)
    {
    case 1:
        choose_color();
        break;
    case 2:
        difficulty(settingChoices);
        break;
    /*case 3:
        music();
        break;*/
    case 4:
        return;
    default:
        printw("Invalid choice. Try again.\n");
        getch();
    }
    } while(choice!=4);

    return;
}
void Scores(const char* current_user){
    
    clear();
    FILE *file = fopen(SCORES, "r");
    if (!file) {
        printw("No Scores Available.\n");
        getch();
        return;
    }
    Player players[MAX];
    int count = 0;

    while (fscanf(file, "%s %d %d %d %ld", players[count].username, &players[count].total_score, 
                  &players[count].gold, &players[count].games_completed, &players[count].first_game_time) != EOF) {
        count++;
    }
    fclose(file);
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (players[i].total_score < players[j].total_score) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }
    printw("Scoreboard\n");
    printw("-----------------------------------------------------------------------------------\n");
    printw("| Rank | Username         | Total Score | Gold       | Games | Experience          \n");
    printw("-----------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        char experience[MAX];
        time_t now = time(NULL);
        double experience_days = difftime(now, players[i].first_game_time) / (60 * 60 * 24);
        snprintf(experience, MAX, "%.0f days", experience_days);

        if (strcmp(players[i].username, current_user) == 0) {
            attron(A_BOLD); 
            attron(A_UNDERLINE);
        }
        if (i < 3) { 
            attron(COLOR_PAIR(1));
            attron(A_ITALIC);
        }

        printw("| %-4d | %-15s | %-11d | %-10d | %-5d | %-18s  \n", i + 1, players[i].username, 
               players[i].total_score, players[i].gold, players[i].games_completed, experience);

        if (i < 3) {
            attroff(COLOR_PAIR(1)); 
            attroff(A_ITALIC);
        }
        if (strcmp(players[i].username, current_user) == 0) {
            attroff(A_BOLD); 
            attroff(A_UNDERLINE);
        }
    }

    printw("-----------------------------------------------------------------------------------\n");
    printw("Press any key to return to the previous menu.\n");
    getch();
}




int isOverlapping(Room room, Room rooms[], int count) {
    for (int i = 0; i < count; i++) {
        Room r = rooms[i];
        if (!((room.x+room.width +5 < r.x && room.x<r.x) || (room.x > r.x && r.x + r.width +5 < room.x) || 
            (room.y+room.height +5 < r.y && room.y<r.y) || (room.y > r.y && r.y + r.height +5 < room.y))) {
            return 1;
        }
    }
    return 0;
}

Room createRandomRoom(int mapWidth, int mapHeight) {
    Room room;
    room.width = 6 + rand() % 5;  
    room.height = 6 + rand() % 5; 
    room.x = 2 + rand() % (mapWidth - room.width - 3); 
    room.y = 2 + rand() % (mapHeight - room.height - 3); 
    return room;
}

void drawRoom(char map[WIDTH][HEIGHT], Room* room, Trap traps[], int roomNum){
    for(int i=0; i<room->width; i++){
        for(int j=0; j<room->height; j++){
            if(i==0 || i==room->width-1 || j==0 || j==room->height-1){
                map[i+room->x][j+room->y]=WALL;
            }
            else {
                map[i+room->x][j+room->y]=FLOOR;
            }
        }
    }
    while(1){
        room->doorX=room->x + rand() % (room->width);
        room->doorY=room->y + rand() % (room->height);
        if(map[room->doorX][room->doorY]==WALL){
            map[room->doorX][room->doorY]=DOOR;
            break;
        }
    }
    if(roomNum==1 || roomNum==3 || roomNum==5 || roomNum==7 || roomNum==9){
        while(1){
        room->stairX=room->x + rand() % (room->width);
        room->stairY=room->y + rand() % (room->height);
        if(map[room->stairX][room->stairY]==WALL){
            map[room->stairX][room->stairY]=STAIRCASE;
            break;
        }
        }
    }
    int trapcount= rand()%2;
    if(trapcount==1){
        room->trapX=room->x+1+rand()%(room->width-2);
        room->trapY=room->y+1+rand()%(room->height-2);
        if(map[room->trapX][room->trapY]==FLOOR){
        //map[room->trapX][room->trapY]=TRAP;
        
        traps[trapCounter].x = room->trapX;
        traps[trapCounter].y = room->trapY;
        traps[trapCounter].active = false; 
        trapCounter++;
        }
        
    }
    

    int pillarcount= rand()%2;
    //if(pillarcount==1){
    while(pillarcount){
        
        room->pillarX=room->x+1+rand()%(room->width-2);
        room->pillarY=room->y+1+rand()%(room->height-2);
        
        if(map[room->pillarX][room->pillarY]==FLOOR){
        map[room->pillarX][room->pillarY]=PILLAR;
        break;
        }
    }
    //}
    int windowcount=rand()%2;
    //if(windowcount==1){
    while(windowcount){
        
        room->windowX=room->x + rand() % (room->width);
        room->windowY=room->y + rand() % (room->height);
        if(map[room->windowX][room->windowY]==WALL){
            map[room->windowX][room->windowY]=WINDOW;
            break;
        }
    }
    //}
    
    
    
}

void printMap(char map [WIDTH][HEIGHT], int playerX, int playerY, Room rooms[]){
    for(int i=0; i<WIDTH; i++){
        for(int j=0; j<HEIGHT; j++){
            for (int t = 0; t < trapCounter; t++) {
                if(traps[t].x == i && traps[t].y == j) {
                    if (traps[t].active) {
                        map[i][j] = TRAP; 
                    }
                    else {
                        map[i][j] = FLOOR; 
                    }
                    break;
                }
            }
            if(i == rooms[0].sdoorX && j == rooms[0].sdoorY && rooms[0].sdoorfound == false && floors>1) {
                mvaddch(j, i, WALL); 
            }
            else if (i == playerX && j == playerY) {
                attron(COLOR_PAIR(color));
                mvaddch(j, i, 'P'); 
                attroff(COLOR_PAIR(color));
            }
            
            else if (visited[i][j] || showFullMap) {  
                mvaddch(j, i, map[i][j]);
            } else {
                mvaddch(j, i, ' ');  
            }
        }
        
    }
    for(int i=0; i<=WIDTH; i++){
        mvaddch(HEIGHT, i, '_');
    }
    for(int j=0; j<=HEIGHT; j++){
        mvaddch(j, WIDTH, '|');
    }
    move(1, WIDTH+2);
    printw("Messages");
    
    refresh();
}

void generateMap(char map[WIDTH][HEIGHT], int roomCount, Player*newplayer, Room rooms[]) {
    //Room rooms[roomCount]; 
    int createdRooms = 0;
 
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            map[i][j] = EMPTY;
            visited[i][j] = 0;
        }
    }

    while (createdRooms < roomCount) {
        Room newRoom = createRandomRoom(WIDTH, HEIGHT);
        if (!isOverlapping(newRoom, rooms, createdRooms)) { 
            rooms[createdRooms++] = newRoom; 
            drawRoom(map, &newRoom, traps, createdRooms-1); 
        }
    }
    Room room=rooms[0];
    newplayer->x = room.x + room.width / 2;
    newplayer->y = room.y + room.height / 2;
    
    /*for (int i = 0; i < createdRooms - 1; i++) {
        connectRooms(map, rooms[i], rooms[i + 1]);
    }*/
}

void updateVisibility(int playerX, int playerY, Room rooms[], int roomCount) {
    for (int i = 0; i < roomCount; i++) {
        Room r = rooms[i];
        if (playerX >= r.x && playerX < r.x + r.width &&
            playerY >= r.y && playerY < r.y + r.height) {
            for (int x = r.x; x < r.x + r.width; x++) {
                for (int y = r.y; y < r.y + r.height; y++) {
                    visited[x][y] = 1;
                }
            } 
            return; 
        }
    }
}

void generateFloor(char map[WIDTH][HEIGHT], Room room, Player*newplayer, Room rooms[]){
    int createdRooms=1;
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            map[i][j] = EMPTY;
            visited[i][j] = 0;
        }
    }

    rooms[0].x=room.x;
    rooms[0].y=room.y;
    rooms[0].width=room.width;
    rooms[0].height=room.height;
    rooms[0].stairX=room.stairX;
    rooms[0].stairY=room.stairY;
       

    for(int i=room.x; i<room.x+room.width; i++){
        for(int j=room.y; j<room.y+room.height; j++){
            if(i==room.x || i==room.width-1+room.x || j==room.y || j==room.y+room.height-1){
                map[i][j]=WALL;
            }
            else if(i==room.stairX && j==room.stairY){
                map[i][j]='>';
            }
            else {
                map[i][j]=FLOOR;
            }
            visited[i][j]=1;
        }
    }


    while (createdRooms < 6) {
        Room newRoom = createRandomRoom(WIDTH, HEIGHT);
        if (!isOverlapping(newRoom, rooms, createdRooms)) { 
            rooms[createdRooms++] = newRoom; 
            drawRoom(map, &newRoom, traps, createdRooms-1); 
        }
    }
    Room r0=rooms[0];
            for(int i=r0.x; i<r0.x + r0.width; i++){
                for(int j=r0.y; j<r0.y + r0.height; j++){
                    visited[i][j]=1;
                }
            }
    /*newplayer->x=room.stairX;
    newplayer->y=room.stairY;
    printMap(map, room.stairX, room.stairY);
    refresh();*/
    
}


void movePlayer(int *playerX, int *playerY, char map[WIDTH][HEIGHT], Room rooms[], int roomCount, Player*player) {
    char ch;
    
    noecho();
    while (1) {  
        int newX = *playerX, newY = *playerY;
        ch=getch();
        int a=0;

        switch (ch) {    
            case '8':
                newY--;
                break;  
            case '2':
                newY++;
                break; 
            case '4':
                newX--;
                break;
            case '6':
                newX++;
                break;
            case '7': 
                newX--;
                newY--;
                break;
            case '9': 
                newX++;
                newY--;
                break;
            case '1': 
                newX--;
                newY++;
                break;
            case '3': 
                newX++;
                newY++;
                break;
            case 'q':
            case 'Q':
                return;
            case 'm':
            case 'M':
                if(showFullMap){
                    showFullMap=0;
                }
                else {
                    showFullMap=1;
                }
                clear();
                printMap(map, *playerX, *playerY, rooms);
                refresh();
                a=1;
                break;
        }
        if(a){
            //showFullMap=0;
            continue;
        }

        if(map[newX][newY]==WALL){
            continue;
        }

        /*if(floors>1 && newX == rooms[0].sdoorX && newY == rooms[0].sdoorY) {
            rooms[0].sdoorfound = true;
            map[newX][newY] = DOOR; 
           *playerX = newX;
           *playerY = newY;
            updateVisibility(*playerX, *playerY, rooms, roomCount);
            clear();
            printMap(map, *playerX, *playerY, rooms);
            move(3, WIDTH+2);
            attron(COLOR_PAIR(2));
            printw("Secret door discovered!");
            attroff(COLOR_PAIR(2));
            refresh();
            continue;
        }*/


        if(map[newX][newY]==STAIRCASE && (map[*playerX][*playerY]==FLOOR || map[*playerX][*playerY]==TRAP)&&floors<=4){
            
            clear();
            
            floors++;
            Room templateRoom;
            bool found = false;
            for (int i = 0; i < roomCount; i++) {
                if (newX >= rooms[i].x && newX < rooms[i].x + rooms[i].width &&
                    newY >= rooms[i].y && newY < rooms[i].y + rooms[i].height) {
                    templateRoom = rooms[i];
                    found = true;
                    break;
                }
            }
            trapCounter=0;
            generateFloor(map, templateRoom, player, rooms);
            map[newX][newY]='>';
            printMap(map, newX, newY, rooms);
            
            move(3, WIDTH+2);
            attron(COLOR_PAIR(2));
            printw("Welcome to the %dnd floor!", floors);
            attroff(COLOR_PAIR(2));
            refresh();
            continue;
        }

        else if(map[newX][newY]==STAIRCASE && (map[*playerX][*playerY]==FLOOR || map[*playerX][*playerY]==TRAP)&&floors>4){
            attron(COLOR_PAIR(3));
            printw("No more floors!");
            attroff(COLOR_PAIR(3));
            continue;
        }

        
        if ((map[newX][newY] == FLOOR || map[newX][newY] == EMPTY || map[newX][newY] == DOOR || map[newX][newY]==TRAP)&&map[newX][newY]!=WALL) {
            *playerX = newX;
            *playerY = newY;
            updateVisibility(*playerX, *playerY, rooms, roomCount);
        }
        clear();
        for (int t = 0; t < trapCounter; t++) {
            if (traps[t].x == newX && traps[t].y == newY) {
        
                traps[t].active = true;
                player->health --;
                move(3, WIDTH+2);
                attron(COLOR_PAIR(3));
                printw("Oops! You hit a trap...Your current health is %d.", player->health);
                attroff(COLOR_PAIR(3));
            }
        }

        
        printMap(map, newX, newY, rooms);
        refresh();
    }
    echo();
}

void New_Game(char level, Player*newplayer){
    clear();
    int levelnum;
    if(level=='1'){
        levelnum=1;
    }
    if(level=='2'){
        levelnum=2;
    }
    if(level=='3'){
        levelnum=3;
    }
    if(level=='4'){
        levelnum=4;
    }
    else {
        levelnum=1;
    }
    srand(time(NULL));  
    char map[WIDTH][HEIGHT];
    int roomCount = 6+levelnum;
    Room rooms[roomCount];
    trapCounter=0;
    generateMap(map, roomCount, newplayer, rooms);
    Room r0=rooms[0];
    for(int i=r0.x; i<r0.x + r0.width; i++){
        for(int j=r0.y; j<r0.y + r0.height; j++){
            visited[i][j]=1;
        }
    }
    printMap(map, newplayer->x, newplayer->y, rooms);
    
    
    movePlayer(&newplayer->x, &newplayer->y, map, rooms, roomCount, newplayer);
    echo();
    refresh();
    getch();
}



void game_menu(const char* current_user, Player*newplayer){
    int choice;
    char settingChoices[4];
    
    do{
    clear();
    
    printw("1. New Game\n");
    printw("2. Settings\n");
    printw("3. Scoreboard\n");
    printw("4. Profile\n");
    printw("5. Resume Game\n");
    printw("6. Back to the main menu\n");
    printw("Enter your choice: \n");
    printw("(Choose the difficulty level in settings before starting a new game.)\n");
    scanw("%d", &choice);

    switch (choice) {
            case 1:
            showFullMap=0;
                char level = settingChoices[1];
                New_Game(level, newplayer);
                break;
            case 2:
                Settings(settingChoices);
                break;
            case 3:
                Scores(current_user);
                break;
            /*case 4:
                profile_menu();
                break;
            case 5:
                Resume_Game();*/
            case 6:
                return;
            default:
                printw("Invalid choice. Try again.\n");
                getch();
    }
    } while(choice != 6);
}

void main_menu(char user_name[MAX]){
    Player *newplayer=(Player*)malloc(sizeof(Player));
    newplayer->health=10;
    int choice;
    do {
        clear();
        printw("Main Menu\n");
        printw("1. Register Menu\n");
        printw("2. Login Menu\n");
        printw("3. Pre-Game Menu\n");
        printw("4. Exit\n");
        printw("Enter your choice: \n");
        printw("\n(Login before going to the Pre-Game menu.)\n ");
        scanw("%d", &choice);

        switch (choice) {
            case 1:
                create_user();
                break;
            case 2:
                login_user(user_name, newplayer);
                break;
            case 3:
                color=0;
                game_menu(user_name, newplayer);
                break;
            case 4:
                free(newplayer);
                return;
            default:
                printw("Invalid choice. Try again.\n");
                getch();
        }
    } while (choice != 4);

}

int main(){
    char username[MAX];
    initscr();
    curs_set(0);
    init_colors();
    main_menu(username);
    endwin();
    return 0;
}
