#include "user.h"

#include <stdio.h>
#include <string.h>

#include "auth.h"
#include "data.h"
#include "ui.h"

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
