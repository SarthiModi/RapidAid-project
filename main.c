#include <stdio.h>
#include "graph.h"
#include "resource.h"
#include "user.h"
#include "admin.h"

int main() {
    // Initialize city, ambulances, hospitals
    initGraph(); 
    buildCity();

    addAmbulance(1, "Ravi", 0);
    addAmbulance(2, "Sita", 3);
    addAmbulance(3, "Amit", 2);

    addHospital(1, "City Hospital", 0, 5);
    addHospital(2, "Green Care", 4, 3);

    // Optionally initialize demo user
    initDemoUser();

    int choice;
    while(1) {
        printf("\n=== MAIN MENU ===\n");
        printf("1. User Login\n");
        printf("2. Admin Login\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                userPage();          // Opens User Login / Sign Up page
                break;
            case 2:
                if(adminLogin())     // Admin login handled separately
                    adminMenu();
                break;
            case 3:
                printf("Exiting system...\n");
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }

    return 0;
}
