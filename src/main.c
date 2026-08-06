#include <stdio.h>
#include <stdlib.h>

#include "auth.h"
#include "data.h"
#include "manager.h"
#include "model.h"
#include "ui.h"
#include "user.h"

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
