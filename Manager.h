
//解决vs2022不能使用scanf
#define _CRT_SECURE_NO_WARNINGS 1


//头文件包含
//#include <stdio.h>
//#include <string.h>
//#include <stdbool.h>
//#include <Windows.h>
//#include <stdlib.h>
//#include<time.h>
#include"User.h"

// 内部函数声明
void Manager_Operation(struct Package* head1, struct User* head2);
void Manager_Put_in(struct Package* head1, struct User* head2);
void Manager_Put_Out(struct Package* PackageLine, struct User* UserLine);
void Manager_Search(struct Package* head1, struct User* head2);
LinkPeople Manager_Delete_User(LinkPeople head2,LinkPackage* PackageLine);
struct User* Manager_Search_Name(struct User* head);
struct Package* Manager_Search_Number(struct Package* head);
LinkPackage delete_user_packages(LinkPackage* PackageLine,char* UserName);
void Manager_Search_ways(struct Package* head);
void Manager_Search_distance(struct Package* head);
//int Manager_log_in(struct Manager* head3);

// 定义 Manager 结构体,管理员
struct Manager {
    char Account[10];
    char Password[10];
    struct Manager* next;
};

typedef struct Manager  Manager;
typedef Manager* LinkManager;


LinkPackage CreateNewPackage(LinkPeople UserLine,LinkPackage PackageLine,char* packNum,char* puCode,char* owner,int* State){
	int i;
	time_t now = time(NULL); 
    struct tm* Now=localtime(&now);
    FILE* PackageFile=fopen("Package.txt","ab");
	LinkPackage tmp=PackageLine;
	LinkPackage NewPackage=(LinkPackage)malloc(sizeof(struct Package));
    SetPackageTime(NewPackage,1);
    //printf("%4d.%02d.%02d %02d:%02d:%02d  %d\n",NewPackage->IN_year,NewPackage->IN_month,NewPackage->IN_day,NewPackage->IN_hour,NewPackage->IN_minute,NewPackage->IN_second,NewPackage->IN_Yday);
	NewPackage->NextPackage=NULL;
	strcpy(NewPackage->PackNUM,packNum);
	strcpy(NewPackage->PUcode,puCode);
	strcpy(NewPackage->Owner,owner);
    NewPackage->state[1]=State[1];
    NewPackage->state[0]=State[0];
    NewPackage->state[2]=State[2];
    NewPackage->state[3]=0;
    NewPackage->value=-1;
	
	if(PackageLine==NULL){
		PackageLine=NewPackage;
	}
	else {
		while(tmp->NextPackage!=NULL)tmp=tmp->NextPackage;
		tmp->NextPackage=NewPackage;
	}
	fwrite(NewPackage,sizeof(struct Package)-sizeof(LinkPackage),1,PackageFile);
    fflush(PackageFile);
    fclose(PackageFile);

	FILE* Journal=fopen("ManagerJournal.txt","a");
    fprintf(Journal,"[ %4d.%02d.%02d %02d:%02d:%02d ] 新包裹入库-----",Now->tm_year+1900,Now->tm_mon+1,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
    fprintf(Journal,"包裹编号：%-6.4s 取件码：%-6.4s 取件人：%-6.4s \r\n",packNum,puCode,owner);
    fflush(Journal);
    fclose(Journal);
	return PackageLine;
}

LinkPackage CreateP_interface(LinkPeople UserLine,LinkPackage PackageLine){
	LinkPeople ReferingUser=UserLine;
    LinkPackage ReferingPackage=PackageLine;
	int state[3],i;
	char packNUM[5],PUcode[5],Owner[5];
	state[0]=1;
	state[2]=-1;
    printf("录入包裹：\n");
    //判断包裹好or损
	printf("包裹是否损坏：\n");
	printf("1.完好\n");
	printf("0.有损\n");
	printf("输入：");
    //getchar();
	scanf("%d",&state[1]);
    printf("\n");
	while(state[1]!=1&&state[1]!=0){
		printf("1.完好\n");
		printf("0.有损\n");
		printf("输入有误请重新输入：");
		scanf("%d",&state[1]);
		printf("\n");
	}

    getchar();
	printf("请输入包裹编号：");
	gets(packNUM);
    while(ReferingPackage){
        if(strlen(packNUM)!=4){
            printf("请输入四位数编号：");
            gets(packNUM);
            continue;
        }
        if(!strcmp(ReferingPackage->PackNUM,packNUM)){
            ReferingPackage=PackageLine;
            //存在大部分编号被占用的情况是否应设置一个推荐机制，方便编号的录入
            printf("该编号已存在请重新输入：");
            gets(packNUM);
        }
        ReferingPackage=ReferingPackage->NextPackage;
    }
	printf("\n");


    ReferingPackage=PackageLine;
	printf("请输入包裹取件码：");
	gets(PUcode);
    while(ReferingPackage){
        if(strlen(PUcode)!=4){
            printf("请输入四位数取件码：");
            gets(PUcode);
            continue;
        }
        if(!strcmp(ReferingPackage->PUcode,PUcode)){
            ReferingPackage=PackageLine;
            //同编号输入的问题
            printf("该取件码已存在请重新输入：");
            gets(PUcode);
        }
        ReferingPackage=ReferingPackage->NextPackage;
    }
	printf("\n");

    ReferingUser=UserLine;
	printf("请输入包裹主人：");
	gets(Owner);
    while(strcmp(ReferingUser->Name,Owner)){
        if(!ReferingUser){
            ReferingUser=UserLine;
            printf("该用户不存在，请重新输入：");
            scanf("%s",Owner);
        }
        ReferingUser=ReferingUser->NextPeople;
    }
    addUserMessage(ReferingUser, "您的快递已到站！请尽快取件！");
	printf("\n");
    
	PackageLine=CreateNewPackage(UserLine,PackageLine,packNUM,PUcode,Owner,state);
    return PackageLine;
}
//[ %4d.%02d.%02d %02d:%02d:%02d ] ;Now->tm_year+1900,Now->tm_mon,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec
// 管理员操作函数
void Manager_Operation(struct Package* head1, struct User* head2) {
    int flag;
    do {
        struct tm* Now=GetNowTime();
        FILE* log_file = fopen("Manager.txt", "a");
        if (log_file == NULL) {
            perror("无法打开日志文件");
            return;
        }

        printf("\n管理员您好！请选择你要做的操作：\n");
        printf("1--入库快递\n");
        printf("2--出库快递\n");
        printf("3--查询快递\n");
        printf("4--注销用户\n");
        printf("5--查看库存\n");  // 新增选项
        printf("6--退出程序\n");
        printf("请输入选择：");
        if (scanf("%d", &flag) != 1||flag>5||flag<1) {
            
            printf("输入无效，请重新输入！\n");
            // 清空输入缓冲区
            while (getchar() != '\n');
            fclose(log_file);
            continue;
        }
       system("cls");
        switch (flag) {
        case 1:
            Manager_Put_in(head1, head2);fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员进行快递入库操作\n",Now->tm_year+1900,Now->tm_mon+1,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
            break;
        case 2:
            Manager_Put_Out(head1, head2);fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员进行快递出库操作\n",Now->tm_year+1900,Now->tm_mon+1,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
            break;
        case 3:
            Manager_Search(head1, head2);fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员进行快递查询操作\n",Now->tm_year+1900,Now->tm_mon+1,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
            break;
        case 4:
            head2=Manager_Delete_User(head2,&head1);fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员进行注销账户操作\n",Now->tm_year+1900,Now->tm_mon+1,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
            break;
        case 5:
            Manager_View_Inventory(head1);
            if (log_file) {
                fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员查看库存\n",
                        Now->tm_year+1900, Now->tm_mon+1, Now->tm_mday,
                        Now->tm_hour, Now->tm_min, Now->tm_sec);
            }
            break;
        case 6:
            printf("退出程序\n");
            break;
            
        }

       
        fclose(log_file);
    } while (flag != 6);//do while循环实现反复选择
    return;
}

/*查找的函数*/
//查找的主函数
void Manager_Search(struct Package* head1, struct User* head2) {
    while (1) {
       
        printf("请选择查询快递的方式：\n");
        Sleep(1);
        printf("1--按用户姓名查找\n");
        printf("2--按快递单号查找\n");
        printf("3--按快递运输方式查找\n");
        printf("4--按运输距离查找\n");
        printf("5--退出程序\n");
        printf("请输入："); 
        int i;
        
        if (scanf("%d", &i) != 1) {
            
            printf("输入无效，请输入数字！\n");
            // 清空输入缓冲区
            while (getchar() != '\n');
            continue;//重新开始新的循环，让客户有机会重新输入正确的选择
        }
        getchar();
        if (i == 1) {  // 按用户姓名查找
            struct User* flag1 = Manager_Search_Name(head2);//调用查找姓名函数查找，返回找到的用户
            if (flag1 == NULL) {
                
                printf("未查询到该用户！请重试！\n");
            }
            else {
                struct Package* temp = flag1->PackageLine;
                int cnt = 0;
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
                printf("以上为该用户的所有快递\n");
            }
        }  // 结束
        else if (i == 2) {  // 按快递单号查找
            struct Package* temp = Manager_Search_Number(head1);//调用单号查找函数查找
            if (temp != NULL) {
                printf("找到包裹：\n");
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
            else {
                
                printf("未找到该快递单号对应的包裹！\n");
            }
        }
        else if (i == 3) {
            Manager_Search_ways(head1);
        }
        else if (i == 4) {
            Manager_Search_distance(head1);
        }
        else if (i == 5) {
            printf("欢迎再次使用！\n");
            break;
        }
        else {  // 输入失败
            
            printf("非法输入！请重试！\n");
        }

        printf("输入1继续，或按任意键退出快递查询：");
        char ch;
        getchar();
        gets(&ch);
        if(ch=='1')continue;
        else break;
    }
}


//姓名查找
struct User* Manager_Search_Name(struct User* head) {
    char Username[5];
    printf("请输入用户名称："); 
    fgets(Username, sizeof(Username), stdin);
    Username[strcspn(Username, "\n")] = 0;

    struct User* tmp = head;  // 创建一个新节点用于遍历链表
    while (tmp != NULL) {
        if (strcmp(Username, tmp->Name) == 0) {
            return tmp;
        }
        tmp = tmp->NextPeople;
    }
    return NULL;
}  // 结束
//单号查找
struct Package* Manager_Search_Number(struct Package* head) {
    char Number[5];
    printf("请输入快递编号："); 
    fgets(Number, sizeof(Number), stdin);
    Number[strcspn(Number, "\n")] = 0;
    struct Package* p = head;
    while (p != NULL) {
        if (strcmp(Number, p->PackNUM) == 0) {
            return p;
        }
        p = p->NextPackage;
    }
    return NULL;
}
//运输方式查找
void Manager_Search_ways(struct Package* head) {
    printf("请输入您想查找的运输方式：\n");
    printf("0--特快\n");
    printf("1--标快\n");
    printf("输入2退出查找\n");

    int n;
    if (scanf("%d", &n) != 1) {
        printf("输入无效，请输入数字！\n");
        // 清空输入缓冲区
        while (getchar() != '\n');
        return;
    }

    struct Package* p = head;
    while (p != NULL) {
        if (n == 0 && p->transport == true) {
            
            printf("包裹信息：编号 %s, 取件码 %s, 重量 %.2f, 价格 %d, 运输方式 %s, 距离 %s, 保险 %s, 物主 %s, 存取状况 %d\n",
                p->PackNUM, p->PUcode, p->weight, p->value,
                "快速送",
                p->distance ? "省内" : "省外",
                p->insurance ? "有" : "无",
                p->Owner, p->state[0]);
        }
        else if (n == 1 && p->transport == false) {
            
            printf("包裹信息：编号 %s, 取件码 %s, 重量 %.2f, 价格 %d, 运输方式 %s, 距离 %s, 保险 %s, 物主 %s, 存取状况 %d\n",
                p->PackNUM, p->PUcode, p->weight, p->value,
                "普通送",
                p->distance ? "省内" : "省外",
                p->insurance ? "有" : "无",
                p->Owner, p->state[0]);
        }
        p = p->NextPackage;
    }

    if (n == 2) {
        return;
    }
    else {
       
        printf("错误输入！请重试！\n");
    }
}  // 结束


//运输距离查找
void Manager_Search_distance(struct Package* head) {
    
    printf("请输入您想查找的运输距离：\n");
    printf("0--省内\n");
    printf("1--省外\n");
    printf("输入2退出查找\n");
    int n;
    if (scanf("%d", &n) != 1) {
        
        printf("输入无效，请输入数字！\n");
        // 清空输入缓冲区
        while (getchar() != '\n');
        return;
    }

    struct Package* p = head;
    while (p != NULL) {
        if (n == 0 && p->distance == true) {
            printf("包裹信息：编号 %s, 取件码 %s, 重量 %.2f, 价格 %d, 运输方式 %s, 距离 %s, 保险 %s, 物主 %s, 存取状况 %d\n",
                p->PackNUM, p->PUcode, p->weight, p->value,
                p->transport ? "快速送" : "普通送",
                "省内",
                p->insurance ? "有" : "无",
                p->Owner, p->state[0]);
        }
        else if (n == 1 && p->distance == false) {
            printf("包裹信息：编号 %s, 取件码 %s, 重量 %.2f, 价格 %d, 运输方式 %s, 距离 %s, 保险 %s, 物主 %s, 存取状况 %d\n",
                p->PackNUM, p->PUcode, p->weight, p->value,
                p->transport ? "快速送" : "普通送",
                "省外",
                p->insurance ? "有" : "无",
                p->Owner, p->state[0]);
        }
        p = p->NextPackage;
    }

    if (n == 2) {
        return;
    }
    else {
        
        printf("错误输入！请重试！\n");
    }
}


/*入库快递函数*/
void Manager_Put_in(struct Package* head1, struct User* head2) {
    struct Package* bag = head1; // 包裹链
    struct User* people = head2; // 用户链
    char r[] = "您的快递已到站！请尽快取件！";

    // 打开日志文件，以追加模式写入
    while (1) {
        head1=CreateP_interface(head2,head1);
       
        printf("是否退出入库包裹程序:\n");
        printf("1--继续存入\n");
        printf("或按任意键返回上个界面\n");
        char ch;
        gets(&ch);
        if(ch=='1')continue;
        else break;
        while (getchar() != '\n');
    }
    return;
}

// 清空输入缓冲区
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 安全获取字符串输入
int getSafeStringInput(char *buffer, int bufferSize) {
    if (fgets(buffer, bufferSize, stdin) == NULL) {
        return 0; // 输入失败
    }
    
    // 去除换行符
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    } else {
        // 输入过长，清空缓冲区
        clearInputBuffer();
        return 0;
    }
    
    return 1;
}

// 安全获取数字输入
int getSafeIntInput(int *value) {
    char buffer[20];
    if (!getSafeStringInput(buffer, sizeof(buffer))) {
        return 0;
    }
    
    // 验证是否为纯数字
    for (char *p = buffer; *p != '\0'; p++) {
        if (!isdigit(*p)) {
            return 0;
        }
    }
    
    return sscanf(buffer, "%d", value) == 1;
}


/*出库快递函数*/
void Manager_Put_Out(struct Package* PackageLine, struct User* UserLine) {
    struct tm* Now = GetNowTime();
    FILE* ManagerJournal = fopen("ManagerJournal.txt", "a");
    if (ManagerJournal == NULL) {
        printf("无法打开管理员日志文件！\n");
        return;
    }

    char packNum[5], ownerName[5];
    
    // 输入包裹编号
    printf("请输入要取出的包裹编号：");
    while (1) {
        if (!getSafeStringInput(packNum, sizeof(packNum))) {
            printf("输入无效，请重新输入包裹编号：");
            continue;
        }
        break;
    }

    // 查找包裹
    LinkPackage targetPkg = PackageLine;
    while (targetPkg != NULL && strcmp(targetPkg->PackNUM, packNum) != 0) {
        targetPkg = targetPkg->NextPackage;
    }

    if (targetPkg == NULL) {
        printf("未找到该包裹！\n");
        fprintf(ManagerJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员取件：包裹%s不存在！\n",
                Now->tm_year+1900, Now->tm_mon+1, Now->tm_mday,
                Now->tm_hour, Now->tm_min, Now->tm_sec, packNum);
        fclose(ManagerJournal);
        return;
    }

    // 检查包裹状态
    if (targetPkg->state[0] == 0) {
        printf("该包裹已经出库，不能重复取件！\n");
        fprintf(ManagerJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员尝试取件：包裹%s已出库！\n",
                Now->tm_year+1900, Now->tm_mon+1, Now->tm_mday,
                Now->tm_hour, Now->tm_min, Now->tm_sec, packNum);
        fclose(ManagerJournal);
        return;
    }

    // 查找包裹所有者
    LinkPeople owner = UserLine;
    while (owner != NULL && strcmp(owner->Name, targetPkg->Owner) != 0) {
        owner = owner->NextPeople;
    }

    if (owner != NULL) {
        // 更新用户包裹链中的状态
        LinkPackage userPkg = owner->PackageLine;
        while (userPkg != NULL && strcmp(userPkg->PackNUM, packNum) != 0) {
            userPkg = userPkg->NextPackage;
        }
        if (userPkg != NULL && userPkg->state[0] == 1) {
            userPkg->state[0] = 0;
        }
    }

    // 更新主包裹链状态
    targetPkg->state[0] = 0;
    SetPackageTime(targetPkg, 0); // 设置出库时间

    // 发送消息给用户
    if (owner != NULL) {
        addUserMessage(owner, "您的快递已出库！请确认是否为本人取走！");
    }

    printf("包裹 %s 已成功取出！\n", packNum);
    fprintf(ManagerJournal, "[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员取件：包裹%s（所有者：%s）已出库！\n",
            Now->tm_year+1900, Now->tm_mon+1, Now->tm_mday,
            Now->tm_hour, Now->tm_min, Now->tm_sec, packNum, targetPkg->Owner);
    fclose(ManagerJournal);
}


/*删除用户函数*/

LinkPeople Manager_Delete_User(LinkPeople head2, LinkPackage* PackageLine) {
    FILE* log_file = fopen("ManagerJournal.txt", "a");
    getchar();
    printf("请输入要删除的用户账号：");
    char input_ac[10];
    gets(input_ac);
    printf("请输入要删除的用户密码：");
    char input_pw[10];
    gets(input_pw);
    
    struct User* current = head2;
    struct User* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->ACnumber, input_ac) == 0) {
            printf("找到该账号！");
            if(strcmp(current->Password, input_pw) == 0) {
                // 删除用户的所有包裹
                *PackageLine=delete_user_packages(PackageLine, current->Name);

                if (prev == NULL) {
                    // 如果要删除的是头节点
                    head2 = current->NextPeople;
                } else {
                    // 连接前后节点
                    prev->NextPeople = current->NextPeople;
                }

                // 释放当前用户节点的内存
                LinkPeople toDelete = current;
                current = current->NextPeople;
                free(toDelete);
                
                // 更新用户文件
                FILE* file = fopen("User.tmp", "wb");
                FILE* REMOVE = fopen("User.txt", "rb");
                if (!file) {
                    perror("无法打开用户文件");
                    return head2;
                }

                LinkPeople tmp = head2;
                while (tmp != NULL) {
                    fwrite(tmp, sizeof(struct User) - sizeof(struct User*)-sizeof(LinkPackage), 1, file);
                    tmp = tmp->NextPeople;
                }
                fflush(REMOVE);
                fclose(REMOVE);
                fclose(file);
                remove("User.txt");
                rename("User.tmp", "User.txt");

                fprintf(log_file, "用户删除成功！账号: %s 密码：%s\n", input_ac, input_pw);
                printf("用户删除成功！\n");
                fclose(log_file);
                return head2;
            } else {
                printf("密码输入错误！请按任意键重试！\n");
                printf("退出程序请输入0！");
                char a;
                scanf("%c", &a);
                if(a == '0') {
                    return head2;
                }
            }
        }
        prev = current;
        current = current->NextPeople;
    }
    fprintf(log_file, "未找到匹配的用户账号和密码，删除失败！\n");
    printf("未找到匹配的用户账号和密码，删除失败！\n");
    fclose(log_file);
    return head2;
}

// 删除用户的所有包裹并从链表中移除
LinkPackage delete_user_packages(LinkPackage* PackageLine, char* UserName) {
    LinkPackage prev = NULL;
    LinkPackage current = PackageLine;
    
    // 创建临时文件用于保存剩余的包裹
    FILE* tmp_file = fopen("Package.tmp", "wb");
    if (!tmp_file) {
        perror("无法创建临时包裹文件");
        return PackageLine;
    }

    LinkPackage newHead = NULL;
    LinkPackage last = NULL;
    
    while (current != NULL) {
        if (strcmp(current->Owner, UserName)){
            // 不是该用户的包裹，保留
            if (newHead == NULL) {
                newHead = current;
                last = current;
            } else {
                last->NextPackage = current;
                last = current;
            }
            // 写入临时文件
            fwrite(current, sizeof(struct Package) - sizeof(LinkPackage), 1, tmp_file);
        } else {
            // 是该用户的包裹，需要删除
            LinkPackage toDelete = current;
            current = current->NextPackage;
            free(toDelete);
            continue;
        }
        current = current->NextPackage;
    }
    
    if (last != NULL) {
        last->NextPackage = NULL;
    }
    
    fclose(tmp_file);
    
    // 更新主包裹文件
    remove("Package.txt");
    rename("Package.tmp", "Package.txt");
    
    return newHead;
}


/*管理员登录函数*/
int Manager_log_in(struct Manager* head3) {
    int Remainingtime = 5;
    printf("管理员您好！请登录：\n");
    while (1) {
        if (Remainingtime == 0) {
            printf("剩余次数已用完！即将退出程序！");
            break;
            printf("1");
        }
        Sleep(1); // 在这里停留一秒之后再进行下一步
        printf("账号：\n");
        char LeaderAccount[10]; // 账号
        scanf("%s", LeaderAccount); // 临时建立一个名字用以输入管理员的账号，用于对比是否为本人
        Manager* find; // 建立一个指针寻找
        find = head3; // 令find为表头指针
        char passwordtmp[10]; // 密码
        printf("密码：\n");
        scanf("%s", passwordtmp);
        while (find != NULL) {
            if (strcmp(find->Account, LeaderAccount) == 0 && strcmp(find->Password, passwordtmp) == 0) {
                
                printf("登陆成功！请进行下一步操作！\n");
                Sleep(1);
                return 0;
            }
            find = find->next;
        }
        if (find == NULL) {
            printf("账号或密码输入错误！\n");
            --Remainingtime;
        }
        printf("你还有%d次机会尝试！", Remainingtime);
    }
    return -1;
}

/*
    存-1表示无该状态
    state[0]存取状况1--库中，0--出库；
    state[1]存放包裹状况（是否损坏或丢失）1-完好 0-损坏 ；
    state[2]运输状态，1-运达，0-途中（用户互送）；
    state[3]是否超时，1-滞留超时，0-未超时
    */

void checkOvetTime(LinkPackage ReferingPackage,LinkPeople UserLine){
	time_t now = time(NULL); 
    struct tm* Now = localtime(&now);
    int currentYear = Now->tm_year + 1900;
    int currentYday = Now->tm_yday;

    while(ReferingPackage != NULL) {
        // 只检查在库中且未超时的包裹
        if(ReferingPackage->state[0] == 1 && ReferingPackage->state[3] == 0) {
            // 查找包裹所有者
            LinkPeople owner = UserLine;
            while(owner != NULL && strcmp(owner->Name, ReferingPackage->Owner) != 0) {
                owner = owner->NextPeople;
            }
            
            if(owner == NULL) {
                ReferingPackage = ReferingPackage->NextPackage;
                continue;
            }

            // 计算在库天数
            int daysInStorage = 0;
            
            if(ReferingPackage->IN_year == currentYear) {
                // 同一年
                daysInStorage = currentYday - ReferingPackage->IN_Yday;
            } else {
                // 跨年计算
                daysInStorage = AllYearDay(ReferingPackage->IN_year) - ReferingPackage->IN_Yday + currentYday;
            }

            // 检查是否超时（超过3天）
            if(daysInStorage > 3) {
                ReferingPackage->state[3] = 1; // 标记为超时
                addUserMessage(owner, "您的包裹滞留超时，已被送回仓库！");
                
                // 更新出库时间（模拟送回仓库）
                SetPackageTime(ReferingPackage, 0);
            } 
            // 检查是否即将超时（2-3天）
            else if(daysInStorage >= 2) {
                addUserMessage(owner, "您的包裹即将滞留超时，请尽快取回！");
            }
        }
        ReferingPackage = ReferingPackage->NextPackage;
    }
}

void checkDelivery(LinkPackage ReferingPackage,LinkPeople UserLine){
    time_t now = time(NULL); 
    struct tm* Now = localtime(&now);
    int currentYear = Now->tm_year + 1900;
    int currentYday = Now->tm_yday;

    while(ReferingPackage != NULL) {
        // 只检查运输中(state[2]==0)且未送达的包裹
        if(ReferingPackage->state[2] == 0 && ReferingPackage->state[0] == -1) {
            // 查找包裹所有者
            LinkPeople owner = UserLine;
            while(owner != NULL && strcmp(owner->Name, ReferingPackage->Owner) != 0) {
                owner = owner->NextPeople;
            }
            
            if(owner == NULL) {
                ReferingPackage = ReferingPackage->NextPackage;
                continue;
            }

            // 计算运输天数
            int daysInTransit = 0;
            
            if(ReferingPackage->IN_year == currentYear) {
                // 同一年
                daysInTransit = currentYday - ReferingPackage->IN_Yday;
            } else {
                // 跨年计算
                daysInTransit = AllYearDay(ReferingPackage->IN_year) - ReferingPackage->IN_Yday + currentYday;
            }

            // 检查是否已送达（超过3天）
            if(daysInTransit > 3) {
                ReferingPackage->state[2] = 1; // 标记为已送达
                
                if(ReferingPackage->IsUser) {
                    // 给用户发送的包裹
                    addUserMessage(owner, "您的包裹已送达目的地！");
                    
                    // 更新为在库状态
                    ReferingPackage->state[0] = 1;
                    SetPackageTime(ReferingPackage, 1); // 重置入库时间
                } else {
                    // 给非用户发送的包裹
                    ReferingPackage->state[0] = 0; // 直接标记为已出库
                    SetPackageTime(ReferingPackage, 0); // 设置出库时间
                }
            }
        }
        ReferingPackage = ReferingPackage->NextPackage;
    }
}


// 查看库存函数
void Manager_View_Inventory(LinkPackage PackageLine){
    if (PackageLine == NULL) {
        printf("当前库存为空！\n");
        return;
    }

    struct tm* Now = GetNowTime();
    FILE* log_file = fopen("ManagerJournal.txt", "a");
    if (log_file == NULL) {
        printf("无法打开日志文件！\n");
    } else {
        fprintf(log_file, "[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员查看库存\n",
                Now->tm_year+1900, Now->tm_mon+1, Now->tm_mday,
                Now->tm_hour, Now->tm_min, Now->tm_sec);
        fclose(log_file);
    }

    printf("\n========== 当前库存包裹列表 ==========\n");
    printf("总数\t\t包裹编号\t取件码\t\t物主\t\t重量(kg)\t入库时间\t状态\n");

    int count = 0;
    LinkPackage current = PackageLine;
    while (current != NULL) {
        // 只显示在库中的包裹(state[0]==1且未超时state[3]==0)
        if (current->state[0] == 1 && current->state[3] == 0) {
            count++;
            
            // 格式化入库时间
            char inTime[20];
            snprintf(inTime, sizeof(inTime), "%02d-%02d %02d:%02d",
                    current->IN_month, current->IN_day,
                    current->IN_hour, current->IN_minute);
            
            // 格式化状态
            char status[20];
            if (current->state[1] == 0) {
                strcpy(status, "有损坏");
            } else {
                strcpy(status, "完好");
            }

            printf("%-4d\t\t%-4s\t\t%-4s\t\t%-4s\t\t%-4.2f\t\t%-4s\t%-4s\n",
                   count, current->PackNUM, current->PUcode,
                   current->Owner, current->weight, inTime, status);
        }
        current = current->NextPackage;
        if (count > 0) {
            getchar();
            printf("\n输入包裹编号查看详情，或输入0返回: ");
            char input[10];
            gets(input);
            
            if (input != "0") {
                LinkPackage pkg = PackageLine;
                while (pkg != NULL) {
                    if (strcmp(pkg->PackNUM, input) == 0) {
                        DisplayPackageDetails(pkg);
                        break;
                    }
                    pkg = pkg->NextPackage;
                }
                
                if (pkg == NULL) {
                    printf("未找到该包裹！\n");
                }
            }
            else break;
        }
    }

    if (count == 0) {
        printf("\n当前没有在库包裹！\n");
    } else {
        printf("\n共计 %d 件在库包裹\n", count);
    }
    
    printf("\n按任意键返回...");
    getchar();
}
