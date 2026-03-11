#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin.h"
#include "user.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: rapid.exe <command> [args]\n");
        printf("Commands:\n");
        printf("  add_ambulance <id> <loc>\n");
        printf("  add_hospital <id> <loc> <beds>\n");
        printf("  update_status <id> <status>\n");
        printf("  view_ambulances\n");
        printf("  view_hospitals\n");
        printf("  view_requests\n");
        printf("  approve_request\n");
        printf("  find_ambulance <id>\n");
        printf("  view_completed\n");
        printf("  report <userID> <location> <severity> <contact>\n");
        printf("  check_status <userID>\n");
        printf("  confirm_arrival <req_id>\n");
        return 0;
    }

    if (strcmp(argv[1], "add_ambulance") == 0 && argc == 4) {
        admin_add_ambulance(argv[2], argv[3]);
    } 
    else if (strcmp(argv[1], "add_hospital") == 0 && argc == 5) {
        admin_add_hospital(argv[2], argv[3], atoi(argv[4]));
    }
    else if (strcmp(argv[1], "update_status") == 0 && argc == 4) {
        admin_update_ambulance_status(argv[2], argv[3]);
    } 
    else if (strcmp(argv[1], "view_ambulances") == 0) {
        admin_view_ambulances();
    } 
    else if (strcmp(argv[1], "view_hospitals") == 0) {
        admin_view_hospitals();
    } 
    else if (strcmp(argv[1], "view_requests") == 0) {
        admin_view_requests();
    } 
    else if (strcmp(argv[1], "approve_request") == 0) {
        admin_approve_request();
    } 
    else if (strcmp(argv[1], "find_ambulance") == 0 && argc == 3) {
        admin_find_ambulance(argv[2]);
    } 
    else if (strcmp(argv[1], "view_completed") == 0) {
        admin_view_completed();
    } 
    else if (strcmp(argv[1], "report") == 0 && argc == 6) {
        user_report_accident(argv[2], argv[3], argv[4], argv[5]);
    } 
    else if (strcmp(argv[1], "check_status") == 0 && argc == 3) {
        user_check_status(argv[2]);
    } 
    else if (strcmp(argv[1], "confirm_arrival") == 0 && argc == 3) {
        user_confirm_arrival(argv[2]);
    } 
    else {
        printf("Invalid command or wrong args. Run without arguments to see usage.\n");
    }

    return 0;
}
