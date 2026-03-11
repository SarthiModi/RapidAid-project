#include "admin.h"
#include "resources.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#define SLEEP(s) Sleep((s) * 1000)
#else
#include <unistd.h>
#define SLEEP(s) sleep(s)
#endif

/* ====================== BASIC OPERATIONS ====================== */

void admin_add_ambulance(const char *id, const char *loc)
{
    Ambulance *list = loadAmbulances("data/ambulances.txt");
    appendAmbulance(&list, id, loc, "available");
    saveAmbulances("data/ambulances.txt", list);
    printf("Ambulance %s added at %s (available)\n", id, loc);
    freeAmbulances(list);
}

void admin_add_hospital(const char *id, const char *loc, int beds)
{
    Hospital *list = loadHospitals("data/hospitals.txt");
    appendHospital(&list, id, loc, beds);
    saveHospitals("data/hospitals.txt", list);
    printf("Hospital %s added at %s (%d beds)\n", id, loc, beds);
    freeHospitals(list);
}

void admin_update_ambulance_status(const char *id, const char *new_status)
{
    Ambulance *list = loadAmbulances("data/ambulances.txt");

    for (Ambulance *a = list; a; a = a->next) {
        if (strcmp(a->id, id) == 0) {
            strcpy(a->status, new_status);
            break;
        }
    }

    saveAmbulances("data/ambulances.txt", list);
    printf("Ambulance %s status updated to %s\n", id, new_status);
    freeAmbulances(list);
}

/* ====================== VIEWS ====================== */

void admin_view_ambulances(void)
{
    Ambulance *list = loadAmbulances("data/ambulances.txt");

    if (!list) {
        printf("No ambulances found.\n");
        return;
    }

    printf("===== Ambulances =====\n");

    for (Ambulance *a = list; a; a = a->next) {
        printf("%s %s %s\n", a->id, a->location, a->status);
    }

    printf("======================\n");
    freeAmbulances(list);
}

void admin_view_hospitals(void)
{
    Hospital *list = loadHospitals("data/hospitals.txt");

    if (!list) {
        printf("No hospitals found.\n");
        return;
    }

    printf("===== Hospitals =====\n");

    for (Hospital *h = list; h; h = h->next) {
        printf("%s %s %d\n", h->id, h->location, h->beds);
    }

    printf("=====================\n");
    freeHospitals(list);
}

void admin_view_requests(void)
{
    Request *reqs = loadRequests("data/requests.txt");

    if (!reqs) {
        printf("No pending accident reports found.\n");
        return;
    }

    printf("===== Pending Requests =====\n");

    for (Request *r = reqs; r; r = r->next) {
        printf("Request ID: %d | User: %s | Location: %s | Severity: %s | Contact: %s\n",
               r->id, r->userID, r->location, r->severity, r->contact);
    }

    printf("============================\n");
    freeRequests(reqs);
}

/* =================== BST SEARCH ==================== */

void admin_find_ambulance(const char *id)
{
    AmbNode *root = buildAmbulanceBST("data/ambulances.txt");
    AmbNode *res = searchAmbBST(root, id);

    if (!res) {
        printf("Ambulance %s NOT found.\n", id);
        freeAmbBST(root);
        return;
    }

    printf("Ambulance found:\nID: %s\nLocation: %s\nStatus: %s\n",
           res->data.id, res->data.location, res->data.status);

    freeAmbBST(root);
}

/* =================== COMPLETED STACK ==================== */

void admin_view_completed(void)
{
    Completed *top = loadCompleted("data/completed.txt");

    if (!top) {
        printf("No completed records.\n");
        return;
    }

    printf("===== Completed Requests (top = most recent) =====\n");

    for (Completed *c = top; c; c = c->next) {
        printf("Req %d | User %s | Amb %s | Hosp %s | Time %ds\n",
               c->req_id, c->userID, c->amb_id, c->hosp_id, c->total_time);
    }

    printf("=================================================\n");
    freeCompleted(top);
}

/* ==================== APPROVE REQUEST (PQ + SIMULATION) ==================== */

void admin_approve_request(void)
{
    Request *reqs = loadRequests("data/requests.txt");

    if (!reqs) {
        printf("No pending requests.\n");
        return;
    }

    /* Build PQ */
    ReqPQ *pq = pq_create(32);
    pq_build_from_list(pq, reqs);

    Request *chosen = pq_pop(pq);

    if (!chosen) {
        printf("No suitable request.\n");
        pq_free(pq);
        freeRequests(reqs);
        return;
    }

    printf("\nApproving Request ID: %d | User: %s | Location: %s | Severity: %s\n",
           chosen->id, chosen->userID, chosen->location, chosen->severity);

    /* Load graph + resources */
    loadGraph("data/graph.txt");

    Ambulance *alist = loadAmbulances("data/ambulances.txt");
    Hospital *hlist = loadHospitals("data/hospitals.txt");

    /* Find nearest available ambulance */
    int bestDist = INT_MAX;
    Ambulance *bestA = NULL;
    char bestRoute[1024];
    bestRoute[0] = '\0';

    for (Ambulance *a = alist; a; a = a->next) {
        if (strcmp(a->status, "available") != 0) {
            continue;
        }

        char route[1024];
        route[0] = '\0';

        int d = dijkstra_with_path(a->location, chosen->location, route, sizeof(route));

        if (d < bestDist) {
            bestDist = d;
            bestA = a;
            strncpy(bestRoute, route, sizeof(bestRoute) - 1);
        }
    }

    if (!bestA) {
        printf("No available ambulances!\n");
        freeAmbulances(alist);
        freeHospitals(hlist);
        pq_free(pq);
        freeRequests(reqs);
        return;
    }

    /* Find nearest hospital */
    int bestHdist = INT_MAX;
    Hospital *bestH = NULL;
    char hRoute[1024];
    hRoute[0] = '\0';

    for (Hospital *h = hlist; h; h = h->next) {
        char route2[1024];
        route2[0] = '\0';

        int d2 = dijkstra_with_path(chosen->location, h->location, route2, sizeof(route2));

        if (d2 < bestHdist) {
            bestHdist = d2;
            bestH = h;
            strncpy(hRoute, route2, sizeof(hRoute) - 1);
        }
    }

    if (!bestH) {
        printf("No hospitals found.\n");
        freeAmbulances(alist);
        freeHospitals(hlist);
        pq_free(pq);
        freeRequests(reqs);
        return;
    }

    printf("Assigned Ambulance: %s (%s) | Route: %s | Dist: %d km\n",
           bestA->id, bestA->location, bestRoute, bestDist);

    printf("Destination Hospital: %s (%s) | Dist from accident: %d km | Route: %s\n",
           bestH->id, bestH->location, bestHdist, hRoute);

    /* Mark ambulance busy */
    strcpy(bestA->status, "busy");
    saveAmbulances("data/ambulances.txt", alist);

    /* Save to assigned.txt */
    FILE *fa = fopen("data/assigned.txt", "a");

    if (fa) {
        fprintf(fa, "%d %s %s %s %s %s %s\n",
                chosen->id, chosen->userID, chosen->location,
                chosen->severity, chosen->contact,
                bestA->id, bestH->id);
        fclose(fa);
    }

    /* Remove chosen from request list */
    int reqID = chosen->id;
    char savedUser[32];
    strcpy(savedUser, chosen->userID);

    Request *newhead = NULL;

    for (Request *t = reqs; t;) {
        Request *next = t->next;

        if (t->id == chosen->id) {
            free(t);
        } else {
            t->next = NULL;

            if (!newhead) {
                newhead = t;
            } else {
                Request *x = newhead;
                while (x->next) {
                    x = x->next;
                }
                x->next = t;
            }
        }

        t = next;
    }

    saveRequests("data/requests.txt", newhead);

    printf("Dispatching ambulance... (simulation)\n");

    int travel1 = (bestDist == INT_MAX ? 0 : bestDist) * 5;
    int travel2 = (bestHdist == INT_MAX ? 0 : bestHdist) * 5;
    int totalTime = travel1 + travel2;

    printf("Estimated time: %d seconds (to accident %d s, to hospital %d s)\n",
           totalTime, travel1, travel2);

    if (travel1 > 0) {
        printf("Amb enroute to accident...\n");
        printf("Amb reached accident.\n");
    }

    if (travel2 > 0) {
        printf("Amb transporting to hospital...\n");
        printf("Amb reached hospital.\n");
    }

    int bedRes = decrement_hospital_bed("data/hospitals.txt", bestH->id);

    if (bedRes == 1) {
        printf("Hospital %s bed decremented.\n", bestH->id);
    }

    mark_ambulance_available("data/ambulances.txt", bestA->id);

    /* Push onto completed stack */
    Completed *top = loadCompleted("data/completed.txt");
    pushCompleted(&top, reqID, savedUser, bestA->id, bestH->id, totalTime);
    saveCompleted("data/completed.txt", top);
    freeCompleted(top);

    printf("✔ Request %d completed. Ambulance %s now available.\n", reqID, bestA->id);

    freeAmbulances(alist);
    freeHospitals(hlist);
    pq_free(pq);
    freeRequests(newhead);
}

