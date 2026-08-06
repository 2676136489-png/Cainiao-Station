#include "ui.h"

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
