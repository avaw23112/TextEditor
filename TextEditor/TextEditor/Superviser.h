#pragma once
#include "Tool.h"
#include "VirtualBuffer.h"

//六个命令的线程布置以及子线程布置
//输入输出及显示控件
//切换操作

//控制对象
class Superviser
{
public:
	Superviser();
	~Superviser();
	//功能
	int SP_Console();								//输入输出控制
	int SP_ConsoleInit();
	int SP_ComTranslate(SET_OF_COMMEND COM, std::string commend);
	void SP_EXIT();


private:
	//控制台管理
	HANDLE hStdIn;									//控制台句柄
	DWORD dwMode;									//控制台模式

	//线程管理
	VirtualBuffer* SP_VB_object[32];				//最高支持同时打开32份文件，但带有显示功能的只有4份，保护措施还没加（怎么辨别是前4个？）
	std::thread SP_Thread[18];						//线程数18，详情见线程初始化
	std::mutex SP_mtx[32];							//锁匹配资源数
	std::condition_variable SP_cv[32];				//条件变量数对应资源数
	int current_ptr;								//目前指针指向
	int exiting_number;								//已存在多少个对象

	void VB_Show();									//数据传出
	void VB_Color();								//颜色渲染
	void OP_Find();									//OP区FIND命令
	void OP_Save();									//OP区FIND命令
	void OP_Insert();								//OP区Insert命令
	void OP_Modify();								//OP区Modify命令
	void OP_Delete();								//OP区Delete命令
	void OP_SubDelete();							//OP区Delete子命令
	void OP_SubModify();							//OP区Modify命令

	void ThreadInit();								//线程初始化
	void ReInit();									//重新显示所有对象
};

