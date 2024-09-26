#include "DataCache.h"

void DataCache::DC_CharCount(char a)
{
	if ('a' < a && a < 'z' || 'A' < a && a < 'Z')
		char_num++;
	else if ('0' < a && a < '9')
		number_num++;
	else if (a == ' ')
		blank_num++;
	all_char_num++;
}
void DataCache::DC_CharDelete(char a)
{
	if ('a' < a && a < 'z' || 'A' < a && a < 'Z')
		char_num--;
	else if ('0' < a && a < '9')
		number_num--;
	else if (a == ' ')
		blank_num--;
	all_char_num--;
}
void DC_DeterminChar(std::string object, int& Char, int& Number, int& Blank)
{
	for (auto a = object.begin(); a < object.end(); a++)
	{
		if ('a' < *a && *a < 'z' || 'A' < *a && *a < 'Z')
			Char++;
		else if ('0' < *a && *a < '9')
			Number++;
		else if (*a == ' ')
			Blank++;
	}
}
//将RF区所有数据读入DC区
int DataCache::DC_Keep()
{
	std::string temp = "";
	int size = 0;
	//与其增量修改，不如删掉重构
	//增量修改的性能提升虽大，但写起来过于麻烦
	while (DC_RF->RF_DataStream(temp, size))
	{
		//新增一行
		data_cache.push_back(std::vector<char>());
		//在新增行中新增size列
		data_cache[row_end] = std::vector<char>(size);
		//统计出最长的行
		col_end = max(col_end, size);
		//将temp复制进该行中
		int i = 0;
		for (auto it = temp.begin(); it < temp.end(); it++)
		{
			data_cache[row_end][i] = *it;
			DC_CharCount(*it);
			i++;
		}
		row_end++;
	}
	if (data_cache.size() == 0)
	{
		data_cache.push_back(std::vector<char>());
	}
	return 0;
}

//接收来自RF的字符串流，根据流的信息构造每一行的vector
//直到构造完成
void DataCache::DC_Prepara()
{
	row_count = 0;
	row_end = 0;
	all_char_num = 0;
	char_num = 0;
	number_num = 0;
	blank_num = 0;
	col_end = 0;
}

DataCache::DataCache(std::string file_name, size_t object_n)
{
	DC_Prepara();
	object_num = object_n;
	//构造实际物理文件区
	DC_RF = new RealFile(file_name);
	if (DC_RF == NULL) {
		perror("Create RealFile fail");
		return;
	}
	DC_Keep();
}
DataCache::DataCache()
{
	DC_Prepara();
	object_num = 0;
	DC_RF = new RealFile();
	if (DC_RF == NULL) {
		perror("Create RealFile fail");
		return;
	}
	DC_Keep();
}

//从data_cache读出每行的字符串
int  DataCache::DC_DataStream(std::string& temp_string, size_t& size)
{
	int string_size = 0;							//字符串计数器
	std::string temp_s = "";
	if (row_count >= row_end)
	{
		row_count = 0;
		temp_string = temp_s;
		return size = string_size;
	}
	for (auto it = data_cache[row_count].begin();
		it < data_cache[row_count].end(); it++)
	{
		if (*it != '\0') {
			temp_s.push_back(*it);
			string_size++;
		}
	}
	temp_string = temp_s;
	row_count++;
	return size = string_size;
}

//将DC区所有数据保存入RF区
int DataCache::DC_Save()
{
	std::string temp = "";
	size_t size = 0;
	size_t max_size = DC_Size();
	while (DC_DataStream(temp, size))
	{
		DC_RF->RF_Save(max_size, temp);
	}
	return 0;
}

//KMP算法
void GetNext(std::vector<size_t>& next, std::string pattern)
{
	size_t j = 0;										//j还代指最长公共前后缀
	next[0] = 0;									//初始化
	size_t length = pattern.size();
	for (int i = 1; i < length; i++) {				//i指针指向的是后缀末尾，j指针指向的是前缀末尾
		while (j > 0 && pattern[i] != pattern[j])	//前后缀不相同，去找j前一位的最长相等前后缀
			j = next[j - 1];
		if (pattern[i] == pattern[j])				//前后缀相同，j指针后移
			j++;
		next[i] = j;								//更新next数组
	}
}
//找到一串就返回该串的起始地址
void KMP(std::vector<size_t>& next, std::queue<size_t>& prepara, std::string pattern, std::string main_s)
{
	size_t j = 0;										//最长公共前后缀
	size_t length = main_s.size();
	size_t p_end = pattern.size();
	for (size_t i = 0; i < length; i++)
	{
		while (j > 0 && main_s[i] != pattern[j])
			j = next[j - 1];
		if (main_s[i] == pattern[j])
			j++;
		if (j == p_end)								//匹配完成
		{
			prepara.push(i - j + 1);						//传回位置队列
			j = 0;
		}
	}
}

DataCache::~DataCache()
{
	delete DC_RF;
	DC_RF = NULL;
}

int DataCache::DC_Updata_Modify(size_t object_num, std::vector<std::tuple<int, int, int>> Draw_Vec)		//用于给控制命令留出刷新方法
{
	TaskQueue::TQ_GetTask(DC_MODIFY, object_num, Draw_Vec);
	return 0;
}

#define PREPARATION(String) 	size_t String##_size = String##.size();\
std::vector<size_t> next = std::vector<size_t>(String##_size);\
std::queue<size_t> prepara;\
GetNext(next, String);\
std::string temp = "";\
size_t size = 0;\
int row = 0;\
std::vector<std::tuple<int, int, int>> Draw_Vec = std::vector<std::tuple<int, int, int>>()

//功能部分
int DataCache::DC_Insert(int row_i, int col, std::string insert_s) {
	if (row_i > row_end || col > col_end || col < 0) {
		return 1;
	}
	for (auto i = insert_s.begin(); i < insert_s.end(); i++)
	{
		auto it = data_cache[row_i].begin() + col++;
		data_cache[row_i].insert(it, *i);
		DC_CharCount(*i);
	}
	return 0;
}
int DataCache::DC_Insert_Row(int row, int col)
{
	auto dc_it = data_cache.begin() + row + 1;
	data_cache.insert(dc_it, std::vector<char>());
	row_end++;
	int new_row = row + 1;
	auto it = data_cache[row].begin() + col;
	for (it; it < data_cache[row].end(); it++)
	{
		if (*it != '\0') {
			data_cache[new_row].push_back(*it);
			DC_CharCount(*it);
		}
	}
	if (data_cache[new_row].empty()) data_cache[new_row].push_back(' ');
	data_cache[row].erase(data_cache[row].begin() + col, data_cache[row].end());
	return 0;
}

int DataCache::DC_Delete_Row(int row)
{
	auto dc_it = data_cache.begin() + row;
	if (dc_it == data_cache.begin()) {
		return 1;
	}
	for (auto it = data_cache[row].begin(); it < data_cache[row].end(); it++)
		DC_CharDelete(*it);
	data_cache.erase(dc_it);
	row_end--;
	return 0;
}

int DataCache::DC_Once_SubInsert(int row_i, int col, char a) {
	auto it = data_cache[row_i].begin() + col;
	data_cache[row_i].insert(it, a);
	DC_CharCount(a);
	return 0;
}

void DataCache::DC_COUT(int row, int col_be, int col_end)
{
	auto it = data_cache[row].begin() + col_be;
	auto it_end = data_cache[row].begin() + col_end;
	for (it; it < it_end; it++)
	{
		std::cout << *it;
	}
}

int DataCache::DC_Once_SubDelete(int row, int col)
{
	if (row >= 0 && col >= 0) {
		auto it = data_cache[row].begin() + col;
		auto dc_it = data_cache.begin() + row;
		DC_CharDelete(*it);
		data_cache[row].erase(it);
		return 1;
	}
	return 0;
}

int DataCache::DC_Delete(std::string delete_s, int number) {
	//KMP匹配字符后，需要在DC返回匹配到的前后迭代器vector<char>
	//然后将返回的迭代器插入队列中备用
	//需要将任务交予U/M队列
	//计算M1,M2
	PREPARATION(delete_s);
	std::queue<DELETE_E> prepara_delete;
	int count = 0;
	while (DC_DataStream(temp, size))
	{
		KMP(next, prepara, delete_s, temp);
		while (!prepara.empty()) {
			size_t col_be = front_element<size_t>(prepara);
			size_t col_end = col_be + delete_s_size;
			Draw_Vec.push_back({ row,col_be,col_end });
			if (count < number) {
				prepara_delete.push({ col_be,col_end,row });	//记住不删Col_end
				count++;
			}
		}
		row++;
	}

	//计算这次更新的字符
	int size_s = prepara_delete.size(),Char = 0,Number = 0,Blank = 0;
	DC_DeterminChar(delete_s, Char, Number, Blank);
	Char += Char * size_s; Number += Number * size_s; Blank += Blank * size_s;
	all_char_num += Char + Number + Blank;
	char_num += Char; number_num += Number; blank_num += Blank;

	if (prepara_delete.empty()) {
		perror("empty queue;");
		return 0;
	}

	//先调用WAIT()自旋，等待operator的命令再决定
	if (SWITCH::WAIT())
	{
		//如果同意删除，把删除就绪队列送入任务，同时恢复染色,统计去除了多少字
	TaskQueue::TQ_InSubQueue(prepara_delete, object_num, prepara_delete.size(), row);
	}
	SWITCH::pop();
	return 0;
}

//单一线程处理：流水线调度,删除准备队列中各坐标中间的字符串
//从外部读入数据给子队列处理程序，再让子队列处理程序传参给对象处理程序
int DataCache::DC_SubDelete(DELETE_QE temp)
{
	int old_row = 0, addition = 0;
	while (!temp.empty()) {
		auto temp_e = front_element<DELETE_E>(temp);
		int col = std::get<0>(temp_e);
		int col_end = std::get<1>(temp_e);
		int row = std::get<2>(temp_e);
		if (row != old_row) {
			addition = 0;
			old_row = row;
		}
		if (!data_cache[row].empty()) {
			auto it = data_cache[row].begin() + (col - addition);
			auto it_end = data_cache[row].begin() + (col_end - addition);
			data_cache[row].erase(it, it_end);
			addition += col_end - col;
		}
	}
	return 0;
}

int DataCache::DC_Modify(std::string original_s, std::string replace_s) {
	//KMP匹配字符后，需要在DC返回匹配到的前后迭代器vector<char>
	//然后将返回的迭代器插入队列中备用
	//需要将任务交予U/M队列
	//计算M1,M2
	PREPARATION(original_s);
	std::queue<MODIFY_E> prepara_modify;
	int count = 0;
	while (DC_DataStream(temp, size))
	{
		KMP(next, prepara, original_s, temp);
		while (!prepara.empty()) {
			size_t col_be = front_element<size_t>(prepara);
			size_t col_end = col_be + original_s_size;
			Draw_Vec.push_back({ row,col_be,col_end });
			prepara_modify.push({ col_be,col_end,row });
		}
		row++;
	}
	//计算这次更新的字符
	int size_s = prepara_modify.size(), O_Char = 0, O_Number = 0, O_Blank = 0;
	DC_DeterminChar(original_s, O_Char, O_Number, O_Blank);
	int R_Char = 0, R_Number = 0, R_Blank = 0;
	DC_DeterminChar(original_s, R_Char, R_Number, R_Blank);
	O_Char = (O_Char - R_Char)* size_s; O_Number = (O_Number - R_Number)* size_s; O_Blank = (O_Blank - R_Blank)* size_s;
	all_char_num += O_Char + O_Number + O_Blank;
	char_num += O_Char; number_num += O_Number; blank_num += O_Blank;

	if (prepara_modify.empty()) {
		perror("empty queue;");
		return 0;
	}

	TaskQueue::TQ_GiveCV(DC_MODIFY).notify_all();
	if (SWITCH::WAIT())
	{
		//如果同意修改，把删除就绪队列送入任务，同时恢复染色
		TaskQueue::TQ_InSubQueue(prepara_modify, object_num, replace_s, prepara_modify.size(), row);
	}
	SWITCH::pop();
	return 0;
}

//从外部读入数据给子队列处理程序，再让子队列处理程序传参给对象处理程序

int DataCache::DC_SubModify(MODIFY_QE temp, std::string replace_s)
{
	int length = replace_s.size(), o_len = -999, addition = 0, old_row = 0;
	while (!temp.empty()) {
		MODIFY_E temp_e = front_element<MODIFY_E>(temp);
		int col = std::get<0>(temp_e);
		int col_end = std::get<1>(temp_e);
		int row = std::get<2>(temp_e);
		if (row != old_row) {
			addition = 0;
			old_row = row;
		}
		auto it = data_cache[row].begin() + (col - addition);
		auto it_end = data_cache[row].begin() + (col_end - addition);
		data_cache[row].erase(it, it_end);

		//在删去的地方直接重新插入
		if (col > 0) col--;
		for (auto i = replace_s.begin(); i < replace_s.end(); i++)
		{
			auto it = data_cache[row].begin() + col++;
			data_cache[row].insert(it, *i);
		}
		addition += col_end - col - length;
	}

	return 0;
}
int DataCache::DC_Find(std::string pattern) {
	//KMP匹配字符后，需要将任务交予U/M队列
	//U/M队列需要row,col_be,col_end
	PREPARATION(pattern);
	while (DC_DataStream(temp, size))
	{
		KMP(next, prepara, pattern, temp);
		while (!prepara.empty()) {
			size_t col_be = front_element<size_t>(prepara);
			size_t col_end = col_be + pattern_size;					//不包括最后一个元素
			Draw_Vec.push_back({ row,col_be,col_end });
		}
		row++;
	}
	DC_Updata_Modify(object_num, Draw_Vec);
	TaskQueue::TQ_GiveCV(DC_MODIFY).notify_all();
	
	return 0;
}
