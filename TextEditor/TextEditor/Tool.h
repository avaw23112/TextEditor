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

//�ַ�ת����
class Converter {
public:
	LPCWSTR Convert(const std::string& s);
	// ȷ���ڶ����������ڽ���ǰ��Ҫ�ͷ� wideString
	std::wstring wideString;
	// ���������в���Ҫ�ͷ� wideString����Ϊ�����Զ��ͷ�
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
// std::tuple_size<Tuple>(value) > 0����Ƿ�Ϊ�գ���Ϊ�ղ�ʹ��

#define GET_QUEUE(parameter,QUEUE_NAME) \
	if (!TQ_only->QUEUE_NAME.empty()) {\
		parameter element = front_element<parameter>(TQ_only->QUEUE_NAME); \
		return element;\
	}\
	return TUPLE_NULL;

//�������,��Ӧ���ţ�
enum TASK {
	DC_MODIFY = 0,				//DC�������������Ҫ��������Ⱦ����Ӧ��ɫ
	DC_SYNC = 1,				//DC����LB����ͬ��
	DC_SAVE = 2,				//DC�������ݴ���RF��
	DC_KEEP = 3,				//��RF��������ת��DC�������˹���ʱ��������
	VB_SHOW = 4,				//��DC����������VB��ȫ����ʾ��1��4���޾����ģ����ϸ���̣߳�
	LB_SYNC = 5,				//LB����DC����ͬ��
	OP_FIND = 6,				//OP���Ĳ���ָ��
	OP_SAVE = 7,				//OP���ı���ָ��
	VB_CMAP = 8,				//VB���Ķ�LB���Ĺ��ӳ��
	OP_INSERT = 9,				//OP���Ĳ���ָ��
	OP_DELETE = 10,				//OP����ɾ��ָ��
	OP_MODIFY = 11,				//OP�����޸�ָ��
	INSERT_S = 12,				//OP��������INS
	DELETE_S = 13,				//OP��������DEL
	MODIFY_S = 14,				//OP��������MOD
	ONCE_DEL = 15				//VB��������ɾ
};

//����ģʽʵ�ֶ�����������з���
//����ģʽ�������������������ֻ��һ������
//ֻ��Ҫ��װ����������м���
class TaskQueue {
public:
	//�ӿڲ���
	static TaskQueue* TQ_GetTaskQueue();
	static void TQ_DeTaskQueue();

	//���ܲ���
	//��Ҫ�ǻ�ȡ����Ķ�̬��Ϊ,switch����������ʵ��
	//��������VB_CMAP������
	static void TQ_GetTask(TASK task_name, int object_num);																//����1~5,10~11������
	static void TQ_GetTask(TASK task_name, int object_num,															    //����0������
		std::vector<std::tuple<int, int, int>>);
	static void TQ_GetTask(TASK task_name, int object_num,															    //����6������
		int cur_row, int cur_col, std::string ANS, char temp);
	static void TQ_GetTask(TASK task_name, int object_num, int row, int col, std::string insert_s);						//����7~9������
	static void TQ_GetTask(TASK task_name, int object_num, DELETE_E temp_e);//��ʱ���ã�����ɾ�ֽ�
	static void TQ_GetTask(TASK task_name, int object_num, std::string delete_s, int number);
	static void TQ_GetTask(TASK task_name, int object_num, std::string original_s, std::string replace_s);
	static void TQ_GetTask(TASK task_name, int object_num, std::string pattern);

	//�Լ��Ƴ�����Ķ�̬��Ϊ,����ָ����ģ�尲ȫ��
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
	//1.���ʱû�ѱ����ӣ�����������Ҳ������
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
	//����ģʽ����ֹ�ⲿ����
	TaskQueue();
	~TaskQueue();

	//˽�л���ֹ�ⲿ�����͸�ֵ
	TaskQueue(const TaskQueue& TQ_copy);
	const TaskQueue& operator=(const TaskQueue& TQ_equal);

	//�������
	std::queue<std::pair<int,																			//���������
		std::vector<std::tuple<int, int, int>>>> DC_ModifyQueue;										//row,col_be,col_end
	std::queue<int> DC_SyncQueue;																		//���������
	std::queue<int> DC_SaveQueue;																		//...
	std::queue<int> DC_KeepQueue;																		//...
	std::queue<int> DC_ShowQueue;																		//...
	std::queue<int> LB_SyncQueue;																		//...
	std::queue<int> OP_SaveQueue;																		//...
	std::queue<std::tuple<int, std::string>> OP_FindQueue;																		//...
	std::queue<std::tuple<int, int, int, std::string, char>> VB_CmapQueue;								//�ݶ������У���ţ�����У��У�ת���ַ����ַ�
	std::queue<DELETE_E> DC_OnceDELQueue;

	//��ˮ�ߵ����㷨�����ں���Ĳ��ֵ���
	//��Ϊ�ṹ���ʣ����Բ���̰���㷨���

	//ǰ������ȡ���������KMP�Լ�һЩ���ͺ�õ�������Ҫ������
	//ǰ������Ҫ���ǽ���Kmp�Ĳ���������������м���Ԫ��
	std::queue<std::tuple<int, int, int, std::string>>		OP_InsertQueue;								//��ţ��У��У��ַ���
	std::queue<std::tuple<int, std::string, int>>			OP_DeleteQueue;								//��ţ�Ҫɾ�����ַ���
	std::queue<std::tuple<int, std::string, std::string>>	OP_ModifyQueue;								//��ţ�Ҫ�޸ĵ��ַ������滻�ִ�

	//������ǰ�����ȡ����ӦM1��M2ʱ��������ˮ���ȣ��������߳̽��к���
	//������Ҫ����ǰ����õ���Ԫ�ض��У��Լ�m1,m2ʱ��

	//Operator�����дָ�ϣ������񼯣�����������������������

	//Task�ǵ���ģʽ���������ظ��ȸ�
	template <typename T>
	struct CMP_N1 {
		bool operator()(T a1, T  a2)
		{
			//��N1��a���е�������
			return std::get<1>(a1) < std::get<1>(a2);
		}
	};
	template <typename T>
	struct CMP_N2 {
		bool operator()(T b1, T  b2)
		{
			//��N2��B���еݼ�����
			return std::get<2>(b1) >= std::get<2>(b2);
		}
	};

	template <typename T>
	static T get_element(std::priority_queue<T, std::vector<T>, CMP_N1<T> >& N1, std::priority_queue<T, std::vector<T>, CMP_N2<T>>& N2)
	{
		//ͨ����Χ������Խ���ȡ
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

	//���ȶ���ʽд������KMP�ʱ���������ȶ��У����Ҹ�����ˮ�ߵ����㷨��Ҫ������
	//��KMP����������������Y�����ȳ���N1��Ԫ�أ��ٳ���N2��Ԫ��
	//����������N����������ȶ���
	//ÿ������vec��������vec,N1,N2,�ֱ��a<b�ģ�a>=b��

	//insert������������׷����ʼλ�ã�string�ǲ������
	std::priority_queue<INSERT_NEED, std::vector<INSERT_NEED>, CMP_N1<INSERT_NEED>>  OP_IV_N1;
	std::priority_queue<INSERT_NEED, std::vector<INSERT_NEED>, CMP_N2<INSERT_NEED>>  OP_IV_N2;

	//delete��һǰһ���������ɾ���м䲿��
	std::priority_queue <DELETE_NEED, std::vector<DELETE_NEED>, CMP_N1<DELETE_NEED>>  OP_DV_N1;
	std::priority_queue <DELETE_NEED, std::vector<DELETE_NEED>, CMP_N2<DELETE_NEED>> OP_DV_N2;

	//modify��1.һǰһ���������ɾ���м䲿�֣��ٸ��ݺ������һ�������루���ܲͨ�ã�
	//		  2.kmpʱ�ͱȽ�ԭ�����洮�ĳ��Ȳ�...(���Ӹ��Ӷȣ��Ȳ���)
	std::priority_queue <MODIFY_NEED, std::vector<MODIFY_NEED>, CMP_N1<MODIFY_NEED>>  OP_MV_N1;
	std::priority_queue <MODIFY_NEED, std::vector<MODIFY_NEED>, CMP_N2<MODIFY_NEED>> OP_MV_N2;

	//mutex���л�����
	std::mutex MUTEX[16];			//15������ÿ���������Ӧ������
	std::condition_variable TQ_cv[16];

	//�����ľ������static����̬ʹ�ö���ָ��Ψһ
	static TaskQueue* TQ_only;
};

class SWITCH {
private:
	static SWITCH* ST_only;
	//����ģʽ����ֹ�ⲿ����
	SWITCH() {};
	~SWITCH() {};

	//˽�л���ֹ�ⲿ�����͸�ֵ
	SWITCH(const SWITCH& TQ_copy);
	const SWITCH& operator=(const SWITCH& TQ_equal);

	std::queue<size_t> ready_q;
	std::mutex mtx;
	std::mutex mtx_q;
	std::atomic<bool> LOCK = 0;
	std::atomic<bool> Option = 0;

	//���ظ��������Դ�������͹ر�
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
	//ͨ��һ��������Դ�����̼߳������Ϊ
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
	//�߳�ͨ���ȴ���Դ������(�������������ܱ��ֿ���)
	//ͨ��һ���������м�¼��������
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
