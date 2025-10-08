#include <stdio.h>
#include <string.h>
#include "graph.h"
#include "resource.h"

typedef struct {
    char username[50];
    char password[50];
} User;

#define MAX_USERS 10
static User users[MAX_USERS];
static int userCount = 1;  // initially 1 demo user

// ----------------- Demo user -----------------
void initDemoUser() {
    strcpy(users[0].username, "user1");
    strcpy(users[0].password, "pass123");
}

// ----------------- User Sign Up -----------------
void userSignUp() {
    char uname[50], pass[50];
    FILE *fp = fopen("users.txt", "a+");
    if (!fp) {
        printf("Error opening users file.\n");
        return;
    }

    printf("\n--- User Sign Up ---\n");
    printf("Enter new username: ");
    scanf("%49s", uname);
    printf("Enter new password: ");
    scanf("%49s", pass);

    // Check if user already exists
    char fileUname[50], filePass[50];
    while (fscanf(fp, "%49s %49s", fileUname, filePass) != EOF) {
        if (strcmp(fileUname, uname) == 0) {
            printf("❌ Username already exists!\n");
            fclose(fp);
            return;
        }
    }

    fprintf(fp, "%s %s\n", uname, pass);
    fclose(fp);
    printf("✅ User registered successfully!\n");
}

// ----------------- User Login -----------------
int userLogin(int *userIndex) {
    char uname[50], pass[50];
    int attempts = 3;

    while (attempts--) {
        printf("\n--- User Login ---\n");
        printf("Username: ");
        scanf("%49s", uname);
        printf("Password: ");
        scanf("%49s", pass);

        FILE *fp = fopen("users.txt", "r");
        if (!fp) {
            printf("Error opening users file.\n");
            return 0;
        }

        char fileUname[50], filePass[50];
        int found = 0;
        while (fscanf(fp, "%49s %49s", fileUname, filePass) != EOF) {
            if (strcmp(uname, fileUname) == 0 && strcmp(pass, filePass) == 0) {
                found = 1;
                break;
            }
        }
        fclose(fp);

       if (found) {
    printf("✅ Login successful! Welcome %s\n", uname);

    // Check if user is already in users array
    int foundIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, uname) == 0) {
            foundIndex = i;
            break;
        }
    }

    // If not found, add to users array
    if (foundIndex == -1 && userCount < MAX_USERS) {
        strcpy(users[userCount].username, uname);
        strcpy(users[userCount].password, pass);
        foundIndex = userCount;
        userCount++;
    }

    *userIndex = foundIndex;
    return 1;
} else {
            printf("❌ Invalid credentials. Attempts left: %d\n", attempts);
        }
    }
    return 0;
}

// ----------------- User Menu -----------------
void userMenu(int userIndex) {
   

    const char *username = users[userIndex].username;
    int choice;

    while (1) {
        printf("\n--- USER MENU ---\n");
        printf("1. Show City Map\n");
        printf("2. Show Ambulances\n");
        printf("3. Show Hospitals\n");
        printf("4. Report Accident\n");
        printf("5. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: printGraph(); break;
            case 2: printAmbulances(); break;
            case 3: printHospitals(); break;
            case 4: {
                int loc;
                printf("Enter accident location (0-%d): ", N-1);
                scanf("%d", &loc);

                if (loc < 0 || loc >= N) {
                    printf("❌ Invalid location!\n");
                    break;
                }

                if (isAccidentLogged(username, loc)) {
                    printf("⚠️ Accident already reported at this location by you.\n");
                } else {
                    reportAccident(loc, username);
                }
                break;
            }
            case 5: return; // logout
            default: printf("Invalid choice!\n");
        }
    }
}

// ----------------- User Page -----------------
void userPage() {
    int choice, userIndex;

    while (1) {
        printf("\n--- USER PORTAL ---\n");
        printf("1. Login\n");
        printf("2. Sign Up\n");
        printf("3. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            if (userLogin(&userIndex)) {
                userMenu(userIndex);
            }
        } else if (choice == 2) {
            userSignUp();
        } else if (choice == 3) {
            break; // back to main menu
        } else {
            printf("Invalid choice!\n");
        }
    }
}
