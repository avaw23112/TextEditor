#pragma once
#include "Tool.h"
#include "VirtualBuffer.h"

//����������̲߳����Լ����̲߳���
//�����������ʾ�ؼ�
//�л�����

//���ƶ���
class Superviser
{
public:
	Superviser();
	~Superviser();
	//����
	int SP_Console();								//�����������
	int SP_ConsoleInit();
	int SP_ComTranslate(SET_OF_COMMEND COM, std::string commend);
	void SP_EXIT();


private:
	//����̨����
	HANDLE hStdIn;									//����̨���
	DWORD dwMode;									//����̨ģʽ

	//�̹߳���
	VirtualBuffer* SP_VB_object[32];				//���֧��ͬʱ��32���ļ�����������ʾ���ܵ�ֻ��4�ݣ�������ʩ��û�ӣ���ô�����ǰ4������
	std::thread SP_Thread[18];						//�߳���18��������̳߳�ʼ��
	std::mutex SP_mtx[32];							//��ƥ����Դ��
	std::condition_variable SP_cv[32];				//������������Ӧ��Դ��
	int current_ptr;								//Ŀǰָ��ָ��
	int exiting_number;								//�Ѵ��ڶ��ٸ�����

	void VB_Show();									//���ݴ���
	void VB_Color();								//��ɫ��Ⱦ
	void OP_Find();									//OP��FIND����
	void OP_Save();									//OP��FIND����
	void OP_Insert();								//OP��Insert����
	void OP_Modify();								//OP��Modify����
	void OP_Delete();								//OP��Delete����
	void OP_SubDelete();							//OP��Delete������
	void OP_SubModify();							//OP��Modify����

	void ThreadInit();								//�̳߳�ʼ��
	void ReInit();									//������ʾ���ж���
};

