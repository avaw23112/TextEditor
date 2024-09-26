#include "RealFile.h"
#include "Tool.h"

size_t RealFile::RF_SetMapping(LARGE_INTEGER fileSize)
{
	hMapFile = CreateFileMapping(
		hFile, // 文件句柄
		NULL, // 安全属性
		PAGE_READWRITE, // 保护选项
		0, // 最高大小（高32位）
		0, // 最大大小（低32位）
		NULL); // 映射名称

	if (hMapFile == NULL) {
		std::cerr << "Cannot create file mapping." << std::endl;
		return 1;
	}

	// 映射文件视图:将文件内容映射入进程地址空间
	lpBase = MapViewOfFile(
		hMapFile, // 文件映射句柄
		FILE_MAP_ALL_ACCESS, // 访问权限
		0, // 文件偏移量（高32位）
		0, // 文件偏移量（低32位）
		fileSize.QuadPart); // 映射的大小
	//因为还未涉及大小超过32位整数的文件（也就是2GB）的处理

	if (lpBase == NULL) {
		std::cerr << "Cannot map view of file." << std::endl;
		return 1;
	}

	// 操作映射的文件内容到操作指针
	cur_file_ptr = pre_file_ptr = (char*)lpBase;
	end_file_ptr = pre_file_ptr + fileSize.QuadPart;
	return 0;
}
size_t RealFile::RF_ReSetFile(LARGE_INTEGER fileSize)
{
	if (!SetFilePointerEx(hFile, fileSize, NULL, FILE_BEGIN)) {
		perror("move file ptr fail");
		return 0;
	}
	// 接下来调用 SetEndOfFile 来截断或扩展文件
	if (!SetEndOfFile(hFile)) {
		perror("reset file size fail");
		return 0;
	}
	return 1;
}
int RealFile::RF_FileOpen()
{
	Converter converter;
	LPCWSTR file_nameL = converter.Convert("OPERATOR.txt");
	//无名则代表是OP区申请的，或自己新建的
	hFile = CreateFile(
		file_nameL, // 文件名
		GENERIC_READ | GENERIC_WRITE, // 打开方式
		0, // 共享模式
		NULL, // 安全属性
		OPEN_ALWAYS, // 创建新文件
		FILE_ATTRIBUTE_NORMAL, // 文件属性
		NULL); // 模板
	//先给这个文件写些内存，避免映射失败
	LARGE_INTEGER fileSize;
	fileSize.QuadPart = 100;
	file_size = fileSize.QuadPart;
	RF_ReSetFile(fileSize);
	// 设置映射
	return RF_SetMapping(fileSize);
}

int RealFile::RF_FileOpen(std::string file_name)
{
	Converter converter;
	LPCWSTR file_nameL = converter.Convert(file_name);
	// 打开文件
	hFile = CreateFile(
		file_nameL, // 文件名
		GENERIC_READ | GENERIC_WRITE, // 打开方式
		0, // 共享模式
		NULL, // 安全属性
		OPEN_ALWAYS, // 打开已有文件,如果没有则创建一个
		FILE_ATTRIBUTE_NORMAL, // 文件属性
		NULL); // 模板
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "Cannot open file." << std::endl;
		return 1;
	}
	// 获取文件大小
	LARGE_INTEGER fileSize;
	GetFileSizeEx(hFile, &fileSize);
	if (fileSize.QuadPart == 0) {
		fileSize.QuadPart = 1000;
		RF_ReSetFile(fileSize);
	}
	file_size = static_cast<size_t>(fileSize.QuadPart);
	// 设置映射
	return RF_SetMapping(fileSize);
}
RealFile::~RealFile()//释放映射，文件，指针
{
	UnmapViewOfFile(lpBase);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	cur_file_ptr = pre_file_ptr = end_file_ptr = NULL;
}
//确定文件映射对象，转移指针
RealFile::RealFile(std::string file_name)
{
	//将所有指针初始化为空，等待操作
	char_count = 0;
	cur_file_ptr = NULL;
	pre_file_ptr = NULL;
	end_file_ptr = NULL;
	hFile = INVALID_HANDLE_VALUE;
	hMapFile = NULL;
	lpBase = NULL;
	file_size = 0;
	int handle = RF_FileOpen(file_name);
	if (handle != 0) {
		perror("FileOpen fail.");
		this->~RealFile();
	}
}
RealFile::RealFile()
{
	//将所有指针初始化为空，等待操作
	char_count = 0;
	cur_file_ptr = NULL;
	pre_file_ptr = NULL;
	end_file_ptr = NULL;
	hFile = INVALID_HANDLE_VALUE;
	hMapFile = NULL;
	lpBase = NULL;
	file_size = 0;
	int handle = RF_FileOpen();
	if (handle != 0) {
		perror("FileOpen fail.");
		this->~RealFile();
	}
}
//虚拟化一个数据流
//1.方便数据缓存区取出数据
//要求：流分行，每行的字数检测。（方便直接构造vector）
//每行的空格不能忽略，也算一行内
//结束标志：返回值为0
int RealFile::RF_DataStream(std::string& temp_string, int& size)
{
	//file_size文件大小,cur_file_ptr当前指针位置,pre_file_ptr指针开始位置,char_count字节计数器
	int string_size = 0;//字符串计数器
	std::string temp_s = "";
	while (char_count < file_size)
	{
		char temp = *cur_file_ptr;
		temp_s.push_back(temp);
		cur_file_ptr++;
		char_count++;
		string_size++;
		if (temp == '\n')
			break;
	}
	//当读取完全部数据，指针回滚
	if (string_size == 0)
	{
		char_count = 0;
		cur_file_ptr = pre_file_ptr;
	}
	temp_string = temp_s;
	return size = string_size;
}

size_t RealFile::RF_ReMapping(size_t size)
{
	LARGE_INTEGER data_size;
	data_size.QuadPart = size;
	if (lpBase == NULL)
	{
		perror("mapping is not existing");
		return 0;
	}
	// 重新设置文件大小，设置文件映射，然后重映射
	// 将文件指针移动到新的大小位置，这里不关心当前指针位置
	UnmapViewOfFile(lpBase);
	CloseHandle(hMapFile);
	RF_ReSetFile(data_size);
	RF_SetMapping(data_size);
	return size;
}
//存文件需要改变映射
//接受数据缓存区的总大小，取消当前映射，重新映射一块满足
//总大小的区域，随后将数区的数据全部读入，完成RF保存
int RealFile::RF_Save(size_t size, std::string temp)
{
	//如果修改后的文件总大小大于原文件，重新映射
	if (file_size != size && file_size != 0) {
		file_size = RF_ReMapping(size);
	}
	if (file_size == 0)
		return -1;
	//将数区的数据逐一复制过来即可
	for (auto it = temp.begin(); it < temp.end(); it++)
	{
		//不怕死可以不加，轻则某文件被完全破坏，重则系统崩溃只能重装
		assert(cur_file_ptr < end_file_ptr);
		*cur_file_ptr = *it;
		cur_file_ptr++;
	}
	//当保存完所有字符，进行指针回滚
	if (cur_file_ptr = end_file_ptr - 1)
		cur_file_ptr = pre_file_ptr;
	return 0;
}