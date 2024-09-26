#pragma once
#ifndef OP
#define OP

#include "Tool.h"
//1.����﷨��⹦��(�﷨����⣬�����㷨)
//OPEN   <string file_name>
//DELETE <int ob_num> <string delete_string> <int delelte_num>
//FIND   <int ob_num> <string find_string>
//INSERT <int ob_num> <int row> <int col> <string insert_string>
//MODIFY <int ob_num> <string original_string> <string replace_string>
//CLOSE	 ��������

//ע���﷨��
enum SET_OF_COMMEND
{
	CO_OPEN = 1,
	CO_DELETE = 2,
	CO_INSERT = 3,
	CO_MODIFY = 4,
	CO_FIND = 5,
	//File_name = 5,
	STRING = 6,
	INT_M = 7,
	NOBODY = 8,
	ALL = 9,
	NO,
	YES,

	//��������
	CO_INT_DELTETE_INT,
	CO_INT_DELTETE_ALL,
	CO_ALL_DELTETE_INT,
	CO_ALL_DELTETE_ALL,
	CO_INT_INSERT,
	CO_ALL_INSERT,
	CO_INT_MODIFY,
	CO_ALL_MODIFY,
	CO_INT_FIND,
	CO_ALL_FIND,
	CO_SAVE,
	CO_INFOR,
};

struct TempCommend
{
	SET_OF_COMMEND commend;
	int next_left;
	int next_right;
	SET_OF_COMMEND end_commend;
};


struct commend
{
	std::string CO_OPEN	  =	"OPEN";
	std::string CO_DELETE = "DELETE";
	std::string CO_FIND	  =	"FIND";
	std::string CO_INSERT = "INSERT";
	std::string CO_MODIFY = "MODIFY";
	std::string CO_SAVE = "SAVE";
	std::string CO_INFOR = "INFOR";
};

class Operator
{
public:
	Operator();
	~Operator();

	//������
	SET_OF_COMMEND OP_control(std::string commend);			//�Եõ����ַ������н����������ܿ�����ά�����ַ���
private:
	SET_OF_COMMEND OP_TypeAnalysis(std::string commend);
	bool OP_StringAnalysis(std::string commend);
	SET_OF_COMMEND BackingFind(std::vector<std::string>& commend_vec);
	SET_OF_COMMEND sub_back(std::vector<std::string>& commend_vec, int i, int j);
	/*bool OP_FileNameAnalysis(std::string commend);*/
	bool OP_IntAnalysis(std::string commend);
	commend set_of_commend;
};

#endif