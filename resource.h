#ifndef RESOURCES_H
#define RESOURCES_H

typedef struct {
    int id;
    char driver[50];
    int location;
    int available;
} Ambulance;

typedef struct {
    int id;
    char name[50];
    int location;
    int beds;
} Hospital;

void addAmbulance(int id, const char *name, int loc);
void printAmbulances();
void addHospital(int id, const char *name, int loc, int beds);
void printHospitals();
void reportAccident(int location, const char* username);
int isAccidentLogged(const char* username, int location);

extern Ambulance ambulances[];
extern Hospital hospitals[];
extern int ambCount, hosCount;

#endif
