#pragma once
#ifndef TOOL
#define TOOL

#include <cstring>
#include <string>
#include <locale>
#include <codecvt>
#include <thread>
#include <functional> 
#include <windows.h>
#include <queue>
#include <mutex>
#include <algorithm>
#include <condition_variable>
#include <sstream>
#include <vector>
#include <iostream>
#include <array>
#include <atomic>

//字符转换器
class Converter {
public:
	LPCWSTR Convert(const std::string& s);
	// 确保在对象生命周期结束前不要释放 wideString
	std::wstring wideString;
	// 析构函数中不需要释放 wideString，因为它会自动释放
};
template <typename T>
static T front_element(std::queue<T>& q)
{
	T temp = std::move(q.front());
	q.pop();
	return temp;
}
#define DC_SYNC_NEED int
#define DC_SAVE_NEED int
#define DC_KEEP_NEED int
#define DC_SHOW_NEED int
#define LB_SYNC_NEED int
#define OP_SAVE_NEED int
#define DC_MODIFY_NEED std::pair<int,std::vector<std::tuple<int, int, int>>>
#define VB_CMAP_NEED std::tuple<int, int, int, std::string, char>
#define OP_INSERT_NEED std::tuple<int, int, int, std::string>
#define OP_DELETE_NEED std::tuple<int, std::string,int>
#define OP_MODIFY_NEED std::tuple<int, std::string, std::string>
#define OP_FIND_NEED std::tuple<int, std::string>
#define INSERT_NEED std::tuple<std::queue<std::tuple<int,int>>,int,std::string, int, int>
#define DELETE_NEED std::tuple<std::queue<std::tuple<int,int,int>>,int, int, int>
#define MODIFY_NEED std::tuple<std::queue<std::tuple<int,int,int>>,int, std::string, int, int>
#define INSERT_QE std::queue<std::tuple<int,int>>
#define DELETE_QE std::queue<std::tuple<int,int,int>>
#define MODIFY_QE std::queue<std::tuple<int,int,int>>
#define INSERT_E std::tuple<int,int>
#define DELETE_E std::tuple<int,int,int>
#define MODIFY_E std::tuple<int,int,int>													//row,col_be,col_end

#define 	TUPLE_NULL  {}
// std::tuple_size<Tuple>(value) > 0检查是否为空，不为空才使用

#define GET_QUEUE(parameter,QUEUE_NAME) \
	if (!TQ_only->QUEUE_NAME.empty()) {\
		parameter element = front_element<parameter>(TQ_only->QUEUE_NAME); \
		return element;\
	}\
	return TUPLE_NULL;

//任务分类,对应锁号！
enum TASK {
	DC_MODIFY = 0,				//DC区接受命令后，需要在主区渲染出对应颜色
	DC_SYNC = 1,				//DC区对LB区的同步
	DC_SAVE = 2,				//DC区的数据存入RF区
	DC_KEEP = 3,				//将RF区的数据转入DC区，除了构造时几乎不用
	VB_SHOW = 4,				//将DC区的数据在VB区全部显示，1和4是无竞争的，如何细化线程？
	LB_SYNC = 5,				//LB区对DC区的同步
	OP_FIND = 6,				//OP区的查找指令
	OP_SAVE = 7,				//OP区的保存指令
	VB_CMAP = 8,				//VB区的对LB区的光标映射
	OP_INSERT = 9,				//OP区的插入指令
	OP_DELETE = 10,				//OP区的删除指令
	OP_MODIFY = 11,				//OP区的修改指令
	INSERT_S = 12,				//OP区子命令INS
	DELETE_S = 13,				//OP区子命令DEL
	MODIFY_S = 14,				//OP区子命令MOD
	ONCE_DEL = 15				//VB区操作，删
};

//单例模式实现多对象的任务队列泛化
//单例模式：这个类在整个程序中只有一个对象
//只需要封装锁和任务队列即可
class TaskQueue {
public:
	//接口部分
	static TaskQueue* TQ_GetTaskQueue();
	static void TQ_DeTaskQueue();

	//功能部分
	//主要是获取任务的多态行为,switch配合任务分类实现
	//可能少了VB_CMAP的重载
	static void TQ_GetTask(TASK task_name, int object_num);																//处理1~5,10~11号任务
	static void TQ_GetTask(TASK task_name, int object_num,															    //处理0号任务
		std::vector<std::tuple<int, int, int>>);
	static void TQ_GetTask(TASK task_name, int object_num,															    //处理6号任务
		int cur_row, int cur_col, std::string ANS, char temp);
	static void TQ_GetTask(TASK task_name, int object_num, int row, int col, std::string insert_s);						//处理7~9号任务
	static void TQ_GetTask(TASK task_name, int object_num, DELETE_E temp_e);//暂时弃用，单独删字节
	static void TQ_GetTask(TASK task_name, int object_num, std::string delete_s, int number);
	static void TQ_GetTask(TASK task_name, int object_num, std::string original_s, std::string replace_s);
	static void TQ_GetTask(TASK task_name, int object_num, std::string pattern);

	//以及推出任务的多态行为,智能指针让模板安全化
	template <typename T>
	static T TQ_GiveTask() { return 0; }
	template <typename T>
	static T TQ_GiveTask(TASK task_name) { return 0; }
	template <>
	static DC_MODIFY_NEED TQ_GiveTask(TASK task_name) { GET_QUEUE(DC_MODIFY_NEED, DC_ModifyQueue); }
	template <>
	static VB_CMAP_NEED  TQ_GiveTask(TASK task_name) { GET_QUEUE(VB_CMAP_NEED, VB_CmapQueue); }
	template <>
	static OP_INSERT_NEED TQ_GiveTask(TASK task_name) { GET_QUEUE(OP_INSERT_NEED, OP_InsertQueue); }
	template <>
	static OP_DELETE_NEED TQ_GiveTask(TASK task_name) { GET_QUEUE(OP_DELETE_NEED, OP_DeleteQueue); }
	template <>
	static OP_MODIFY_NEED TQ_GiveTask(TASK task_name) { GET_QUEUE(OP_MODIFY_NEED, OP_ModifyQueue); }
	template <>
	static OP_FIND_NEED TQ_GiveTask(TASK task_name) { GET_QUEUE(OP_FIND_NEED, OP_FindQueue); }
	template <>
	static DELETE_E TQ_GiveTask(TASK task_name) { GET_QUEUE(DELETE_E, DC_OnceDELQueue); }

	template <>
	static int TQ_GiveTask(TASK task_name) {
		switch (task_name)
		{
		case DC_SYNC: {GET_QUEUE(int, DC_SyncQueue); }; break;
		case DC_SAVE: {GET_QUEUE(int, DC_SaveQueue); }; break;
		case DC_KEEP: {GET_QUEUE(int, DC_KeepQueue); }; break;
		case VB_SHOW: {GET_QUEUE(int, DC_ShowQueue); }; break;
		case LB_SYNC: {GET_QUEUE(int, LB_SyncQueue); }; break;
		case OP_SAVE: {GET_QUEUE(int, OP_SaveQueue); }; break;
		default:perror("give error task!!"); return 0;
		}
	}
	//1.入队时没把编号入队，导致排序后找不到编号
	static void TQ_InSubQueue(std::queue<std::tuple<int, int>>, int,std::string, int m1, int m2);
	static void TQ_InSubQueue(std::queue<std::tuple<int, int, int>>, int object_num, int m1, int m2);
	static void TQ_InSubQueue(std::queue<std::tuple<int, int, int>>,int, std::string, int m1, int m2);


	template <typename T>
	static T TQ_OutSubQueue(int& object_num) {}
	template <typename T>
	static T TQ_OutSubQueue(std::string& s,int& object_num) {}
	template <>
	static INSERT_QE TQ_OutSubQueue(std::string& insert_s,int& object_num) {
		INSERT_NEED queue = get_element<INSERT_NEED>(TQ_only->OP_IV_N1, TQ_only->OP_IV_N2);
		object_num = std::get<1>(queue);
		insert_s = std::get<2>(queue);
		return std::get<0>(queue);
	}
	template <>
	static DELETE_QE TQ_OutSubQueue(int& object_num) {
		DELETE_NEED queue = get_element<DELETE_NEED>(TQ_only->OP_DV_N1, TQ_only->OP_DV_N2);
		object_num = std::get<1>(queue);
		return std::get<0>(queue);
	}
	template <>
	static MODIFY_QE TQ_OutSubQueue(std::string& modify_s, int& object_num) {
		MODIFY_NEED queue = get_element<MODIFY_NEED>(TQ_only->OP_MV_N1, TQ_only->OP_MV_N2);
		object_num = std::get<1>(queue);
		modify_s = std::get<2>(queue);
		return std::get<0>(queue);
	}

	static void TQ_ClearSubQueue(TASK task_name);
	static size_t TQ_SubQueueSize(TASK task_name);

	static bool empty(TASK);
	static std::mutex& TQ_GiveLock(TASK);
	static std::condition_variable& TQ_GiveCV(TASK);
	static void ClearColor();
	static void QueueCopy(std::queue<DC_MODIFY_NEED> queue);
private:
	//单例模式将禁止外部构造
	TaskQueue();
	~TaskQueue();

	//私有化禁止外部拷贝和赋值
	TaskQueue(const TaskQueue& TQ_copy);
	const TaskQueue& operator=(const TaskQueue& TQ_equal);

	//任务分类
	std::queue<std::pair<int,																			//处理对象编号
		std::vector<std::tuple<int, int, int>>>> DC_ModifyQueue;										//row,col_be,col_end
	std::queue<int> DC_SyncQueue;																		//处理对象编号
	std::queue<int> DC_SaveQueue;																		//...
	std::queue<int> DC_KeepQueue;																		//...
	std::queue<int> DC_ShowQueue;																		//...
	std::queue<int> LB_SyncQueue;																		//...
	std::queue<int> OP_SaveQueue;																		//...
	std::queue<std::tuple<int, std::string>> OP_FindQueue;																		//...
	std::queue<std::tuple<int, int, int, std::string, char>> VB_CmapQueue;								//暂定参数有：编号，光标行，列，转义字符，字符
	std::queue<DELETE_E> DC_OnceDELQueue;

	//流水线调度算法：关于后处理的部分调度
	//因为结构合适，所以采用贪心算法解决

	//前处理：先取得任务进行KMP以及一些传送后得到后处理需要的序列
	//前处理需要的是进行Kmp的参数，并向后处理序列加入元素
	std::queue<std::tuple<int, int, int, std::string>>		OP_InsertQueue;								//编号，行，列，字符串
	std::queue<std::tuple<int, std::string, int>>			OP_DeleteQueue;								//编号，要删除的字符串
	std::queue<std::tuple<int, std::string, std::string>>	OP_ModifyQueue;								//编号，要修改的字符串，替换字串

	//后处理：在前处理获取到对应M1，M2时间后进行流水调度，再启动线程进行后处理
	//后处理需要的是前处理得到的元素队列，以及m1,m2时间

	//Operator命令编写指南（子任务集）：↓↓↓↓↓↓↓↓↓↓

	//Task是单例模式，不用怕重复度高
	template <typename T>
	struct CMP_N1 {
		bool operator()(T a1, T  a2)
		{
			//对N1的a进行递增排序
			return std::get<1>(a1) < std::get<1>(a2);
		}
	};
	template <typename T>
	struct CMP_N2 {
		bool operator()(T b1, T  b2)
		{
			//对N2的B进行递减排序
			return std::get<2>(b1) >= std::get<2>(b2);
		}
	};

	template <typename T>
	static T get_element(std::priority_queue<T, std::vector<T>, CMP_N1<T> >& N1, std::priority_queue<T, std::vector<T>, CMP_N2<T>>& N2)
	{
		//通过外围来控制越界读取
		if (!N1.empty()) {
			T temp = N1.top();
			N1.pop();
			return temp;
		}
		else if (!N2.empty())
		{
			T temp = N2.top();
			N2.pop();
			return temp;
		}
		else
			return TUPLE_NULL;
	}

	//优先队列式写法：在KMP活动时，插入优先队列，并且根据流水线调度算法的要求排序
	//在KMP结束后，如果输入的是Y：则先出对N1的元素，再出队N2的元素
	//如果输入的是N：则清空优先队列
	//每个功能vec连带两个vec,N1,N2,分别存a<b的，a>=b的

	//insert：迭代器用于追踪起始位置，string是插入对象
	std::priority_queue<INSERT_NEED, std::vector<INSERT_NEED>, CMP_N1<INSERT_NEED>>  OP_IV_N1;
	std::priority_queue<INSERT_NEED, std::vector<INSERT_NEED>, CMP_N2<INSERT_NEED>>  OP_IV_N2;

	//delete：一前一后迭代器，删除中间部分
	std::priority_queue <DELETE_NEED, std::vector<DELETE_NEED>, CMP_N1<DELETE_NEED>>  OP_DV_N1;
	std::priority_queue <DELETE_NEED, std::vector<DELETE_NEED>, CMP_N2<DELETE_NEED>> OP_DV_N2;

	//modify：1.一前一后迭代器，删除中间部分，再根据后迭代器一个个插入（性能差，通用）
	//		  2.kmp时就比较原串和替串的长度差...(增加复杂度，先不了)
	std::priority_queue <MODIFY_NEED, std::vector<MODIFY_NEED>, CMP_N1<MODIFY_NEED>>  OP_MV_N1;
	std::priority_queue <MODIFY_NEED, std::vector<MODIFY_NEED>, CMP_N2<MODIFY_NEED>> OP_MV_N2;

	//mutex队列互斥锁
	std::mutex MUTEX[16];			//15个队列每个都分配对应互斥锁
	std::condition_variable TQ_cv[16];

	//单例的精髓就是static，静态使得对象指针唯一
	static TaskQueue* TQ_only;
};

class SWITCH {
private:
	static SWITCH* ST_only;
	//单例模式将禁止外部构造
	SWITCH() {};
	~SWITCH() {};

	//私有化禁止外部拷贝和赋值
	SWITCH(const SWITCH& TQ_copy);
	const SWITCH& operator=(const SWITCH& TQ_equal);

	std::queue<size_t> ready_q;
	std::mutex mtx;
	std::mutex mtx_q;
	std::atomic<bool> LOCK = 0;
	std::atomic<bool> Option = 0;

	//开关负责这个资源的启动和关闭
	static void OPEN() {
		std::unique_lock<std::mutex> lk(ST_only->mtx);
		ST_only->LOCK = true;
	}

public:
	static void CLOSE() {
		while (true) {
			std::unique_lock<std::mutex> lk(ST_only->mtx_q);
			if (ST_only->ready_q.empty()) {
				ST_only->LOCK = false;
				break;
			}
		}
	}
	//通过一个共享资源控制线程集体的行为
	static void AGREE()
	{
		ST_only->Option = true;
		ST_only->OPEN();
		ST_only->CLOSE();
		ST_only->Option = false;
	}
	static void DISAGREE()
	{
		ST_only->Option = false;
		ST_only->OPEN();
		ST_only->CLOSE();
	}
	//线程通过等待资源而自旋(必须是自旋才能保持控制)
	//通过一个就绪队列记录任务数量
	static bool WAIT() {
		std::unique_lock<std::mutex> lk(ST_only->mtx_q);
		ST_only->ready_q.push(0);
		lk.unlock();
		while (true) {
			std::unique_lock<std::mutex> lk(ST_only->mtx);
			if (ST_only->LOCK) {
				break;
			}
		}
		return ST_only->Option;
	}
	static void pop() {
		std::unique_lock<std::mutex> lk(ST_only->mtx_q);
		ST_only->ready_q.pop();
	}
};

#endif
