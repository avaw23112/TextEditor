#include "Tool.h"

//将string转化为LPCWSTR类
LPCWSTR Converter::Convert(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	wideString = converter.from_bytes(s);
	//c_str返回的是指针而非wstring对象，函数结束后指针悬浮，函数出错
	return wideString.data();
}

//创建单例
TaskQueue* TaskQueue::TQ_only = new TaskQueue();
SWITCH* SWITCH::ST_only = new SWITCH();

TaskQueue* TaskQueue::TQ_GetTaskQueue()
{
	//如果没有对象则创建一个
	if (TQ_only == nullptr)
	{
		TQ_only = new TaskQueue();
	}
	return TQ_only;
}
void TaskQueue::TQ_DeTaskQueue()
{
	if (TQ_only != nullptr) {
		delete TQ_only;
		TQ_only = nullptr;
	}
}

#define QUEUE_INIT(QUEUE_NAME) QUEUE_NAME##1(),QUEUE_NAME##2()

TaskQueue::TaskQueue() :DC_ModifyQueue(), DC_SyncQueue(),
DC_SaveQueue(), DC_KeepQueue(), DC_ShowQueue(), LB_SyncQueue(),
VB_CmapQueue(), OP_InsertQueue(), OP_DeleteQueue(), OP_ModifyQueue(),
QUEUE_INIT(OP_IV_N), QUEUE_INIT(OP_DV_N), QUEUE_INIT(OP_MV_N) {}

TaskQueue::~TaskQueue() { TQ_only = nullptr; }

#define PUSH_QUEUE1(MUTEX_NUM,QUEUE_NAME) std::unique_lock<std::mutex> mtx(TQ_only->MUTEX[MUTEX_NUM]);\
										  TQ_only->QUEUE_NAME.push(object_num)

void TaskQueue::TQ_GetTask(TASK task_name, int object_num)
{
	switch (task_name)
	{
	case DC_SYNC: {PUSH_QUEUE1(1, DC_SyncQueue); }; break;
	case DC_SAVE: {PUSH_QUEUE1(2, DC_SaveQueue); }; break;
	case DC_KEEP: {PUSH_QUEUE1(3, DC_KeepQueue); }; break;
	case VB_SHOW: {PUSH_QUEUE1(4, DC_ShowQueue); }; break;
	case LB_SYNC: {PUSH_QUEUE1(5, LB_SyncQueue); }; break;
	case OP_SAVE: {PUSH_QUEUE1(7, OP_SaveQueue); }; break;
	default:perror("1~7 get error task!!");
	}
}																	    //处理1~5,10~11号任务
#define PUSH_QUEUE(QUEUE_NAME,TASK_NAME,TASK_ODINARY,...) 	if (TASK_ODINARY == TASK_NAME)\
															TQ_only->QUEUE_NAME.push(__VA_ARGS__);\
															else perror("get error task!!")

void TaskQueue::TQ_GetTask(TASK task_name, int object_num,															    //处理0号任务
	std::vector<std::tuple<int, int, int>> DC_ModifyNeed) {
	PUSH_QUEUE(DC_ModifyQueue, task_name, DC_MODIFY, std::make_pair(object_num, DC_ModifyNeed));
}
void TaskQueue::TQ_GetTask(TASK task_name, int object_num,															    //处理6号任务
	int cur_row, int cur_col, std::string ANS, char temp) {
	PUSH_QUEUE(VB_CmapQueue, task_name, VB_CMAP, { object_num,cur_row,cur_col,ANS,temp });
}
void TaskQueue::TQ_GetTask(TASK task_name, int object_num, int row, int col, std::string insert_s) {					//处理7~9号任务
	PUSH_QUEUE(OP_InsertQueue, task_name, OP_INSERT, { object_num,row,col,insert_s });
}
void TaskQueue::TQ_GetTask(TASK task_name, int object_num, DELETE_E temp_e) {
	PUSH_QUEUE(DC_OnceDELQueue, task_name, ONCE_DEL, temp_e);
}
void TaskQueue::TQ_GetTask(TASK task_name, int object_num, std::string delete_s, int number) {
	PUSH_QUEUE(OP_DeleteQueue, task_name, OP_DELETE, { object_num,delete_s,number });
}
void TaskQueue::TQ_GetTask(TASK task_name, int object_num, std::string original_s, std::string replace_s) {
	PUSH_QUEUE(OP_ModifyQueue, task_name, OP_MODIFY, { object_num,original_s,replace_s });
}
void TaskQueue::TQ_GetTask(TASK task_name, int object_num, std::string pattern) {
	PUSH_QUEUE(OP_FindQueue, task_name, OP_FIND, { object_num,pattern });
}

//如果给的参数无法接受，必然直接爆，所以就不管错误提示了
//如果得到锁后，发现队列中已经没有元素，将报错，加一个检测即可
//直接使用模板会导致空指针传出错误的问题,智能指针重构即可

void TaskQueue::TQ_InSubQueue(std::queue<std::tuple<int, int>> temp,int object_num, std::string insert_s, int m1, int m2)
{
	if (m1 <= m2) TQ_only->OP_IV_N1.push({ temp, object_num,insert_s,m1,m2 });
	else TQ_only->OP_IV_N2.push({ temp,object_num,insert_s,m1,m2 });
}
void TaskQueue::TQ_InSubQueue(std::queue<std::tuple<int, int, int>> temp,int object_num, int m1, int m2)
{
	if (m1 <= m2) TQ_only->OP_DV_N1.push({ temp,object_num,m1,m2 });
	else TQ_only->OP_DV_N2.push({ temp,object_num,m1,m2 });
}
void TaskQueue::TQ_InSubQueue(std::queue<std::tuple<int, int, int>> temp,int object_num, std::string modify_s, int m1, int m2)
{
	if (m1 <= m2) TQ_only->OP_MV_N1.push({ temp, object_num,modify_s,m1,m2 });
	else TQ_only->OP_MV_N2.push({ temp,object_num,modify_s,m1,m2 });
}

#define CLEAR_ALL(QUEUE_NAME) 		while(!TQ_only->QUEUE_NAME##1.empty())\
									TQ_only->QUEUE_NAME##1.pop();\
									while (!TQ_only->QUEUE_NAME##2.empty())\
								TQ_only->QUEUE_NAME##2.pop()

void TaskQueue::TQ_ClearSubQueue(TASK task_name) {
	switch (task_name)
	{
	case INSERT_S: {
		CLEAR_ALL( OP_IV_N);
	}; break;
	case DELETE_S: {
		CLEAR_ALL( OP_DV_N);
	}; break;
	case MODIFY_S: {
		CLEAR_ALL( OP_MV_N);
	}; break;
	default:perror("Clear error!!"); break;
	}
}

size_t TaskQueue::TQ_SubQueueSize(TASK task_name)
{
	switch (task_name)
	{
	case INSERT_S: {
		return TQ_only->OP_IV_N1.size() + TQ_only->OP_IV_N2.size();
	}; break;
	case DELETE_S: {
		return TQ_only->OP_DV_N1.size() + TQ_only->OP_DV_N2.size();
	}; break;
	case MODIFY_S: {
		return TQ_only->OP_MV_N1.size() + TQ_only->OP_MV_N2.size();
	}; break;
	default: perror("Get size error!!"); break;
	}
	return 0;
}

#define QUEUE_EMPTY(QUEUE_NAME) return TQ_only->QUEUE_NAME.empty();

bool TaskQueue::empty(TASK task)
{
	switch (task)
	{
	case DC_MODIFY: {QUEUE_EMPTY(DC_ModifyQueue)}; break;
	case DC_SYNC: {QUEUE_EMPTY(DC_SyncQueue)}; break;
	case DC_SAVE: {QUEUE_EMPTY(DC_SaveQueue)}; break;
	case DC_KEEP: {QUEUE_EMPTY(DC_KeepQueue)}; break;
	case VB_SHOW: {QUEUE_EMPTY(DC_ShowQueue)}; break;
	case LB_SYNC: {QUEUE_EMPTY(LB_SyncQueue)}; break;
	case OP_FIND: {QUEUE_EMPTY(OP_FindQueue)}; break;
	case OP_SAVE: {QUEUE_EMPTY(OP_SaveQueue)}; break;
	case VB_CMAP: {QUEUE_EMPTY(VB_CmapQueue)}; break;
	case OP_INSERT: {QUEUE_EMPTY(OP_InsertQueue)}; break;
	case OP_DELETE: {QUEUE_EMPTY(OP_DeleteQueue)}; break;
	case OP_MODIFY: {QUEUE_EMPTY(OP_ModifyQueue)}; break;
	case INSERT_S:{
		if (TQ_only->OP_IV_N1.empty() && TQ_only->OP_IV_N2.empty())
			return true;
	}; break;
	case DELETE_S:{
		if (TQ_only->OP_DV_N1.empty() && TQ_only->OP_DV_N2.empty())
			return true;
	}; break;
	case MODIFY_S:{
		if (TQ_only->OP_MV_N1.empty() && TQ_only->OP_MV_N2.empty())
			return true;
	}; break;
	default: return false;
	}
}

std::mutex& TaskQueue::TQ_GiveLock(TASK task)
{
	switch (task)
	{
	case DC_MODIFY: {return TQ_only->MUTEX[0]; }break;
	case DC_SYNC: {return TQ_only->MUTEX[1]; }break;
	case DC_SAVE: {return TQ_only->MUTEX[2]; }break;
	case DC_KEEP: {return TQ_only->MUTEX[3]; }break;
	case VB_SHOW: {return TQ_only->MUTEX[4]; }break;
	case LB_SYNC: {return TQ_only->MUTEX[5]; }break;
	case OP_FIND: {return TQ_only->MUTEX[6]; }break;
	case OP_SAVE: {return TQ_only->MUTEX[7]; }break;
	case VB_CMAP: {return TQ_only->MUTEX[8]; }break;
	case OP_INSERT: {return TQ_only->MUTEX[9]; }break;
	case OP_DELETE: {return TQ_only->MUTEX[10]; }break;
	case OP_MODIFY: {return TQ_only->MUTEX[11]; }break;
	case INSERT_S: {return TQ_only->MUTEX[12]; }break;
	case DELETE_S: {return TQ_only->MUTEX[13]; }break;
	case MODIFY_S: {return TQ_only->MUTEX[14]; }break;
	default: {return TQ_only->MUTEX[15]; }break;
	}
	return TQ_only->MUTEX[15];
}

std::condition_variable& TaskQueue::TQ_GiveCV(TASK task)
{
	switch (task)
	{
	case DC_MODIFY: {return TQ_only->TQ_cv[0]; }break;
	case DC_SYNC: {return TQ_only->TQ_cv[1]; }break;
	case DC_SAVE: {return TQ_only->TQ_cv[2]; }break;
	case DC_KEEP: {return TQ_only->TQ_cv[3]; }break;
	case VB_SHOW: {return TQ_only->TQ_cv[4]; }break;
	case LB_SYNC: {return TQ_only->TQ_cv[5]; }break;
	case OP_FIND: {return TQ_only->TQ_cv[6]; }break;
	case OP_SAVE: {return TQ_only->TQ_cv[7]; }break;
	case VB_CMAP: {return TQ_only->TQ_cv[8]; }break;
	case OP_INSERT: {return TQ_only->TQ_cv[9]; }break;
	case OP_DELETE: {return TQ_only->TQ_cv[10]; }break;
	case OP_MODIFY: {return TQ_only->TQ_cv[11]; }break;
	case INSERT_S: {return TQ_only->TQ_cv[12]; }break;
	case DELETE_S: {return TQ_only->TQ_cv[13]; }break;
	case MODIFY_S: {return TQ_only->TQ_cv[14]; }break;
	default: {return TQ_only->TQ_cv[15]; }break;
	}
}

//置空颜色
void TaskQueue::ClearColor()
{
	std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(DC_MODIFY));
	while (!TaskQueue::empty(DC_MODIFY))
	{
		TQ_only->DC_ModifyQueue.pop();
	}
}

void TaskQueue::QueueCopy(std::queue<DC_MODIFY_NEED> queue)
{
	if (TQ_only->DC_ModifyQueue.empty())
	{
		TQ_only->DC_ModifyQueue = queue;
	}
}
