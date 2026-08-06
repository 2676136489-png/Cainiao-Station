#include <stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#ifndef Package_H
#define Package_H
struct Package {
    char PackNUM[5];//包裹编号
    char PUcode[5];//取件码
    float weight;//重量
    int transport;//运输方式,true为快速送，false为普通送
    int value;//物品的价格（以整数记），高于1000元物品令加收包装费，为总体运费的1.1倍
    int distance;//距离,分为省内和省外，true为省内
    int insurance;//保险，若丢件和损坏可以获得【物品原价】赔偿；未保价物品根据【运费】的三倍赔偿
    char Owner[5];//包裹物主
    char Receiver[5];
    char PhoneNUM[12];
    int IsUser;//1是
    int state[4];
    /*
    存-1表示无该状态
    state[0]存取状况1--库中，0--出库；
    state[1]存放包裹状况（是否损坏或丢失）1-完好 0-损坏 ；
    state[2]运输状态，1-运达，0-途中（用户互送），-1-途中（向非用户的人送）；
    state[3]是否超时，1-滞留超时，0-未超时
    */
    int IN_second,IN_minute,IN_hour,IN_day,IN_month,IN_year,IN_Yday;//入库时间
    int OUT_second,OUT_minute,OUT_hour,OUT_day,OUT_month,OUT_year,OUT_Yday;//出库时间
    float money;//运费
    struct Package* NextPackage;
};

typedef struct Package* LinkPackage;
typedef struct Package Package;


void printHorizontalLine(int length) 
{
    for (int i = 0; i < length; i++) 
	{
        printf("-");
    }
    printf("\n");
}

// 打印居中对齐的内容
void printCenteredLine(const char* content, int width) 
{
    int padding = (width - strlen(content)) / 2;
    printf("|");
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s", content);
    for (int i = 0; i < width - strlen(content) - padding; i++) 
	{
        printf(" ");
    }
    printf("|\n");
}

// 显示包裹信息的函数
void displayPackageInfo(struct Package* package) 
{
    if (package == NULL) {
        printf("包裹信息为空。\n");
        return;
    }

    int width = 50; // 边框宽度
    printHorizontalLine(width);

    char buffer[256];
    // 打印包裹编号
    snprintf(buffer, sizeof(buffer), "包裹编号: %s", package->PackNUM);
    printCenteredLine(buffer, width);

    // 打印取件码
    if(package->state[0]==1|| package->state[0]==0)
    snprintf(buffer, sizeof(buffer), "取件码: %s", package->PUcode);
    printCenteredLine(buffer, width);

    // 打印重量
    snprintf(buffer, sizeof(buffer), "重量: %.2f 千克", package->weight);
    printCenteredLine(buffer, width);

    // 打印距离
    snprintf(buffer, sizeof(buffer), "距离: %s", (package->distance ? "省内" : "省外"));
    printCenteredLine(buffer, width);

    // 打印保险
    snprintf(buffer, sizeof(buffer), "保险: %s", (package->insurance ? "保价" : "未保价"));
    printCenteredLine(buffer, width);

    // 打印包裹主人
    snprintf(buffer, sizeof(buffer), "包裹主人: %s", package->Owner);
    printCenteredLine(buffer, width);

    // 根据state状态打印取件状态、丢失状态、派送状态和超时状态
    if (package->state[0]) {
        snprintf(buffer, sizeof(buffer), "取件状态: 已取，出库时间: %d-%d-%d %d:%d:%d",
            package->OUT_year + 1900, package->OUT_month + 1, package->OUT_day,
            package->OUT_hour, package->OUT_minute, package->OUT_second);
    }
    else {
        snprintf(buffer, sizeof(buffer), "取件状态: 未取，入库时间: %d-%d-%d %d:%d:%d",
            package->IN_year + 1900, package->IN_month + 1, package->IN_day,
            package->IN_hour, package->IN_minute, package->IN_second);
    }
    printCenteredLine(buffer, width);

    if (package->state[1]) {
        snprintf(buffer, sizeof(buffer), "包裹丢失状态: 已找回");
    }
    else {
        snprintf(buffer, sizeof(buffer), "包裹丢失状态: 未找回");
    }
    printCenteredLine(buffer, width);

    if (package->state[2]) {
        snprintf(buffer, sizeof(buffer), "派送状态: 已派送");
    }
    else {
        snprintf(buffer, sizeof(buffer), "派送状态: 未派送");
    }
    printCenteredLine(buffer, width);

    if (package->state[3]) {
        snprintf(buffer, sizeof(buffer), "是否超时: 已经超时");
    }
    else {
        snprintf(buffer, sizeof(buffer), "是否超时: 未超时");
    }
    printCenteredLine(buffer, width);

    // 打印运费
    snprintf(buffer, sizeof(buffer), "运费: %.2f 元", package->money);
    printCenteredLine(buffer, width);

    printHorizontalLine(width);
}

void SetPackageTime(struct Package* SetPackage,int state){//state用于辨别是出库还是入库，1--入库/途中，0--出库/送达
    time_t now = time(NULL); 
    struct tm* Now=localtime(&now);
    if(state){
        SetPackage->IN_second=Now->tm_sec;
        SetPackage->IN_minute=Now->tm_min;
        SetPackage->IN_hour=Now->tm_hour;
        SetPackage->IN_day=Now->tm_mday;
        SetPackage->IN_month=Now->tm_mon;
        SetPackage->IN_year=Now->tm_year;
        SetPackage->IN_Yday=Now->tm_yday;
    }
    else{
        SetPackage->OUT_second=Now->tm_sec;
        SetPackage->OUT_minute=Now->tm_min;
        SetPackage->OUT_hour=Now->tm_hour;
        SetPackage->OUT_day=Now->tm_mday;
        SetPackage->OUT_month=Now->tm_mon;
        SetPackage->OUT_year=Now->tm_year;
        SetPackage->OUT_Yday=Now->tm_yday;
    }
}

bool is_leap_year(int year){
	return (year%4==0&&year%100!=0)||(year%400==0);
}

int AllYearDay(int year){
	if(is_leap_year(year))return 366;
	else return 365;
}

/*
    存-1表示无该状态
    state[0]存取状况1--库中，0--出库；
    state[1]存放包裹状况（是否损坏或丢失）1-完好 0-损坏 ；
    state[2]运输状态，1-运达，0-途中（用户互送），-1-途中（向非用户的人送）；
    state[3]是否超时，1-滞留超时，0-未超时
    */


// void handleOverTimePackage(LinkPackage PackageLine)
// {
//     while(PackageLine){
//         if(checkOvetTime(PackageLine))PackageLine->state[3]=1;
//         PackageLine=PackageLine->NextPackage;
//     }
// }

//初始化包裹链
LinkPackage InsertPackage() {
    LinkPackage tmp = NULL;
    LinkPackage FirstPackage = NULL;
    FILE* INSERT;
    INSERT = fopen("Package.txt", "rb");
    while (1) {
        LinkPackage New_Package = (LinkPackage)malloc(sizeof(struct Package));
        
        size_t readSize = fread(New_Package, sizeof(struct Package) - sizeof(struct Package*), 1, INSERT);
        if (readSize == 0) {
            free(New_Package);
            break;
        }
        New_Package->NextPackage = NULL;
        if (FirstPackage == NULL) {
            FirstPackage = New_Package;
        }
        else {
            tmp->NextPackage = New_Package;
        }
        tmp = New_Package;
    }
    fclose(INSERT);
    return FirstPackage;
}


struct tm* GetNowTime(){
	time_t now = time(NULL); 
    struct tm* Now=localtime(&now);
	return Now;
}

void Keep_In_PackageFile(LinkPackage PackageLine){
    FILE* KEEP;
    KEEP=fopen("Package.txt","wb");
    while(1){
        if(PackageLine==NULL)break;
        fwrite(PackageLine,sizeof(struct Package)-sizeof(LinkPackage),1,KEEP);
        PackageLine=PackageLine->NextPackage;
    }
    fclose(KEEP);
}
//LinkPackage Package_write(float WEIGHT, bool TRANSPORT, int VALUE, bool DISTANCE, bool INSURANCE, struct Package* FirstPackage);



#endif  