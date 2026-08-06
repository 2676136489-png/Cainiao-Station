#include <stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include <ctype.h>

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

// 判断是否为闰年
bool is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 计算一年中的总天数
int AllYearDay(int year) {
    return is_leap_year(year) ? 366 : 365;
}

// 设置包裹时间
void SetPackageTime(struct Package* pkg, int state) {
    time_t now = time(NULL);
    struct tm* tm_now = localtime(&now);
    
    if(state) { // 入库/运输开始时间
        pkg->IN_second = tm_now->tm_sec;
        pkg->IN_minute = tm_now->tm_min;
        pkg->IN_hour = tm_now->tm_hour;
        pkg->IN_day = tm_now->tm_mday;
        pkg->IN_month = tm_now->tm_mon + 1;
        pkg->IN_year = tm_now->tm_year + 1900;
        pkg->IN_Yday = tm_now->tm_yday;
    } else { // 出库时间
        pkg->OUT_second = tm_now->tm_sec;
        pkg->OUT_minute = tm_now->tm_min;
        pkg->OUT_hour = tm_now->tm_hour;
        pkg->OUT_day = tm_now->tm_mday;
        pkg->OUT_month = tm_now->tm_mon + 1;
        pkg->OUT_year = tm_now->tm_year + 1900;
        pkg->OUT_Yday = tm_now->tm_yday;
    }
}

void DisplayPackageDetails(LinkPackage pkg) {
    if (pkg == NULL) return;

    printf("\n========== 包裹详细信息 ==========\n");
    printf("包裹编号: %s\n", pkg->PackNUM);
    printf("取件码: %s\n", pkg->PUcode);
    printf("物主: %s\n", pkg->Owner);
    printf("收件人: %s\n", pkg->Receiver);
    printf("重量: %.2f kg\n", pkg->weight);
    if(pkg->value!=-1)printf("价值: %d 元\n", pkg->value);
    else printf("价值: 不适用\n");
    printf("运输方式: %s\n", pkg->transport ? "加急" : "标准");
    printf("距离: %s\n", pkg->distance ? "省内" : "省外");
    printf("保险: %s\n", pkg->insurance ? "已购买" : "未购买");
    
    // 格式化时间
    char inTime[20], outTime[20];
    snprintf(inTime, sizeof(inTime), "%04d-%02d-%02d %02d:%02d",
            pkg->IN_year, pkg->IN_month, pkg->IN_day,
            pkg->IN_hour, pkg->IN_minute);
    
    if (pkg->state[0] == 0) {
        snprintf(outTime, sizeof(outTime), "%04d-%02d-%02d %02d:%02d",
                pkg->OUT_year, pkg->OUT_month, pkg->OUT_day,
                pkg->OUT_hour, pkg->OUT_minute);
    } else {
        strcpy(outTime, "未出库");
    }
    
    printf("入库时间: %s\n", inTime);
    printf("出库时间: %s\n", outTime);
    
    // 状态信息
    printf("\n当前状态:\n");
    printf("库存状态: %s\n", pkg->state[0] == 1 ? "在库" : "已出库");
    printf("包裹状况: %s\n", pkg->state[1] == 1 ? "完好" : "损坏");
    printf("运输状态: %s\n", 
           pkg->state[2] == -1 ? "不适用" : 
           (pkg->state[2] == 0 ? "运输中" : "已送达"));
    printf("超时状态: %s\n", pkg->state[3] == 1 ? "已超时" : "正常");
    
    printf("\n按任意键返回...");
    getchar();
}

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