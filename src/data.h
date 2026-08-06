#ifndef CAINIAO_DATA_H
#define CAINIAO_DATA_H

#include "model.h"

int ensure_data_dir(void);
LinkPackage load_packages(void);
LinkUser load_users(void);
void save_packages(LinkPackage head);
void save_users(LinkUser head);
void seed_demo_data(void);
void append_log(const char* file_name, const char* fmt, ...);

#endif
