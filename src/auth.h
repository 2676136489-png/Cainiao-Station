#ifndef CAINIAO_AUTH_H
#define CAINIAO_AUTH_H

#include "model.h"

LinkManager create_default_managers(void);
LinkUser login_user(LinkUser* users);
LinkUser register_user_interactive(LinkUser* users);
int login_manager(LinkManager managers);
int cancel_user_account(LinkUser* users, LinkUser user, LinkPackage* packages);

#endif
