#include "RealFile.h"
#include "Tool.h"

size_t RealFile::RF_SetMapping(LARGE_INTEGER fileSize)
{
	hMapFile = CreateFileMapping(
		hFile, // �ļ����
		NULL, // ��ȫ����
		PAGE_READWRITE, // ����ѡ��
		0, // ��ߴ�С����32λ��
		0, // ����С����32λ��
		NULL); // ӳ������

	if (hMapFile == NULL) {
		std::cerr << "Cannot create file mapping." << std::endl;
		return 1;
	}

	// ӳ���ļ���ͼ:���ļ�����ӳ������̵�ַ�ռ�
	lpBase = MapViewOfFile(
		hMapFile, // �ļ�ӳ����
		FILE_MAP_ALL_ACCESS, // ����Ȩ��
		0, // �ļ�ƫ��������32λ��
		0, // �ļ�ƫ��������32λ��
		fileSize.QuadPart); // ӳ��Ĵ�С
	//��Ϊ��δ�漰��С����32λ�������ļ���Ҳ����2GB���Ĵ���

	if (lpBase == NULL) {
		std::cerr << "Cannot map view of file." << std::endl;
		return 1;
	}

	// ����ӳ����ļ����ݵ�����ָ��
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
	// ���������� SetEndOfFile ���ضϻ���չ�ļ�
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
	//�����������OP������ģ����Լ��½���
	hFile = CreateFile(
		file_nameL, // �ļ���
		GENERIC_READ | GENERIC_WRITE, // �򿪷�ʽ
		0, // ����ģʽ
		NULL, // ��ȫ����
		OPEN_ALWAYS, // �������ļ�
		FILE_ATTRIBUTE_NORMAL, // �ļ�����
		NULL); // ģ��
	//�ȸ�����ļ�дЩ�ڴ棬����ӳ��ʧ��
	LARGE_INTEGER fileSize;
	fileSize.QuadPart = 100;
	file_size = fileSize.QuadPart;
	RF_ReSetFile(fileSize);
	// ����ӳ��
	return RF_SetMapping(fileSize);
}

int RealFile::RF_FileOpen(std::string file_name)
{
	Converter converter;
	LPCWSTR file_nameL = converter.Convert(file_name);
	// ���ļ�
	hFile = CreateFile(
		file_nameL, // �ļ���
		GENERIC_READ | GENERIC_WRITE, // �򿪷�ʽ
		0, // ����ģʽ
		NULL, // ��ȫ����
		OPEN_ALWAYS, // �������ļ�,���û���򴴽�һ��
		FILE_ATTRIBUTE_NORMAL, // �ļ�����
		NULL); // ģ��
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "Cannot open file." << std::endl;
		return 1;
	}
	// ��ȡ�ļ���С
	LARGE_INTEGER fileSize;
	GetFileSizeEx(hFile, &fileSize);
	if (fileSize.QuadPart == 0) {
		fileSize.QuadPart = 1000;
		RF_ReSetFile(fileSize);
	}
	file_size = static_cast<size_t>(fileSize.QuadPart);
	// ����ӳ��
	return RF_SetMapping(fileSize);
}
RealFile::~RealFile()//�ͷ�ӳ�䣬�ļ���ָ��
{
	UnmapViewOfFile(lpBase);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	cur_file_ptr = pre_file_ptr = end_file_ptr = NULL;
}
//ȷ���ļ�ӳ�����ת��ָ��
RealFile::RealFile(std::string file_name)
{
	//������ָ���ʼ��Ϊ�գ��ȴ�����
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
	//������ָ���ʼ��Ϊ�գ��ȴ�����
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
//���⻯һ��������
//1.�������ݻ�����ȡ������
//Ҫ�������У�ÿ�е�������⡣������ֱ�ӹ���vector��
//ÿ�еĿո��ܺ��ԣ�Ҳ��һ����
//������־������ֵΪ0
int RealFile::RF_DataStream(std::string& temp_string, int& size)
{
	//file_size�ļ���С,cur_file_ptr��ǰָ��λ��,pre_file_ptrָ�뿪ʼλ��,char_count�ֽڼ�����
	int string_size = 0;//�ַ���������
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
	//����ȡ��ȫ�����ݣ�ָ��ع�
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
	// ���������ļ���С�������ļ�ӳ�䣬Ȼ����ӳ��
	// ���ļ�ָ���ƶ����µĴ�Сλ�ã����ﲻ���ĵ�ǰָ��λ��
	UnmapViewOfFile(lpBase);
	CloseHandle(hMapFile);
	RF_ReSetFile(data_size);
	RF_SetMapping(data_size);
	return size;
}
//���ļ���Ҫ�ı�ӳ��
//�������ݻ��������ܴ�С��ȡ����ǰӳ�䣬����ӳ��һ������
//�ܴ�С�������������������ȫ�����룬���RF����
int RealFile::RF_Save(size_t size, std::string temp)
{
	//����޸ĺ���ļ��ܴ�С����ԭ�ļ�������ӳ��
	if (file_size != size && file_size != 0) {
		file_size = RF_ReMapping(size);
	}
	if (file_size == 0)
		return -1;
	//��������������һ���ƹ�������
	for (auto it = temp.begin(); it < temp.end(); it++)
	{
		//���������Բ��ӣ�����ĳ�ļ�����ȫ�ƻ�������ϵͳ����ֻ����װ
		assert(cur_file_ptr < end_file_ptr);
		*cur_file_ptr = *it;
		cur_file_ptr++;
	}
	//�������������ַ�������ָ��ع�
	if (cur_file_ptr = end_file_ptr - 1)
		cur_file_ptr = pre_file_ptr;
	return 0;
}