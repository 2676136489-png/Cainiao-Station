
//解决vs2022不能使用scanf
#define _CRT_SECURE_NO_WARNINGS 1


//头文件包含
#include <stdio.h>
//#include <string.h>
//#include <stdbool.h>
//#include <Windows.h>
#include <stdlib.h>
//#include<time.h>
#include"User.h"

// 内部函数声明
void Manager_Operation(struct Package* head1, struct User* head2);
void Manager_Put_in(struct Package* head1, struct User* head2);
void Manager_Put_Out(struct Package* head1, struct User* head2);
void Manager_Search(struct Package* head1, struct User* head2);
void Manager_Delete_User(struct User** head2);
int Manager_log_in(struct Manager* head3);
struct User* Manager_Search_Name(struct User* head);
struct Package* Manager_Search_Number(struct Package* head);
void free_user_packages(struct Package* package_head);
void Manager_Search_ways(struct Package* head);
void Manager_Search_distance(struct Package* head);
int Manager_log_in(struct Manager* head3);

// 定义 Manager 结构体,管理员
struct Manager {
    char Account[10];
    char Password[10];
    struct Manager* next;
};

typedef struct Manager  Manager;
typedef Manager* LinkManager;

// for(i=0;i<3;i++){
//     switch(State[i]){
//         case 1:NewPackage->state[i]=State[i];break;
//         case 0:NewPackage->state[i]=State[i];break;
//         case -1:NewPackage->state[i]=-1;
//     }
//     if(i==0&&State[i])SetPackageTime(NewPackage,State[i]);
// }

LinkPackage CreateNewPackage(LinkPeople UserLine,LinkPackage PackageLine,char* packNum,char* puCode,char* owner,int* State){
	int i;
	time_t now = time(NULL); 
    struct tm* Now=localtime(&now);
    FILE* PackageFile=fopen("Package.txt","ab");
	LinkPackage tmp=PackageLine;
	LinkPackage NewPackage=(LinkPackage)malloc(sizeof(struct Package));
	NewPackage->NextPackage=NULL;
	strcpy(NewPackage->PackNUM,packNum);
	strcpy(NewPackage->PUcode,puCode);
	strcpy(NewPackage->Owner,owner);
    NewPackage->state[1]=State[1];
    NewPackage->state[0]=State[0];
    NewPackage->state[2]=State[2];
    NewPackage->state[3]=0;
	
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
    fprintf(Journal,"[ %4d.%02d.%02d %02d:%02d:%02d ] 新包裹入库-----",Now->tm_year+1900,Now->tm_mon,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
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
    char r[] = "您的快递已到站！请尽快取件！";
    strcpy(ReferingUser->message[(++ReferingUser->l)%30], r);
    ReferingUser->NewMessage=1;
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

        printf("管理员您好！请选择你要做的操作：\n");
        
        printf("1--入库快递\n");
        printf("2--出库快递\n");
        printf("3--查询快递\n");
        printf("4--注销用户\n");
        printf("5--退出程序\n");
        printf("请输入：");
        if (scanf("%d", &flag) != 1) {
            
            printf("输入无效，请输入数字！\n");
            // 清空输入缓冲区
            while (getchar() != '\n');
            fclose(log_file);
            continue;
        }
       
        switch (flag) {
        case 1:
            Manager_Put_in(head1, head2);fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员进行快递入库操作\n",Now->tm_year+1900,Now->tm_mon,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
            break;
        case 2:
            Manager_Put_Out(head1, head2);fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员进行快递出库操作\n",Now->tm_year+1900,Now->tm_mon,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
            break;
        case 3:
            Manager_Search(head1, head2);fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员进行快递查询操作\n",Now->tm_year+1900,Now->tm_mon,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
            break;
        case 4:
            Manager_Delete_User(&head2);fprintf(log_file,"[ %4d.%02d.%02d %02d:%02d:%02d ] 管理员进行注销账户操作\n",Now->tm_year+1900,Now->tm_mon,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec);
            break;
        case 5:
            
            printf("退出程序\n");
            break;
            
        }

       
        fclose(log_file);
    } while (flag != 5);//do while循环实现反复选择
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


/*出库快递函数*/
void Manager_Put_Out(struct Package* head1, struct User* head2) {
    struct Package* bag = head1; // 包裹链
    struct User* people = head2; // 用户链
    char r[] = "您的快递已出库！请确认是否为本人取走！";

    // 打开日志文件，以追加模式写入
    FILE* log_file = fopen("Manager.txt", "a");

    while (1) {
        
        fprintf(log_file, "请输入取件人的账号：\n");
        printf("请输入取件人的账号：\n");
        char Ac[4];
        fgets(Ac, sizeof(Ac), stdin);
        // 去除 fgets 可能读取的换行符
        Ac[strcspn(Ac, "\n")] = 0;
        fprintf(log_file, "用户输入的取件人账号: %s\n", Ac);

        // 查找取件人
        struct User* found_user = NULL;
        struct User* current_user = head2;
        while (current_user != NULL) {
            if (strcmp(Ac, current_user->ACnumber) == 0) {
                found_user = current_user;
                break;
            }
            current_user = current_user->NextPeople;
        }

        if (found_user == NULL) {
            
            fprintf(log_file, "未找到该取件人，请重新输入！\n");
            printf("未找到该取件人，请重新输入！\n");
            continue;
        }

        fprintf(log_file, "请输入待取出快递的编号：\n");
        printf("请输入待取出快递的编号：\n");
        char num[5];
        fgets(num, sizeof(num), stdin);
        // 去除 fgets 可能读取的换行符
        num[strcspn(num, "\n")] = 0;
        fprintf(log_file, "用户输入的快递编号: %s\n", num);

        // 查找快递包裹
        struct Package* found_package = NULL;
        struct Package* current_package = head1;
        while (current_package != NULL) {
            if (strcmp(num, current_package->PackNUM) == 0) {
                found_package = current_package;
                break;
            }
            current_package = current_package->NextPackage;
        }

        if (found_package == NULL) {
            
            fprintf(log_file, "未找到该快递编号，请重新输入！\n");
            printf("未找到该快递编号，请重新输入！\n");
            continue;
        }

        if (found_package->state[0] == 0) {
           
            fprintf(log_file, "该快递未入库，不能出库！\n");
            printf("该快递未入库，不能出库！\n");
            continue;
        }

        if (strcmp(found_package->Owner, found_user->Name) != 0) {
           
           
            fprintf(log_file, "该快递不属于此用户，不能出库！\n");
            printf("该快递不属于此用户，不能出库！\n");
            continue;
        }

        // 标记快递为已出库
        found_package->state[0] = 0;
        // 给用户发送消息
        strcpy(found_user->message[++(found_user->l)], r);

      
        fprintf(log_file, "快递出库成功！取件人: %s, 快递编号: %s\n", found_user->Name, found_package->PackNUM);
        printf("快递出库成功！\n");

        
        fprintf(log_file, "是否退出出库包裹程序:\n1--继续取出\n2--退出程序\n");
        printf("是否退出出库包裹程序:\n");
        printf("1--继续取出\n");
        printf("2--退出程序\n");
        int i;
        // 清空输入缓冲区
        while (getchar() != '\n');
        if (scanf("%d", &i) != 1) {
           
            fprintf(log_file, "输入无效，请输入数字！\n");
            printf("输入无效，请输入数字！\n");
            // 清空输入缓冲区
            while (getchar() != '\n');
            continue;
        }
        fprintf(log_file, "用户输入的选择: %d\n", i);
        if (i == 2) {
            break;
        }
        // 清空输入缓冲区
        while (getchar() != '\n');
    }
    // 关闭日志文件
    fclose(log_file);
    return;
}


/*删除用户函数*/
void Manager_Delete_User(struct User** head2) {
    FILE* log_file = fopen("", "a");

    printf("请输入要删除的用户账号：\n");
    char input_ac[5];
    getchar();
    fgets(input_ac, sizeof(input_ac), stdin);
    getchar();
    input_ac[strcspn(input_ac, "\n")] = 0;
    
    printf("请输入要删除的用户密码：\n");
    getchar();
    char input_pw[5];
    fgets(input_pw, sizeof(input_pw), stdin);
    input_pw[strcspn(input_pw, "\n")] = 0;
    fprintf(log_file, "用户输入的密码: %s\n", input_pw);

    struct User* current = *head2;
    struct User* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->ACnumber, input_ac) == 0 && strcmp(current->Password, input_pw) == 0) {
            // 释放用户名下包裹链表的内存
            free_user_packages(current->PackageLine);

            if (prev == NULL) {
                // 如果要删除的是头节点
                *head2 = current->NextPeople;
            }
            else {
                // 连接前后节点
                prev->NextPeople = current->NextPeople;
            }

            // 释放当前用户节点的内存
            free(current);

            fprintf(log_file, "用户删除成功！账号: %s 密码：%s\n", input_ac,input_pw);
            printf("用户删除成功！\n");
            fclose(log_file);
            return;
        }
        prev = current;
        current = current->NextPeople;
    }
    fprintf(log_file, "未找到匹配的用户账号和密码，删除失败！\n");
    printf("未找到匹配的用户账号和密码，删除失败！\n");
    fclose(log_file);
}


// 释放用户名下包裹链表的内存
void free_user_packages(struct Package* package_head) {
    struct Package* current = package_head;
    struct Package* next;
    while (current != NULL) {
        next = current->NextPackage;
        free(current);
        current = next;
    }
}


/*管理员登录函数*/
int Manager_log_in(struct Manager* head3) {
    int Remainingtime = 5;
    while (1) {
        printf("管理员您好！请登录：\n");
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
            Remainingtime--;
        }
        printf("你还有%d次机会尝试！", Remainingtime);
        if (Remainingtime == 0) {
            printf("剩余次数已用完！即将退出程序！");
            return -1;
        }
        
    }
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
	struct tm* Now=localtime(&now);
    while(ReferingPackage){
        if(ReferingPackage->state[0]==1){
            while(strcmp(ReferingPackage->Owner,UserLine->Name)){
                UserLine=UserLine->NextPeople;
            }
            if(ReferingPackage->state[0]==1){
                if(ReferingPackage->IN_year==Now->tm_year){
                    if((Now->tm_yday-ReferingPackage->IN_Yday)>3){
                        ReferingPackage->state[3]=1;
                        strcpy(UserLine->message[(++UserLine->l)%30],"您的包裹滞留超时");
                        UserLine->NewMessage=1;
                    }
                    else ReferingPackage->state[3]=0;
        
                    if((Now->tm_yday-ReferingPackage->IN_Yday)>2){
                        strcpy(UserLine->message[(++UserLine->l)%30],"您的包裹即将滞留超时，请尽快取回");
                        UserLine->NewMessage=1;
                    }
                }
                else {
                    if((AllYearDay(ReferingPackage->IN_year)-ReferingPackage->IN_Yday+Now->tm_yday)>3){
                        ReferingPackage->state[3]=1;
                        strcpy(UserLine->message[(++UserLine->l)%30],"您的包裹滞留超时");
                        UserLine->NewMessage=1;
                    }
                    else ReferingPackage->state[3]=0;
        
                    if((AllYearDay(ReferingPackage->IN_year)-ReferingPackage->IN_Yday+Now->tm_yday)>2){
                        strcpy(UserLine->message[(++UserLine->l)%30],"您的包裹即将滞留超时，请尽快取回");
                        UserLine->NewMessage=1;
                    }
                }
            }
        }
        ReferingPackage=ReferingPackage->NextPackage;
    }
}

void checkDelivery(LinkPackage ReferingPackage,LinkPeople UserLine){
    time_t now = time(NULL); 
	struct tm* Now=localtime(&now);
    while(ReferingPackage){
        if(ReferingPackage->state[2]==0){
            while(strcmp(ReferingPackage->Owner,UserLine->Name)){
                UserLine=UserLine->NextPeople;
            }
                if(ReferingPackage->IN_year==Now->tm_year){
                    if((Now->tm_yday-ReferingPackage->IN_Yday)>3){
                        ReferingPackage->state[2]=1;
                        strcpy(UserLine->message[(++UserLine->l)%30],"您的包裹已送达");
                        UserLine->NewMessage=1;
                    }
                    else ReferingPackage->state[2]=0;
                }
                else {
                    if((AllYearDay(ReferingPackage->IN_year)-ReferingPackage->IN_Yday+Now->tm_yday)>3){
                        strcpy(UserLine->message[(++UserLine->l)%30],"您的包裹已送达");
                        ReferingPackage->state[2]=1;
                    }
                    else ReferingPackage->state[2]=0;
                }
        }
        ReferingPackage=ReferingPackage->NextPackage;
    }
}

