#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include<stdlib.h>
#include "Manager.h"



// 取件
void pickUpPackage(LinkPeople currentUser, LinkPackage PackageLine) {
    if (currentUser->PackageLine == NULL) {
        printf("             您没有待取的包裹。\n");
        return;
    }
    printf("             请输入取件码：");
    char PUCODE[5];
    scanf("%4s", PUCODE);
    // 调用取件函数
    int state = PickUp_Package(PackageLine, PUCODE, currentUser);

    if(state){
        printf("取件成功！");
    }
    else printf("取件失败！");
}





// 寄件
LinkPackage sendPackage(LinkPeople currentUser, LinkPackage PackageLine,LinkPeople UserLine) {
    struct tm* Now=GetNowTime();
    char NAME[5],PhoneNum[12];
    float weight;
    int transport, distance, insurance;
    int value;
    int isUser;
    printf("             收件人是否是程序用户：1.是  0.不是\n");
    scanf("%d",&isUser);
    while(1){
        if(isUser){
            printf("             请输入收件人名称：");
            scanf("%s",NAME);
            while(UserLine){
                if(!strcmp(NAME,UserLine->Name))isUser=0;
                else {
                    isUser=1;
                    break;
                }
            }
        }
        else {
            printf("             请输入收件人名称：");
            scanf("%s",NAME);
            printf("             请输入收件人联系电话：");
            scanf("%s",PhoneNum);
            break;
        }
        if(isUser)break;
        else {
            char ch;
            printf("             非用户！重新输入选1，或按任意键退出...  ");
            gets(&ch);
            if(ch=='1'){
                isUser=1;
                continue;
            }
            else return NULL;
        }
    }
    
    printf("             请输入包裹重量(kg)：");
    scanf("%f", &weight);
    while (weight <= 0) 
    {
        printf("          包裹重量必须为正数，请重新输入：");
        scanf("%f", &weight);
    }
    printf("             是否选择加急运输，1: 是，0: 否：");
    scanf("%d", (int*)&transport);
    while (transport != 0 && transport != 1)
    {
        printf("             输入无效，请输入 0 或 1：");
        scanf("%d", (int*)&transport);
    }
    printf("             请注意：物品价值超过一千元会收取 1.1 倍运费 \n");
    printf("             请输入包裹物品价值：");
    scanf("%d", &value);
    while (value <= 0) 
    {
        printf("             物品价值必须为正数，请重新输入：");
        scanf("%d", &value);
    }
    printf("             是否为省内，1: 是，0: 否：");
    scanf("%d", (int*)&distance);
    while (distance != 0 && distance != 1) 
    {
        printf("             输入无效，请输入 0 或 1：");
        scanf("%d", (int*)&distance);
    }
    printf("             是否购买保险，1: 是，0: 否：");
    scanf("%d", (int*)&insurance);
    while (insurance != 0 && insurance != 1) 
    {
        printf("             输入无效，请输入 0 或 1：\n");
        scanf("%d", (int*)&insurance);
    }
        printf("             是否支付成功？\n");
        printf("             1--是\n             2--否\n");
        int j;
        scanf("%d", &j);
        while (j != 1)
        {
            if (j != 1 && j != 2)//考虑用户输入错误或者误触
            {
                printf("                   请输入正确业务选择项：\n              1--是\n              2--否\n");
            }
            else
            {
                printf("         请完成支付\n");
            }
            printf("是否支付成功？\n");
            printf("             1--是\n              2--否\n");
            scanf("%d", &j);
        }
    // 创建一个新包裹
    
    /*
    state[0]存取状况1--库中，0--出库；
    state[1]存放包裹状况（是否损坏或丢失）1-完好 0-损坏 ；
    state[2]运输状态，1-运达，0-途中；
    state[3]是否超时，1-滞留超时，0-未超时
    */

    // 调用包裹写入函数
    PackageLine=Package_write(weight, transport, value, distance, insurance, PackageLine,currentUser,NAME,isUser,PhoneNum);
    if(isUser==0){
        LinkPackage ReferingPackage=PackageLine;
        while(strcmp(ReferingPackage->Receiver,NAME)){
            ReferingPackage=ReferingPackage->NextPackage;
        }   
    }

    // 将新包裹插入用户包裹链和主包裹链

    FILE* UserJournal=fopen("UserJournal.txt","a");
    fprintf(UserJournal,"[ %4d.%02d.%02d %02d:%02d:%02d ] 用户%s 寄件：寄件成功！",Now->tm_year+1900,Now->tm_mon,Now->tm_mday,Now->tm_hour,Now->tm_min,Now->tm_sec,currentUser->Name);
    if(isUser)fprintf(UserJournal,"收件人（用户）：%s \n",NAME);
    else fprintf(UserJournal,"收件人（非用户）：%s  联系电话：%s \n",NAME,PhoneNum);
    fflush(UserJournal);
    fclose(UserJournal);
    printf("                寄件成功！\n");
    return PackageLine;
}

//1.用户提示信息问题 2.寄件，收件者不在系统中时 3.界面，框架 4.Manager中函数调整问题(over) 5.用户快递超时问题

int main() {
    LinkPackage PackageLine = InsertPackage();
    LinkPeople UserLine = Initialization_User();
    //超时与送达
    checkOvetTime(PackageLine,UserLine);
    checkDelivery(PackageLine,UserLine);

    People_Package(UserLine, PackageLine);

    //管理员初始化9
    LinkManager p1=(LinkManager)malloc(sizeof(struct Manager));
    LinkManager p2=(LinkManager)malloc(sizeof(struct Manager));
    strcpy(p1->Account, "0001");
    strcpy(p1->Password, "8888");
    p1->next = p2;
    p2->next = NULL;
    strcpy(p2->Account, "0002");
    strcpy(p2->Password, "9999");
    p1->next = p2;
    p2->next = NULL;
    LinkManager head3 = p1;//管理员头指针
    


    int m; // 角色选择
    int n; // 业务选择
    int Remainingtime = 5; // 管理员登录剩余尝试次数
    printf("                    请选择角色\n                    1--管理员\n                    2--客户\n");
    printf("                     选择：");
	scanf("%d", &m);
    if (m == 1)
    { /*管理员登录*/
       if (Manager_log_in(head3) == 0) 
       {
           Manager_Operation(PackageLine, UserLine);

        }
       else if (Manager_log_in(head3) == -1) 
       {
           printf("即将退出程序！");
       }

    } // 管理员登陆部分到此结束

    else if (m == 2) 
    {
    	system("cls");
        printf("                  用户身份为客户\n");
        printf("            若您已经拥有账号，请输入 1\n       若您尚未拥有账号，请输入 2 前往注册页面\n");
        int k;
        printf("                     选择：");
        while (scanf("%d", &k) != 1 || (k != 1 && k != 2)) 
        {
            printf("             输入无效，请输入 1 或 2。\n");
            printf("                     选择：");
            while (getchar() != '\n'); // 清空输入缓冲区
        }

        LinkPeople currentUser = NULL;
        if (k == 1) 
        {
            currentUser = login(UserLine);
        }
        else if (k == 2) 
        {
            char acnumber[5];
            char password[5];
            system("cls");
            while (1) {
                printf("                 请输入注册账号\n");
                printf("                     账号：");
                if (scanf("%4s", acnumber) != 1) 
                {
                    printf("             输入无效，请输入正确的账号。\n");
                    printf("                  账号：");
                    continue;
                }
                // 在注册时，我需要先判断账号是否重复
                LinkPeople check = UserLine;
                int isDuplicate = 0;
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
                    printf("             该账号已存在，请更换账号。\n");
                }
                else 
                {
                    break;
                }
            }
            printf("             请输入注册账号密码\n");
            printf("                     密码：");
            if (scanf("%4s", password) != 1)
            {
                printf("             输入无效，请输入正确的密码。\n");
                printf("                     密码：");
            }
            else 
            { 
                UserLine=Register(acnumber, acnumber, password, UserLine);
                printf("             新账号已经注册成功。\n");
                
                // 找到新注册的用户
                system("cls");
                LinkPeople find = UserLine;
                while (find != NULL) 
                {
                    if (strcmp(find->ACnumber, acnumber) == 0) 
                    {
                        currentUser = find;
                        break;
                    }
                    find = find->NextPeople;
                }
            }
        }
        if (currentUser != NULL) 
        {
            do {
                printf("              请选择您要办理的业务\n");
                printf("                  1--取件\n");
                printf("                  2--查看包裹状态\n");
                printf("                  3--寄件(上门取件)\n");
                printf("                  4--注销账号\n");
                printf("                  5--查看信息\n");
                printf("                  0--退出系统\n");
                while (scanf("%d", &n) != 1&&n<0&&n>=5) //同样考虑用户在输入时的误触
                {
                    printf("             输入无效，请输入正确的选项。\n");
                    printf("                     选择：");
                    while (getchar() != '\n'); // 清空输入缓冲区
                }
                int lenth;
                int w=0;
                switch (n) 
                { 
                case 1:{
                	system("cls");
                    char a[5];
                    printf("             请输入您的取件码：");
                    printf("               取件码：");
                    getchar();
                    fgets(a, 5, stdin);//用于匹配取件码
                    lenth = (int)strlen(a);
                    if (lenth != 0 && a[lenth - 1] == '\n')
                        a[lenth - 1] = '0';
                    int r=PickUp_Package(PackageLine,a, currentUser);//这里要修改
                    if(r==1)printf("             取件成功!");
                    else printf("             取件失败!");
                    break;
                }  
                case 2:
                	system("cls");
                    checkPackageStatus(currentUser);//displayPackageInfo(currentUser->PackageLine);
                    break;
                case 3:
                	system("cls");
                    PackageLine=sendPackage(currentUser, PackageLine,UserLine);
                    break;
                case 4:
                	system("cls");
                   w= cancelAccount(&UserLine, currentUser);
                   if (w == 1)
                       currentUser = NULL;
                   else
                       break;
                    break;
                case 5:
				system("cls");
				ScanMessage(currentUser);
				break;
                case 0:
                	system("cls");
                    printf("                 退出系统。\n");
                    break;
                default:
                	system("cls");
                    printf("             无效选项，请重新输入。\n");
                    break;
                }
            } while (n != 0 );
        }
    }
    Keep_In_UserFile(UserLine);
    Keep_In_PackageFile(PackageLine);
    printf("                 感谢再次使用\n");
    return 0;
}