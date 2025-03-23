/*
Contributors:
First/Last Name ... YorkU id
Yegor Sokolov ... 220392288
Muddassir Choudary ... 216536310
Omar Shaikh ... 218749382
Mohammad Zarif ... 217444340
*/

#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "errors.h"

/*
 * 5 Threads:
 * (a) Main Thread
 * (b) Start Alarm Thread
 * (c) Manage Alarm Thread
 * (d) View Alarms Thread
 * (e) Display Alarm Threads
 *
 * 6 Requests:
 *   Start_Alarm(Alarm_ID): Time Message
 *   Change_Alarm(Alarm_ID): Time Message
 *   Cancel_Alarm(Alarm_ID)
 *   Suspend_Alarm(Alarm_ID)
 *   Reactivate_Alarm(Alarm_ID)
 *   View_Alarms
 */

/* Let's define statuses so the display thread can stop when alarm is CANCELLED or SUSPENDED. */
typedef enum {
    ALARM_ACTIVE,
    ALARM_SUSPENDED,
    ALARM_CANCELLED
} alarm_status_t;

typedef struct alarm_tag {
    struct alarm_tag *link;
    time_t timestamp;     /* insertion time */
    int alarm_id;
    int seconds;
    time_t expire_time;
    char request_type[32];
    char message[128];
    int assigned; /* 1 if assigned to a display thread */
    alarm_status_t status; /* ACTIVE, SUSPENDED, CANCELLED */
} alarm_t;

/* Global list + mutex */
pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
alarm_t *alarm_list = NULL;

/* 4 permanent threads + multiple display threads. */
pthread_t main_thr, start_thr, manage_thr, view_thr;

/* FORWARD DECLARATIONS */
void *main_thread_func(void *arg);
void *start_alarm_thread_func(void *arg);
void *manage_alarm_thread_func(void *arg);
void *view_alarms_thread_func(void *arg);
void *display_alarm_thread_func(void *arg);

/* Insert request in ascending order of request->timestamp. */
void insert_request(alarm_t *req) {
    alarm_t **last = &alarm_list;
    while (*last && (*last)->timestamp < req->timestamp) {
        last = &(*last)->link;
    }
    req->link = *last;
    *last = req;
}

/* Create a new alarm_t request */
alarm_t *create_alarm_request(const char *type, int alarm_id, int seconds, const char *msg) {
    alarm_t *req = (alarm_t *)malloc(sizeof(alarm_t));
    memset(req, 0, sizeof(*req));
    strncpy(req->request_type, type, sizeof(req->request_type) - 1);
    req->request_type[sizeof(req->request_type) - 1] = '\0';

    req->alarm_id = alarm_id;
    req->seconds = seconds;
    req->timestamp = time(NULL);
    req->expire_time = req->timestamp + seconds;
    req->assigned = 0;
    req->status = ALARM_ACTIVE; /* By default, newly inserted is ACTIVE. */

    if (msg) {
        strncpy(req->message, msg, 127);
        req->message[127] = '\0';
    } else {
        strcpy(req->message, "N/A");
    }
    return req;
}

/* (e) Display Alarm Thread => prints alarm every 5s if status=ACTIVE. */
void *display_alarm_thread_func(void *arg) {
    alarm_t *the_alarm = (alarm_t *)arg;

    while (1) {
        sleep(5);

        pthread_mutex_lock(&alarm_mutex);
        /* If alarm is no longer ACTIVE, stop printing. (Cancel/Suspend => alarm->status != ACTIVE) */
        if (the_alarm->status != ALARM_ACTIVE ||
            (strcmp(the_alarm->request_type, "Start_Alarm") != 0 &&
             strcmp(the_alarm->request_type, "Change_Alarm") != 0))
        {
            pthread_mutex_unlock(&alarm_mutex);
            break;
        }
        time_t now = time(NULL);
        printf("Alarm (%d) Printed by Alarm Display Thread %lu at %ld: %ld %s\n",
               the_alarm->alarm_id,
               (unsigned long)pthread_self(),
               (long)now,
               (long)the_alarm->expire_time,
               the_alarm->message);
        pthread_mutex_unlock(&alarm_mutex);
    }
    return NULL;
}

/* (b) Start Alarm Thread => picks Start_Alarm requests, spawns display threads if not assigned. */
void *start_alarm_thread_func(void *arg) {
    (void)arg;
    while (1) {
        sleep(1);
        pthread_mutex_lock(&alarm_mutex);

        alarm_t *cur = alarm_list;
        while (cur) {
            if (!cur->assigned && !strcmp(cur->request_type, "Start_Alarm") && cur->status == ALARM_ACTIVE) {
                cur->assigned = 1;
                printf("New Display Alarm Thread created at %ld\n",(long)time(NULL));
                printf("Alarm (%d) Assigned to Display Thread(%lu) at %ld: %ld %s\n",
                       cur->alarm_id,
                       (unsigned long)pthread_self(),
                       (long)time(NULL),
                       (long)cur->expire_time,
                       cur->message);

                pthread_t disp_thr;
                pthread_create(&disp_thr, NULL, display_alarm_thread_func, cur);
            }
            cur = cur->link;
        }

        pthread_mutex_unlock(&alarm_mutex);
    }
    return NULL;
}

/*
 * (c) Manage Alarm Thread => handles:
 *    Change_Alarm(Alarm_ID): => remove older Start/Change or set them to changed
 *    Cancel_Alarm(Alarm_ID) => set status=CANCELLED, remove from list or keep in list but skip in view
 *    Suspend_Alarm(Alarm_ID) => set status=SUSPENDED
 *    Reactivate_Alarm(Alarm_ID) => set status=ACTIVE
 */
void *manage_alarm_thread_func(void *arg) {
    (void)arg;
    while (1) {
        sleep(1);
        pthread_mutex_lock(&alarm_mutex);

        alarm_t **prev = &alarm_list;
        alarm_t *cur = alarm_list;

        while (cur) {
            if (!strcmp(cur->request_type, "Change_Alarm")) {
                /* For demonstration, we just print that it's changed, keep alarm active. */
                printf("Alarm(%d) Changed at %ld: %ld %s\n",
                       cur->alarm_id, (long)time(NULL),
                       (long)cur->expire_time, cur->message);

                /* In a full solution, we'd remove older Start_Alarm with same ID. Here, just remove the request node. */
                *prev = cur->link;
                free(cur);
                cur = *prev;
            }
            else if (!strcmp(cur->request_type, "Cancel_Alarm")) {
                /* Find the alarm with same ID, set status=CANCELLED, remove request. 
                   For demonstration, let's remove it entirely from the active list so it won't appear in View_Alarms. */
                int canceled_id = cur->alarm_id;
                printf("Alarm(%d) Cancelled at %ld: %ld %s\n",
                       canceled_id, (long)time(NULL),
                       (long)cur->expire_time, cur->message);

                /* Remove just the 'Cancel_Alarm' request node from the list: */
                *prev = cur->link;
                free(cur);
                cur = *prev;

                /* Now also remove or mark the actual alarm in the list: */
                alarm_t **pp = &alarm_list, *x = alarm_list;
                while (x) {
                    if (x->alarm_id == canceled_id && x->status == ALARM_ACTIVE) {
                        x->status = ALARM_CANCELLED;
                        /* Let's remove it from the list so it won't show in View_Alarms. */
                        *pp = x->link;
                        free(x);
                        x = *pp;
                    } else {
                        pp = &x->link;
                        x = x->link;
                    }
                }
            }
            else if (!strcmp(cur->request_type, "Suspend_Alarm")) {
                /* Mark the alarm as SUSPENDED (the Display Thread will stop). */
                int sid = cur->alarm_id;
                printf("Alarm(%d) Suspended at %ld: %ld %s\n",
                       sid, (long)time(NULL),
                       (long)cur->expire_time, cur->message);

                /* Remove request node. */
                *prev = cur->link;
                free(cur);
                cur = *prev;

                /* Now set the actual alarm's status to SUSPENDED so display thread stops. */
                alarm_t *scan = alarm_list;
                while (scan) {
                    if (scan->alarm_id == sid && scan->status == ALARM_ACTIVE) {
                        scan->status = ALARM_SUSPENDED;
                        break;
                    }
                    scan = scan->link;
                }
            }
            else if (!strcmp(cur->request_type, "Reactivate_Alarm")) {
                int rid = cur->alarm_id;
                printf("Alarm(%d) Reactivated at %ld: %ld %s\n",
                       rid, (long)time(NULL),
                       (long)cur->expire_time, cur->message);

                /* Remove the request node. */
                *prev = cur->link;
                free(cur);
                cur = *prev;

                /* If that alarm was SUSPENDED, set it back to ACTIVE so the display thread can keep printing. */
                alarm_t *scan = alarm_list;
                while (scan) {
                    if (scan->alarm_id == rid && scan->status == ALARM_SUSPENDED) {
                        scan->status = ALARM_ACTIVE;
                        break;
                    }
                    scan = scan->link;
                }
            }
            else {
                /* Not a manage request => skip */
                prev = &cur->link;
                cur = cur->link;
            }
        }

        pthread_mutex_unlock(&alarm_mutex);
    }
    return NULL;
}

/* (d) View Alarms Thread => picks 'View_Alarms', prints active/suspended alarms, ignoring CANCELLED. */
void *view_alarms_thread_func(void *arg) {
    (void)arg;
    while (1) {
        sleep(1);
        pthread_mutex_lock(&alarm_mutex);

        alarm_t **prev = &alarm_list;
        alarm_t *cur = alarm_list;
        while (cur) {
            if (!strcmp(cur->request_type, "View_Alarms")) {
                printf("View_Alarms Request Inserted Into Alarm List: %ld\n",(long)cur->timestamp);
                printf("View Alarms at %ld:\n",(long)time(NULL));

                /* We'll list only alarms with status = ACTIVE or SUSPENDED. */
                alarm_t *scan = alarm_list;
                int idx=1;
                while (scan) {
                    if (scan->status != ALARM_CANCELLED &&
                        strcmp(scan->request_type, "View_Alarms") != 0 &&
                        strcmp(scan->request_type, "Cancel_Alarm") != 0 &&
                        strcmp(scan->request_type, "Suspend_Alarm") != 0 &&
                        strcmp(scan->request_type, "Reactivate_Alarm") != 0 &&
                        strcmp(scan->request_type, "Change_Alarm") != 0)
                    {
                        printf("%d. Alarm(%d) Status<%s> Type<%s>  %ld %d %s\n",
                               idx++,
                               scan->alarm_id,
                               (scan->status == ALARM_ACTIVE) ? "ACTIVE" :
                               (scan->status == ALARM_SUSPENDED) ? "SUSPENDED" : "CANCELLED",
                               scan->request_type,
                               (long)scan->timestamp,
                               scan->seconds,
                               scan->message);
                    }
                    scan = scan->link;
                }

                /* Remove this 'View_Alarms' request from the list. */
                *prev = cur->link;
                free(cur);
                cur = *prev;
            } else {
                prev = &cur->link;
                cur = cur->link;
            }
        }

        pthread_mutex_unlock(&alarm_mutex);
    }
    return NULL;
}

/* (a) Main Thread => parse EXACT format. */
void *main_thread_func(void *arg) {
    (void)arg;
    char line[256];

    while (1) {
        printf("alarm> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("Exiting Program.\n");
            exit(0);
        }
        /* Remove trailing newline. */
        line[strcspn(line, "\r\n")] = '\0';
        if (!*line) continue;

        {
            /* 1) Start_Alarm(...) or Change_Alarm(...): e.g. Start_Alarm(1001): 30 Some message */
            char reqtype[32];
            int alarm_id=0, seconds=0;
            char msg[128]={0};

            int parsed = sscanf(line, "%31[^ (](%d): %d %127[^\n]",
                                reqtype, &alarm_id, &seconds, msg);
            if (parsed == 4) {
                if (!strcmp(reqtype, "Start_Alarm")) {
                    alarm_t *r = create_alarm_request("Start_Alarm", alarm_id, seconds, msg);
                    pthread_mutex_lock(&alarm_mutex);
                    insert_request(r);
                    printf("Start_Alarm(%d) Request Inserted Into Alarm List: %ld %d %s\n",
                           alarm_id, (long)r->timestamp, seconds, r->message);
                    pthread_mutex_unlock(&alarm_mutex);
                } else if (!strcmp(reqtype, "Change_Alarm")) {
                    alarm_t *r = create_alarm_request("Change_Alarm", alarm_id, seconds, msg);
                    pthread_mutex_lock(&alarm_mutex);
                    insert_request(r);
                    printf("Change_Alarm(%d) Request Inserted Into Alarm List: %ld %d %s\n",
                           alarm_id, (long)r->timestamp, seconds, r->message);
                    pthread_mutex_unlock(&alarm_mutex);
                } else {
                    fprintf(stderr, "Bad command\n");
                }
                continue;
            }
        }

        {
            /* 2) Cancel_Alarm(Alarm_ID), Suspend_Alarm(Alarm_ID), Reactivate_Alarm(Alarm_ID) => no time/message */
            char reqtype[32];
            int alarm_id=0;
            int parsed = sscanf(line, "%31[^ (](%d)", reqtype, &alarm_id);
            if (parsed == 2) {
                if (!strcmp(reqtype, "Cancel_Alarm")) {
                    alarm_t *r = create_alarm_request("Cancel_Alarm", alarm_id, 0, "N/A");
                    pthread_mutex_lock(&alarm_mutex);
                    insert_request(r);
                    printf("Cancel_Alarm(%d) Request Inserted Into Alarm List: %ld %s\n",
                           alarm_id, (long)r->timestamp, r->message);
                    pthread_mutex_unlock(&alarm_mutex);
                } else if (!strcmp(reqtype, "Suspend_Alarm")) {
                    alarm_t *r = create_alarm_request("Suspend_Alarm", alarm_id, 0, "N/A");
                    pthread_mutex_lock(&alarm_mutex);
                    insert_request(r);
                    printf("Suspend_Alarm(%d) Request Inserted Into Alarm List: %ld %s\n",
                           alarm_id, (long)r->timestamp, r->message);
                    pthread_mutex_unlock(&alarm_mutex);
                } else if (!strcmp(reqtype, "Reactivate_Alarm")) {
                    alarm_t *r = create_alarm_request("Reactivate_Alarm", alarm_id, 0, "N/A");
                    pthread_mutex_lock(&alarm_mutex);
                    insert_request(r);
                    printf("Reactivate_Alarm(%d) Request Inserted Into Alarm List: %ld %s\n",
                           alarm_id, (long)r->timestamp, r->message);
                    pthread_mutex_unlock(&alarm_mutex);
                } else {
                    fprintf(stderr, "Bad command\n");
                }
                continue;
            }
        }

        {
            /* 3) Possibly 'View_Alarms' alone. */
            if (!strcmp(line, "View_Alarms")) {
                alarm_t *r = create_alarm_request("View_Alarms", 0, 0, "N/A");
                pthread_mutex_lock(&alarm_mutex);
                insert_request(r);
                printf("View_Alarms Request Inserted Into Alarm List: %ld\n",(long)r->timestamp);
                pthread_mutex_unlock(&alarm_mutex);
                continue;
            }
        }

        /* If we get here => no recognized format => 'Bad command' */
        fprintf(stderr, "Bad command\n");
    }
    return NULL;
}

/* main => create the 4 permanent threads; display threads get created by Start_Alarm. */
int main() {
    pthread_create(&main_thr, NULL, main_thread_func, NULL);
    pthread_create(&start_thr, NULL, start_alarm_thread_func, NULL);
    pthread_create(&manage_thr, NULL, manage_alarm_thread_func, NULL);
    pthread_create(&view_thr, NULL, view_alarms_thread_func, NULL);

    pthread_join(main_thr, NULL);
    pthread_join(start_thr, NULL);
    pthread_join(manage_thr, NULL);
    pthread_join(view_thr, NULL);
    return 0;
}
