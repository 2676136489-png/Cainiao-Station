#include "data.h"

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
