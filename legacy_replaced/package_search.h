#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 1024
struct Package{
    char PackNUM[5];//包裹编号
    char PUcode[5];//取件码
    float weight;//重量
    bool transport;//运输方式,true为快速送，false为普通送
    int value;//物品的价格（以整数记），高于1000元物品令加收包装费，为总体运费的1.1倍
    bool distance;//距离,分为省内和省外，true为省内
    bool insurance;//保险，若丢件和损坏可以获得【物品原价】赔偿；未保价物品根据【运费】的三倍赔偿
    char Owner[4];//包裹物主
    int state[3];//state[0]存取状况1--库中，0--出库；state[1]存放包裹状况（是否损坏或丢失）1-完好 0-损坏 ；state[2]运输状态，1-运达，0-途中
    int IN_second,IN_minute,IN_hour,IN_day,IN_month,IN_year,IN_Yday;//入库时间
    int OUT_second,OUT_minute,OUT_hour,OUT_day,OUT_month,OUT_year,OUT_Yday;//出库时间
    float money;//运费
    struct Package *NextPackage;
};
//查询库存
void list_operation(struct Package *head){
	FILE *package_file;
    printf("查询库存\n");
    printf("1--送入本驿站\n");
    printf("2--由本驿站送出\n");
    int flag;
    scanf("%d",&flag);
    while(flag!=1&&flag!=2){
    	printf("请重新输入\n");
    	scanf("%d",&flag);
	}
	system("cls");//清屏
	if(flag==1){
		package_file=fopen("package_in","r");
	}
	else package_file=fopen("package_out","r");
	if(package_file==NULL){
		perror("无法打开文件");
		return;
	}
	char line[MAX_LINE_LENGTH];
	while(fgets(line,MAX_LINE_LENGTH,package_file)!=NULL){
		printf("%s",line);
	}
	fclose(package_file);
}
void package_search(struct Package *head){
	FILE *package_in_file;
	FILE *package_out_file;
	package_in_file=fopen("package_in","w");
	package_out_file=fopen("package_out","w");
	if(package_in_file==NULL){
		perror("Error opening file");
		return;
	}
	if(package_out_file==NULL){
		perror("Error opening file");
		return;
	}
	int in=0,out=0;//在库及出库的包裹数量
	struct Package *p=head;
	while(p!=NULL){
		if(p->state[0]==1){
			fprintf(package_in_file,"%s %s\n",p->PackNUM,p->Owner);
			in++;
		}
		if(p->state[2]==0){
			fprintf(package_out_file,"%s %s\n",p->PackNUM,p->Owner);
			out++;
		}
		p=p->NextPackage;
	}
	fprintf(package_in_file,"%d",in);
	fprintf(package_out_file,"%d",out);
	fclose(package_in_file);
	fclose(package_out_file);
}