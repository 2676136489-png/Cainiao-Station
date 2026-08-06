#include "auth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "ui.h"

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
