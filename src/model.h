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
