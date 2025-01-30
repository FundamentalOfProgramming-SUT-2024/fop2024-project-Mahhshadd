#include<stdio.h>
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
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
#define HEIGHT 70
#define WIDTH 70

void init_colors() {
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
}

typedef struct {
    int x, y;     
    int width, height;  
} Room;

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
void login_user(char user_name[MAX]){
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
    getch();
    return;
}

char difficulty(){
    char level;
    clear();
    printw("How difficult do you want the game to be? \nEnter 1, 2, 3 or 4. \n1 is the easiest level!\n");
    scanw("%c", &level);
    return level;
}

void Settings(char settingChoices[4]){
    int choice;
    char setting_Choices[4];
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
    /*case 1:
        choose_color();
        break;*/
    case 2:
        setting_Choices[1]=difficulty();
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
    strcpy(settingChoices, setting_Choices);
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
    room.width = 4 + rand() % 5;  
    room.height = 4 + rand() % 5; 
    room.x = rand() % (mapWidth - room.width - 1); 
    room.y = rand() % (mapHeight - room.height - 1); 
    return room;
}

void drawRoom(char map[WIDTH][HEIGHT], Room room){
    for(int i=0; i<room.width; i++){
        for(int j=0; j<room.height; j++){
            if(i==0 || i==room.width-1 || j==0 || j==room.height-1){
                map[i+room.x][j+room.y]=WALL;
            }
            else {
                map[i+room.x][j+room.y]=FLOOR;
            }
        }
    }
}

void printMap(char map [WIDTH][HEIGHT]){
    for(int i=0; i<WIDTH; i++){
        for(int j=0; j<HEIGHT; j++){
            
            mvaddch(i, j, map[i][j]);
        }
        
    }
    refresh();
}

void generateMap(char map[WIDTH][HEIGHT], int roomCount) {
    Room rooms[roomCount]; 
    int createdRooms = 0;
 
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            map[i][j] = EMPTY;
        }
    }

    while (createdRooms < roomCount) {
        Room newRoom = createRandomRoom(WIDTH, HEIGHT);
        if (!isOverlapping(newRoom, rooms, createdRooms)) { 
            rooms[createdRooms++] = newRoom; 
            drawRoom(map, newRoom); 
        }
    }
    
    /*for (int i = 0; i < createdRooms - 1; i++) {
        connectRooms(map, rooms[i], rooms[i + 1]);
    }*/
}

void New_Game(int level){
    clear();
    srand(time(NULL));  
    char map[WIDTH][HEIGHT];
    int roomCount = 6+level;
    generateMap(map, roomCount);
    printMap(map);
    refresh();
    getch();
}



void game_menu(const char* current_user){
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
                int level = 2;
                New_Game(level);
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
                login_user(user_name);
                break;
            case 3:
                game_menu(user_name);
                break;
            case 4:
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
    init_colors();
    main_menu(username);
    endwin();
    return 0;
}
