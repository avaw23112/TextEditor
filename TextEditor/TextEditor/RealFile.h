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
	//接口部分
	int RF_DataStream(std::string& temp_string, int& size);		//用于把数据传入数据缓存区的接口,接受缓存对象字符串，返回该行长度
	int RF_Save(size_t size, std::string temp);					//用于接收数据缓存区的数据，并保存

	//构造区域
	RealFile(std::string file_name);							//确定文件映射对象，转移指针
	RealFile();													//确定文件映射对象，转移指针
	~RealFile();												//释放映射，释放指针
private:
	int RF_FileOpen(std::string file_name);
	int RF_FileOpen();											//封装mmap函数，留出文件映射指针
	size_t RF_ReMapping(size_t size);							//重新映射
	size_t RF_SetMapping(LARGE_INTEGER fileSize);				//设置文件映射和指针
	size_t RF_ReSetFile(LARGE_INTEGER fileSize);
	size_t char_count;											//字节计数器
	char* cur_file_ptr;											//read those characters from file
	char* pre_file_ptr;											//rollback the cur_ptr
	char* end_file_ptr;											//保护指针不越界
	HANDLE hFile;												//keep file openning
	HANDLE hMapFile;											//keep file mapping
	LPVOID lpBase;												//keep file_ptr
	size_t file_size;											//file size
};

#endif