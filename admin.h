#ifndef ADMIN_H
#define ADMIN_H

/* admin operations */
void admin_add_ambulance(const char *id, const char *loc);
void admin_add_hospital(const char *id, const char *loc, int beds);
void admin_update_ambulance_status(const char *id, const char *new_status);

/* views */
void admin_view_ambulances(void);
void admin_view_hospitals(void);
void admin_view_requests(void);

/* advanced */
void admin_approve_request(void);

/* new DS features */
void admin_find_ambulance(const char *id);    /* BST search */
void admin_view_completed(void);             /* show completed stack */

#endif
