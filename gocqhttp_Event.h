#pragma once
#include"gocqhttp_err.h"

#define ACCEPT_COUNT_MAX	20
#define Event_Response		"HTTP/1.1 204 OK\r\n\r\n"

/*事件消息*/
typedef enum
{
	message_event,			//消息
	notice_event,			//通知
	unknow_event			//未知
}event_type;		//事件类型

typedef enum
{
	private_message,	//私聊事件
	group_message,		//群消息事件
	unknow_message		//未知消息事件
}message_type;		//消息事件类型

typedef struct Event_List_Node
{
	char* msg;						//事件消息
	struct Event_List_Node* next;	//下一节点
}event_list_node;	//消息事件节点（链表）

typedef struct
{
	int read;				//是否允许evebt_get函数读取
	event_list_node* head;	//事件
}event_list_head;					//消息事件节点头（链表）
event_list_head head;

////////////
/*私聊事件*/
////////////
typedef struct
{
	unsigned long time;			//事件发生的时间戳
	unsigned long self_id;		//收到事件的机器人 QQ 号
	char post_type[20];			//上报类型
	char message_type[20];		//消息类型
	char sub_type[20];			//消息子类型
	unsigned long target_id;	//我也不知道是干啥的，文档也没写，问了开发者他也不知道，但值应该和self_id字段一样
	int temp_source;			//临时对话来源
	int message_id;				//消息ID
	unsigned long user_id;		//发送者QQ号
	char message[1024];			//消息内容
	char raw_message[1024];		//原始消息内容
	int font;					//字体
	struct
	{
		unsigned long user_id;	//发送者QQ号
		unsigned long group_id;	//群号
		char nickname[50];		//昵称
		char sex[10];			//性别
		int age;				//年龄
	}sender;		//发送人信息
}private_message_event_data;

#define PRIVATE_MESSAGE_EVENT_FORM_1	"%*[^{]{\"font\":%d,\"message\":\"%[^\"]\",\"message_id\":%d,\"message_type\":\"%[^\"]\",\"post_type\":\"%[^\"]\",\"raw_message\":\"%[^\"]\",\"self_id\":%ld,\"sender\":{\"age\":%d,\"nickname\":\"%[^\"]\",\"sex\":\"%[^\"]\",\"user_id\":%ld},\"sub_type\":\"%[^\"]\",\"target_id\":%ld,\"time\":%ld,\"user_id\":%ld%*s"
#define PRIVATE_MESSAGE_EVENT_FORM_2	"%*[^{]{\"font\":%d,\"message\":\"%[^\"]\",\"message_id\":%d,\"message_type\":\"%[^\"]\",\"post_type\":\"%[^\"]\",\"raw_message\":\"%[^\"]\",\"self_id\":%ld,\"sender\":{\"age\":%d,\"group_id\":%ld,\"nickname\":\"%[^\"]\",\"sex\":\"%[^\"]\",\"user_id\":%ld},\"sub_type\":\"%[^\"]\",\"temp_sourc\":%d\"time\":%ld,\"user_id\":%ld%*s"

private_message_event_data private_message_event_analysis(char* data);		//解析事件

//////////////
/*群消息事件*/
//////////////
typedef struct
{
	unsigned long time;			//事件发生时间戳
	unsigned long self_id;		//收到事件的机器人QQ号
	char post_type[20];			//上报类型
	char message_type[20];		//消息类型
	int message_seq;			//别问为什么这个文档里没有，我问了开发者，他也忘了
	char sub_type[20];			//消息子类型
	int message_id;				//消息ID
	unsigned long group_id;		//群号
	unsigned long user_id;		//发送者QQ号
	struct
	{
		unsigned long id;		//匿名用户ID
		char name[40];			//匿名用户昵称
		char flag[100];			//匿名用户flag，调用禁言API时需传入
	}anonymous;	//匿名消息(非匿名为null)
	char message[1024];			//消息类容
	char raw_message[1024];		//原始消息内容
	int font;					//字体
	struct
	{
		unsigned long user_id;	//发送者QQ号
		char nickname[50];		//昵称
		char card[50];			//群名片/备注
		char sex[10];			//性别
		int age;				//年龄
		char area[100];			//地区
		char level[10];			//成员等级
		char role[20];			//角色
		char title[20];			//专属头衔
	}sender;		//发送人信息
}group_message_event_data;

#define GROUP_MESSAGE_EVENT_FORM		"%*[^{]{\"anonymous\":%[^,],\"font\":%d,\"group_id\":%ld,\"message\":\"%[^\"]\",\"message_id\":%d,\"message_seq\":%d,\"message_type\":\"%[^\"]\",\"post_type\":\"%[^\"]\",\"raw_message\":\"%[^\"]\",\"self_id\":%ld,\"sender\":{\"age\":%d,\"area\":\"%[^\"]\",\"card\":\"%[^\"]\",\"level\":\"%[^\"]\",\"nickname\":\"%[^\"]\",\"role\":\"%[^\"]\",\"sex\":\"%[^\"]\",\"title\":\"%[^\"]\",\"user_id\":%ld},\"sub_type\":\"%[^\"]\",\"time\":%ld,\"user_id\":%ld%*s"
#define GROUP_MESSAGE_EVENT_FORM_ANOYMOUS	"{\"flag\":\"%[^\"]\",\"id\":%ld,\"name\":\"%[^\"]\"}"

group_message_event_data group_message_event_analysis(char* data);			//解析事件

////////////
/*未知事件*/
////////////
typedef struct
{
	char* data;					//事件类容
}unknow_event_data;

/*创建Event服务端*/
cqhttp_err init_gocqhttpEvent(
	const char* ip,
	const int port,
	void(*response)(void* data)				//消息事件响应函数
);

/*事件列表操作*/
void event_add(char* data);		//增加事件
char* event_get(void);			//获取事件

/*所有事件*/
typedef union
{
	private_message_event_data private_message;
	group_message_event_data group_message;
}event_data;

/*接收Event*/
cqhttp_err recv_event(void);

////////////////
/*事件消息检索*/
////////////////

//上报类型检索
#define EVENT_TYPE_FORM		"\"post_type\":\"%[^\"]\""
event_type event_type_switch(char* data);

//消息类型检索
#define	MESSAGE_TYPE_FORM	"\"message_type\":\"%[^\"]\""
message_type message_type_switch(char* data);

//通知类型检索
// #define NOTICE_TYPE_FORM	""
//notice_type notice_type_switch(char* data);