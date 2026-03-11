#ifndef USER_H
#define USER_H

void user_report_accident(const char *userID, const char *location, const char *severity, const char *contact);
void user_check_status(const char *userID); /* checks pending/assigned/completed */
void user_confirm_arrival(const char *request_id_str); 

#endif
