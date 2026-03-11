#include "resources.h"
#include <stdlib.h>
#include <string.h>


/* ---------------- Ambulance list ---------------- */
Ambulance* loadAmbulances(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) return NULL;
    Ambulance *head = NULL, *tail = NULL;
    char id[32], loc[64], status[16];
    while (fscanf(f, "%31s %63s %15s", id, loc, status) == 3) {
        Ambulance *a = (Ambulance*)malloc(sizeof(Ambulance));
        strcpy(a->id, id); 
        strcpy(a->location, loc); 
        strcpy(a->status, status);
        a->next = NULL;
        if (!head) head = tail = a; 
        else { tail->next = a; 
            tail = a; 
        }
    }
    fclose(f);
    return head;
}

void saveAmbulances(const char *file, Ambulance *head) {
    FILE *f = fopen(file, "w");
    if (!f) return;
    for (Ambulance *a = head; a; a = a->next)
        fprintf(f, "%s %s %s\n", a->id, a->location, a->status);
    fclose(f);
}

void appendAmbulance(Ambulance **head, const char *id, const char *loc, const char *status) {
    Ambulance *a = (Ambulance*)malloc(sizeof(Ambulance));
    strcpy(a->id, id);
    strcpy(a->location, loc);
    strcpy(a->status, status);
    a->next = *head;
    *head = a;
}

void freeAmbulances(Ambulance *head) {
    while (head) {
        Ambulance *t = head; 
        head = head->next; 
        free(t);
    }
}

/* ---------------- Hospitals ---------------- */
Hospital* loadHospitals(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) return NULL;
    Hospital *head = NULL, *tail = NULL;
    char id[32], loc[64]; int beds;
    while (fscanf(f, "%31s %63s %d", id, loc, &beds) == 3) {
        Hospital *h = (Hospital*)malloc(sizeof(Hospital));
        strcpy(h->id, id); 
        strcpy(h->location, loc);
         h->beds = beds; 
         h->next = NULL;
        if (!head) head = tail = h; 
        else {
             tail->next = h; 
             tail = h; 
        }
    }
    fclose(f);
    return head;
}

void saveHospitals(const char *file, Hospital *head) {
    FILE *f = fopen(file, "w");
    if (!f) return;
    for (Hospital *h = head; h; h = h->next) fprintf(f, "%s %s %d\n", h->id, h->location, h->beds);
    fclose(f);
}

void appendHospital(Hospital **head, const char *id, const char *loc, int beds) {
    Hospital *h = (Hospital*)malloc(sizeof(Hospital));
    strcpy(h->id, id); 
    strcpy(h->location, loc); 
    h->beds = beds; 
    h->next = *head; 
    *head = h;
}

void freeHospitals(Hospital *head) {
    while (head) { Hospital *t = head; head = head->next; free(t); }
}

/* ---------------- Requests ---------------- */
Request* loadRequests(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) return NULL;
    Request *head = NULL, *tail = NULL;
    while (1) {
        Request *r = (Request*)malloc(sizeof(Request));
        if (!r) break;
        if (fscanf(f, "%d %31s %63s %15s %31s", &r->id, r->userID, r->location, r->severity, r->contact) != 5) {
            free(r); break;
        }
        r->next = NULL;
        if (!head) head = tail = r; else { tail->next = r; tail = r; }
    }
    fclose(f);
    return head;
}

void saveRequests(const char *file, Request *head) {
    FILE *f = fopen(file, "w");
    if (!f) return;
    for (Request *r = head; r; r = r->next)
        fprintf(f, "%d %s %s %s %s\n", r->id, r->userID, r->location, r->severity, r->contact);
    fclose(f);
}

static int getNextRequestID_local(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) return 1;
    int max = 0, id; char u[32], loc[64], sev[16], ct[32];
    while (fscanf(f, "%d %31s %63s %15s %31s", &id, u, loc, sev, ct) == 5) if (id > max) max = id;
    fclose(f);
    return max + 1;
}

void appendRequest(Request **head, const char *userID, const char *loc, const char *sev, const char *contact) {
    Request *r = (Request*)malloc(sizeof(Request));
    r->id = getNextRequestID_local("data/requests.txt");
    strcpy(r->userID, userID); strcpy(r->location, loc); strcpy(r->severity, sev); strcpy(r->contact, contact);
    r->next = NULL;
    if (!*head) *head = r; else { Request *t = *head; while (t->next) t = t->next; t->next = r; }
}

void freeRequests(Request *head) {
    while (head) { Request *t = head; head = head->next; free(t); }
}

/* ---------------- BST for ambulances ---------------- */
AmbNode* insertAmbBST(AmbNode *root, const Ambulance *a) {
    if (!root) {
        AmbNode *n = (AmbNode*)malloc(sizeof(AmbNode));
        n->data = *a; n->left = n->right = NULL; return n;
    }
    if (strcmp(a->id, root->data.id) < 0) root->left = insertAmbBST(root->left, a);
    else root->right = insertAmbBST(root->right, a);
    return root;
}

AmbNode* buildAmbulanceBST(const char *file) {
    Ambulance *list = loadAmbulances(file);
    AmbNode *root = NULL;
    for (Ambulance *a = list; a; a = a->next) root = insertAmbBST(root, a);
    freeAmbulances(list);
    return root;
}

AmbNode* searchAmbBST(AmbNode *root, const char *id) {
    if (!root) return NULL;
    int cmp = strcmp(id, root->data.id);
    if (cmp == 0) return root;
    if (cmp < 0) return searchAmbBST(root->left, id);
    return searchAmbBST(root->right, id);
}

void freeAmbBST(AmbNode *root) {
    if (!root) return;
    freeAmbBST(root->left);
    freeAmbBST(root->right);
    free(root);
}

/* ---------------- Priority Queue for Requests ---------------- */
static int req_priority(Request *r) {
    return (strcmp(r->severity, "serious") == 0) ? 1 : 2;
}

ReqPQ* pq_create(int cap) {
    ReqPQ *pq = (ReqPQ*)malloc(sizeof(ReqPQ));
    pq->arr = (Request**)malloc(sizeof(Request*) * (cap+2));
    pq->size = 0; pq->cap = cap+2; return pq;
}
void pq_free(ReqPQ *pq) { if (!pq) return; free(pq->arr); free(pq); }

static int pq_cmp(Request *a, Request *b) {
    int pa = req_priority(a), pb = req_priority(b);
    if (pa != pb) return pa < pb; /* lower numeric = higher priority */
    return a->id < b->id;
}

static void pq_heapify_up(ReqPQ *pq, int idx) {
    while (idx > 0) {
        int p = (idx - 1) / 2;
        if (!pq_cmp(pq->arr[idx], pq->arr[p])) break;
        Request *tmp = pq->arr[idx]; 
        pq->arr[idx] = pq->arr[p]; 
        pq->arr[p] = tmp;
        idx = p;
    }
}
static void pq_heapify_down(ReqPQ *pq, int idx) {
    while (1) {
        int l = idx*2 + 1, r = idx*2 + 2, best = idx;
        if (l < pq->size && pq_cmp(pq->arr[l], pq->arr[best])) best = l;
        if (r < pq->size && pq_cmp(pq->arr[r], pq->arr[best])) best = r;
        if (best == idx) break;
        Request *tmp = pq->arr[idx]; pq->arr[idx] = pq->arr[best]; pq->arr[best] = tmp;
        idx = best;
    }
}

void pq_push(ReqPQ *pq, Request *r) {
    if (pq->size >= pq->cap) {
        pq->cap *= 2;
        pq->arr = (Request**)realloc(pq->arr, sizeof(Request*) * pq->cap);
    }
    pq->arr[pq->size++] = r;
    pq_heapify_up(pq, pq->size - 1);
}

Request* pq_pop(ReqPQ *pq) {
    if (pq->size == 0) return NULL;
    Request *res = pq->arr[0];
    pq->arr[0] = pq->arr[--pq->size];
    pq_heapify_down(pq, 0);
    return res;
}

void pq_build_from_list(ReqPQ *pq, Request *list) {
    for (Request *r = list; r; r = r->next) pq_push(pq, r);
}

/* ---------------- Completed stack ---------------- */
Completed* loadCompleted(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) return NULL;
    Completed *top = NULL;
    int id, t; char user[32], amb[32], hosp[32];
    while (fscanf(f, "%d %31s %31s %31s %d", &id, user, amb, hosp, &t) == 5) {
        pushCompleted(&top, id, user, amb, hosp, t);
    }
    fclose(f);
    return top;
}

void saveCompleted(const char *file, Completed *top) {
    FILE *f = fopen(file, "w");
    if (!f) return;
    /* Need to print from bottom to top; easiest is to reverse into array */
    int count = 0;
    for (Completed *c = top; c; c = c->next) ++count;
    Completed **arr = (Completed**)malloc(sizeof(Completed*) * count);
    int idx = count - 1;
    for (Completed *c = top; c; c = c->next) arr[idx--] = c;
    for (int i = 0; i < count; ++i) {
        Completed *c = arr[i];
        fprintf(f, "%d %s %s %s %d\n", c->req_id, c->userID, c->amb_id, c->hosp_id, c->total_time);
    }
    free(arr);
    fclose(f);
}

void pushCompleted(Completed **top, int req_id, const char *user, const char *amb, const char *hosp, int total_time) {
    Completed *c = (Completed*)malloc(sizeof(Completed));
    c->req_id = req_id;
    strcpy(c->userID, user); strcpy(c->amb_id, amb); strcpy(c->hosp_id, hosp);
    c->total_time = total_time;
    c->next = *top; *top = c;
}

Completed* popCompleted(Completed **top) {
    if (!*top) return NULL;
    Completed *c = *top; *top = c->next; return c;
}

void freeCompleted(Completed *top) {
    while (top) { Completed *t = top; top = top->next; free(t); }
}

/* ---------------- assigned.txt helpers ---------------- */
int find_and_remove_assigned(const char *assigned_file, int request_id, char *out_ambulance_id, char *out_hospital_id) {
    FILE *f = fopen(assigned_file, "r");
    if (!f) return -1;
    char line[512];
    char **lines = NULL; size_t cnt = 0; int found = 0;
    while (fgets(line, sizeof(line), f)) {
        int id; char uid[32], loc[64], sev[16], contact[32], amb[32], hosp[32];
        int sc = sscanf(line, "%d %31s %63s %15s %31s %31s %31s", &id, uid, loc, sev, contact, amb, hosp);
        if (sc == 7 && id == request_id) {
            found = 1;
            if (out_ambulance_id) strcpy(out_ambulance_id, amb);
            if (out_hospital_id) strcpy(out_hospital_id, hosp);
            continue;
        }
        lines = (char**)realloc(lines, sizeof(char*) * (cnt+1));
        lines[cnt] = strdup(line); cnt++;
    }
    fclose(f);
    if (!found) {
        for (size_t i=0;i<cnt;i++){
              free(lines[i]);
        } 
        free(lines);
        return 0;
    }
    f = fopen(assigned_file, "w");
    if (!f) { 
        for (size_t i=0;i<cnt;i++){
            free(lines[i]); 
        }
        free(lines); 
        return -1; 
    }
    for (size_t i=0;i<cnt;i++) { fputs(lines[i], f); free(lines[i]); }
    free(lines); fclose(f);
    return 1;
}

void mark_ambulance_available(const char *file, const char *ambulance_id) {
    Ambulance *alist = loadAmbulances(file);
    for (Ambulance *a = alist; a; a = a->next) if (strcmp(a->id, ambulance_id) == 0) strcpy(a->status, "available");
    saveAmbulances(file, alist); freeAmbulances(alist);
}

int decrement_hospital_bed(const char *file, const char *hospital_id) {
    Hospital *hlist = loadHospitals(file);
    if (!hlist) return -1;
    int res = -1;
    for (Hospital *h = hlist; h; h = h->next) {
        if (strcmp(h->id, hospital_id) == 0) {
            if (h->beds > 0) { h->beds -= 1; res = 1; }
            else res = 0;
            break;
        }
    }
    saveHospitals(file, hlist);
    freeHospitals(hlist);
    return res;
}
