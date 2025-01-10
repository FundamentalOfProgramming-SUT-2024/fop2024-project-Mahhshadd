#include<stdio.h>
#include<ncurses.h>
#include<string.h>
#define MAX 1000
#define USERS "users.txt"

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
void login_user(){
    clear();
    printw("Login\n");
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
    getch();
    return;
}
void game_menu(){
    int choice;
    do{
    clear();
    
    printw("1. Start a New Game\n");
    printw("2. Settings\n");
    printw("3. Scores\n");
    printw("4. Profile\n");
    printw("Enter your choice: ");
    scanw("%d", &choice);

    switch (choice) {
            case 1:
                New_Game();
                break;
            case 2:
                Settings();
                break;
            case 3:
                Scores();
                break;
            case 4:
                profile_menu();
                break;
            default:
                printw("Invalid choice. Try again.\n");
                getch();
    }
    } while(choice != 4);
}

void main_menu(){
    int choice;
    do {
        clear();
        printw("Main Menu\n");
        printw("1. Create New User\n");
        printw("2. Login\n");
        printw("3. Game Menu\n");
        printw("4. Exit\n");
        printw("Enter your choice: ");
        scanw("%d", &choice);

        switch (choice) {
            case 1:
                create_user();
                break;
            case 2:
                login_user();
                break;
            case 3:
                game_menu();
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
    initscr();
    main_menu();
    endwin();
    return 0;
}
