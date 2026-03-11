#include "user.h"
#include "resources.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --------------------------------------------------
   Submit accident report
-------------------------------------------------- */
void user_report_accident(const char *userID, const char *location,
                          const char *severity, const char *contact)
{
    Request *reqs = loadRequests("data/requests.txt");

    appendRequest(&reqs, userID, location, severity, contact);
    saveRequests("data/requests.txt", reqs);

    printf("\n========== Accident Report Submitted ==========\n");
    printf("User ID: %s\n", userID);
    printf("Location: %s\n", location);
    printf("Severity: %s\n", severity);
    printf("Contact: %s\n", contact);
    printf("Status: Pending admin approval.\n");
    printf("================================================\n");

    freeRequests(reqs);
}

/* --------------------------------------------------
   Check accident request status
-------------------------------------------------- */
void user_check_status(const char *userID)
{
    int found = 0;

    printf("\n===== Status for user %s =====\n", userID);

    /* ---------- Check pending ---------- */
    FILE *f = fopen("data/requests.txt", "r");
    if (f != NULL) {
        int id;
        char uid[32], loc[64], sev[16], contact[32];

        while (fscanf(f, "%d %31s %63s %15s %31s",
                      &id, uid, loc, sev, contact) == 5) {
            if (strcmp(uid, userID) == 0) {
                printf("⏳ Pending: Req %d at %s (Severity %s)\n",
                       id, loc, sev);
                found = 1;
            }
        }
        fclose(f);
    }

    /* ---------- Check assigned ---------- */
    f = fopen("data/assigned.txt", "r");
    if (f != NULL) {
        int id;
        char uid[32], loc[64], sev[16], contact[32];
        char amb[32], hosp[32];

        while (fscanf(f,
                      "%d %31s %63s %15s %31s %31s %31s",
                      &id, uid, loc, sev, contact, amb, hosp) == 7) {
            if (strcmp(uid, userID) == 0) {
                printf("🚑 Assigned: Req %d | Ambulance %s | Hospital %s\n",
                       id, amb, hosp);
                found = 1;
            }
        }
        fclose(f);
    }

    /* ---------- Check completed ---------- */
    f = fopen("data/completed.txt", "r");
    if (f != NULL) {
        int id, t;
        char uid[32], amb[32], hosp[32];

        while (fscanf(f, "%d %31s %31s %31s %d",
                      &id, uid, amb, hosp, &t) == 5) {
            if (strcmp(uid, userID) == 0) {
                printf("✅ Completed: Req %d | Ambulance %s | Hospital %s | Time %ds\n",
                       id, amb, hosp, t);
                found = 1;
            }
        }
        fclose(f);
    }

    if (!found) {
        printf("No active or past reports found for %s.\n", userID);
    }

    printf("===============================\n");
}

/* --------------------------------------------------
   Confirm arrival (legacy/manual)
-------------------------------------------------- */
void user_confirm_arrival(const char *request_id_str)
{
    int id = atoi(request_id_str);

    if (id <= 0) {
        printf("Invalid request ID.\n");
        return;
    }

    char amb[32] = {0};
    char hosp[32] = {0};

    int res = find_and_remove_assigned("data/assigned.txt", id, amb, hosp);

    if (res == 1) {
        decrement_hospital_bed("data/hospitals.txt", hosp);
        mark_ambulance_available("data/ambulances.txt", amb);

        printf("Confirmed arrival for request %d; ambulance %s freed.\n",
               id, amb);
    } else if (res == 0) {
        printf("No assignment found for %d\n", id);
    } else {
        printf("Error processing assigned file.\n");
    }
}
