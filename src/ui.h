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
