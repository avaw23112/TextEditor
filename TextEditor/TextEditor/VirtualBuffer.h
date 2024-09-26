#pragma once
#ifndef VB
#define VB

#include "DataCache.h"
#include "Tool.h"
#include "Operator.h"

//1.ͨ�����þ�����ƺ���ʾ�ı�
//2.�����Լ�����ÿ�����⻯�������Ĵ�С
//3.��ȡ�����豸�����룬��س����ո���ĸ�Ȱ���
//4.�����꣬ͼ��,
//VB��ֻ���������Խ�����DC�Ľ����������������������룡
static void Move(int row, int col)
{
	std::cout << "\033[" << row << ";" << col << "H";
}
enum INSTRUCTION
{
	//���β���
	VB_DELETE,
	VB_INSERT,

	//ָ�����
	VB_OP_DELETE,
	VB_OP_INSERT,
	VB_OP_MODIFY,
	VB_OP_FIND,
	VB_OP_SAVE,

	//�ƶ�����
	LEFT,
	RIGHT,
	DOWN,
	UP,
};

class VirtualBuffer
{
public:
	//���첿��
	VirtualBuffer(std::string file_name, int num);
	VirtualBuffer(Operator* op);
	~VirtualBuffer();

	void VB_Init();											//��̬������ֶ���

	//���ܲ���
	void VB_Mapping(int real_x, int real_y, int& x, int& y);//��DC���Ĺ��ӳ��
	void VB_CursorMapping(int& x, int& y);
	void VB_MatrixMapping();
	void VB_GetCursor(int& x, int& y);						//��ȡ��ǰVB���Ĺ��
	void VB_MoveCursor(INSTRUCTION);
	void VB_Cursor();

	//�����ӿ�
	int VB_Operator(INSTRUCTION instruction, char a);		//���ӳ��

	//����ӿ�
	int VB_Operator(INSTRUCTION instruction, int row, int col, std::string insert_s);			//����
	int VB_Operator(INSTRUCTION instruction, std::string delete_s, int num);					//ɾ��
	int VB_Operator(INSTRUCTION instruction, std::string original_s, std::string modify_s);		//�滻
	int VB_Operator(INSTRUCTION instruction, std::string find_s);								//��ѯ
	int VB_Operator(INSTRUCTION instruction);													//����

	int VB_Show();																				//��DC����Show�νӣ�������þ�������ı�
	int VB_RowShow();																			//��DC����Show�νӣ��ض�ˢ��ĳ��
	int VB_CharNumShow();																		
	int VB_Color(std::vector<std::tuple<int, int, int>> color_vec);								//����ĳ���У���ͷ����β���ַ�������ɫ
	int VB_SubDelete(DELETE_QE temp);
	int VB_SubModify(MODIFY_QE temp, std::string replace_s);
	int VB_RowSize(int row) { return VB_DC->DC_RowSize(row); }
	Operator* VB_op_ptr() { return VB_OP; }
	std::string VB_GetCommend();
	void VB_ModifyRowSize();
private:
	int VB_current_num;							//��¼�Լ���ǰ�ı�ţ��ɳ�ʼ��˳�������
	
	int VB_cursor_x;							//��¼�Լ��Ĺ��λ��
	int VB_cursor_y;							//��¼�Լ��Ĺ��λ��
	int VB_limit_x_be;							//��¼�Լ�������λ�ã����Ͻǣ�
	int VB_limit_y_be;							//��¼�Լ�������λ��
	int VB_limit_x_end;							//��¼�Լ�������λ�ã����½ǣ�
	int VB_limit_y_end;							//��¼�Լ�������λ��

	int VB_matrix_x_be;							//���þ������Ͻǣ�
	int VB_matrix_y_be;
	int VB_matrix_x_end;						//���þ������½ǣ�
	int VB_matrix_y_end;

	//ƫ����
	int VB_right;								//��ƫ����
	int VB_down;								//��ƫ����
	int VB_MaxCol;
	int VB_MaxRow;

	int VB_OldRow;
	int VB_OldDown;
	int RowSize;
	int JudgeRow;
	//��������
	int VB_SubInit(int windowsX);
	void DrawLine(int x, int end_y);
	void Draw_ROW_Line();
	void VB_prepara();

	//��������
	DataCache* VB_DC;
	Operator* VB_OP;

	//��꾺��������
	std::mutex cursol_mtx;
};

static CONSOLE_SCREEN_BUFFER_INFO csbi;		//��������
static HANDLE hStdOut;						//���ھ��
static int VB_OP_LINE;
static int VB_number;						//����ͳ���ִ�VB�����Ե�����С

#endif // !
