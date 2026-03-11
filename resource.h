#ifndef RESOURCES_H
#define RESOURCES_H

#include <stdio.h>

/* Basic structures */
typedef struct Ambulance {
    char id[32];
    char location[64];
    char status[16];   /* "available" or "busy" */
    struct Ambulance *next;
} Ambulance;

typedef struct Hospital {
    char id[32];
    char location[64];
    int beds;
    struct Hospital *next;
} Hospital;

typedef struct Request {
    int id;
    char userID[32];
    char location[64];
    char severity[16];   /* "normal" / "serious" */
    char contact[32];
    struct Request *next;
} Request;

/* Completed item stored on stack */
typedef struct Completed {
    int req_id;
    char userID[32];
    char amb_id[32];
    char hosp_id[32];
    int total_time; /* seconds */
    struct Completed *next;
} Completed;

/* ---------------- file-backed linked lists ---------------- */
Ambulance* loadAmbulances(const char *file);
void saveAmbulances(const char *file, Ambulance *head);
void appendAmbulance(Ambulance **head, const char *id, const char *loc, const char *status);
void freeAmbulances(Ambulance *head);

Hospital* loadHospitals(const char *file);
void saveHospitals(const char *file, Hospital *head);
void appendHospital(Hospital **head, const char *id, const char *loc, int beds);
void freeHospitals(Hospital *head);

Request* loadRequests(const char *file);
void saveRequests(const char *file, Request *head);
void appendRequest(Request **head, const char *userID, const char *loc, const char *sev, const char *contact);
void freeRequests(Request *head);

/* ---------------- BST for ambulances (by id) ---------------- */
typedef struct AmbNode {
    Ambulance data;       /* note: stores a copy */
    struct AmbNode *left, *right;
} AmbNode;

AmbNode* buildAmbulanceBST(const char *file);
AmbNode* insertAmbBST(AmbNode *root, const Ambulance *a);
AmbNode* searchAmbBST(AmbNode *root, const char *id);
void freeAmbBST(AmbNode *root);

/* ---------------- Priority Queue for Requests (min-heap) ---------------- */
/* priority: serious (higher) -> normal; within same severity, lower id first */
typedef struct {
    Request **arr;
    int size;
    int cap;
} ReqPQ;

ReqPQ* pq_create(int cap);
void pq_free(ReqPQ *pq);
void pq_push(ReqPQ *pq, Request *r);
Request* pq_pop(ReqPQ *pq);
void pq_build_from_list(ReqPQ *pq, Request *list);

/* ---------------- Stack for Completed requests ---------------- */
Completed* loadCompleted(const char *file);
void saveCompleted(const char *file, Completed *top);
void pushCompleted(Completed **top, int req_id, const char *user, const char *amb, const char *hosp, int total_time);
Completed* popCompleted(Completed **top);
void freeCompleted(Completed *top);

/* ---------------- assigned.txt helpers ---------------- */
/* Each assigned line: <req_id> <userID> <location> <severity> <contact> <ambulanceID> <hospitalID> */
int find_and_remove_assigned(const char *assigned_file, int request_id, char *out_ambulance_id, char *out_hospital_id);
void mark_ambulance_available(const char *file, const char *ambulance_id);
int decrement_hospital_bed(const char *file, const char *hospital_id);

#endif /* RESOURCES_H */
