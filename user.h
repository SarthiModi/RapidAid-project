#ifndef USER_H
#define USER_H

#include "graph.h"     // For printGraph()
#include "resource.h"  // For reportAccident()
// ---------------- Constants ----------------
#define MAX_USERS 10

// ----------------- User Structures -----------------
typedef struct {
    char username[50];
    char password[50];
} User;

// ----------------- Function Prototypes -----------------

// Login / Sign-up page (first screen inside User Login)
void userPage();

// User menu after login
void userMenu(int userIndex);


// User sign-up
void userSignUp();

// User login validation
int userLogin(int *userIndex);

// Optional: initialize a demo user
void initDemoUser();

#endif
