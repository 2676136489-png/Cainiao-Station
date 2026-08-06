#include "model.h"

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
