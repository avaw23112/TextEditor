#pragma once
#ifndef VB
#define VB

#include "DataCache.h"
#include "Tool.h"
#include "Operator.h"

//1.通过剪裁矩阵控制和显示文本
//2.调控以及分配每个虚拟化缓冲区的大小
//3.获取输入设备的输入，如回车，空格，字母等按键
//4.管理光标，图像,
//VB区只负责管理光标越界和与DC的交互，不负责管理操作和输入！
static void Move(int row, int col)
{
	std::cout << "\033[" << row << ";" << col << "H";
}
enum INSTRUCTION
{
	//单次操作
	VB_DELETE,
	VB_INSERT,

	//指令操作
	VB_OP_DELETE,
	VB_OP_INSERT,
	VB_OP_MODIFY,
	VB_OP_FIND,
	VB_OP_SAVE,

	//移动命令
	LEFT,
	RIGHT,
	DOWN,
	UP,
};

class VirtualBuffer
{
public:
	//构造部分
	VirtualBuffer(std::string file_name, int num);
	VirtualBuffer(Operator* op);
	~VirtualBuffer();

	void VB_Init();											//多态处理多种对象

	//功能部分
	void VB_Mapping(int real_x, int real_y, int& x, int& y);//与DC区的光标映射
	void VB_CursorMapping(int& x, int& y);
	void VB_MatrixMapping();
	void VB_GetCursor(int& x, int& y);						//获取当前VB区的光标
	void VB_MoveCursor(INSTRUCTION);
	void VB_Cursor();

	//操作接口
	int VB_Operator(INSTRUCTION instruction, char a);		//光标映射

	//命令接口
	int VB_Operator(INSTRUCTION instruction, int row, int col, std::string insert_s);			//插入
	int VB_Operator(INSTRUCTION instruction, std::string delete_s, int num);					//删除
	int VB_Operator(INSTRUCTION instruction, std::string original_s, std::string modify_s);		//替换
	int VB_Operator(INSTRUCTION instruction, std::string find_s);								//查询
	int VB_Operator(INSTRUCTION instruction);													//保存

	int VB_Show();																				//与DC区的Show衔接，构造剪裁矩阵输出文本
	int VB_RowShow();																			//与DC区的Show衔接，特定刷新某行
	int VB_CharNumShow();																		
	int VB_Color(std::vector<std::tuple<int, int, int>> color_vec);								//更改某个行，列头，列尾的字符串的颜色
	int VB_SubDelete(DELETE_QE temp);
	int VB_SubModify(MODIFY_QE temp, std::string replace_s);
	int VB_RowSize(int row) { return VB_DC->DC_RowSize(row); }
	Operator* VB_op_ptr() { return VB_OP; }
	std::string VB_GetCommend();
	void VB_ModifyRowSize();
private:
	int VB_current_num;							//记录自己当前的编号（由初始化顺序决定）
	
	int VB_cursor_x;							//记录自己的光标位置
	int VB_cursor_y;							//记录自己的光标位置
	int VB_limit_x_be;							//记录自己的限制位置（左上角）
	int VB_limit_y_be;							//记录自己的限制位置
	int VB_limit_x_end;							//记录自己的限制位置（右下角）
	int VB_limit_y_end;							//记录自己的限制位置

	int VB_matrix_x_be;							//剪裁矩阵（左上角）
	int VB_matrix_y_be;
	int VB_matrix_x_end;						//剪裁矩阵（右下角）
	int VB_matrix_y_end;

	//偏移量
	int VB_right;								//右偏移量
	int VB_down;								//下偏移量
	int VB_MaxCol;
	int VB_MaxRow;

	int VB_OldRow;
	int VB_OldDown;
	int RowSize;
	int JudgeRow;
	//附属功能
	int VB_SubInit(int windowsX);
	void DrawLine(int x, int end_y);
	void Draw_ROW_Line();
	void VB_prepara();

	//交互对象
	DataCache* VB_DC;
	Operator* VB_OP;

	//光标竞争的问题
	std::mutex cursol_mtx;
};

static CONSOLE_SCREEN_BUFFER_INFO csbi;		//窗口数据
static HANDLE hStdOut;						//窗口句柄
static int VB_OP_LINE;
static int VB_number;						//用于统计现存VB个数以调整大小

#endif // !
