#include <stdio.h>
#include <sys/types.h>          /* See NOTES */ 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>          /* See NOTES */
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sqlite3.h>
#include <pthread.h>
#include <signal.h>

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

#define N 20
#define DATABASE "manage.db"
typedef struct{
	int id;
	char name[N];
	char pwd[N];
	char sex;//m:man f:wenman
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
typedef struct{
	int fd;
	sqlite3 *db;
}param;

void do_allqueryadance(int fd,sqlite3 *db,MSG *msg);
void do_queryadance(int fd,sqlite3 *db,MSG *msg);
void do_allqueryinfo(int fd,sqlite3 *db,MSG *msg);
void getdate(char *date);
void do_adance(int fd,sqlite3 *db,MSG *msg);
void do_updatepwd(int fd,sqlite3 *db,MSG *msg);
void do_updaterole(int fd,sqlite3 *db,MSG *msg);
void do_queryinfo(int fd,sqlite3 *db,MSG *msg);
void add_eply(int fd,sqlite3 *db,MSG *msg);
void do_update(int fd,sqlite3 *db,MSG *msg);
void do_querybyname(int fd,sqlite3 *db,MSG *msg);
void do_deleinfo(int fd,sqlite3 *db,MSG *msg);
void do_register(int fd,sqlite3 *db,MSG *msg);
void do_login(int fd,sqlite3 *db,MSG *msg);
void * handler(void *fd);

sqlite3 *db;

int callbackadance(void* arg,int ncolumns ,char** f_value,char** f_name);

int callback(void* arg,int ncolumns ,char** f_value,char** f_name);

void handint(int signo){
	if(signo == SIGINT){
		sqlite3_close(db);
		printf("数据库关闭\n");
		exit(1);
	}

}


int main(int argv, char * argc[]){
	if(argv < 3){
		printf("Usage %s ip port\n",argc[0]);
		return -1;
	}

	int fd,newfd;
	MSG msg;
	//sqlite3 *db;	
	char *errmsg;

	if(sqlite3_open(DATABASE,&db)!= SQLITE_OK){
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}else{
		printf("open database success\n");
	}

	fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd < 0){
		perror("socket is error");
		return -1;
	}
	
	int b_reuse = 1;
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&b_reuse,sizeof(int));

	struct sockaddr_in sin,csin;
	bzero(&sin,sizeof(struct sockaddr_in));
	bzero(&csin,sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argc[2]));
#if 1
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
#else
	if(inet_pton(AF_INET,argc[1],(void *)&sin.sin_addr)!=1){
		perror("ip_pton");
		exit(1);
	}
#endif
	if(bind(fd,(struct sockaddr *)&sin,sizeof(struct sockaddr_in))==-1){
		perror("bind error");
		exit(1);
	}
	if(listen(fd,5)<0){
		perror("listen");
		exit(1);
	}
	printf("server is start ...\n");

	signal(SIGCHLD, SIG_IGN);
	signal(SIGINT,handint);
	pthread_t thread_t;
	param para;
	while(1){
		socklen_t len=sizeof(csin);
			if((newfd = accept(fd,(struct sockaddr *)&csin,&len))<0){
				perror("accept");
				exit(1);
			}
		para.fd = newfd;
		para.db = db;
		if(pthread_create(&thread_t,NULL,handler,&para)!=0){
			perror("pthread_create is error");
			exit(1);
		}	

	}
}

void * handler(void *para){
	//int	fd = *(int *)newfd;
	param *pa = NULL;
	pa = (param *)para;
	MSG msg;
	strcpy(msg.data,"welcom connect server\n");
	send(pa->fd,msg.data,sizeof(MSG),0);
	int m;
	while(recv(pa->fd,&msg,sizeof(MSG),0)>0){
			m = msg.type;
		switch(m){
			case R:
				do_register(pa->fd,pa->db,&msg);
				break;
			case L:
				do_login(pa->fd,pa->db,&msg);
				break;				
			case Z:
				add_eply(pa->fd,pa->db,&msg);
				break;
			case D:
				do_deleinfo(pa->fd,pa->db,&msg);
				break;
			case U:
				do_update(pa->fd,pa->db,&msg);
				break;
			case Q:
				do_querybyname(pa->fd,pa->db,&msg);
				break;
			case K:
				do_adance(pa->fd,pa->db,&msg);
				break;
			case I:
				do_querybyname(pa->fd,pa->db,&msg);
				//do_queryinfo(pa->fd,pa->db,&msg);
				break;
			case J:
				do_updaterole(pa->fd,pa->db,&msg);
				break;
			case M:
				do_updatepwd(pa->fd,pa->db,&msg);
				break;
			case A:
				do_allqueryinfo(pa->fd,pa->db,&msg);
				break;
			case S:
				do_queryadance(pa->fd,pa->db,&msg);
				break;
			case Y:
				do_allqueryadance(pa->fd,pa->db,&msg);
				break;
			default :
				printf("invalid data\n");
				
		}

	}
}

/*
void do_queryinfo(int fd,sqlite3 *db,MSG *msg){
	char sql[128];			
	char *errmsg;
	char **resultp;
	int nrow,ncloumn;

	sprintf(sql,"select * from uinfo where id=%d",msg->ep.id);
	printf("sql %s\n",sql);
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncloumn,&errmsg)
			                                          !=SQLITE_OK){
		printf("%s\n",errmsg);
		return;
	}
	if(nrow == 1){

	}

}*/

void do_allqueryadance(int fd,sqlite3 *db,MSG *msg){
	char sql[128];
	char *errmsg;
	
	sprintf(sql,"select * from adance");
	printf("%s\n",sql);
	strcpy(msg->data,"OK");
	if(sqlite3_exec(db,sql,callbackadance,&fd,&errmsg)<0){
		printf("%s\n",errmsg);
		return;
	}
	
	strcpy(msg->data,"done");
	if(send(fd,msg,sizeof(MSG),0)<0){
		printf("faile to send\n");
		return;
	}
}

void do_queryadance(int fd,sqlite3 *db,MSG *msg){
	char sql[128];
	char *errmsg;
	strcpy(msg->data,"OK");
	sprintf(sql,"select * from adance where id=%d",msg->u.id);
	printf("%s\n",sql);
	if(sqlite3_exec(db,sql,callbackadance,&fd,&errmsg)<0){
		printf("%s\n",errmsg);
		return;
	}
	strcpy(msg->data,"done");

	if(send(fd,msg,sizeof(MSG),0)<0){
		printf("faile to send\n");
		return;
	}
}


void do_allqueryinfo(int fd,sqlite3 *db,MSG *msg){
	char sql[128];
	char *errmsg;
	char **resultp;
	int nrow,ncloumn;
	
	sprintf(sql,"select * from uinfo");
	printf("%s\n",sql);
	/*if(sqlite3_exec(db,sql,callback,&fd,&errmsg)<0){
		printf("%s\n",errmsg);
		return;
	}*/
	
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncloumn,&errmsg)!=SQLITE_OK){
		printf("%s\n",errmsg);
		return ;
	}
	int i,j,index;
	index = ncloumn;
	strcpy(msg->data,"OK");
	for(i = 0;i < nrow; i++){
		msg->ep.id = atoi(resultp[index++]);
		strcpy(msg->ep.name,resultp[index++]);
		msg->ep.sex = *(char*)resultp[index++];
		msg->ep.age = atoi(resultp[index++]);
		strcpy(msg->ep.dept,resultp[index++]);
		msg->ep.salary = atoi(resultp[index++]);
		strcpy(msg->ep.address,resultp[index++]);
		printf("id:%d name:%s\n",msg->ep.id,msg->ep.name);
		send(fd,msg,sizeof(MSG),0);
	}

	strcpy(msg->data,"done");

	send(fd,msg,sizeof(MSG),0);
	
	/*strcpy(msg->data,"");
	
	if(send(fd,msg,sizeof(MSG),0)<0){
		printf("faile to send\n");
		return;
	}*/

}

//创建考勤记录(打卡)
void do_adance(int fd,sqlite3 *db,MSG *msg){
	char date[64];
	char sql[128];
	char *errmsg;
	getdate(date);
	sprintf(sql,"insert into adance (id,kq) values(%d,'%s')",msg->u.id,date);
	printf("adance:%s\n",sql);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)<0){
		printf("%s\n",errmsg);
		return;
	}
	strcpy(msg->data,date);
	
	if(send(fd,msg,sizeof(MSG),0)<0){
		printf("faile to send\n");
		return;
	}
}

int queryuser(sqlite3 *db,user *u){
	char sql[128];
	char *errmsg;
	char **resultp;
	int nrow,ncloumn;
	
	
	sprintf(sql,"select * from user where id=%d",u->id);
	
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncloumn,&errmsg)!=SQLITE_OK){
		printf("%s\n",errmsg);
		return 0;
	}
	if(nrow == 1){
		u->id = atoi(resultp[ncloumn++]);
		strcpy(u->name,resultp[ncloumn++]);
		strcpy(u->pwd,resultp[ncloumn++]);
		strcpy(u->role,resultp[ncloumn++]);
		return 1;		
	}else{
		return 0;
	}
}
void do_updatepwd(int fd,sqlite3 *db,MSG *msg){
	char sql[128];
	char *errmsg;
	user u;
	u.id = msg->u.id;
	if(!queryuser(db,&u)){
		strcpy(msg->data,"没有此用户!");
		send(fd,msg,sizeof(MSG),0);
		return;
	}
	printf("u id,name:%d,%s\n",u.id,u.name);
	
	if(strncmp(u.pwd,msg->u.pwd,strlen(u.pwd))!=0){
		//printf("原始密码错误!\n");
		strcpy(msg->data,"原始密码错误!");
	    send(fd,msg,sizeof(MSG),0);
		return;
	}

	sprintf(sql,"update user set pwd ='%s' where id = %d",msg->data,msg->u.id);
	printf("sql %s\n",sql);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
		printf("%s\n",errmsg);
		return;
	}
	strcpy(msg->data,"OK");
	send(fd,msg,sizeof(MSG),0);

}

void do_updaterole(int fd,sqlite3 *db,MSG *msg){
	char sql[128];
	char *errmsg;
	sprintf(sql,"update user set role ='%s' where id = %d",msg->u.role,msg->u.id);
	printf("sql %s\n",sql);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
		printf("%s\n",errmsg);
		return;
	}
	strcpy(msg->data,"OK");
	send(fd,msg,sizeof(MSG),0);
}

void do_update(int fd,sqlite3 *db,MSG *msg){
	char sql[128];
	char *errmsg;
	sprintf(sql,"update uinfo set name='%s',sex='%c',age=%d,dept='%s',salary=%d,address='%s' where id = %d",msg->ep.name,msg->ep.sex,msg->ep.age,msg->ep.dept,msg->ep.salary,msg->ep.address,msg->ep.id);
	printf("sql %s\n",sql);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
		printf("%s\n",errmsg);
		return;
	}
	strcpy(msg->data,"OK");
	send(fd,msg,sizeof(MSG),0);
}

void do_querybyname(int fd,sqlite3 *db,MSG *msg){
	char sql[128];			
	char *errmsg;
	char **resultp;
	int nrow,ncloumn;

	sprintf(sql,"select * from uinfo where id=%d",msg->ep.id);
	printf("sql %s\n",sql);
	if(sqlite3_get_table(db,sql,&resultp,&nrow,&ncloumn,&errmsg)
			                                          !=SQLITE_OK){
		printf("%s\n",errmsg);
		return;
	}
	if(nrow == 1){
		printf("查到用户\n");
		msg->ep.id = atoi(resultp[ncloumn++]);
		strcpy(msg->ep.name,resultp[ncloumn++]);
		msg->ep.sex = *(char*)resultp[ncloumn++];
		msg->ep.age = atoi(resultp[ncloumn++]);
		strcpy(msg->ep.dept,resultp[ncloumn++]);
		msg->ep.salary = atoi(resultp[ncloumn++]);
		strcpy(msg->ep.address,resultp[ncloumn++]);
		
		strcpy(msg->data,"OK");
		printf("id=%d\n",msg->ep.id);
		printf("=%s\n",msg->ep.name);
	}else{
		strcpy(msg->data,"未查到用户信息!");
	}
	send(fd,msg,sizeof(MSG),0);
							
 }

void do_deleinfo(int fd,sqlite3 *db,MSG *msg){
	char sql[128];
	char *errmsg;
	char **resultp;
	int nrow,ncloumn;
	sprintf(sql,"select * from uinfo where id=%d",msg->ep.id);

	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg)!= SQLITE_OK){
		printf("%s\n",errmsg);
		return ;
	}
	if(nrow != 1){
		strcpy(msg->data,"没查到要删除的用户\n");
		send(fd,msg,sizeof(MSG),0);
		return;
	}
	
	sprintf(sql,"delete from uinfo where id=%d",msg->ep.id);

	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
		printf("fail to delete\n");
	}
	
	strcpy(msg->data,"OK");
	if(send(fd,msg,sizeof(MSG),0) < 0){
		perror("fail to send");
		return;
	}

}


void do_login(int fd,sqlite3 *db,MSG *msg){
	char sql[128];
	char **resultp;
	int nrow,ncloumn;
	char *errmsg;
	printf("name: %s,pwd: %s\n",msg->u.name,msg->u.pwd);
	sprintf(sql,"select * from user where name='%s'",msg->u.name);
	printf("收到验证信息\n");
	//查询数据库是否有name 用户
	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg)!= SQLITE_OK){
		printf("%s\n",errmsg);
		return ;
	}
	if(nrow == 1){
		printf("查询到验证信息\n");
		if(strncmp(msg->u.pwd,resultp[ncloumn+2],
							strlen(resultp[ncloumn+2]))==0){
			printf("upwd %s,resultpwd %s\n",msg->u.pwd,resultp[ncloumn+2]);
			//读取权限，将当前用户角色赋值给全局变量
			msg->u.id=atoi(resultp[ncloumn]);
			//printf("u.id %d\n",msg->u.id);
			strcpy(msg->u.role,resultp[ncloumn+ncloumn-1]);
			strcpy(msg->data,"OK");
			//strcpy(msg->role,role);
		//	printf("role:%s\n",role);
		//	send(fd,msg,sizeof(MSG),0);
			
		}else {
			strcpy(msg->data,"用户名或密码错误!");
			}
	}else{
		strcpy(msg->data,"用户名或密码错误!");
	}
	send(fd,msg,sizeof(MSG),0);
}

void do_register(int fd,sqlite3 *db,MSG *msg){
	printf("client register...\n");
	char sql[128];
	char **resultp;
	int nrow,ncloumn;
	char *errmsg;

	sprintf(sql,"select * from user where name='%s'",msg->u.name);
	//printf("sql:%s\n",sql);
	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg)!= SQLITE_OK){
		printf("%s\n",errmsg);
		return ;
	}
	if(nrow == 1){
		strcpy(msg->data,"user is exist!");
		send(fd,msg,sizeof(MSG),0);
		return;
	}	
	sprintf(sql,"insert into user values (%d,'%s','%s','user')",msg->u.id,msg->u.name,msg->u.pwd);
			printf("sql = %s\n",sql);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
		printf("%s\n",errmsg);
		return;
	}
	strcpy(msg->data,"register ok");
	send(fd,msg,sizeof(MSG),0);
}
void add_eply(int fd,sqlite3 *db,MSG *msg){
	//printf("client add oply...\n");
	char sql[128];
	char **resultp;
	int nrow,ncloumn;
	char *errmsg;

	sprintf(sql,"select * from user where id='%d'",msg->ep.id);
	//printf("sql:%s\n",sql);
	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg)!= SQLITE_OK){
		printf("%s\n",errmsg);
		return ;
	}
	if(nrow != 1){
		strcpy(msg->data,"该用户未注册!");
		send(fd,msg,sizeof(MSG),0);
		return;
	}	

	sprintf(sql,"select * from uinfo where id='%d'",msg->ep.id);
	//printf("sql:%s\n",sql);
	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg)!= SQLITE_OK){
		printf("%s\n",errmsg);
		return ;
	}
	if(nrow == 1){
		strcpy(msg->data,"该用户信息已存在!");
		send(fd,msg,sizeof(MSG),0);
		return;
	}	

	//sprintf(sql,"insert into uinfo values (%d,'%s','%c',%d,'%s',%d,'%s')",msg->ep.id,msg->ep.name,msg->ep.sex,msg->ep.age,msg->ep.dept,msg->ep.salary);
	sprintf(sql,"insert into uinfo values (%d,'%s','%c',%d,'%s',%d,'%s')",msg->ep.id,msg->ep.name,msg->ep.sex,msg->ep.age,msg->ep.dept,msg->ep.salary,msg->ep.address);
			printf("sql = %s\n",sql);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK){
		printf("%s\n",errmsg);
		return;
	}
	strcpy(msg->data,"添加成功");
	send(fd,msg,sizeof(MSG),0);
	
}

void getdate(char *date){
	time_t tm;
	time(&tm);
	struct tm * tp;
	tp= localtime(&tm);
	sprintf(date,"%d:%d:%d:%d:%d:%d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
	printf("%s\n",date);
}

/*
int callback(void* arg,int ncolumns ,char** f_value,char** f_name){
	int fd = *(int *)arg;
	MSG msg;
	msg.ep.id = atoi(f_value[0]);
	strcpy(msg.ep.name,f_value[1]);
	msg.ep.sex = *(char*)f_value[2];
	msg.ep.age = atoi(f_value[3]);
	strcpy(msg.ep.dept,f_value[4]);
	msg.ep.salary = atoi(f_value[5]);
	strcpy(msg.ep.address,f_value[6]);
	send(fd,msg,sizeof(MSG),0);
	return 0;
}*/

int callbackadance(void* arg,int ncolumns ,char** f_value,char** f_name){
	int fd = *(int *)arg;
	MSG msg;
	
	msg.ep.id = atoi(f_value[0]);
	strcpy(msg.data,f_value[1]);

	if(send(fd,&msg,sizeof(MSG),0)<0){
		printf("faile to send\n");
		return;
	}
	return 0;
	
}