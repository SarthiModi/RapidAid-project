#include <stdio.h>
#include <string.h>
#include "admin.h"
#include "resource.h"
#include "user.h"

int adminLogin() {
    char uname[50], pass[50];
    printf("\n--- Admin Login ---\nUsername: "); scanf("%s", uname);
    printf("Password: "); scanf("%s", pass);
    if(strcmp(uname,"admin")==0 && strcmp(pass,"admin123")==0) {
        printf("✅ Admin login successful!\n"); return 1;
    }
    printf("❌ Invalid admin credentials!\n"); return 0;
}

void viewAccidentLog() {
    FILE *fp=fopen("accident_log.txt","r");
    if(!fp){ printf("⚠️ No accident records found!\n"); return; }
    char line[200]; printf("\n--- Accident Logs ---\n");
    while(fgets(line,sizeof(line),fp)) printf("%s",line);
    fclose(fp); printf("-------------------\n");
}

void adminMenu() {
    int choice;
    while(1){
        printf("\n--- Admin Menu ---\n1.View Accident Logs\n2.Show Ambulances\n3.Show Hospitals\n4.Logout\nEnter choice: ");
        scanf("%d",&choice);
        switch(choice){
            case 1: viewAccidentLog(); break;
            case 2: printAmbulances(); break;
            case 3: printHospitals(); break;
            case 4: printf("Logging out of admin...\n"); return;
            default: printf("Invalid choice!\n");
        }
    }
}
