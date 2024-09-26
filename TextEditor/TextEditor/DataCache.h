#pragma once
#ifndef DC
#define DC

#include "RealFile.h"
#include "Tool.h"

class DataCache
{
public:
	//�ӿڲ���
	int  DC_DataStream(std::string& temp_string, size_t& size);									//�����ݴ��������ļ���
	int DC_Save();																				//�����������ݴ�������
	int DC_Keep();																				//�������ļ��������ݽ���
	int DC_Updata_Modify(size_t object_num, std::vector<std::tuple<int, int, int>> Draw_Vec);	
																								//���ڸ�������������ˢ�·���
	int DC_Size() { return all_char_num; }														//ͳ���ַ�����
	int DC_COL() { return col_end; };															//ͳ�����
	int DC_ROW() { return row_end; };															//ͳ�����
	int DC_RowSize(int row) { return data_cache[row].size(); }

	void DC_ClearDS() { row_count = 0; };
	void DC_SetDS(int row) { row_count = row; };
	void DC_COUT(int row, int col_be, int col_end);
	int DC_CharNum() { return char_num; };
	int DC_NumberNum() { return number_num; };
	int DC_BlankNum() { return blank_num; };
	int DC_AllCharNum() { return all_char_num; };												

	//OP���ܲ���
	int DC_Insert(int row, int col, std::string insert_s);
	int DC_Insert_Row(int row, int col);
	int DC_Delete(std::string delete_s, int number);											//����ѡ��ɾ�����ٸ�ƥ�䵽���ַ���
	int DC_SubDelete(DELETE_QE temp);
	int DC_Delete_Row(int row);
	int DC_Modify(std::string original_s, std::string replace_s);
	int DC_SubModify(MODIFY_QE temp, std::string);
	int DC_Find(std::string pattern);

	int DC_Once_SubDelete(int row, int col);
	int DC_Once_SubInsert(int row_i, int col, char a);



	//��������
	DataCache(std::string file_name, size_t object_n);
	DataCache();
	//DataCache(Operator OP_temp);
	~DataCache();
private:
	std::vector<std::vector<char>> data_cache;      //����char��vector��Ϊ�˷����������
	void DC_CharCount(char a);						//���¸��ַ�������
	void DC_CharDelete(char a);						//���¸��ַ�������			
	void DC_Prepara();
	RealFile* DC_RF;								//ƥ���RF��
	int row_count;                               //��β������
	int row_end;                                 //�ļ�β������
	int col_end;									//��е��м�����

	//�����йض��ַ��Ĳ���ֱ�������ڴ�
	int all_char_num;                            //�ַ�������
	int char_num;								//Ӣ���ַ�������
	int number_num;								//�����ַ�������
	int blank_num;								//�ո��ַ�������

	//�Լ��ڶ������еļǺ�
	int object_num;
};

#endif