#include <stdio.h>
#include <string.h>
#include <time.h>
#include "resource.h"

Ambulance ambulances[5];
Hospital hospitals[5];
int ambCount = 0, hosCount = 0;

void addAmbulance(int id, const char *name, int loc) {
    ambulances[ambCount].id = id;
    strcpy(ambulances[ambCount].driver, name);
    ambulances[ambCount].location = loc;
    ambulances[ambCount].available = 1;
    ambCount++;
}

void printAmbulances() {
    printf("\nAmbulances:\n");
    for (int i = 0; i < ambCount; i++) {
        printf("ID:%d | Driver:%s | Location:%d | Status:%s\n",
               ambulances[i].id, ambulances[i].driver,
               ambulances[i].location,
               ambulances[i].available ? "Available" : "Busy");
    }
}

void addHospital(int id, const char *name, int loc, int beds) {
    hospitals[hosCount].id = id;
    strcpy(hospitals[hosCount].name, name);
    hospitals[hosCount].location = loc;
    hospitals[hosCount].beds = beds;
    hosCount++;
}

void printHospitals() {
    printf("\nHospitals:\n");
    for (int i = 0; i < hosCount; i++) {
        printf("ID:%d | Name:%s | Location:%d | Beds:%d\n",
               hospitals[i].id, hospitals[i].name,
               hospitals[i].location, hospitals[i].beds);
    }
}


void reportAccident(int location, const char* username) {
    FILE *fp = fopen("accident_log.txt", "a");
    if(!fp) {
        printf("Error opening accident log file.\n");
        return;
    }

    // Get system time
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    fprintf(fp, "%s %d %02d:%02d:%02d\n", username, location,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

    fclose(fp);

    printf("✅ Accident reported at location %d by user %s\n", location, username);
}

int isAccidentLogged(const char* username, int location) {
    FILE *fp = fopen("accident_log.txt", "r");
    if (!fp) return 0; // No log file means no previous accident

    char fileUser[50];
    int fileLoc;
    int h, m, s;

    while (fscanf(fp, "%s %d %d:%d:%d", fileUser, &fileLoc, &h, &m, &s) != EOF) {
        if (strcmp(username, fileUser) == 0 && fileLoc == location) {
            fclose(fp);
            return 1; // Accident already logged by this user at this location
        }
    }

    fclose(fp);
    return 0; // Not found
}