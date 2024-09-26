#pragma once
#ifndef RF
#define RF

#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>


class RealFile
{
public:
	//�ӿڲ���
	int RF_DataStream(std::string& temp_string, int& size);		//���ڰ����ݴ������ݻ������Ľӿ�,���ܻ�������ַ��������ظ��г���
	int RF_Save(size_t size, std::string temp);					//���ڽ������ݻ����������ݣ�������

	//��������
	RealFile(std::string file_name);							//ȷ���ļ�ӳ�����ת��ָ��
	RealFile();													//ȷ���ļ�ӳ�����ת��ָ��
	~RealFile();												//�ͷ�ӳ�䣬�ͷ�ָ��
private:
	int RF_FileOpen(std::string file_name);
	int RF_FileOpen();											//��װmmap�����������ļ�ӳ��ָ��
	size_t RF_ReMapping(size_t size);							//����ӳ��
	size_t RF_SetMapping(LARGE_INTEGER fileSize);				//�����ļ�ӳ���ָ��
	size_t RF_ReSetFile(LARGE_INTEGER fileSize);
	size_t char_count;											//�ֽڼ�����
	char* cur_file_ptr;											//read those characters from file
	char* pre_file_ptr;											//rollback the cur_ptr
	char* end_file_ptr;											//����ָ�벻Խ��
	HANDLE hFile;												//keep file openning
	HANDLE hMapFile;											//keep file mapping
	LPVOID lpBase;												//keep file_ptr
	size_t file_size;											//file size
};

#endif