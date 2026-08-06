#define _CRT_SECURE_NO_WARNINGS 1

/* ========== src/model.h ========== */
#ifndef CAINIAO_MODEL_H
#define CAINIAO_MODEL_H

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#define MAX_NAME 20
#define MAX_ACCOUNT 20
#define MAX_PASSWORD 20
#define MAX_PHONE 20
#define MAX_CODE 12
#define MAX_MESSAGE_CONTENT 80
#define MAX_MESSAGES 30
#define MAX_MANAGER_ACCOUNT 20
#define MAX_MANAGER_PASSWORD 20

#define PKG_STATE_IN_TRANSIT (-1)
#define PKG_STATE_PICKED 0
#define PKG_STATE_IN_STOCK 1

#define PKG_CONDITION_DAMAGED 0
#define PKG_CONDITION_INTACT 1

#define TRANSIT_STATE_NONE (-1)
#define TRANSIT_STATE_SHIPPING 0
#define TRANSIT_STATE_ARRIVED 1

#define OVERDUE_NO 0
#define OVERDUE_YES 1

#define TRANSPORT_STANDARD 0
#define TRANSPORT_EXPRESS 1

#define DISTANCE_OUTSIDE 0
#define DISTANCE_INSIDE 1

#define INSURANCE_NO 0
#define INSURANCE_YES 1

typedef struct Message {
    char content[MAX_MESSAGE_CONTENT];
    time_t timestamp;
} Message;

typedef struct Package {
    char pack_num[MAX_CODE];
    char pickup_code[MAX_CODE];
    float weight;
    int transport;
    int value;
    int distance;
    int insurance;
    char owner[MAX_NAME];
    char receiver[MAX_NAME];
    char phone[MAX_PHONE];
    int is_user;
    int state[4];
    int in_year, in_month, in_day, in_hour, in_minute, in_second, in_yday;
    int out_year, out_month, out_day, out_hour, out_minute, out_second, out_yday;
    float money;
    struct Package* next;
} Package;

typedef struct User {
    char name[MAX_NAME];
    char account[MAX_ACCOUNT];
    char password[MAX_PASSWORD];
    Package* packages;
    Message messages[MAX_MESSAGES];
    int message_count;
    int message_head;
    int new_messages;
    struct User* next;
} User;

typedef struct Manager {
    char account[MAX_MANAGER_ACCOUNT];
    char password[MAX_MANAGER_PASSWORD];
    struct Manager* next;
} Manager;

typedef Package* LinkPackage;
typedef User* LinkUser;
typedef Manager* LinkManager;

/* 链表与查询 */
LinkPackage find_package_by_number(LinkPackage head, const char* number);
LinkPackage find_package_by_code(LinkPackage head, const char* code);
LinkUser find_user_by_account(LinkUser head, const char* account);
LinkUser find_user_by_name(LinkUser head, const char* name);

/* 统计 */
int count_packages(LinkPackage head);
int count_in_stock(LinkPackage head);
int count_in_transit(LinkPackage head);
int count_overdue(LinkPackage head);
int count_users(LinkUser head);
float total_revenue(LinkPackage head);
int has_active_packages(const User* user);

/* 用户与包裹的创建、挂载 */
User* create_user(const char* name, const char* account, const char* password);
void append_user(LinkUser* head, User* user);
Package* create_package(const char* number, const char* code);
void append_package(LinkPackage* head, Package* pkg);
void generate_package_number(LinkPackage head, char* out, size_t size);
void generate_pickup_code(LinkPackage head, char* out, size_t size);
void set_package_time(Package* pkg, int is_in);
float compute_shipping_fee(const Package* pkg);
int is_leap_year(int year);
int all_year_days(int year);
int days_since_in(const Package* pkg);
void simulate_delivery_and_overdue(LinkPackage packages, LinkUser users);
void add_user_message(User* user, const char* content);
void refresh_user_packages(LinkUser users, LinkPackage packages);
int remove_user_and_packages(LinkUser* users, LinkUser target, LinkPackage* packages);

/* 状态文案 */
const char* storage_status_text(const Package* pkg);
const char* condition_text(int condition);
const char* transport_text(int transport);
const char* distance_text(int distance);
const char* transit_text(int transit);
const char* package_direction_text(const Package* pkg, const User* viewer);

/* 内存释放 */
void free_user_package_list(Package* head);
void free_users(LinkUser head);
void free_packages(LinkPackage head);
void free_managers(LinkManager head);

#endif

/* ========== src/data.h ========== */
#ifndef CAINIAO_DATA_H
#define CAINIAO_DATA_H


int ensure_data_dir(void);
LinkPackage load_packages(void);
LinkUser load_users(void);
void save_packages(LinkPackage head);
void save_users(LinkUser head);
void seed_demo_data(void);
void append_log(const char* file_name, const char* fmt, ...);

#endif

/* ========== src/ui.h ========== */
#ifndef CAINIAO_UI_H
#define CAINIAO_UI_H

#include <stdbool.h>
#include <stddef.h>

#define UI_WIDTH 92

#define COLOR_RESET 0
#define COLOR_BLACK 30
#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_BLUE 34
#define COLOR_MAGENTA 35
#define COLOR_CYAN 36
#define COLOR_WHITE 37
#define COLOR_GRAY 90
#define COLOR_BRIGHT_RED 91
#define COLOR_BRIGHT_GREEN 92
#define COLOR_BRIGHT_YELLOW 93
#define COLOR_BRIGHT_BLUE 94
#define COLOR_BRIGHT_MAGENTA 95
#define COLOR_BRIGHT_CYAN 96
#define COLOR_BRIGHT_WHITE 97

void ui_init(void);
void ui_clear_screen(void);
void ui_reset_color(void);
void ui_set_color(int fg, int bg, int bold);
void ui_header(const char* title, const char* subtitle);
void ui_rule(void);
void ui_panel(const char* title, int color);
void ui_panel_end(void);
void ui_center_line(const char* text, int color, int bold);
void ui_menu(const char* title, const char* items[], int count);
void ui_pause(const char* message);
void ui_notice(const char* text, int color, int bold);

int read_line(const char* prompt, char* buffer, size_t size);
int read_int(const char* prompt, int* value);
int read_int_range(const char* prompt, int min, int max, int* value);
int read_float_range(const char* prompt, float min, float max, float* value);
bool confirm(const char* question);
void clear_input_buffer(void);

#endif

/* ========== src/auth.h ========== */
#ifndef CAINIAO_AUTH_H
#define CAINIAO_AUTH_H


LinkManager create_default_managers(void);
LinkUser login_user(LinkUser* users);
LinkUser register_user_interactive(LinkUser* users);
int login_manager(LinkManager managers);
int cancel_user_account(LinkUser* users, LinkUser user, LinkPackage* packages);

#endif

/* ========== src/user.h ========== */
#ifndef CAINIAO_USER_H
#define CAINIAO_USER_H


int user_console(LinkPackage* packages, LinkUser* users, LinkUser current);

#endif

/* ========== src/manager.h ========== */
#ifndef CAINIAO_MANAGER_H
#define CAINIAO_MANAGER_H


void manager_console(LinkPackage* packages, LinkUser* users);

#endif

/* ========== src/model.c ========== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LinkPackage find_package_by_number(LinkPackage head, const char* number) {
    while (head != NULL) {
        if (strcmp(head->pack_num, number) == 0) return head;
        head = head->next;
    }
    return NULL;
}

LinkPackage find_package_by_code(LinkPackage head, const char* code) {
    while (head != NULL) {
        if (strcmp(head->pickup_code, code) == 0) return head;
        head = head->next;
    }
    return NULL;
}

LinkUser find_user_by_account(LinkUser head, const char* account) {
    while (head != NULL) {
        if (strcmp(head->account, account) == 0) return head;
        head = head->next;
    }
    return NULL;
}

LinkUser find_user_by_name(LinkUser head, const char* name) {
    while (head != NULL) {
        if (strcmp(head->name, name) == 0) return head;
        head = head->next;
    }
    return NULL;
}

int count_packages(LinkPackage head) {
    int n = 0;
    while (head != NULL) { n++; head = head->next; }
    return n;
}

int count_in_stock(LinkPackage head) {
    int n = 0;
    while (head != NULL) {
        if (head->state[0] == PKG_STATE_IN_STOCK && head->state[3] == OVERDUE_NO) n++;
        head = head->next;
    }
    return n;
}

int count_in_transit(LinkPackage head) {
    int n = 0;
    while (head != NULL) {
        if (head->state[0] == PKG_STATE_IN_TRANSIT) n++;
        head = head->next;
    }
    return n;
}

int count_overdue(LinkPackage head) {
    int n = 0;
    while (head != NULL) {
        if (head->state[0] == PKG_STATE_IN_STOCK && head->state[3] == OVERDUE_YES) n++;
        head = head->next;
    }
    return n;
}

int count_users(LinkUser head) {
    int n = 0;
    while (head != NULL) { n++; head = head->next; }
    return n;
}

float total_revenue(LinkPackage head) {
    float sum = 0.0f;
    while (head != NULL) {
        if (head->money > 0) sum += head->money;
        head = head->next;
    }
    return sum;
}

int has_active_packages(const User* user) {
    const Package* p = user->packages;
    while (p != NULL) {
        if (p->state[0] == PKG_STATE_IN_TRANSIT ||
            (p->state[0] == PKG_STATE_IN_STOCK && p->state[3] == OVERDUE_NO)) {
            return 1;
        }
        p = p->next;
    }
    return 0;
}

User* create_user(const char* name, const char* account, const char* password) {
    User* u = (User*)calloc(1, sizeof(User));
    if (u == NULL) return NULL;
    strncpy(u->name, name, MAX_NAME - 1);
    strncpy(u->account, account, MAX_ACCOUNT - 1);
    strncpy(u->password, password, MAX_PASSWORD - 1);
    u->message_head = 0;
    return u;
}

void append_user(LinkUser* head, User* user) {
    if (*head == NULL) {
        *head = user;
        return;
    }
    User* tail = *head;
    while (tail->next != NULL) tail = tail->next;
    tail->next = user;
}

Package* create_package(const char* number, const char* code) {
    Package* p = (Package*)calloc(1, sizeof(Package));
    if (p == NULL) return NULL;
    strncpy(p->pack_num, number, MAX_CODE - 1);
    strncpy(p->pickup_code, code, MAX_CODE - 1);
    p->state[0] = PKG_STATE_IN_TRANSIT;
    p->state[1] = PKG_CONDITION_INTACT;
    p->state[2] = TRANSIT_STATE_SHIPPING;
    p->state[3] = OVERDUE_NO;
    return p;
}

void append_package(LinkPackage* head, Package* pkg) {
    if (*head == NULL) {
        *head = pkg;
        return;
    }
    LinkPackage tail = *head;
    while (tail->next != NULL) tail = tail->next;
    tail->next = pkg;
}

static int code_exists(LinkPackage head, const char* field, int candidate) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%04d", candidate);
    while (head != NULL) {
        const char* value = (strcmp(field, "number") == 0) ? head->pack_num : head->pickup_code;
        if (strcmp(value, buf) == 0) return 1;
        head = head->next;
    }
    return 0;
}

void generate_package_number(LinkPackage head, char* out, size_t size) {
    int candidate = 1001;
    while (candidate <= 9999 && code_exists(head, "number", candidate)) candidate++;
    if (candidate > 9999) candidate = 1001;
    snprintf(out, size, "%04d", candidate);
}

void generate_pickup_code(LinkPackage head, char* out, size_t size) {
    int candidate = 2001;
    while (candidate <= 9999 && code_exists(head, "code", candidate)) candidate++;
    if (candidate > 9999) candidate = 2001;
    snprintf(out, size, "%04d", candidate);
}

int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int all_year_days(int year) {
    return is_leap_year(year) ? 366 : 365;
}

void set_package_time(Package* pkg, int is_in) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    if (is_in) {
        pkg->in_year = t->tm_year + 1900;
        pkg->in_month = t->tm_mon + 1;
        pkg->in_day = t->tm_mday;
        pkg->in_hour = t->tm_hour;
        pkg->in_minute = t->tm_min;
        pkg->in_second = t->tm_sec;
        pkg->in_yday = t->tm_yday;
    } else {
        pkg->out_year = t->tm_year + 1900;
        pkg->out_month = t->tm_mon + 1;
        pkg->out_day = t->tm_mday;
        pkg->out_hour = t->tm_hour;
        pkg->out_minute = t->tm_min;
        pkg->out_second = t->tm_sec;
        pkg->out_yday = t->tm_yday;
    }
}

float compute_shipping_fee(const Package* pkg) {
    float fee = 0.0f;
    if (pkg->transport == TRANSPORT_EXPRESS) {
        if (pkg->distance == DISTANCE_INSIDE) {
            fee = 12.0f;
            if (pkg->weight > 1.0f) fee += (pkg->weight - 1.0f) * 2.0f;
        } else {
            fee = 22.0f;
            if (pkg->weight > 1.0f) fee += (pkg->weight - 1.0f) * 8.0f;
        }
    } else {
        if (pkg->distance == DISTANCE_INSIDE) {
            fee = 10.0f;
            if (pkg->weight > 1.0f) fee += (pkg->weight - 1.0f) * 2.0f;
        } else {
            fee = 12.0f;
            if (pkg->weight > 1.0f) fee += (pkg->weight - 1.0f) * 5.0f;
        }
    }
    if (pkg->value >= 1000) fee *= 1.1f;
    if (pkg->insurance == INSURANCE_YES) {
        fee += 2.0f;
        if (pkg->value >= 1000) fee += (float)(pkg->value - 1000) * 0.003f;
    }
    return fee;
}

int days_since_in(const Package* pkg) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    int current_year = t->tm_year + 1900;
    int current_yday = t->tm_yday;
    int days;
    if (pkg->in_year == current_year) {
        days = current_yday - pkg->in_yday;
    } else {
        days = all_year_days(pkg->in_year) - pkg->in_yday + current_yday;
    }
    return days < 0 ? 0 : days;
}

void simulate_delivery_and_overdue(LinkPackage packages, LinkUser users) {
    LinkPackage p = packages;
    while (p != NULL) {
        LinkUser owner = find_user_by_name(users, p->owner);
        int days = days_since_in(p);

        if (p->state[0] == PKG_STATE_IN_STOCK && p->state[3] == OVERDUE_NO) {
            if (days > 3) {
                p->state[3] = OVERDUE_YES;
                set_package_time(p, 0);
                if (owner != NULL) {
                    add_user_message(owner, "您的包裹滞留超时，已被送回仓库，请联系工作人员处理。");
                }
            } else if (days == 2 || days == 3) {
                if (owner != NULL) {
                    add_user_message(owner, "您的包裹即将滞留超时，请尽快取回。");
                }
            }
        }

        if (p->state[0] == PKG_STATE_IN_TRANSIT && p->state[2] == TRANSIT_STATE_SHIPPING) {
            if (days > 3) {
                p->state[2] = TRANSIT_STATE_ARRIVED;
                if (p->is_user == 1) {
                    p->state[0] = PKG_STATE_IN_STOCK;
                    set_package_time(p, 1);
                    if (owner != NULL) {
                        add_user_message(owner, "您的包裹已送达目的地，请尽快取件。");
                    }
                } else {
                    p->state[0] = PKG_STATE_PICKED;
                    set_package_time(p, 0);
                    if (owner != NULL) {
                        add_user_message(owner, "您的包裹已送达收件人并完成签收。");
                    }
                }
            }
        }
        p = p->next;
    }
}

void add_user_message(User* user, const char* content) {
    if (user == NULL || content == NULL) return;
    int index = (user->message_head + user->message_count) % MAX_MESSAGES;
    strncpy(user->messages[index].content, content, MAX_MESSAGE_CONTENT - 1);
    user->messages[index].content[MAX_MESSAGE_CONTENT - 1] = '\0';
    user->messages[index].timestamp = time(NULL);
    if (user->message_count < MAX_MESSAGES) {
        user->message_count++;
    } else {
        user->message_head = (user->message_head + 1) % MAX_MESSAGES;
    }
    if (user->new_messages < MAX_MESSAGES) user->new_messages++;
}

void refresh_user_packages(LinkUser users, LinkPackage packages) {
    LinkUser u = users;
    while (u != NULL) {
        free_user_package_list(u->packages);
        u->packages = NULL;
        LinkPackage p = packages;
        while (p != NULL) {
            if (strcmp(p->owner, u->name) == 0) {
                Package* clone = (Package*)malloc(sizeof(Package));
                if (clone == NULL) break;
                *clone = *p;
                clone->next = NULL;
                if (u->packages == NULL) {
                    u->packages = clone;
                } else {
                    Package* tail = u->packages;
                    while (tail->next != NULL) tail = tail->next;
                    tail->next = clone;
                }
            }
            p = p->next;
        }
        u = u->next;
    }
}

int remove_user_and_packages(LinkUser* users, LinkUser target, LinkPackage* packages) {
    LinkUser prev = NULL;
    LinkUser cur = *users;
    while (cur != NULL && cur != target) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL) return 0;
    if (prev != NULL) prev->next = cur->next;
    else *users = cur->next;

    free_user_package_list(cur->packages);

    LinkPackage pprev = NULL;
    LinkPackage pcur = *packages;
    while (pcur != NULL) {
        LinkPackage next = pcur->next;
        if (strcmp(pcur->owner, target->name) == 0) {
            if (pprev != NULL) pprev->next = next;
            else *packages = next;
            free(pcur);
        } else {
            pprev = pcur;
        }
        pcur = next;
    }
    free(cur);
    return 1;
}

const char* storage_status_text(const Package* pkg) {
    if (pkg->state[0] == PKG_STATE_IN_TRANSIT) return "运输中";
    if (pkg->state[0] == PKG_STATE_IN_STOCK) {
        return pkg->state[3] == OVERDUE_YES ? "超时滞留" : "待取件";
    }
    return "已取件";
}

const char* condition_text(int condition) {
    return condition == PKG_CONDITION_INTACT ? "完好" : "有损";
}

const char* transport_text(int transport) {
    return transport == TRANSPORT_EXPRESS ? "加急" : "标准";
}

const char* distance_text(int distance) {
    return distance == DISTANCE_INSIDE ? "省内" : "省外";
}

const char* transit_text(int transit) {
    if (transit == TRANSIT_STATE_SHIPPING) return "运输中";
    if (transit == TRANSIT_STATE_ARRIVED) return "已送达";
    return "不适用";
}

const char* package_direction_text(const Package* pkg, const User* viewer) {
    if (viewer != NULL && strcmp(pkg->receiver, viewer->name) == 0) return "收件";
    if (viewer == NULL) return "—";
    return "寄件";
}

void free_user_package_list(Package* head) {
    while (head != NULL) {
        Package* next = head->next;
        free(head);
        head = next;
    }
}

void free_users(LinkUser head) {
    while (head != NULL) {
        LinkUser next = head->next;
        free_user_package_list(head->packages);
        free(head);
        head = next;
    }
}

void free_packages(LinkPackage head) {
    while (head != NULL) {
        LinkPackage next = head->next;
        free(head);
        head = next;
    }
}

void free_managers(LinkManager head) {
    while (head != NULL) {
        LinkManager next = head->next;
        free(head);
        head = next;
    }
}

/* ========== src/data.c ========== */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define MKDIR(p) mkdir(p, 0755)
#endif

static void make_path(char* out, size_t size, const char* name) {
    snprintf(out, size, "data/%s", name);
}

static void trim_line(char* line) {
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[len - 1] = '\0';
        len--;
    }
}

int ensure_data_dir(void) {
    return MKDIR("data") == 0;
}

LinkPackage load_packages(void) {
    char path[128];
    make_path(path, sizeof(path), "packages.txt");
    FILE* f = fopen(path, "r");
    if (f == NULL) return NULL;

    LinkPackage head = NULL;
    LinkPackage tail = NULL;
    char line[1024];
    while (fgets(line, sizeof(line), f) != NULL) {
        trim_line(line);
        if (line[0] == '\0' || line[0] == '#') continue;

        char num[MAX_CODE], code[MAX_CODE], owner[MAX_NAME], receiver[MAX_NAME], phone[MAX_PHONE];
        float weight = 0.0f, money = 0.0f;
        int transport = 0, value = 0, distance = 0, insurance = 0;
        int is_user = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        int in_y = 0, in_m = 0, in_d = 0, in_h = 0, in_min = 0, in_s = 0, in_yd = 0;
        int out_y = 0, out_m = 0, out_d = 0, out_h = 0, out_min = 0, out_s = 0, out_yd = 0;

        int matched = sscanf(line,
            "%11[^|]|%11[^|]|%f|%d|%d|%d|%d|%19[^|]|%19[^|]|%19[^|]|%d|%d|%d|%d|%d"
            "|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%f",
            num, code, &weight, &transport, &value, &distance, &insurance,
            owner, receiver, phone, &is_user,
            &s0, &s1, &s2, &s3,
            &in_y, &in_m, &in_d, &in_h, &in_min, &in_s, &in_yd,
            &out_y, &out_m, &out_d, &out_h, &out_min, &out_s, &out_yd,
            &money);
        if (matched != 30) continue;

        Package* p = create_package(num, code);
        if (p == NULL) break;
        p->weight = weight;
        p->transport = transport;
        p->value = value;
        p->distance = distance;
        p->insurance = insurance;
        strncpy(p->owner, owner, MAX_NAME - 1);
        strncpy(p->receiver, receiver, MAX_NAME - 1);
        strncpy(p->phone, phone, MAX_PHONE - 1);
        if (strcmp(p->phone, "-") == 0) p->phone[0] = '\0';
        p->is_user = is_user;
        p->state[0] = s0;
        p->state[1] = s1;
        p->state[2] = s2;
        p->state[3] = s3;
        p->in_year = in_y; p->in_month = in_m; p->in_day = in_d;
        p->in_hour = in_h; p->in_minute = in_min; p->in_second = in_s; p->in_yday = in_yd;
        p->out_year = out_y; p->out_month = out_m; p->out_day = out_d;
        p->out_hour = out_h; p->out_minute = out_min; p->out_second = out_s; p->out_yday = out_yd;
        p->money = money;

        if (head == NULL) head = p;
        else tail->next = p;
        tail = p;
    }
    fclose(f);
    return head;
}

void save_packages(LinkPackage head) {
    char path[128];
    make_path(path, sizeof(path), "packages.txt");
    FILE* f = fopen(path, "w");
    if (f == NULL) return;
    fprintf(f, "# pack_num|pickup|weight|transport|value|distance|insurance|owner|receiver|phone|is_user|state0..3|in time|out time|money\n");
    while (head != NULL) {
        fprintf(f, "%s|%s|%.2f|%d|%d|%d|%d|%s|%s|%s|%d|%d|%d|%d|%d"
                   "|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%.2f\n",
                head->pack_num, head->pickup_code, head->weight,
                head->transport, head->value, head->distance, head->insurance,
                head->owner, head->receiver, head->phone[0] ? head->phone : "-", head->is_user,
                head->state[0], head->state[1], head->state[2], head->state[3],
                head->in_year, head->in_month, head->in_day, head->in_hour,
                head->in_minute, head->in_second, head->in_yday,
                head->out_year, head->out_month, head->out_day, head->out_hour,
                head->out_minute, head->out_second, head->out_yday,
                head->money);
        head = head->next;
    }
    fclose(f);
}

LinkUser load_users(void) {
    char path[128];
    make_path(path, sizeof(path), "users.txt");
    FILE* f = fopen(path, "r");
    if (f == NULL) return NULL;

    LinkUser head = NULL;
    LinkUser tail = NULL;
    char line[1024];
    while (fgets(line, sizeof(line), f) != NULL) {
        trim_line(line);
        if (line[0] == '\0' || line[0] == '#') continue;

        if (line[0] == 'M' && tail != NULL) {
            long long ts = 0;
            char content[MAX_MESSAGE_CONTENT];
            if (sscanf(line, "M|%lld|%79[^\n]", &ts, content) == 2) {
                int index = (tail->message_head + tail->message_count) % MAX_MESSAGES;
                strncpy(tail->messages[index].content, content, MAX_MESSAGE_CONTENT - 1);
                tail->messages[index].content[MAX_MESSAGE_CONTENT - 1] = '\0';
                tail->messages[index].timestamp = (time_t)ts;
                if (tail->message_count < MAX_MESSAGES) tail->message_count++;
                else tail->message_head = (tail->message_head + 1) % MAX_MESSAGES;
            }
            continue;
        }

        char name[MAX_NAME], account[MAX_ACCOUNT], password[MAX_PASSWORD];
        int message_count = 0, message_head = 0, new_messages = 0;
        if (sscanf(line, "%19[^|]|%19[^|]|%19[^|]|%d|%d|%d",
                   name, account, password, &message_count, &message_head, &new_messages) != 6) {
            continue;
        }
        User* u = create_user(name, account, password);
        if (u == NULL) break;
        if (message_count < 0) message_count = 0;
        if (message_count > MAX_MESSAGES) message_count = MAX_MESSAGES;
        if (message_head < 0 || message_head >= MAX_MESSAGES) message_head = 0;
        u->message_count = message_count;
        u->message_head = message_head;
        u->new_messages = new_messages;
        if (head == NULL) head = u;
        else tail->next = u;
        tail = u;
    }
    fclose(f);
    return head;
}

void save_users(LinkUser head) {
    char path[128];
    make_path(path, sizeof(path), "users.txt");
    FILE* f = fopen(path, "w");
    if (f == NULL) return;
    fprintf(f, "# name|account|password|message_count|message_head|new_messages\n");
    while (head != NULL) {
        fprintf(f, "%s|%s|%s|%d|%d|%d\n",
                head->name, head->account, head->password,
                head->message_count, head->message_head, head->new_messages);
        for (int i = 0; i < head->message_count; i++) {
            int index = (head->message_head + i) % MAX_MESSAGES;
            fprintf(f, "M|%lld|%s\n",
                    (long long)head->messages[index].timestamp,
                    head->messages[index].content);
        }
        head = head->next;
    }
    fclose(f);
}

static Package* make_demo_package(const char* number, const char* code,
                                  const char* owner, const char* receiver,
                                  float weight, int transport, int value,
                                  int distance, int insurance, int is_user,
                                  int state0, int state1, int state2, int state3,
                                  int days_ago) {
    Package* p = create_package(number, code);
    if (p == NULL) return NULL;
    strncpy(p->owner, owner, MAX_NAME - 1);
    strncpy(p->receiver, receiver, MAX_NAME - 1);
    strncpy(p->phone, "", MAX_PHONE - 1);
    p->weight = weight;
    p->transport = transport;
    p->value = value;
    p->distance = distance;
    p->insurance = insurance;
    p->is_user = is_user;
    p->state[0] = state0;
    p->state[1] = state1;
    p->state[2] = state2;
    p->state[3] = state3;
    p->money = (state0 == PKG_STATE_IN_STOCK) ? 0.0f : compute_shipping_fee(p);

    time_t when = time(NULL) - (time_t)days_ago * 86400L;
    struct tm* t = localtime(&when);
    p->in_year = t->tm_year + 1900;
    p->in_month = t->tm_mon + 1;
    p->in_day = t->tm_mday;
    p->in_hour = t->tm_hour;
    p->in_minute = t->tm_min;
    p->in_second = t->tm_sec;
    p->in_yday = t->tm_yday;
    return p;
}

void seed_demo_data(void) {
    LinkUser users = NULL;
    LinkUser zhang = create_user("张三", "zhangsan", "123456");
    LinkUser li = create_user("李四", "lisi", "123456");
    LinkUser wang = create_user("王五", "wangwu", "123456");
    append_user(&users, zhang);
    append_user(&users, li);
    append_user(&users, wang);
    add_user_message(zhang, "欢迎使用菜鸟驿站智能快递管理系统。");
    add_user_message(li, "欢迎使用菜鸟驿站智能快递管理系统。");
    add_user_message(wang, "欢迎使用菜鸟驿站智能快递管理系统。");

    LinkPackage packages = NULL;
    append_package(&packages, make_demo_package("1001", "2024", "张三", "张三", 2.5f, TRANSPORT_STANDARD, -1, DISTANCE_INSIDE, INSURANCE_NO, 1, PKG_STATE_IN_STOCK, PKG_CONDITION_INTACT, TRANSIT_STATE_ARRIVED, OVERDUE_NO, 5));
    append_package(&packages, make_demo_package("1002", "2025", "张三", "张三", 1.0f, TRANSPORT_STANDARD, -1, DISTANCE_INSIDE, INSURANCE_NO, 1, PKG_STATE_IN_STOCK, PKG_CONDITION_INTACT, TRANSIT_STATE_ARRIVED, OVERDUE_NO, 1));
    append_package(&packages, make_demo_package("1003", "2026", "李四", "李四", 3.2f, TRANSPORT_STANDARD, -1, DISTANCE_OUTSIDE, INSURANCE_YES, 1, PKG_STATE_IN_STOCK, PKG_CONDITION_DAMAGED, TRANSIT_STATE_ARRIVED, OVERDUE_NO, 1));
    append_package(&packages, make_demo_package("1004", "2027", "王五", "赵六", 1.6f, TRANSPORT_EXPRESS, 800, DISTANCE_OUTSIDE, INSURANCE_YES, 1, PKG_STATE_IN_TRANSIT, PKG_CONDITION_INTACT, TRANSIT_STATE_SHIPPING, OVERDUE_NO, 5));
    append_package(&packages, make_demo_package("1005", "2028", "李四", "钱七", 4.0f, TRANSPORT_STANDARD, 1500, DISTANCE_INSIDE, INSURANCE_YES, 0, PKG_STATE_IN_TRANSIT, PKG_CONDITION_INTACT, TRANSIT_STATE_SHIPPING, OVERDUE_NO, 5));

    save_users(users);
    save_packages(packages);
    append_log("system_journal.txt", "首次启动，已生成演示数据。");
    free_users(users);
    free_packages(packages);
}

void append_log(const char* file_name, const char* fmt, ...) {
    char path[128];
    make_path(path, sizeof(path), file_name);
    FILE* f = fopen(path, "a");
    if (f == NULL) return;

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] ",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);

    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fclose(f);
}

/* ========== src/ui.c ========== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

static void print_chars(char ch, int count) {
    for (int i = 0; i < count; i++) putchar(ch);
}

static int display_width(const char* text) {
    int width = 0;
    const unsigned char* p = (const unsigned char*)text;
    while (*p != '\0') {
        if (*p < 0x80) {
            width++;
            p++;
        } else {
            int extra = 0;
            if ((*p & 0xE0) == 0xC0) extra = 1;
            else if ((*p & 0xF0) == 0xE0) extra = 2;
            else if ((*p & 0xF8) == 0xF0) extra = 3;
            p += extra + 1;
            width += 2;
        }
    }
    return width;
}

static void trim_whitespace(char* text) {
    size_t len = strlen(text);
    while (len > 0 && (text[len - 1] == ' ' || text[len - 1] == '\t')) {
        text[len - 1] = '\0';
        len--;
    }
}

void ui_init(void) {
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode)) {
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void ui_clear_screen(void) {
    printf("\033[2J\033[H");
}

void ui_reset_color(void) {
    printf("\033[0m");
}

void ui_set_color(int fg, int bg, int bold) {
    printf("\033[");
    int wrote = 0;
    if (bold) {
        printf("1");
        wrote = 1;
    }
    if (fg != COLOR_RESET) {
        if (wrote) printf(";");
        printf("%d", fg);
        wrote = 1;
    }
    if (bg != COLOR_RESET) {
        if (wrote) printf(";");
        printf("%d", bg);
    }
    printf("m");
}

void ui_center_line(const char* text, int color, int bold) {
    int width = display_width(text);
    int pad = (UI_WIDTH - width) / 2;
    if (pad < 0) pad = 0;
    printf("%*s", pad, "");
    ui_set_color(color, COLOR_RESET, bold);
    printf("%s", text);
    ui_reset_color();
    printf("\n");
}

void ui_header(const char* title, const char* subtitle) {
    ui_clear_screen();
    ui_set_color(COLOR_CYAN, COLOR_RESET, 1);
    printf("┌");
    print_chars('─', UI_WIDTH - 2);
    printf("┐\n");
    ui_center_line(title, COLOR_BRIGHT_WHITE, 1);
    if (subtitle != NULL) ui_center_line(subtitle, COLOR_BRIGHT_CYAN, 0);
    ui_set_color(COLOR_CYAN, COLOR_RESET, 1);
    printf("└");
    print_chars('─', UI_WIDTH - 2);
    printf("┘\n");
    ui_reset_color();
}

void ui_rule(void) {
    ui_set_color(COLOR_CYAN, COLOR_RESET, 0);
    printf("  ");
    print_chars('─', UI_WIDTH - 4);
    printf("\n");
    ui_reset_color();
}

void ui_panel(const char* title, int color) {
    int width = display_width(title);
    int remaining = UI_WIDTH - 6 - width;
    if (remaining < 0) remaining = 0;
    ui_set_color(color, COLOR_RESET, 1);
    printf("┌─ %s ", title);
    print_chars('─', remaining);
    printf("┐\n");
    ui_reset_color();
}

void ui_panel_end(void) {
    ui_set_color(COLOR_CYAN, COLOR_RESET, 1);
    printf("└");
    print_chars('─', UI_WIDTH - 2);
    printf("┘\n");
    ui_reset_color();
}

void ui_menu(const char* title, const char* items[], int count) {
    ui_panel(title, COLOR_BRIGHT_CYAN);
    for (int i = 0; i < count; i++) {
        ui_set_color(COLOR_BRIGHT_GREEN, COLOR_RESET, 1);
        printf("   %2d. ", i + 1);
        ui_reset_color();
        printf("%s\n", items[i]);
    }
    ui_panel_end();
}

void ui_notice(const char* text, int color, int bold) {
    ui_set_color(color, COLOR_RESET, bold);
    printf("   %s\n", text);
    ui_reset_color();
}

void ui_pause(const char* message) {
    printf("\n   %s", message != NULL ? message : "按回车键返回...");
    clear_input_buffer();
    getchar();
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int read_line(const char* prompt, char* buffer, size_t size) {
    if (prompt != NULL) printf("%s", prompt);
    if (buffer == NULL || size == 0) return 0;
    if (fgets(buffer, (int)size, stdin) == NULL) return 0;
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else if (len == size - 1) {
        clear_input_buffer();
    }
    return 1;
}

int read_int(const char* prompt, int* value) {
    char line[64];
    if (!read_line(prompt, line, sizeof(line))) return 0;
    trim_whitespace(line);
    char* end = NULL;
    long number = strtol(line, &end, 10);
    if (end == line || *end != '\0') return 0;
    *value = (int)number;
    return 1;
}

int read_int_range(const char* prompt, int min, int max, int* value) {
    int number = 0;
    while (1) {
        if (read_int(prompt, &number) && number >= min && number <= max) {
            *value = number;
            return 1;
        }
        ui_notice("输入无效，请输入范围内的数字。", COLOR_BRIGHT_RED, 1);
    }
}

int read_float_range(const char* prompt, float min, float max, float* value) {
    char line[64];
    float number = 0.0f;
    while (1) {
        if (read_line(prompt, line, sizeof(line))) {
            trim_whitespace(line);
            char* end = NULL;
            number = strtof(line, &end);
            if (end != line && *end == '\0' && number >= min && number <= max) {
                *value = number;
                return 1;
            }
        }
        ui_notice("输入无效，请输入有效数值。", COLOR_BRIGHT_RED, 1);
    }
}

bool confirm(const char* question) {
    int answer = 0;
    read_int_range(question, 0, 1, &answer);
    return answer == 1;
}

/* ========== src/auth.c ========== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static Manager* make_manager(const char* account, const char* password) {
    Manager* m = (Manager*)calloc(1, sizeof(Manager));
    if (m == NULL) return NULL;
    strncpy(m->account, account, MAX_MANAGER_ACCOUNT - 1);
    strncpy(m->password, password, MAX_MANAGER_PASSWORD - 1);
    return m;
}

LinkManager create_default_managers(void) {
    LinkManager head = make_manager("0001", "8888");
    LinkManager second = make_manager("0002", "9999");
    if (head != NULL) head->next = second;
    return head;
}

LinkUser register_user_interactive(LinkUser* users) {
    ui_panel("新用户注册", COLOR_BRIGHT_GREEN);

    char name[MAX_NAME], account[MAX_ACCOUNT], password[MAX_PASSWORD], password2[MAX_PASSWORD];
    read_line("   姓名/昵称: ", name, sizeof(name));

    while (1) {
        read_line("   登录账号: ", account, sizeof(account));
        if (find_user_by_account(*users, account) != NULL) {
            ui_notice("该账号已存在，请更换账号。", COLOR_BRIGHT_RED, 1);
            continue;
        }
        if (account[0] == '\0') {
            ui_notice("账号不能为空。", COLOR_BRIGHT_RED, 1);
            continue;
        }
        break;
    }

    while (1) {
        read_line("   设置密码: ", password, sizeof(password));
        read_line("   确认密码: ", password2, sizeof(password2));
        if (strcmp(password, password2) != 0) {
            ui_notice("两次密码不一致，请重新设置。", COLOR_BRIGHT_RED, 1);
            continue;
        }
        if (password[0] == '\0') {
            ui_notice("密码不能为空。", COLOR_BRIGHT_RED, 1);
            continue;
        }
        break;
    }

    User* user = create_user(name, account, password);
    if (user == NULL) {
        ui_notice("内存分配失败，注册未完成。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return NULL;
    }
    add_user_message(user, "欢迎使用菜鸟驿站智能快递管理系统。");
    append_user(users, user);
    save_users(*users);
    append_log("user_journal.txt", "用户注册成功：账号=%s 姓名=%s", account, name);
    ui_panel_end();
    ui_notice("注册成功，已自动登录。", COLOR_BRIGHT_GREEN, 1);
    return user;
}

LinkUser login_user(LinkUser* users) {
    char account[MAX_ACCOUNT], password[MAX_PASSWORD];

    while (1) {
        ui_panel("用户登录", COLOR_BRIGHT_CYAN);
        read_line("   账号: ", account, sizeof(account));
        LinkUser user = find_user_by_account(*users, account);
        if (user == NULL) {
            ui_notice("账号不存在。", COLOR_BRIGHT_RED, 1);
            if (confirm("   是否立即注册新账号？1=是 0=否: ")) {
                ui_panel_end();
                return register_user_interactive(users);
            }
            ui_panel_end();
            continue;
        }

        int attempts = 0;
        while (attempts < 3) {
            read_line("   密码: ", password, sizeof(password));
            if (strcmp(user->password, password) == 0) {
                append_log("user_journal.txt", "用户登录成功：账号=%s 姓名=%s", user->account, user->name);
                ui_panel_end();
                return user;
            }
            attempts++;
            ui_notice("密码错误，请重新输入。", COLOR_BRIGHT_RED, 1);
        }
        ui_panel_end();
        ui_notice("连续 3 次密码错误，请稍后重试。", COLOR_BRIGHT_RED, 1);
        return NULL;
    }
}

int login_manager(LinkManager managers) {
    char account[MAX_MANAGER_ACCOUNT], password[MAX_MANAGER_PASSWORD];
    for (int left = 5; left > 0; left--) {
        ui_panel("管理员登录", COLOR_BRIGHT_CYAN);
        read_line("   管理员账号: ", account, sizeof(account));
        read_line("   管理员密码: ", password, sizeof(password));

        LinkManager m = managers;
        while (m != NULL) {
            if (strcmp(m->account, account) == 0 && strcmp(m->password, password) == 0) {
                append_log("manager_journal.txt", "管理员登录成功：账号=%s", account);
                ui_panel_end();
                ui_notice("登录成功。", COLOR_BRIGHT_GREEN, 1);
                return 1;
            }
            m = m->next;
        }
        ui_panel_end();
        if (left - 1 > 0) {
            char message[128];
            snprintf(message, sizeof(message), "账号或密码错误，还可尝试 %d 次。", left - 1);
            ui_notice(message, COLOR_BRIGHT_RED, 1);
        }
    }
    return 0;
}

int cancel_user_account(LinkUser* users, LinkUser user, LinkPackage* packages) {
    ui_panel("注销账号", COLOR_BRIGHT_RED);
    if (!confirm("   确认注销该账号？1=确认 0=取消: ")) {
        ui_panel_end();
        return 0;
    }
    if (has_active_packages(user)) {
        ui_notice("账号仍有运输中或待取件包裹，无法注销。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return 0;
    }

    char account[MAX_ACCOUNT], password[MAX_PASSWORD];
    read_line("   再次输入账号确认: ", account, sizeof(account));
    read_line("   再次输入密码确认: ", password, sizeof(password));
    if (strcmp(user->account, account) != 0 || strcmp(user->password, password) != 0) {
        ui_notice("账号或密码不匹配，注销已取消。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return 0;
    }

    char account_copy[MAX_ACCOUNT], name_copy[MAX_NAME];
    strncpy(account_copy, user->account, MAX_ACCOUNT - 1);
    strncpy(name_copy, user->name, MAX_NAME - 1);

    int removed = remove_user_and_packages(users, user, packages);
    if (!removed) {
        ui_panel_end();
        return 0;
    }
    save_users(*users);
    save_packages(*packages);
    append_log("user_journal.txt", "用户注销成功：账号=%s 姓名=%s", account_copy, name_copy);
    ui_panel_end();
    ui_notice("账号已注销。", COLOR_BRIGHT_GREEN, 1);
    return 1;
}

/* ========== src/user.c ========== */

#include <stdio.h>
#include <string.h>


static void print_timeline(const Package* pkg) {
    int stage = 0;
    if (pkg->state[0] == PKG_STATE_IN_TRANSIT) stage = 1;
    else if (pkg->state[0] == PKG_STATE_IN_STOCK && pkg->state[3] == OVERDUE_NO) stage = 2;
    else if (pkg->state[0] == PKG_STATE_IN_STOCK && pkg->state[3] == OVERDUE_YES) stage = 3;
    else if (pkg->state[0] == PKG_STATE_PICKED) stage = 4;

    const char* stages[] = {"下单", "运输", "到站", "取件"};
    printf("   轨迹: ");
    for (int i = 0; i < 4; i++) {
        if (i < stage) ui_set_color(COLOR_BRIGHT_GREEN, COLOR_RESET, 1);
        else if (i == stage && i < 4) ui_set_color(COLOR_BRIGHT_YELLOW, COLOR_RESET, 1);
        else ui_set_color(COLOR_GRAY, COLOR_RESET, 0);
        printf("%s", i < stage ? "●" : (i == stage && i < 4 ? "▶" : "○"));
        ui_reset_color();
        printf(" %s", stages[i]);
        if (i < 3) printf("  →  ");
    }
    if (pkg->state[0] == PKG_STATE_IN_STOCK && pkg->state[3] == OVERDUE_YES) {
        ui_set_color(COLOR_BRIGHT_RED, COLOR_RESET, 1);
        printf("  [超时滞留]");
        ui_reset_color();
    }
    printf("\n");
}

static void print_package_detail(const Package* pkg, const User* viewer) {
    ui_panel("包裹详情", COLOR_BRIGHT_CYAN);
    printf("   单号: %s\n", pkg->pack_num);
    printf("   取件码: %s\n", pkg->pickup_code);
    printf("   方向: %s\n", package_direction_text(pkg, viewer));
    printf("   寄件人: %s    收件人: %s\n", pkg->owner, pkg->receiver);
    if (pkg->phone[0] != '\0') printf("   收件电话: %s\n", pkg->phone);
    printf("   重量: %.2f kg    运输: %s    距离: %s\n",
           pkg->weight, transport_text(pkg->transport), distance_text(pkg->distance));
    printf("   价值: %d 元    保险: %s    运费: %.2f 元\n",
           pkg->value, pkg->insurance ? "已购买" : "未购买", pkg->money);
    printf("   入库时间: %04d-%02d-%02d %02d:%02d\n",
           pkg->in_year, pkg->in_month, pkg->in_day, pkg->in_hour, pkg->in_minute);
    if (pkg->state[0] == PKG_STATE_PICKED) {
        printf("   出库时间: %04d-%02d-%02d %02d:%02d\n",
               pkg->out_year, pkg->out_month, pkg->out_day, pkg->out_hour, pkg->out_minute);
    }
    printf("   当前状态: %s    包裹状况: %s\n",
           storage_status_text(pkg), condition_text(pkg->state[1]));
    print_timeline(pkg);
    ui_panel_end();
}

static void track_packages(LinkPackage packages, LinkUser current) {
    ui_panel("我的包裹", COLOR_BRIGHT_CYAN);
    if (current->packages == NULL) {
        ui_notice("您当前没有任何包裹记录。", COLOR_GRAY, 0);
        ui_panel_end();
        return;
    }

    printf("   %-4s %-8s %-6s %-10s %-8s %-10s\n", "序号", "单号", "方向", "状态", "取件码", "运费");
    ui_rule();
    int index = 1;
    LinkPackage p = current->packages;
    while (p != NULL) {
        printf("   %-4d %-8s %-6s %-10s %-8s %-10.2f\n",
               index++, p->pack_num, package_direction_text(p, current),
               storage_status_text(p), p->pickup_code, p->money);
        p = p->next;
    }
    ui_panel_end();

    char number[MAX_CODE];
    read_line("   输入单号查看轨迹，输入 0 返回: ", number, sizeof(number));
    if (strcmp(number, "0") == 0) return;
    LinkPackage found = find_package_by_number(packages, number);
    if (found == NULL) {
        ui_notice("未找到该单号。", COLOR_BRIGHT_RED, 1);
        return;
    }
    print_package_detail(found, current);
}

static void pickup_package(LinkPackage* packages, LinkUser* users, LinkUser current) {
    ui_panel("取件", COLOR_BRIGHT_GREEN);
    char code[MAX_CODE];
    read_line("   请输入取件码: ", code, sizeof(code));
    LinkPackage pkg = find_package_by_code(*packages, code);
    if (pkg == NULL) {
        ui_notice("未找到该取件码对应的包裹。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return;
    }
    if (pkg->state[0] != PKG_STATE_IN_STOCK) {
        ui_notice("该包裹当前不在库，无法取件。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return;
    }
    if (pkg->state[3] == OVERDUE_YES) {
        ui_notice("该包裹已超时滞留，请联系前台工作人员处理。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return;
    }
    if (strcmp(pkg->owner, current->name) != 0) {
        if (!confirm("   该包裹不属于当前账号，是否代取？1=确认 0=取消: ")) {
            ui_panel_end();
            return;
        }
    }

    char pack_num[MAX_CODE], owner_name[MAX_NAME];
    strncpy(pack_num, pkg->pack_num, MAX_CODE - 1);
    strncpy(owner_name, pkg->owner, MAX_NAME - 1);

    pkg->state[0] = PKG_STATE_PICKED;
    set_package_time(pkg, 0);
    LinkUser owner = find_user_by_name(*users, owner_name);
    if (owner != NULL) {
        char message[160];
        snprintf(message, sizeof(message), "您的包裹已取件：单号 %s", pack_num);
        add_user_message(owner, message);
    }
    save_packages(*packages);
    append_log("user_journal.txt", "用户 %s 取件成功：单号=%s 取件码=%s", current->name, pack_num, code);
    ui_panel_end();
    ui_notice("取件成功。", COLOR_BRIGHT_GREEN, 1);
}

static void send_package(LinkPackage* packages, LinkUser* users, LinkUser current) {
    ui_panel("寄件", COLOR_BRIGHT_GREEN);

    int recipient_is_user = 0;
    read_int_range("   收件人是否为注册用户？1=是 0=否: ", 0, 1, &recipient_is_user);
    char receiver[MAX_NAME], phone[MAX_PHONE];
    if (recipient_is_user) {
        int total = count_users(*users);
        ui_notice("注册用户列表：", COLOR_BRIGHT_CYAN, 1);
        LinkUser u = *users;
        int i = 1;
        while (u != NULL) {
            printf("   %2d. %-10s（账号: %s）\n", i++, u->name, u->account);
            u = u->next;
        }
        int choice = 0;
        read_int_range("   请选择收件用户编号: ", 1, total, &choice);
        LinkUser selected = *users;
        for (int j = 1; j < choice && selected != NULL; j++) selected = selected->next;
        strncpy(receiver, selected->name, MAX_NAME - 1);
        phone[0] = '\0';
    } else {
        read_line("   收件人姓名: ", receiver, sizeof(receiver));
        read_line("   收件人电话: ", phone, sizeof(phone));
    }

    float weight = 0.0f;
    int transport = 0, value = 0, distance = 0, insurance = 0;
    read_float_range("   包裹重量(kg): ", 0.01f, 1000.0f, &weight);
    read_int_range("   运输方式：0=标准 1=加急: ", 0, 1, &transport);
    read_int_range("   物品价值(元): ", 1, 9999999, &value);
    read_int_range("   运输距离：0=省外 1=省内: ", 0, 1, &distance);
    read_int_range("   是否购买保险：0=否 1=是: ", 0, 1, &insurance);

    float base = 0.0f, extra = 0.0f, value_extra = 0.0f, insurance_fee = 0.0f;
    if (transport == TRANSPORT_EXPRESS) {
        base = (distance == DISTANCE_INSIDE) ? 12.0f : 22.0f;
        extra = (weight > 1.0f) ? (weight - 1.0f) * (distance == DISTANCE_INSIDE ? 2.0f : 8.0f) : 0.0f;
    } else {
        base = (distance == DISTANCE_INSIDE) ? 10.0f : 12.0f;
        extra = (weight > 1.0f) ? (weight - 1.0f) * (distance == DISTANCE_INSIDE ? 2.0f : 5.0f) : 0.0f;
    }
    if (value >= 1000) value_extra = (base + extra) * 0.1f;
    if (insurance) {
        insurance_fee = 2.0f;
        if (value >= 1000) insurance_fee += (float)(value - 1000) * 0.003f;
    }
    float total_fee = base + extra + value_extra + insurance_fee;

    ui_panel("运费明细", COLOR_BRIGHT_YELLOW);
    printf("   基础运费: %.2f 元\n", base);
    printf("   续重费用: %.2f 元\n", extra);
    printf("   高价值加成: %.2f 元\n", value_extra);
    printf("   保价费用: %.2f 元\n", insurance_fee);
    ui_set_color(COLOR_BRIGHT_GREEN, COLOR_RESET, 1);
    printf("   应付合计: %.2f 元\n", total_fee);
    ui_reset_color();
    ui_panel_end();

    int paid = 0;
    while (1) {
        read_int_range("   支付状态：1=已支付 2=未支付 0=取消: ", 0, 2, &paid);
        if (paid == 0) {
            ui_notice("寄件已取消。", COLOR_GRAY, 0);
            ui_panel_end();
            return;
        }
        if (paid == 1) break;
        ui_notice("请完成支付后再次确认。", COLOR_BRIGHT_YELLOW, 1);
    }

    char number[MAX_CODE], pickup[MAX_CODE];
    generate_package_number(*packages, number, sizeof(number));
    generate_pickup_code(*packages, pickup, sizeof(pickup));

    Package* pkg = create_package(number, pickup);
    if (pkg == NULL) {
        ui_notice("内存分配失败，寄件未完成。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return;
    }
    strncpy(pkg->owner, current->name, MAX_NAME - 1);
    strncpy(pkg->receiver, receiver, MAX_NAME - 1);
    strncpy(pkg->phone, phone, MAX_PHONE - 1);
    pkg->is_user = recipient_is_user;
    pkg->weight = weight;
    pkg->transport = transport;
    pkg->value = value;
    pkg->distance = distance;
    pkg->insurance = insurance;
    pkg->state[0] = PKG_STATE_IN_TRANSIT;
    pkg->state[1] = PKG_CONDITION_INTACT;
    pkg->state[2] = TRANSIT_STATE_SHIPPING;
    pkg->state[3] = OVERDUE_NO;
    pkg->money = total_fee;
    set_package_time(pkg, 1);
    append_package(packages, pkg);

    add_user_message(current, "寄件成功，包裹正在运输途中。");
    save_packages(*packages);
    append_log("user_journal.txt", "用户 %s 寄件成功：单号=%s 收件人=%s 运费=%.2f",
               current->name, number, receiver, total_fee);
    ui_panel_end();
    ui_notice("寄件成功，包裹已进入运输流程。", COLOR_BRIGHT_GREEN, 1);
}

static void show_messages(User* user) {
    ui_panel("我的消息", COLOR_BRIGHT_CYAN);
    if (user->message_count == 0) {
        ui_notice("您目前没有任何消息。", COLOR_GRAY, 0);
        ui_panel_end();
        return;
    }
    printf("   共 %d 条消息，其中新消息 %d 条\n", user->message_count, user->new_messages);
    for (int i = user->message_count - 1; i >= 0; i--) {
        int index = (user->message_head + i) % MAX_MESSAGES;
        char time_buf[32];
        struct tm* t = localtime(&user->messages[index].timestamp);
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M", t);
        int is_new = i >= user->message_count - user->new_messages;
        ui_set_color(is_new ? COLOR_BRIGHT_YELLOW : COLOR_GRAY, COLOR_RESET, is_new ? 1 : 0);
        printf("   [%s] %s%s\n", time_buf, user->messages[index].content, is_new ? "  [新]" : "");
        ui_reset_color();
    }
    user->new_messages = 0;
    ui_panel_end();
}

int user_console(LinkPackage* packages, LinkUser* users, LinkUser current) {
    int running = 1;
    while (running) {
        ui_header("客户中心", current->name);
        char status[128];
        snprintf(status, sizeof(status), "当前包裹 %d 件 · 新消息 %d 条",
                 count_packages(current->packages), current->new_messages);
        ui_center_line(status, COLOR_BRIGHT_GREEN, 0);

        const char* items[] = {
            "取件",
            "查看包裹状态",
            "寄件",
            "我的消息",
            "注销账号",
            "退出登录"
        };
        ui_menu("请选择业务", items, 6);

        int choice = 0;
        read_int_range("   请输入业务编号: ", 0, 6, &choice);
        switch (choice) {
        case 1:
            pickup_package(packages, users, current);
            break;
        case 2:
            track_packages(*packages, current);
            break;
        case 3:
            send_package(packages, users, current);
            refresh_user_packages(*users, *packages);
            save_packages(*packages);
            break;
        case 4:
            show_messages(current);
            save_users(*users);
            break;
        case 5:
            if (cancel_user_account(users, current, packages)) {
                return 1;
            }
            break;
        case 0:
            running = 0;
            break;
        default:
            break;
        }
        if (running && (choice == 1 || choice == 2)) {
            refresh_user_packages(*users, *packages);
        }
    }
    return 0;
}

/* ========== src/manager.c ========== */

#include <ctype.h>
#include <stdio.h>
#include <string.h>


static int is_digits(const char* text) {
    if (text[0] == '\0') return 0;
    for (const char* p = text; *p != '\0'; p++) {
        if (!isdigit((unsigned char)*p)) return 0;
    }
    return 1;
}

static void print_package_row(int index, const Package* pkg) {
    printf("   %-4d %-8s %-8s %-10s %-10s %-6s %-10s %8.2f\n",
           index, pkg->pack_num, pkg->pickup_code, pkg->owner,
           package_direction_text(pkg, NULL),
           transport_text(pkg->transport), storage_status_text(pkg), pkg->money);
}

static void print_package_detail(const Package* pkg) {
    ui_panel("包裹详情", COLOR_BRIGHT_CYAN);
    printf("   单号: %s    取件码: %s\n", pkg->pack_num, pkg->pickup_code);
    printf("   寄件人: %s    收件人: %s\n", pkg->owner, pkg->receiver);
    if (pkg->phone[0] != '\0') printf("   收件电话: %s\n", pkg->phone);
    printf("   重量: %.2f kg    运输: %s    距离: %s\n",
           pkg->weight, transport_text(pkg->transport), distance_text(pkg->distance));
    printf("   价值: %d 元    保险: %s    运费: %.2f 元\n",
           pkg->value, pkg->insurance ? "已购买" : "未购买", pkg->money);
    printf("   状态: %s    包裹状况: %s    运输状态: %s    超时: %s\n",
           storage_status_text(pkg), condition_text(pkg->state[1]),
           transit_text(pkg->state[2]), pkg->state[3] ? "是" : "否");
    printf("   入库时间: %04d-%02d-%02d %02d:%02d\n",
           pkg->in_year, pkg->in_month, pkg->in_day, pkg->in_hour, pkg->in_minute);
    if (pkg->state[0] == PKG_STATE_PICKED) {
        printf("   出库时间: %04d-%02d-%02d %02d:%02d\n",
               pkg->out_year, pkg->out_month, pkg->out_day, pkg->out_hour, pkg->out_minute);
    }
    ui_panel_end();
}

static void print_package_table(LinkPackage head, const char* title) {
    ui_panel(title, COLOR_BRIGHT_CYAN);
    printf("   %-4s %-8s %-8s %-10s %-10s %-6s %-10s %8s\n",
           "序号", "单号", "取件码", "寄件人", "方向", "运输", "状态", "运费");
    ui_rule();
    int index = 1;
    while (head != NULL) {
        print_package_row(index++, head);
        head = head->next;
    }
    ui_panel_end();
}

static void manager_dashboard(LinkPackage packages, LinkUser users) {
    ui_panel("系统概览", COLOR_BRIGHT_CYAN);
    int total = count_packages(packages);
    int waiting = count_in_stock(packages);
    int overdue = count_overdue(packages);
    int transit = count_in_transit(packages);
    int picked = total - waiting - overdue - transit;
    if (picked < 0) picked = 0;

    printf("   %-18s %6d      %-18s %6d\n", "注册用户", count_users(users), "包裹总数", total);
    printf("   %-18s %6d      %-18s %6d\n", "待取件", waiting, "运输中", transit);
    printf("   %-18s %6d      %-18s %6d\n", "超时滞留", overdue, "已取件", picked);
    printf("   %-18s %10.2f\n", "累计运费(元)", total_revenue(packages));
    ui_panel_end();
}

static void manager_put_in(LinkPackage* packages, LinkUser users) {
    while (1) {
        ui_panel("包裹入库", COLOR_BRIGHT_GREEN);
        int condition = 0;
        read_int_range("   包裹状况：1=完好 0=有损: ", 0, 1, &condition);

        char number[MAX_CODE], pickup[MAX_CODE];
        char suggested[MAX_CODE];
        generate_package_number(*packages, suggested, sizeof(suggested));
        ui_notice("建议使用编号: ", COLOR_BRIGHT_YELLOW, 0);
        printf("     %s\n", suggested);

        while (1) {
            read_line("   请输入 4 位包裹编号: ", number, sizeof(number));
            if (strlen(number) != 4 || !is_digits(number)) {
                ui_notice("编号必须为 4 位数字。", COLOR_BRIGHT_RED, 1);
                continue;
            }
            if (find_package_by_number(*packages, number) != NULL) {
                ui_notice("该编号已存在，请重新输入。", COLOR_BRIGHT_RED, 1);
                continue;
            }
            break;
        }

        generate_pickup_code(*packages, suggested, sizeof(suggested));
        ui_notice("建议取件码: ", COLOR_BRIGHT_YELLOW, 0);
        printf("     %s\n", suggested);
        while (1) {
            read_line("   请输入 4 位取件码: ", pickup, sizeof(pickup));
            if (strlen(pickup) != 4 || !is_digits(pickup)) {
                ui_notice("取件码必须为 4 位数字。", COLOR_BRIGHT_RED, 1);
                continue;
            }
            if (find_package_by_code(*packages, pickup) != NULL) {
                ui_notice("该取件码已存在，请重新输入。", COLOR_BRIGHT_RED, 1);
                continue;
            }
            break;
        }

        char owner[MAX_NAME];
        LinkUser owner_user = NULL;
        while (1) {
            read_line("   请输入收件人姓名: ", owner, sizeof(owner));
            owner_user = find_user_by_name(users, owner);
            if (owner_user == NULL) {
                ui_notice("用户不存在，请重新输入。", COLOR_BRIGHT_RED, 1);
                continue;
            }
            break;
        }

        Package* pkg = create_package(number, pickup);
        if (pkg == NULL) {
            ui_notice("内存分配失败，入库未完成。", COLOR_BRIGHT_RED, 1);
            ui_panel_end();
            return;
        }
        strncpy(pkg->owner, owner, MAX_NAME - 1);
        strncpy(pkg->receiver, owner, MAX_NAME - 1);
        pkg->is_user = 1;
        pkg->value = -1;
        pkg->money = 0.0f;
        pkg->state[0] = PKG_STATE_IN_STOCK;
        pkg->state[1] = condition;
        pkg->state[2] = TRANSIT_STATE_ARRIVED;
        pkg->state[3] = OVERDUE_NO;
        set_package_time(pkg, 1);
        append_package(packages, pkg);

        add_user_message(owner_user, "您的快递已到站，请尽快取件。");
        save_packages(*packages);
        append_log("manager_journal.txt", "包裹入库：单号=%s 取件码=%s 收件人=%s 状况=%s",
                   number, pickup, owner, condition_text(condition));
        ui_panel_end();
        ui_notice("入库成功。", COLOR_BRIGHT_GREEN, 1);

        if (!confirm("   是否继续入库？1=是 0=否: ")) break;
    }
}

static void manager_put_out(LinkPackage* packages, LinkUser users) {
    ui_panel("包裹出库", COLOR_BRIGHT_GREEN);
    char number[MAX_CODE];
    read_line("   请输入要出库的包裹编号: ", number, sizeof(number));
    LinkPackage pkg = find_package_by_number(*packages, number);
    if (pkg == NULL) {
        ui_notice("未找到该包裹。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return;
    }
    if (pkg->state[0] != PKG_STATE_IN_STOCK) {
        ui_notice("该包裹当前不可出库。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return;
    }

    char owner_name[MAX_NAME];
    strncpy(owner_name, pkg->owner, MAX_NAME - 1);
    pkg->state[0] = PKG_STATE_PICKED;
    set_package_time(pkg, 0);
    LinkUser owner = find_user_by_name(users, owner_name);
    if (owner != NULL) {
        add_user_message(owner, "您的快递已出库，请确认是否为本人取走。");
    }
    save_packages(*packages);
    append_log("manager_journal.txt", "包裹出库：单号=%s 收件人=%s", number, owner_name);
    ui_panel_end();
    ui_notice("出库成功。", COLOR_BRIGHT_GREEN, 1);
}

static void manager_search(LinkPackage packages, LinkUser users) {
    while (1) {
        const char* items[] = {
            "按用户姓名查询",
            "按包裹编号查询",
            "按运输方式查询",
            "按运输距离查询",
            "返回上级"
        };
        ui_menu("快递查询", items, 5);
        int choice = 0;
        read_int_range("   请选择查询方式: ", 1, 5, &choice);

        if (choice == 1) {
            char name[MAX_NAME];
            read_line("   请输入用户姓名: ", name, sizeof(name));
            LinkUser user = find_user_by_name(users, name);
            if (user == NULL) {
                ui_notice("未找到该用户。", COLOR_BRIGHT_RED, 1);
                continue;
            }
            ui_panel("按用户查询结果", COLOR_BRIGHT_CYAN);
            printf("   %-4s %-8s %-8s %-10s %-6s %-10s\n", "序号", "单号", "取件码", "状态", "运输", "运费");
            ui_rule();
            int index = 1;
            LinkPackage p = packages;
            while (p != NULL) {
                if (strcmp(p->owner, name) == 0) print_package_row(index++, p);
                p = p->next;
            }
            ui_panel_end();
        } else if (choice == 2) {
            char number[MAX_CODE];
            read_line("   请输入包裹编号: ", number, sizeof(number));
            LinkPackage pkg = find_package_by_number(packages, number);
            if (pkg == NULL) {
                ui_notice("未找到该包裹。", COLOR_BRIGHT_RED, 1);
            } else {
                print_package_detail(pkg);
            }
        } else if (choice == 3) {
            int transport = 0;
            read_int_range("   运输方式：0=标准 1=加急: ", 0, 1, &transport);
            LinkPackage p = packages;
            LinkPackage filtered = NULL;
            while (p != NULL) {
                if (p->transport == transport) {
                    Package* clone = create_package(p->pack_num, p->pickup_code);
                    if (clone != NULL) {
                        *clone = *p;
                        clone->next = filtered;
                        filtered = clone;
                    }
                }
                p = p->next;
            }
            print_package_table(filtered, "按运输方式查询结果");
            free_packages(filtered);
        } else if (choice == 4) {
            int distance = 0;
            read_int_range("   运输距离：0=省外 1=省内: ", 0, 1, &distance);
            LinkPackage p = packages;
            LinkPackage filtered = NULL;
            while (p != NULL) {
                if (p->distance == distance) {
                    Package* clone = create_package(p->pack_num, p->pickup_code);
                    if (clone != NULL) {
                        *clone = *p;
                        clone->next = filtered;
                        filtered = clone;
                    }
                }
                p = p->next;
            }
            print_package_table(filtered, "按运输距离查询结果");
            free_packages(filtered);
        } else {
            break;
        }
        if (!confirm("   是否继续查询？1=是 0=否: ")) break;
    }
}

static void manager_view_inventory(LinkPackage packages) {
    ui_panel("当前库存", COLOR_BRIGHT_CYAN);
    int index = 1;
    int shown = 0;
    LinkPackage p = packages;
    while (p != NULL) {
        if (p->state[0] == PKG_STATE_IN_STOCK && p->state[3] == OVERDUE_NO) {
            print_package_row(index++, p);
            shown++;
        }
        p = p->next;
    }
    if (shown == 0) ui_notice("当前没有正常待取包裹。", COLOR_GRAY, 0);
    ui_panel_end();
}

static void manager_delete_user(LinkPackage* packages, LinkUser* users) {
    ui_panel("注销用户", COLOR_BRIGHT_RED);
    char account[MAX_ACCOUNT], password[MAX_PASSWORD];
    read_line("   请输入用户账号: ", account, sizeof(account));
    read_line("   请输入用户密码: ", password, sizeof(password));
    LinkUser user = find_user_by_account(*users, account);
    if (user == NULL || strcmp(user->password, password) != 0) {
        ui_notice("账号或密码不匹配，无法注销。", COLOR_BRIGHT_RED, 1);
        ui_panel_end();
        return;
    }
    if (!confirm("   确认注销该用户及其全部包裹？1=确认 0=取消: ")) {
        ui_panel_end();
        return;
    }
    char account_copy[MAX_ACCOUNT], name_copy[MAX_NAME];
    strncpy(account_copy, user->account, MAX_ACCOUNT - 1);
    strncpy(name_copy, user->name, MAX_NAME - 1);
    if (remove_user_and_packages(users, user, packages)) {
        save_users(*users);
        save_packages(*packages);
        append_log("manager_journal.txt", "管理员注销用户：账号=%s 姓名=%s", account_copy, name_copy);
        ui_panel_end();
        ui_notice("用户已注销。", COLOR_BRIGHT_GREEN, 1);
    } else {
        ui_panel_end();
        ui_notice("注销失败。", COLOR_BRIGHT_RED, 1);
    }
}

static void manager_view_logs(void) {
    ui_panel("管理员操作日志", COLOR_BRIGHT_CYAN);
    FILE* f = fopen("data/manager_journal.txt", "r");
    if (f == NULL) {
        ui_notice("暂无操作日志。", COLOR_GRAY, 0);
        ui_panel_end();
        return;
    }
    char line[512];
    int index = 1;
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        printf("   %3d. %s\n", index++, line);
    }
    fclose(f);
    ui_panel_end();
}

void manager_console(LinkPackage* packages, LinkUser* users) {
    int running = 1;
    while (running) {
        ui_header("管理员工作台", "快递入库 / 出库 / 查询 / 库存管理");
        const char* items[] = {
            "系统概览",
            "包裹入库",
            "包裹出库",
            "查询包裹",
            "库存列表",
            "注销用户",
            "查看操作日志",
            "返回主界面"
        };
        ui_menu("请选择操作", items, 8);

        int choice = 0;
        read_int_range("   请输入操作编号: ", 0, 8, &choice);
        switch (choice) {
        case 1:
            manager_dashboard(*packages, *users);
            break;
        case 2:
            manager_put_in(packages, *users);
            break;
        case 3:
            manager_put_out(packages, *users);
            break;
        case 4:
            manager_search(*packages, *users);
            break;
        case 5:
            manager_view_inventory(*packages);
            break;
        case 6:
            manager_delete_user(packages, users);
            break;
        case 7:
            manager_view_logs();
            break;
        case 0:
            running = 0;
            break;
        default:
            break;
        }
        refresh_user_packages(*users, *packages);
        if (running) ui_pause(NULL);
    }
}

/* ========== src/main.c ========== */
#include <stdio.h>
#include <stdlib.h>


static void print_system_overview(LinkUser users, LinkPackage packages) {
    ui_panel("系统运行状态", COLOR_BRIGHT_CYAN);
    int total = count_packages(packages);
    int waiting = count_in_stock(packages);
    int overdue = count_overdue(packages);
    int transit = count_in_transit(packages);
    int picked = total - waiting - overdue - transit;
    if (picked < 0) picked = 0;

    printf("   %-18s %6d      %-18s %6d\n", "注册用户", count_users(users), "包裹总数", total);
    printf("   %-18s %6d      %-18s %6d\n", "待取件", waiting, "运输中", transit);
    printf("   %-18s %6d      %-18s %6d\n", "超时滞留", overdue, "已取件", picked);
    printf("   %-18s %10.2f\n", "累计运费(元)", total_revenue(packages));
    ui_panel_end();
}

int main(void) {
    ui_init();
    ensure_data_dir();

    LinkUser users = load_users();
    LinkPackage packages = load_packages();
    if (users == NULL && packages == NULL) {
        seed_demo_data();
        users = load_users();
        packages = load_packages();
    }

    simulate_delivery_and_overdue(packages, users);
    refresh_user_packages(users, packages);
    LinkManager managers = create_default_managers();

    int running = 1;
    while (running) {
        ui_header("菜鸟驿站智能快递管理系统", "纯 C 实现 · 控制台可视化 · 无第三方依赖");
        print_system_overview(users, packages);

        const char* roles[] = {
            "管理员登录",
            "客户登录 / 注册",
            "退出系统"
        };
        ui_menu("请选择登录角色", roles, 3);
        int role = 0;
        read_int_range("   请输入角色编号: ", 0, 3, &role);

        if (role == 1) {
            if (login_manager(managers)) {
                manager_console(&packages, &users);
            }
            refresh_user_packages(users, packages);
        } else if (role == 2) {
            ui_header("客户入口", NULL);
            const char* customer_items[] = {
                "已有账号，直接登录",
                "注册新账号",
                "返回主界面"
            };
            ui_menu("请选择", customer_items, 3);
            int entry = 0;
            read_int_range("   请输入选项: ", 1, 3, &entry);

            LinkUser current = NULL;
            if (entry == 1) current = login_user(&users);
            else if (entry == 2) current = register_user_interactive(&users);

            if (current != NULL) {
                refresh_user_packages(users, packages);
                user_console(&packages, &users, current);
                refresh_user_packages(users, packages);
            }
        } else {
            running = 0;
        }
    }

    save_users(users);
    save_packages(packages);
    append_log("system_journal.txt", "系统正常退出。");

    free_managers(managers);
    free_users(users);
    free_packages(packages);
    ui_clear_screen();
    ui_center_line("感谢使用菜鸟驿站智能快递管理系统", COLOR_BRIGHT_GREEN, 1);
    return 0;
}
