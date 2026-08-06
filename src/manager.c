#include "manager.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "ui.h"

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
