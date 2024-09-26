#include "VirtualBuffer.h"

//��DC����OP�����ж�̬��ʼ��
//���������
void VirtualBuffer::VB_prepara()
{
	VB_right = VB_down = 0;
	VB_cursor_x = 0;							//��¼�Լ��Ĺ��λ��
	VB_cursor_y = 0;							//��¼�Լ��Ĺ��λ��
	VB_limit_x_be = 0;						//��¼�Լ�������λ�ã����Ͻǣ�
	VB_limit_y_be = 0;							//��¼�Լ�������λ��
	VB_limit_x_end = 0;							//��¼�Լ�������λ�ã����½ǣ�
	VB_limit_y_end = 0;							//��¼�Լ�������λ��

	VB_matrix_x_be = 0;							//��þ������Ͻǣ�
	VB_matrix_y_be = 0;
	VB_matrix_x_end = 0;						//��þ������½ǣ�
	VB_matrix_y_end = 0;

	//ƫ����
	VB_right = 0;								//��ƫ����
	VB_down = 0;								//��ƫ����
	VB_MaxCol = 0;
	VB_OldDown = 0;
	VB_OldRow = VB_cursor_y;

	RowSize = VB_RowSize(0);
	JudgeRow = RowSize + VB_limit_x_be;

}

VirtualBuffer::VirtualBuffer(std::string file_name, int num)
{
	VB_DC = new DataCache(file_name, num);
	VB_OP = NULL;
	VB_current_num = num;
	VB_number++;
	VB_MaxRow = VB_DC->DC_ROW();
	VB_prepara();
	VB_Init();
	TaskQueue::TQ_GetTask(VB_SHOW, VB_current_num);
}
VirtualBuffer::VirtualBuffer(Operator* op)
{
	VB_OP = op;
	VB_current_num = 0;
	VB_number = 0;
	VB_DC = new DataCache();
	VB_MaxRow = VB_DC->DC_ROW();
	VB_prepara();
	if (hStdOut == NULL)
	{
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hStdOut, &csbi);
		VB_OP_LINE = csbi.dwSize.Y - csbi.dwSize.Y / 5;
	}
	VB_Init();
	TaskQueue::TQ_GetTask(VB_SHOW, VB_current_num);
}

VirtualBuffer::~VirtualBuffer()
{
	if (VB_DC != NULL)
		delete VB_DC;
	VB_DC = NULL;

	if (VB_OP != NULL)
		delete VB_OP;
	VB_OP = NULL;
}
void VirtualBuffer::VB_Init()
{
	switch (VB_number) {
	case 0:VB_SubInit(csbi.dwSize.X); VB_Show(); break;
	case 1:VB_SubInit(csbi.dwSize.X); VB_Show(); break;
	case 2:VB_SubInit(csbi.dwSize.X / 2); DrawLine(VB_limit_x_be, VB_limit_y_end); VB_Show(); break;
	case 3:VB_SubInit(csbi.dwSize.X / 3); DrawLine(VB_limit_x_be, VB_limit_y_end); VB_Show(); break;
	case 4:VB_SubInit(csbi.dwSize.X / 4); DrawLine(VB_limit_x_be, VB_limit_y_end); VB_Show(); break;
	}
}
//��X����һ�����ߣ�ֱ������Y
void VirtualBuffer::Draw_ROW_Line()
{
	std::unique_lock<std::mutex> lk(cursol_mtx);
	Move(VB_OP_LINE, 0);
	for (int x = 0; x < csbi.dwSize.X; x++)
		std::cout << "-";
	Move(VB_cursor_y, VB_cursor_x);
}
void VirtualBuffer::DrawLine(int x, int end_y)
{
	std::unique_lock<std::mutex> lk(cursol_mtx);
	int temp_x = x - 1;
	if (temp_x > 0) {
		Move(0, temp_x);
		int count = 0;
		for (int y = 0; y < end_y; y++)
		{
			Move(y, temp_x);
			std::cout << "|";
		}
		Move(VB_cursor_y, VB_cursor_x);
	}
}

int VirtualBuffer::VB_SubInit(int x)
{
	switch (VB_current_num)
	{
	case 0: {
		VB_limit_x_be = 1;	VB_limit_x_end = x;
		VB_limit_y_be = VB_OP_LINE + 1; VB_limit_y_end = csbi.dwSize.Y;
		VB_cursor_x = 1; VB_cursor_y = VB_OP_LINE + 1;
		Draw_ROW_Line();
	}; break;
	case 1: {
		VB_limit_x_be = 1; VB_limit_x_end = x;
		VB_limit_y_be = 1; VB_limit_y_end = VB_OP_LINE-1;
		VB_cursor_x = 1; VB_cursor_y = 1;
	}; break;
	case 2: {
		VB_limit_x_be = x+1; VB_limit_x_end = x * 2;
		VB_limit_y_be = 1; VB_limit_y_end = VB_OP_LINE-1;
		VB_cursor_x = x + 1; VB_cursor_y = 1;
	}; break;
	case 3: {
		VB_limit_x_be = x * 2+1; VB_limit_x_end = x * 3;
		VB_limit_y_be = 1; VB_limit_y_end = VB_OP_LINE-1;
		VB_cursor_x = x * 2 + 1; VB_cursor_y = 1;
		;
	}; break;
	case 4: {
		VB_limit_x_be = x * 3+1; VB_limit_x_end = x * 4;
		VB_limit_y_be = 1; VB_limit_y_end = VB_OP_LINE-1;
		VB_cursor_x = x * 3 + 1; VB_cursor_y = 1;
	}; break;
	}
	return 0;
}
void VirtualBuffer::VB_GetCursor(int& x, int& y)						//��ȡ��ǰVB���Ĺ��
{
	std::unique_lock<std::mutex> lk(cursol_mtx);
	x = VB_cursor_x;
	y = VB_cursor_y;
}
void VirtualBuffer::VB_Mapping(int real_x, int real_y, int& x, int& y)						//��DC���Ĺ��ӳ�䣺����һ��˫�������ɳ��غ��޸�
{
	x = real_x - VB_limit_x_be + VB_right;
	y = real_y - VB_limit_y_be + VB_down;
}
//�ҳ���DCʵ����ʾ�Ĳ���
void VirtualBuffer::VB_MatrixMapping()
{
	VB_Mapping(VB_limit_x_be, VB_limit_y_be, VB_matrix_x_be, VB_matrix_y_be);
	VB_Mapping(VB_limit_x_end, VB_limit_y_end, VB_matrix_x_end, VB_matrix_y_end);
	int temp_dc_col = VB_DC->DC_COL(), temp_dc_row = VB_DC->DC_ROW();
	//��������ļ������С��DC������ô�������Ͱ���ô��
	//��������ļ���������DC������ô�Ͱ�DC�����С������
	VB_matrix_x_end = VB_matrix_x_end > temp_dc_col ? temp_dc_col : VB_matrix_x_end;
	VB_matrix_y_end = VB_matrix_y_end > temp_dc_row ? temp_dc_row : VB_matrix_y_end;
}
void VirtualBuffer::VB_CursorMapping(int& x, int& y)
{
	VB_Mapping(VB_cursor_x, VB_cursor_y, x, y);
}

int VirtualBuffer::VB_Show()											//��DC����Show�νӣ������þ�������ı�
{
	//�������þ�����������
	VB_MatrixMapping();
	std::string temp = "";
	size_t size = 0;
	size_t max_size = 0;
	std::unique_lock<std::mutex> lk(cursol_mtx);
	Move(VB_limit_y_be, VB_limit_x_be);
	int count = VB_limit_y_be + 1;
	for (int y = VB_matrix_y_be; y <= VB_matrix_y_end; y++)
	{
		VB_DC->DC_SetDS(y);
		VB_DC->DC_DataStream(temp, size);
		max_size = max(size, max_size);
		std::string::iterator it;
		if (temp.size() > VB_matrix_x_be) {//�����������������ݲ������
			it = temp.begin() + VB_matrix_x_be;
		}
		else it = temp.end();
		auto it_end = temp.end();
		for (int i = VB_matrix_x_be; i < VB_matrix_x_end; i++)
		{
			if (it < it_end && *it != '\r' && *it != '\n') std::cout << *it++;
			else std::cout << " ";
		}
		//�ص�ԭ��������������

		Move(count++, VB_limit_x_be);
	}
	//���������
	VB_Cursor();
	VB_DC->DC_ClearDS();
	VB_MaxCol = max_size;
	return 0;	
}
int VirtualBuffer::VB_RowShow()										//��DC����Show�νӣ������þ�������ı�
{
	int dc_col = 0, dc_row = 0;
	std::unique_lock<std::mutex> lk(cursol_mtx);
	VB_CursorMapping(dc_col, dc_row);
	VB_MatrixMapping();
	VB_DC->DC_SetDS(dc_row);
	std::string temp = "";
	size_t size = 0;
	VB_DC->DC_DataStream(temp, size);
	auto it = temp.begin() + VB_matrix_x_be;
	auto it_end = temp.begin()+size;
	Move(VB_cursor_y, VB_limit_x_be);
	for (int i = VB_matrix_x_be; i < VB_matrix_x_end; i++)
	{
		if (it < it_end && *it != '\r' && *it != '\n')
			std::cout << *it++;
		else 
			std::cout << " ";
	}
	VB_Cursor();
	//���������
	VB_DC->DC_ClearDS();
	return 0;
}
void VirtualBuffer::VB_ModifyRowSize()
{
	if (VB_OldRow != VB_cursor_y || VB_OldDown != VB_down)
	{
		VB_OldRow = VB_cursor_y;
		VB_OldDown = VB_down;
		int temp_x, temp_y;
		VB_CursorMapping(temp_x,temp_y);
		RowSize = VB_RowSize(temp_y);
		JudgeRow = VB_limit_x_be + RowSize;
		if (temp_x > RowSize)
			VB_cursor_x = JudgeRow -1;
	}
}

void VirtualBuffer::VB_MoveCursor(INSTRUCTION dir)
{
	if (RIGHT == dir)
	{
		if (VB_cursor_x < JudgeRow && VB_cursor_x < VB_limit_x_end) {
			std::unique_lock<std::mutex> lk(cursol_mtx);
			VB_cursor_x++;
			VB_Cursor();
		}
		else if (VB_cursor_x == VB_limit_x_end && VB_right < VB_MaxCol- (VB_limit_x_end - VB_limit_x_be)-1)
		{
			VB_right++;
			VB_Cursor();
			TaskQueue::TQ_GetTask(VB_SHOW, VB_current_num);
			TaskQueue::TQ_GiveCV(VB_SHOW).notify_all();
		}
	}
	if (LEFT == dir)
	{
		if (VB_cursor_x > VB_limit_x_be) {
			std::unique_lock<std::mutex> lk(cursol_mtx);
			VB_cursor_x--;
			VB_Cursor();
		}
		else if (VB_right > 0)
		{
			VB_right--;
			TaskQueue::TQ_GetTask(VB_SHOW, VB_current_num);
			TaskQueue::TQ_GiveCV(VB_SHOW).notify_all();
		}
	}
	if (UP == dir)
	{
		if (VB_cursor_y > VB_limit_y_be) {
			std::unique_lock<std::mutex> lk(cursol_mtx);
			VB_cursor_y--;
			VB_ModifyRowSize();
			VB_Cursor();

		}
		else if (VB_down > 0)
		{
			VB_down--;
			TaskQueue::TQ_GetTask(VB_SHOW, VB_current_num);
			TaskQueue::TQ_GiveCV(VB_SHOW).notify_all();
		}
	}
	if (DOWN == dir)
	{
		if (VB_cursor_y < VB_MaxRow + VB_limit_y_be-1 &&VB_cursor_y < VB_limit_y_end) {
			std::unique_lock<std::mutex> lk(cursol_mtx);
			VB_cursor_y++;
			VB_ModifyRowSize();
			VB_Cursor();
		}
		else if (VB_down < VB_MaxRow - (VB_limit_y_end - VB_limit_y_be) - 1)
		{
			VB_down++;
			VB_ModifyRowSize();
			TaskQueue::TQ_GetTask(VB_SHOW, VB_current_num);
			TaskQueue::TQ_GiveCV(VB_SHOW).notify_all();
		}
	}
}
int VirtualBuffer::VB_Operator(INSTRUCTION instruction, char a)		//���������ܿ����ĵ�����ָ��
{
	switch (instruction)
	{
	case VB_DELETE: {
		int col = 0, row = 0;
		VB_CursorMapping(col, row);
		if (VB_DC->DC_Once_SubDelete(row, col-1))	//ɾ��
			RowSize--;
		VB_RowShow();
		VB_MoveCursor(LEFT);
	}; break;
	case VB_INSERT: {
		int col = 0, row = 0;
		VB_CursorMapping(col, row);
		if (a == 13) {
			VB_DC->DC_Insert_Row(row, col);
			VB_MaxRow++;
			VB_Show();
			VB_MoveCursor(DOWN);
		}
		else {
			VB_DC->DC_Once_SubInsert(row, col, a);
			RowSize++;
			JudgeRow++;
			VB_RowShow();
			VB_MoveCursor(RIGHT);
		}
	}; break;
	default:
		break;
	}
	return 0;
}
int VirtualBuffer::VB_Operator(INSTRUCTION instruction, int row, int col, std::string insert_s)//����
{
	if (VB_OP_INSERT == instruction)
	{
		VB_DC->DC_Insert(row, col, insert_s);
	}
	else perror("error task");
	return 0;
}
int VirtualBuffer::VB_Operator(INSTRUCTION instruction, std::string delete_s, int num)//ɾ��
{
	if (VB_OP_DELETE == instruction)
	{
		VB_DC->DC_Delete(delete_s, num);
		TaskQueue::TQ_GiveCV(DELETE_S).notify_all();
	}
	else perror("error task");
	return 0;
}
int VirtualBuffer::VB_Operator(INSTRUCTION instruction, std::string original_s, std::string modify_s)//����
{
	if (VB_OP_MODIFY == instruction)
	{
		VB_DC->DC_Modify(original_s, modify_s);
		TaskQueue::TQ_GiveCV(MODIFY_S).notify_all();
	}
	else perror("error task");
	return 0;
}
int VirtualBuffer::VB_Operator(INSTRUCTION instruction, std::string find_s)//����
{
	if (VB_OP_FIND == instruction)
	{
		VB_DC->DC_Find(find_s);
	}
	else perror("error task");
	return 0;
}
int VirtualBuffer::VB_Operator(INSTRUCTION instruction)//����
{
	if (VB_OP_SAVE == instruction)
	{
		VB_DC->DC_Save();
	}
	else perror("error task");
	return 0;
}
int VirtualBuffer::VB_SubDelete(DELETE_QE temp)
{
	//�ȴ����������룬Ȼ�����ͳһ������
	VB_DC->DC_SubDelete(temp);
	return 0;
}
int VirtualBuffer::VB_SubModify(MODIFY_QE temp,std::string replace_s)
{
	VB_DC->DC_SubModify(temp, replace_s);
	return 0;
}

int VirtualBuffer::VB_Color(std::vector<std::tuple<int, int, int>> color_vec)				//����ĳ���У���ͷ����β���ַ�������ɫ
{
	//���Ѻ��������ַ���ɫ��Ȼ���ڼ�������ڱ������޷���Ⱦɫ������λ�ã�����оͽ���Ӧλ�õ��ַ�
	//�����һ��Ⱦɫ�����û�оͲ��ܣ�������ߵȴ���һ�λ���
	int size = color_vec.size();
	std::unique_lock<std::mutex> lk(cursol_mtx);
	std::cout << "\x1b[101m"; // ����ǰ����ɫΪ��ɫ
	for (int i = 0; i < size; i++)
	{
		int row = std::get<0>(color_vec[i]) + VB_limit_y_be - VB_down;
		if (row > VB_matrix_y_end || row < VB_matrix_y_be)
			continue;
		int col_be = std::get<1>(color_vec[i]);
		int col_end = std::get<2>(color_vec[i]);
		int real_row = row + VB_limit_y_be - VB_down + 1;
		int real_col_be = col_be + VB_limit_x_be - VB_right + 1;
		if (VB_matrix_x_be < col_be && col_end < VB_matrix_x_end)//ͷ��β
		{
			Move(real_row, real_col_be);
			VB_DC->DC_COUT(row, col_be, col_end);
			VB_Cursor();
		}
		else if (VB_matrix_x_be > col_be && col_end < VB_matrix_x_end)//β
		{
			Move(real_row, real_col_be);
			VB_DC->DC_COUT(row, VB_matrix_x_be, col_end);
			VB_Cursor();
		}
		else if (VB_matrix_x_be < col_be && col_end > VB_matrix_x_end)//ͷ
		{
			Move(real_row, real_col_be);
			VB_DC->DC_COUT(row, col_be, VB_matrix_x_end);
			VB_Cursor();
		}
	}
	std::cout << "\x1b[0m";
	return 0;
}

std::string VirtualBuffer::VB_GetCommend()
{
	size_t size = 0; std::string temp = "";
	int cur_x, cur_y;
	VB_CursorMapping(cur_x, cur_y);
	VB_DC->DC_SetDS(cur_y);
	VB_DC->DC_DataStream(temp,size);
	VB_DC->DC_ClearDS();
	return temp;
}
void VirtualBuffer::VB_Cursor()
{
	Move(VB_cursor_y, VB_cursor_x);
}
int VirtualBuffer::VB_CharNumShow()
{
	Move(VB_limit_y_end - 1, VB_limit_x_be);
	std::cout << " ALL��" << VB_DC->DC_AllCharNum() << " char��" << VB_DC->DC_CharNum()
		<< " number��" << VB_DC->DC_NumberNum() << " blank��" << VB_DC->DC_BlankNum() << std::endl;
	return 0;
}

