#pragma once
#ifndef DC
#define DC

#include "RealFile.h"
#include "Tool.h"

class DataCache
{
public:
	//接口部分
	int  DC_DataStream(std::string& temp_string, size_t& size);									//将数据传入物理文件区
	int DC_Save();																				//将数区的数据存入物区
	int DC_Keep();																				//将物理文件区的数据接收
	int DC_Updata_Modify(size_t object_num, std::vector<std::tuple<int, int, int>> Draw_Vec);	
																								//用于给控制命令留出刷新方法
	int DC_Size() { return all_char_num; }														//统计字符个数
	int DC_COL() { return col_end; };															//统计最长列
	int DC_ROW() { return row_end; };															//统计最长行
	int DC_RowSize(int row) { return data_cache[row].size(); }

	void DC_ClearDS() { row_count = 0; };
	void DC_SetDS(int row) { row_count = row; };
	void DC_COUT(int row, int col_be, int col_end);
	int DC_CharNum() { return char_num; };
	int DC_NumberNum() { return number_num; };
	int DC_BlankNum() { return blank_num; };
	int DC_AllCharNum() { return all_char_num; };												

	//OP功能部分
	int DC_Insert(int row, int col, std::string insert_s);
	int DC_Insert_Row(int row, int col);
	int DC_Delete(std::string delete_s, int number);											//可以选择删除多少个匹配到的字符串
	int DC_SubDelete(DELETE_QE temp);
	int DC_Delete_Row(int row);
	int DC_Modify(std::string original_s, std::string replace_s);
	int DC_SubModify(MODIFY_QE temp, std::string);
	int DC_Find(std::string pattern);

	int DC_Once_SubDelete(int row, int col);
	int DC_Once_SubInsert(int row_i, int col, char a);



	//构造区域
	DataCache(std::string file_name, size_t object_n);
	DataCache();
	//DataCache(Operator OP_temp);
	~DataCache();
private:
	std::vector<std::vector<char>> data_cache;      //采用char型vector是为了方便后续操作
	void DC_CharCount(char a);						//更新各字符计数器
	void DC_CharDelete(char a);						//更新各字符计数器			
	void DC_Prepara();
	RealFile* DC_RF;								//匹配的RF区
	int row_count;                               //行尾计数器
	int row_end;                                 //文件尾计数器
	int col_end;									//最长行的列计数器

	//后续有关对字符的操作直接作用于此
	int all_char_num;                            //字符计数器
	int char_num;								//英文字符计数器
	int number_num;								//数字字符计数器
	int blank_num;								//空格字符计数器

	//自己在对象序列的记号
	int object_num;
};

#endif