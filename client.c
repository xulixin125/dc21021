#include <stdio.h>
#include <sys/types.h>          /* See NOTES */ 
#include <sys/socket.h>
 #include <arpa/inet.h>
#include <stdlib.h>
 #include <strings.h>
 #include <sys/types.h>   
#include <string.h>

#define N 20

#define R 1
#define L 2
#define Z 3
#define D 4
#define U 5
#define Q 6
#define K 7
#define I 8
#define J 9
#define M 10
#define A 11
#define S 12
#define Y 13

typedef struct{
	int id;
	char name[N];
	char pwd[N];
	char sex;//1:man 0:wenman
	int age;
	char dept[N];
	int salary;
	char address[64];
	char role[10];
}epy;

typedef struct{
	int id;
	char name[N];
	char pwd[N];
	char role[10];
}user;

typedef	struct {
	int type;
	char role[10];
	char name[N];
	char data[256];
	user u;
	epy ep;
}MSG;

int do_regster(int fd,MSG *msg);
int do_login(int fd,MSG *msg);
int do_query(int fd,MSG *msg);
int do_history(int fd,MSG *msg);
void do_deleinfo(int fd,MSG *msg);
void do_updatepy(int fd,MSG *msg,int m);
void exit_set();
void do_queryinfo(int fd,MSG *msg);
void do_updaterole(int fd,MSG *msg);
void do_updatepwd(int fd,MSG *msg);
void do_adance(int fd,MSG *msg);
void getdate(char *date);
void do_allqueryinfo(int fd,MSG *msg);
void do_queryadance(int fd,MSG *msg);
void do_allqueryadance(int fd,MSG *msg);
user cur;

int main(int argv,char *argc[]){

	if(argv < 3){
		perror("argv less\n");
	}
	MSG msg;
	int fd;
	fd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argc[2]));
	if(inet_pton(AF_INET,argc[1],(struct sockaddr *)&sin.sin_addr)!=1){
		perror("ipnet");
		exit(1);
	}

	if(connect(fd,(struct sockaddr *)&sin,sizeof(sin))<0){
		perror("connect");
		exit(1);
	}
	if(recv(fd,msg.data,sizeof(MSG),0)<0){
		perror("faile to recv\n");
		exit(1);
	}
	printf("server:%s\n",msg.data);
	int m;
	int rett;

nextstart:
	system("clear");
	int n=1;
	while(1){
		printf("***********************************************\n");
		printf("*                                             *\n");
		printf("*              员 工 管 理 系 统              *\n");
		printf("*                                             *\n");
		printf("*  1.注册     2.登录     3.修改密码   4.退出  *\n");
		printf("*                                             *\n");
		printf("***********************************************\n");
		printf("please choose:");

		scanf("%d",&m);
		getchar();
		
		switch(m){
			case 1:
				do_regster(fd,&msg);
				break;
			case 2:
				rett = do_login(fd,&msg);
				if(rett&&(strncmp(cur.role,"admin",5)==0)){
					printf("id: %d cur.name: %s,cur.role %s\n",cur.id,cur.name,cur.role);
					goto nextadmin;	
				}else if(rett){
					printf("id: %d cur.name: %s,cur.role %s\n",cur.id,cur.name,cur.role);
		   				goto nextuser;	
		  			  }
				if(n == 3){
					printf("密码错误超过3次!\n");
					exit(0);
				}
				n++;
				break;
			case 3:
				do_updatepwd(fd,&msg);
				break;
			case 4:
				close(fd);
				exit(0);
			default :
				printf("ivaid data\n");
		}
	}


nextadmin :
	while(1){
		printf("**********************************************************\n");
		printf("*                                                        *\n");
		printf("*              员 工 管 理 系 统                         *\n");
		printf("*                                                        *\n");
		printf("* 1.增加  2.删除  3.修改  4.查询  5.考勤  6.角色  7.退出 *\n");
		printf("*                                                        *\n");
		printf("**********************************************************\n");
		
		printf("please choose:");
		scanf("%d",&m);
		getchar();
	
		switch(m){
			case 1:
				 add_eply(fd,&msg);
				break;
			case 2:
				do_deleinfo(fd,&msg);
				break;
			case 3:
				goto nextupdate;
				break;
			case 4:
				goto nextview;
				//do_queryinfo(fd,&msg);
				break;
			case 5:
				do_adance(fd,&msg);
				break;
			case 6:
				do_updaterole(fd,&msg);
				break;
			case 7:
				exit_set();
				goto nextstart;

			default:
				printf("invalid\n");

		}

	}




nextuser :{
	while(1){	
		printf("***************************************************\n");
		printf("*                                                 *\n");
		printf("*              员 工 管 理 系 统                  *\n");
		printf("*                                                 *\n");
		printf("**1.查看     2.修改      3.考勤打卡       4.退出  *\n");
		printf("*                                                 *\n");
		printf("***************************************************\n");
	
		printf("please choose:");

		scanf("%d",&m);
		getchar();
	
		switch(m){
			case 1:
				goto nextview;
				//do_queryinfo(fd,&msg);
				break;
			case 2:
				goto nextupdate;
				break;
			case 3:
				do_adance(fd,&msg);
				break;
			case 4:
				exit_set();
				goto nextstart;
			default:
				printf("invalid\n");
				break;

		}
	}		  
  }


nextupdate :{
			
		while(1){
	       printf("**********************************************************\n");
	       printf("*                                                        *\n");
	       printf("*                  修改用户信息                           *\n");
	       printf("*                                                        *\n");
		   printf("*    1.名字 2.性别 3.年龄 4.部门 5.工资 6.地址  7.退出      *\n");
		   printf("*                                                        *\n");
		   printf("**********************************************************\n");
		
     	   printf("please choose:");
		   scanf("%d",&m);
		   getchar();
	     	 
		   /*if(m==7&&!strncmp("user",cur.role,4)){
				printf("非管理员不能修改角色!\n");
				continue;
		   }*/	 
		   if(m==5&&!strncmp("user",cur.role,4)){
				printf("非管理员不能修工资!\n");
				continue;
		   }

		   if(m==7){
			    exit_set();
				goto nextstart;
			}
		   do_updatepy(fd,&msg,m);

		 }
}

nextview :{
	while(1){
		printf("***************************************************\n");
		printf("*                                                 *\n");
		printf("*              员 工 管 理 系 统                  *\n");
		printf("*                                                 *\n");
		printf("*  1.个人信息          2.全体信息         5.退出   *\n");
		printf("*  3.个人考勤记录      4.全体考勤记录              *\n");
		printf("*                                                 *\n");
		printf("***************************************************\n");
	
		printf("please choose:");

		scanf("%d",&m);
		getchar();
	
		switch(m){
			case 1:
				do_queryinfo(fd,&msg);
				break;
			case 2:
				if(!strncmp(cur.role,"user",4)){
					printf("只能查询个人信息!\n");
					break;
				}
				do_allqueryinfo(fd,&msg);
				break;
			case 3:
				do_queryadance(fd,&msg);
				break;
			case 4:
				if(!strncmp(cur.role,"user",4)){
					printf("只能查询个人记录!\n");
					break;
				}
				do_allqueryadance(fd,&msg);
				break;
			case 5:
				exit_set();
				goto nextstart;
			default :
				printf("无效输入\n");
				break;
		}

	}

}

	
}//mainend

void do_allqueryadance(int fd,MSG *msg){
	msg->type = Y;

	if(send(fd,msg,sizeof(MSG),0)<0){
		printf("faile to send\n");
		return;
	}
	while(1){
		if(recv(fd,msg,sizeof(MSG),0)<0){
			printf("faile to recv\n");
			return;
		}
		if(strcmp(msg->data,"done")==0)
			break;

		printf("ID:%d, 时间:%s\n",msg->ep.id,msg->data);

	}
	
}

void do_queryadance(int fd,MSG *msg){
	msg->type = S;
	msg->u.id = cur.id;
	
	if(send(fd,msg,sizeof(MSG),0)<0){
		printf("faile to send\n");
		return;
	}
	
	while(1){
		if(recv(fd,msg,sizeof(MSG),0)<0){
			printf("faile to recv\n");
			return;
		}
		if(strcmp(msg->data,"done")==0)
			break;

		printf("ID: %d,用户 %s,时间: %s\n",msg->ep.id,cur.name,msg->data);

	}
}

void do_allqueryinfo(int fd,MSG *msg){
	msg->type = A;
	printf("查询人：%s\n",cur.name);
	if(send(fd,msg,sizeof(MSG),0)<0){
		printf("faile to send\n");
		return;
	}
	//printf("开始收信息\n");
	strcpy(msg->data,"ok");
	while(1){

		recv(fd,msg,sizeof(MSG),0);
		
		if(strcmp(msg->data,"done")==0)
			break;

		printf("id: %d,名字: %s,性别: %c,年龄: %d,部门: %s,工资: %d,住址: %s\n",msg->ep.id,
	         msg->ep.name,msg->ep.sex,msg->ep.age,msg->ep.dept,msg->ep.salary,msg->ep.address);

	}
}

//打卡
void do_adance(int fd,MSG *msg){
	char date[64];

	msg->type = K;
	msg->u.id = cur.id;
	
	if(send(fd,msg,sizeof(MSG),0)<0){
		printf("fail to send\n");
		return;
	}
	
	if(recv(fd,msg,sizeof(MSG),0)<0){
		perror("fail to recv");
		return;
	}
	
	if(strlen(msg->data)>0){
		printf("打卡成功:\n");
		printf("ID:%d  时间:%s\n",msg->u.id,msg->data);
	}else{
		printf("打卡失败:\n");
	}

}

void do_updatepwd(int fd,MSG *msg){
	
	printf("请输入要修改用户密码的ID:");
	scanf("%d",&msg->u.id);
	getchar();
	printf("请输入原始密码: ");
	scanf("%s",msg->u.pwd);
	getchar();
	printf("请输入新密码: ");
	scanf("%s",msg->data);
		
	msg->type = M;

	if(send(fd,msg,sizeof(MSG),0)<0){
		perror("fail to send");
		return;
	}
	
	if(recv(fd,msg,sizeof(MSG),0)<0){
		perror("fail to recv");
		return;
	}
	if(strncmp("OK",msg->data,2) == 0){
		printf("更改密码成功!\n");
	}else{
		printf("失败: %s\n",msg->data);
	}

}

void do_updaterole(int fd,MSG *msg){
	int f;
	//char role[10];
	printf("请输入要修改用户角色的ID:");
	scanf("%d",&msg->u.id);
	getchar();
	printf("请选择角色: 1.管理员  2.普通用户: ");
	scanf("%d",&f);

	if(f == 1){
		strcpy(msg->u.role,"admin");
	}else if(f == 2){
		strcpy(msg->u.role,"user");
	}
	
	msg->type = J;

	if(send(fd,msg,sizeof(MSG),0)<0){
		perror("fail to send");
		return;
	}
	
	if(recv(fd,msg,sizeof(MSG),0)<0){
		perror("fail to recv");
		return;
	}
	if(strncmp("OK",msg->data,2) == 0){
		printf("更改角色成功!\n");
	}else{
		printf("失败: %s\n",msg->data);
	}

}
//退出清空用户登录
void exit_set(){
	memset(cur.name,0,N);
	memset(cur.pwd,0,N);
	memset(cur.role,0,10);
}

void do_queryinfo(int fd,MSG *msg){
	msg->type = I;

	msg->ep.id = cur.id;
	
	/*if(!strncmp(cur.role,"user",4)){
		msg->ep.id = cur.id;
	}else{
		printf("输入用户ID: ");
		scanf("%d",&msg->ep.id);
		getchar();
	}*/
	
	if(send(fd,msg,sizeof(MSG),0)<0){
		perror("fail to send");
		return;
	}
	
	if(recv(fd,msg,sizeof(MSG),0)<0){
		perror("fail to recv");
		return;
	}
	if(strncmp("OK",msg->data,2) == 0){
		printf("id: %d,名字: %s,性别: %c,年龄: %d,部门: %s,工资: %d,住址: %s\n",msg->ep.id,
	         msg->ep.name,msg->ep.sex,msg->ep.age,msg->ep.dept,msg->ep.salary,msg->ep.address);
	}else{
		printf("未查到用户信息!\n");
	}
}

void do_deleinfo(int fd,MSG *msg){
	msg->type = D;

	printf("输入用户ID: ");
	scanf("%d",&msg->ep.id);
	getchar();
	
	if(cur.id == msg->ep.id){
		printf("不能将自己删除\n");
		return;
	}

	if(send(fd,msg,sizeof(MSG),0)<0){
		perror("fail to send");
		return;
	}
	
	if(recv(fd,msg,sizeof(MSG),0)<0){
		perror("fail to recv");
		return;
	}
	if(strncmp("OK",msg->data,2) == 0){
		printf("用户信息删除成功!\n");
	}else{
		printf("用户信息删除失败:%s\n",msg->data);
	    }
	
}//del end



int do_login(int fd,MSG *msg){
	msg->type = L;
	char name[20];

	printf("intput user name: ");
	scanf("%s",msg->u.name);
	getchar();
	printf("intput user pwd: ");
	scanf("%s",msg->u.pwd);
	getchar();

	strcpy(name,msg->u.name);
	//printf("开始发送验证信息\n");
   //发送用户名和密码给服务器去验证
	if(send(fd,msg,sizeof(MSG),0)<0){
		perror("fail to send");
		return 0;
	}
	//接收服务器的反馈并验证是否登录成功
	if(recv(fd,msg,sizeof(MSG),0)<0){
		perror("fail to recv");
		return 0;
	}
	if(strncmp("OK",msg->data,2) == 0){
		printf("登录成功!\n");
		if(strncmp("admin",msg->u.role,5)==0){
			//记录当前登录的用户及用户角色
			cur.id = msg->u.id;
			strcpy(cur.name,msg->u.name);  
			strcpy(cur.role,msg->u.role);
		}else{
			cur.id = msg->u.id;
			strcpy(cur.name,msg->u.name);  
			strcpy(cur.role,msg->u.role);
			//strcpy(cur.name,name);
			//strcpy(cur.role,"user");
		}
		return 1;
	}else{
		printf("登陆失败: %s \n",msg->data);
		}
	
	return 0;

}

int do_regster(int fd,MSG *msg){
	msg->type = R;

	printf("input user id:");
	scanf("%d",&msg->u.id);

	printf("input user name:");
	scanf("%s",msg->u.name);
	getchar();

	printf("input pass word:");
	scanf("%s",msg->u.pwd);
	getchar();

	if(send(fd,msg,sizeof(MSG),0)<0){
		perror("send to fail");
	}
	if(recv(fd,msg,sizeof(MSG),0)<0){
		printf("send to faile\n");
		return -1;
	}

	printf("%s\n",msg->data);
	return 0;

}


int add_eply(int fd,MSG *msg){
	msg->type = Z;

	printf("input user id:");
	scanf("%d",&msg->ep.id);

	printf("input user name:");
	scanf("%s",msg->ep.name);
	getchar();

	
	printf("input sex m or f: ");
	scanf("%c",&msg->ep.sex);
	getchar();

	printf("input age:");
	scanf("%d",&msg->ep.age);
	getchar();

	printf("input dept: ");
	scanf("%s",msg->ep.dept);
	getchar();

	printf("input salary:");
	scanf("%d",&msg->ep.salary);
	getchar();

	printf("input address:");
	scanf("%s",msg->ep.address);

	if(send(fd,msg,sizeof(MSG),0)<0){
		perror("send to fail");
	}
	if(recv(fd,msg,sizeof(MSG),0)<0){
		printf("send to faile\n");
		return -1;
	}

	printf("%s\n",msg->data);
	return 0;
}

void do_updatepy(int fd,MSG *msg,int m){
	char rename[20];
		
	msg->type = Q;

	if(!strncmp(cur.role,"user",4)){
		msg->ep.id = cur.id;
	}else{
		printf("输入用户ID: ");
		scanf("%d",&msg->ep.id);
		getchar();
	}

   /* if(!strncmp(cur.role,"user",4) && cur.id != msg->ep.id){
		printf("只能修改自己的信息\n");
		return;
	}*/
	//查询用户是否存在
	if(send(fd,msg,sizeof(MSG),0)<0){
		perror("send to faile");
		return;
	}
	if(recv(fd,msg,sizeof(MSG),0)<0){
		perror("send to faile");
		return;
	}

	if(strncmp("OK",msg->data,2)!=0){
		printf("用户不存在\n");
		return;
	}
	//printf("id %d\n",msg->ep.id);

	printf("输入新值: ");
	scanf("%s",rename);
    
	switch(m){
		case 1:
			strcpy(msg->ep.name,rename);
			break;
		case 2:
			msg->ep.sex = rename[0];
			break;
		case 3:
			msg->ep.age = atoi(rename);
			break;
		case 4:
			strcpy(msg->ep.dept,rename);
			printf("msg->dept %s\n",msg->ep.dept);
			break;	
		case 5:
			msg->ep.salary = atoi(rename);
			break;
		case 6:
			strncpy(msg->ep.address,rename,19);
			break;
		//case 7:
		//	strcpy(msg->ep.role,rename);
		//	break;
		default :
			printf("无效的值!\n");
			
			return;
			
	}

	msg->type = U;
	
	send(fd,msg,sizeof(MSG),0);

	recv(fd,msg,sizeof(MSG),0);

	if(strncmp("OK",msg->data,2)!=0){
		printf("updata faile\n");
		return;
	}
	printf("update success\n");
	

	
}

