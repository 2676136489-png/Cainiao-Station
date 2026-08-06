

#define _CRT_SECURE_NO_WARNINGS 1
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdbool.h>
//#include <windows.h>
#include"package.h"
/*
1.主包裹链：从软件打开时，从文件中整体取出的一条记录全部包裹数据的包裹链,建议统一用PackageLine定义

2.主用户链：从软件打开时，从文件中整体取出的一条记录全部用户数据的包裹链,建议统一用UserLine定义

*/
//用户

#ifndef User_H
#define User_H

struct Message {
    char content[60];
    time_t timestamp;  // 消息时间戳
};

struct User {
    char Name[5];
    char ACnumber[5];
    char Password[5];
    struct Package* PackageLine;
    struct User* NextPeople;
    struct Message message[30];  // 修改为结构体数组
    int l;
    int NewMessage;
};

typedef struct User* LinkPeople;//User的链

void addUserMessage(LinkPeople user, const char* content) {
    if (user == NULL || content == NULL) return;
    
    user->l = (user->l + 1) % 30;  // 循环缓冲区
    strncpy(user->message[user->l].content, content, sizeof(user->message[user->l].content) - 1);
    user->message[user->l].content[sizeof(user->message[user->l].content) - 1] = '\0';
    user->message[user->l].timestamp = time(NULL);
    
    if (user->NewMessage < 30) {
        user->NewMessage++;
    }
}
//向用户链中插入用户（从文件中读取）程序开始自动启动
LinkPeople Initialization_User()
{//初始化主用户链
    LinkPeople tmp; 
    LinkPeople FirstPeople=NULL;
    tmp=FirstPeople;
    FirstPeople=NULL;
    tmp=NULL;
    FILE* INSERT;
    INSERT=fopen("User.txt","rb");
    // int cnt=0;
    while(1){
        LinkPeople Now_User=(LinkPeople)malloc(sizeof(struct User));
        // cnt++;
        // printf("%d",cnt);
        int UserNUM=fread(Now_User,sizeof(struct User)-sizeof(LinkPeople)-sizeof(LinkPackage),1,INSERT);//从文件中读出New_User的数据
        if(UserNUM==0)
        {//如果从标记点处往下用户数为零则退出并清空刚创建的New_User
            free(Now_User);
            break;
        }

        Now_User->NextPeople=NULL;
        if(FirstPeople==NULL){
            FirstPeople=Now_User;
        }
        else{
            tmp->NextPeople=Now_User;
        }
        tmp=Now_User;
        tmp->PackageLine=NULL;
    }
    fclose(INSERT);
    return FirstPeople;//用UserLine承接
}


//用户注册(完成)
LinkPeople Register(char* NAME,char* ACNUMBER,char* PASSWORD,LinkPeople FirstPeople){
    struct tm* Now=GetNowTime();
    LinkPeople tmp = FirstPeople;
    LinkPeople NewUser=(LinkPeople)malloc(sizeof(struct User));
    NewUser->PackageLine=NULL;
    NewUser->NextPeople=NULL;
    FILE* UserFile;
    UserFile=fopen("User.txt","ab");

    FILE* Journal=fopen("UserJournal.txt","a");
    fprintf(Journal,"[ %4d.%02d.%02d %02d:%02d:%02d ] 新用户注册-----",Now->tm_year+1900,Now->tm_mon+1,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
    fprintf(Journal,"新用户:%s  注册\n",NAME);
    fprintf(Journal,"新用户--账号：%s  密码：%s\n",ACNUMBER,PASSWORD);
    fclose(Journal);

    NewUser->l=-1;
    strcpy(NewUser->Name,NAME);
    strcpy(NewUser->ACnumber,ACNUMBER);
    strcpy(NewUser->Password,PASSWORD);
    fwrite(NewUser,sizeof(struct User)-sizeof(LinkPeople)-sizeof(LinkPackage),1,UserFile);
    fclose(UserFile);

    //注册时自动插入用户链中
    if (FirstPeople == NULL) {
        FirstPeople = NewUser;
    }
    else {
        while(tmp->NextPeople!=NULL)tmp=tmp->NextPeople;
        tmp->NextPeople = NewUser;
    }
    return FirstPeople;
}


//链接人物名下包裹
void People_Package(LinkPeople UserLine, LinkPackage PackageLine) {
    LinkPeople currentUser = UserLine;
    while (currentUser != NULL) {
        LinkPackage userPackages = NULL;
        LinkPackage lastUserPackage = NULL;

        LinkPackage currentPackage = PackageLine;
        while (currentPackage != NULL) {
            if (strcmp(currentUser->Name, currentPackage->Owner) == 0) {
                LinkPackage newPackage = (LinkPackage)malloc(sizeof(struct Package));
                memcpy(newPackage, currentPackage, sizeof(struct Package));
                
                // 同步状态
                newPackage->state[0] = currentPackage->state[0];
                newPackage->state[1] = currentPackage->state[1];
                newPackage->state[2] = currentPackage->state[2];
                newPackage->state[3] = currentPackage->state[3];
                
                newPackage->NextPackage = NULL;

                if (userPackages == NULL) {
                    userPackages = newPackage;
                    lastUserPackage = newPackage;
                } else {
                    lastUserPackage->NextPackage = newPackage;
                    lastUserPackage = newPackage;
                }
            }
            currentPackage = currentPackage->NextPackage;
        }
        currentUser->PackageLine = userPackages;
        currentUser = currentUser->NextPeople;
    }
}

//用户注销（未完成）
LinkPeople Cancel(LinkPeople FirstPeople,char* People_Name)
{
    struct tm* Now=GetNowTime();
    LinkPeople Slow,tmp;
    Slow=FirstPeople;
    tmp=FirstPeople;
    while(strcmp(tmp->Name,People_Name)&&FirstPeople!=NULL){
        Slow=tmp;
        tmp=tmp->NextPeople;
    }
    if(tmp==NULL){
        printf("无该用户");
        return NULL;
    }
    if(tmp->PackageLine!=NULL){
        printf("您有未完成的包裹，暂无法注销账户");
        FILE* Journal;
        Journal=fopen("UserJournal.txt","a");
        fprintf(Journal,"[ %4d.%02d.%02d %02d:%02d:%02d ] 用户:%s 尝试注销账号，有未完成包裹注销失败\n",Now->tm_year+1900,Now->tm_mon+1,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec,tmp->Name);
        fclose(Journal);
        return FirstPeople;
    }
    FILE* Journal;
    Journal=fopen("UserJournal.txt","a");
    fprintf(Journal,"[ %4d.%02d.%02d %02d:%02d:%02d ] 用户:%s 注销账号\n",Now->tm_year+1900,Now->tm_mon+1,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec,tmp->Name);
    fclose(Journal); 
    if(tmp==Slow){
        FirstPeople=FirstPeople->NextPeople;
    }
    else{
        Slow->NextPeople=tmp->NextPeople;
        Slow=tmp;
    }
    free(Slow);

    FILE* file = fopen("User.tmp", "wb"); // 使用写入二进制模式，会覆盖旧文件
            FILE* REMOVE=fopen("User.txt","rb");
            if (!file) {
                perror("无法打开用户文件");
                return FirstPeople;
            }

            while (FirstPeople != NULL) {
                fwrite(FirstPeople, sizeof(struct User) - sizeof(struct User*)-sizeof(LinkPackage), 1, file);
                FirstPeople =FirstPeople->NextPeople;
            }
            fflush(REMOVE);
            fclose(REMOVE);
            fclose(file);
            remove("User.txt");
            rename("User.tmp","User.txt");

    return FirstPeople;   
}


//(结束后重新保存，每次关闭前都应执行此操作)
void Keep_In_UserFile(LinkPeople FirstPeople){
    if(FirstPeople==NULL)return;
    FILE* KEEP;
    KEEP=fopen("User.txt","wb");
    while(1){
        if(FirstPeople==NULL)break;
        fwrite(FirstPeople,sizeof(struct User)-sizeof(LinkPeople)-sizeof(LinkPackage),1,KEEP);
        FirstPeople=FirstPeople->NextPeople;
    }
    fflush(KEEP);
    fclose(KEEP);
}

//取件函数
int PickUp_Package(LinkPackage PackageLine, char* PUCODE, LinkPeople User, LinkPeople UserLine)
{
    if (PackageLine == NULL || PUCODE == NULL || User == NULL || UserLine == NULL) {
        printf("参数错误！\n");
        return 0;
    }

    struct tm* Now = GetNowTime();
    FILE* UserJournal = fopen("UserJournal.txt", "a");
    if (UserJournal == NULL) {
        printf("无法打开日志文件！\n");
        return 0;
    }

    // (1) 查找包裹
    LinkPackage targetPkg = PackageLine;
    while (targetPkg != NULL && strcmp(targetPkg->PUcode, PUCODE) != 0) {
        targetPkg = targetPkg->NextPackage;
    }

    if (targetPkg == NULL) {
        printf("无该包裹！\n");
        fprintf(UserJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 用户%s 取件：包裹不存在，取件失败！\n",
                Now->tm_year + 1900, Now->tm_mon + 1, Now->tm_mday,
                Now->tm_hour, Now->tm_min, Now->tm_sec, User->Name);
        fclose(UserJournal);
        return 0;
    }

    // (2) 检查包裹是否已经出库
    if (targetPkg->state[0] == 0) {
        printf("该包裹已经出库，不能重复取件！\n");
        fprintf(UserJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 用户%s 尝试重复取件：包裹已出库！\n",
                Now->tm_year + 1900, Now->tm_mon + 1, Now->tm_mday,
                Now->tm_hour, Now->tm_min, Now->tm_sec, User->Name);
        fclose(UserJournal);
        return 0;
    }

    // (3) 检查包裹是否超时
    if (targetPkg->state[3] == 1) {
        printf("包裹滞留超时，已被送回，可去前台联系工作人员处理。\n");
        fprintf(UserJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 用户%s 取件：包裹滞留超时，取件失败！\n",
                Now->tm_year + 1900, Now->tm_mon + 1, Now->tm_mday,
                Now->tm_hour, Now->tm_min, Now->tm_sec, User->Name);
        fclose(UserJournal);
        return 0;
    }

    // (4) 检查当前用户是否为包裹所有者
    int isOwner = (strcmp(targetPkg->Owner, User->Name) == 0);
    if (!isOwner) {
        printf("是否为用户 %s 代取快递？ 1.确认  0.取消\n", targetPkg->Owner);
        int CHOOSE;
        scanf("%d", &CHOOSE);
        if (CHOOSE != 1) {
            fprintf(UserJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 用户%s 代取件操作取消！\n",
                    Now->tm_year + 1900, Now->tm_mon + 1, Now->tm_mday,
                    Now->tm_hour, Now->tm_min, Now->tm_sec, User->Name);
            fclose(UserJournal);
            return 0;
        }
    }

    // (5) 查找包裹所有者，并更新其包裹状态
    LinkPeople owner = UserLine;
    while (owner != NULL && strcmp(owner->Name, targetPkg->Owner) != 0) {
        owner = owner->NextPeople;
    }

    if (owner != NULL) {
        LinkPackage userPkg = owner->PackageLine;
        while (userPkg != NULL && strcmp(userPkg->PUcode, PUCODE) != 0) {
            userPkg = userPkg->NextPackage;
        }
        if (userPkg != NULL && userPkg->state[0] == 1) {  // 只更新未出库的包裹
            userPkg->state[0] = 0;
        }
    }

    // (6) 更新主包裹链的状态（仅在包裹未出库时）
    if (targetPkg->state[0] == 1) {
        targetPkg->state[0] = 0;
        
        // (7) 记录日志
        fprintf(UserJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 用户%s %s取件：已出库！\n",
                Now->tm_year + 1900, Now->tm_mon + 1, Now->tm_mday,
                Now->tm_hour, Now->tm_min, Now->tm_sec, User->Name,
                isOwner ? "" : "代");
        
        fclose(UserJournal);
        return 1;
    } else {
        printf("取件失败：包裹状态异常！\n");
        fprintf(UserJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 用户%s 取件失败：包裹状态异常！\n",
                Now->tm_year + 1900, Now->tm_mon + 1, Now->tm_mday,
                Now->tm_hour, Now->tm_min, Now->tm_sec, User->Name);
        fclose(UserJournal);
        return 0;
    }
}


/*
    存-1表示无该状态
    state[0]存取状况1--库中，0--出库；
    state[1]存放包裹状况（是否损坏或丢失）1-完好 0-损坏 ；
    state[2]运输状态，1-运达，0-途中（用户互送），-1-途中（向非用户的人送）；
    state[3]是否超时，1-滞留超时，0-未超时
    */

//寄出包裹函数(可给用户使用)
LinkPackage Package_write(float WEIGHT, int TRANSPORT, int VALUE, int DISTANCE, int INSURANCE, LinkPackage  FirstPackage,LinkPeople User,char *NAME,int IS,char* PhoneNUM) {
    
    LinkPackage NewPackage = (LinkPackage)malloc(sizeof(struct Package));
    strcpy(NewPackage->Owner,User->Name);
    
    NewPackage->NextPackage = NULL;
    FILE* PackageFile = fopen("Package.txt", "ab");
    
    SetPackageTime(NewPackage,1);
    //state用于辨别是出库还是入库，1--入库/途中，0--出库/送达
    strcpy(NewPackage->Receiver,NAME);
    if(IS==0){
        strcpy(NewPackage->PhoneNUM,PhoneNUM);
        NewPackage->IsUser=0;
    }
    else NewPackage->IsUser=1;
    NewPackage->state[0]=-1;
    NewPackage->state[2]=0;
    NewPackage->distance = DISTANCE;
    NewPackage->insurance = INSURANCE;
    NewPackage->transport = TRANSPORT;
    NewPackage->value = VALUE;
    NewPackage->weight = WEIGHT;

    fwrite(NewPackage, sizeof(struct Package) - sizeof(struct Package*), 1, PackageFile);
    fflush(PackageFile);
    fclose(PackageFile);

    struct Package* temp = FirstPackage;
    if (FirstPackage == NULL) {
        FirstPackage = NewPackage;
    }
    else {
        while (temp->NextPackage != NULL) {
            temp = temp->NextPackage;
        }
        temp->NextPackage = NewPackage;
    }

    float sum_money = 0; // ???
    if (TRANSPORT == true) {
        if (DISTANCE == true) {
            sum_money = 12; // ???12
            if (WEIGHT > 1) sum_money += (WEIGHT - 1) * 2; // ????????2??????
        }
        else {
            sum_money = 22;
            if (WEIGHT > 1) sum_money += (WEIGHT - 1) * 8;
        }
    }
    else {
        if (DISTANCE == true) {
            sum_money = 10;
            if (WEIGHT > 1) sum_money += (WEIGHT - 1) * 2;
        }
        else {
            sum_money = 12;
            if (WEIGHT > 1) sum_money += (WEIGHT - 1) * 5;
        }
    }
    if (VALUE >= 1000) sum_money = sum_money * 1.1;
    if (INSURANCE) {
        if (VALUE < 1000) sum_money += 2;
        else {
            sum_money += 2;
            sum_money += (VALUE - 1000) * 0.003; // ?????????????????????
        }
    }
    NewPackage->money = sum_money;
    return FirstPackage;
}

// 查看包裹状态
void checkPackageStatus(LinkPeople currentUser) {
    if (currentUser->PackageLine == NULL) {
        printf("             您没有待取的包裹。\n");
        return;
    }
    LinkPackage temp = currentUser->PackageLine;
    int cnt=0;
    while (temp != NULL) {
        ++cnt;
        printf("包裹%d----",cnt);
        if(temp->state[0]!=-1&&temp->state[3]==0){
            printf("包裹编号: %s ",temp->PackNUM);
            printf("取件码: %s ", temp->PUcode);
            switch(temp->state[0]){
                case 1:printf("取件状态: 库中 ");break;
                case 0:printf("取件状态：已出库 ");
            }
            switch(temp->state[1]){
                case 1:printf("储存状态：无损坏 \n");break;
                case 0:printf("储存状态：有损 \n");if(temp->insurance==1)printf("发现您买了保险，可与前台联系赔付运费！\n");
            }
        }
        else if(temp->state[0]!=-1&&temp->state[3]==1){
            printf("包裹编号: %s ",temp->PackNUM);
            printf("取件码: %s ", temp->PUcode);
            switch(temp->state[0]){
                case 1:printf("取件状态: 滞留超时已送回！\n");break;
                case 0:printf("取件状态：已出库 \n");
            }
        }
        else if(temp->state[0]==-1&&temp->state[2]!=-1){
            switch(temp->state[2]){
                case 1:printf("运输状态：已送达目的地 \n");break;
                case 0:printf("运输状态：正在运输途中 \n");
            }
        }
        temp = temp->NextPackage;
    }
}

// 注销账号
int cancelAccount(LinkPeople* FirstPeople, LinkPeople currentUser)
{
    printf("    您是否要注销账号，注销后，您无法再此账号上进行任何操作，请再一次点清您的包裹\n");
    printf("              如果您确定要注销账号，请按1，否则请按除1外任意数字\n");
    int j = 0;
    scanf("%d", &j);
    if (j != 1)
    {
        printf("              注销操作已取消\n");
        return 1;
    }
    else if (j == 1)
    {
        *FirstPeople = Cancel(*FirstPeople, currentUser->Name);
        printf("                           账号注销成功！\n");
        return 0;
    }
    return 1;
}

// 登录
LinkPeople login(LinkPeople FirstPeople)
{
    bool flag = false; // 用于判断是否找到账号
    char ZH[5];
    LinkPeople currentUser = NULL;

    while (!flag)
    {
        printf("                     请输入登录账号\n");
        printf("                    账号：");
        if (scanf("%4s", ZH) != 1)
        {
            printf("         输入无效，请输入正确的账号。\n");
            printf("                    账号：");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }
        LinkPeople find = FirstPeople;
        while (find != NULL)
        {
            if (strcmp(find->ACnumber, ZH) == 0)
            {
                flag = true;
                currentUser = find;
                break;
            }
            find = find->NextPeople;
        }
        if (!flag)
        {
            printf("             账号不存在，请重新输入。\n");
            int o;

            do {
                printf("                您是否需要注册账号？\n");//考虑用户是否误触;
                printf("       输入 0 前往注册页面,输入 1 重新输入账号\n");
                if (scanf("%d", &o) != 1)
                {
                    printf("            输入无效，请输入正确的选项。\n");
                    while (getchar() != '\n'); // 清空输入缓冲区
                    continue;
                }
            } while (o != 0 && o != 1);
            if (o == 0)
            {
                char acnumber[5];
                char password[5];
                while (1) {
                    printf("                请输入注册账号:");
                    if (scanf("%4s", acnumber) != 1)
                    {
                        printf("             输入无效，请输入正确的账号。\n");
                        continue;
                    }
                    // 检查账号是否重复
                    LinkPeople check = FirstPeople;
                    int isDuplicate = 0;//这个是检查是否重复的标志
                    while (check != NULL)
                    {
                        if (strcmp(check->ACnumber, acnumber) == 0)
                        {
                            isDuplicate = 1;
                            break;
                        }
                        check = check->NextPeople;
                    }
                    if (isDuplicate)
                    {
                        printf("            该账号已存在，请更换账号。\n");
                    }
                    else
                    {
                        break;
                    }
                }
                printf("              请输入注册账号密码:");
                if (scanf("%4s", password) != 1)
                {
                    printf("            输入无效，请输入正确的密码。\n");
                    continue;
                }
                Register(acnumber, acnumber, password, FirstPeople);
                printf("                    新账号已经注册成功。\n");
                system("cls");
                // 找到新注册的用户
                find = FirstPeople;
                while (find != NULL)
                {
                    if (strcmp(find->ACnumber, acnumber) == 0)
                    {
                        return find;
                    }
                    find = find->NextPeople;
                }
                return NULL;
            }
        }
    }
    // 输入账号后的登录
    bool M = false; // 用于判断是否需要重新输入密码
    char MM[5];
    while (!M)
    {
        printf("                     请输入登录密码\n");
        printf("                       密码：");
        if (scanf("%4s", MM) != 1)
        {
            printf("            输入无效，请输入正确的密码。\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }
        if (strcmp(currentUser->Password, MM) == 0)
        {
            M = true;
            printf("                      密码正确\n");
            system("cls");
        }
        else
        {
            printf("            密码输入错误，请重新输入。\n");
        }
    }
    return currentUser;
}

void ScanMessage(LinkPeople ReferingUser){
    if (ReferingUser == NULL) {
        printf("错误：用户信息无效！\n");
        return;
    }

    if (ReferingUser->l < 0) {
        printf("您目前没有任何消息。\n");
        return;
    }

    printf("\n===== 您的消息 (%d条) =====\n", ReferingUser->l + 1);
    if (ReferingUser->NewMessage) {
        printf("(*) 您有 %d 条新消息\n", ReferingUser->NewMessage);
    }

    // 按时间倒序显示消息
    for (int i = 0; i <= ReferingUser->l; i++) {
        char timeStr[20];
        struct tm* timeinfo = localtime(&ReferingUser->message[i].timestamp);
        strftime(timeStr, sizeof(timeStr), "%m-%d %H:%M", timeinfo);
        
        char newFlag = (ReferingUser->NewMessage > 0 && i >= ReferingUser->l - ReferingUser->NewMessage + 1) 
                      ? '*' : ' ';
        printf("[%02d] %c [%s] %s\n", 
               i, newFlag, timeStr, ReferingUser->message[i].content);
    }

    // 标记所有消息为已读
    ReferingUser->NewMessage = 0;
}

#endif