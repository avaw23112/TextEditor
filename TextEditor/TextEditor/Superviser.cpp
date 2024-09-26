#include "Superviser.h"
static VirtualBuffer* SP_VB_ptr = NULL;				//Ŀǰ��������
std::atomic<bool> flag = true;
Superviser::Superviser()
{
    //VB������0��ȫ����ʼ��Ϊ��
    for (size_t i = 1; i < 32; i++)
    {
        SP_VB_object[i] = nullptr;
    }
    //��ʼ��0��ΪOP����ƽ̨
    Operator* op;
    SP_VB_object[0] = new VirtualBuffer(op = new Operator());
    SP_VB_ptr = SP_VB_object[0];
    current_ptr = 1;
    exiting_number = 1;
    //��ʼ���߳�
    ThreadInit();
}
void Superviser::ThreadInit()
{
    //�������� �� 1��:����������С,����Kmp
    //ɾ������ �� 4��:��Ӧ�����4��������Ϊ��Ҫ���������Ǳ���ά��4���߳�
    //�޸����� �� 4��:��Ӧ�����4������
    //�������� �� 4��:��Ӧ�����4������
    // ������  �� 2��:������ɾ�����޸�
    //�������� �� 1��
    //Ⱦɫ���� �� 1��
    //show���� �� 1��:���ֻ��һ�������Լ����̶߳�û��

    SP_Thread[0] = std::thread(&Superviser::OP_Insert, this);
    for (size_t i = 1; i <= 4; i++)
    {
        SP_Thread[i] = std::thread(&Superviser::OP_Delete, this);
    }
    for (size_t i = 5; i <= 8; i++)
    {
        SP_Thread[i] = std::thread(&Superviser::OP_Find, this);
    }
    for (size_t i = 9; i <= 12; i++)
    {
        SP_Thread[i] = std::thread(&Superviser::OP_Modify, this);
    }
    SP_Thread[13] = std::thread(&Superviser::OP_SubDelete, this);
    SP_Thread[14] = std::thread(&Superviser::OP_SubModify, this);
    SP_Thread[15] = std::thread(&Superviser::OP_Save, this);
    SP_Thread[16] = std::thread(&Superviser::VB_Color, this);
    SP_Thread[17] = std::thread(&Superviser::VB_Show, this);
}

void Superviser::SP_EXIT()
{
    for (int i = 0; i < 32; i++) 
        SP_cv[i].notify_all();
    for (int i = 0; i < 18; i++)
        SP_Thread[i].join();
}
Superviser::~Superviser()
{
    for (int i = 0; i < 32; i++)
        if (SP_VB_object[i] != nullptr)
            delete SP_VB_object[i];
    SP_VB_ptr = nullptr;
}
int Superviser::SP_ConsoleInit()
{
    hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    // ��ȡ��ǰ����̨ģʽ
    dwMode = 0;
    if (!GetConsoleMode(hStdIn, &dwMode)) {
        std::cerr << "Error: Unable to get console mode." << std::endl;
        return 1;
    }

    // ���ô�����������ģʽ
    dwMode |= ENABLE_PROCESSED_INPUT;
    if (!SetConsoleMode(hStdIn, dwMode)) {
        std::cerr << "Error: Unable to set console mode." << std::endl;
        return 1;
    }
    return 0;
}
int Superviser::SP_Console()
{
    SP_ConsoleInit();
    const DWORD bufferSize = 10;
    DWORD dwEventsRead;
    std::array<INPUT_RECORD, bufferSize> irInBuffer;
    // ��Ϣѭ��
    SP_VB_ptr->VB_Cursor();
    while (true && flag) {
        // �ӿ���̨���뻺������ȡ����
        if (!ReadConsoleInput(hStdIn, irInBuffer.data(), static_cast<DWORD>(irInBuffer.size()), &dwEventsRead)) {
            std::cerr << "Error: Unable to read console input." << std::endl;
            break;
        }
        // �������ж�ȡ���¼�
        for (DWORD i = 0; i < dwEventsRead; ++i) {
            if (irInBuffer[i].EventType == KEY_EVENT && irInBuffer[i].Event.KeyEvent.bKeyDown) {
                switch (irInBuffer[i].Event.KeyEvent.wVirtualKeyCode) {
                case VK_UP:
                    SP_VB_ptr->VB_MoveCursor(UP);
                    break;
                case VK_DOWN:
                    SP_VB_ptr->VB_MoveCursor(DOWN);
                    break;
                case VK_LEFT:
                    SP_VB_ptr->VB_MoveCursor(LEFT);
                    break;
                case VK_RIGHT:
                    SP_VB_ptr->VB_MoveCursor(RIGHT);
                    break;
                case VK_BACK: {
                    //DELETE��ͨ����ζ����ɾ��DC����������д�ر�ɾ������,��ʲô�ַ�����ν
                    SP_VB_ptr->VB_Operator(VB_DELETE, '0');
                }break;
                case VK_TAB: {
                    //�л���������˳���л�
                    current_ptr = (current_ptr + 1) % exiting_number;
                    if (SP_VB_object[current_ptr] != NULL)
                        SP_VB_ptr = SP_VB_object[current_ptr];
                    SP_VB_ptr->VB_Cursor();
                }break;
                case  VK_ESCAPE: {
                    ReInit();
                }break;
                case VK_END: {
                    flag = false;
                    std::atomic_thread_fence(std::memory_order_acquire);
                    std::cout << "ending" << std::endl;
                    SP_EXIT();
                    std::cout << "ending success" << std::endl;
                }break;
                default:
                    //�������
                    if (irInBuffer[i].Event.KeyEvent.uChar.AsciiChar != 0) {
                        char temp = irInBuffer[i].Event.KeyEvent.uChar.AsciiChar;
                        if (temp == 13 && SP_VB_ptr->VB_op_ptr() != nullptr)
                        {
                            std::string commend = SP_VB_ptr->VB_GetCommend();
                            SET_OF_COMMEND COM = SP_VB_ptr->VB_op_ptr()->OP_control(commend);
                            //���ݵó���COMMEND����commendΪ��ȷ����
                            if (COM != NOBODY)
                            {
                                SP_ComTranslate(COM, commend);
                                SP_VB_ptr->VB_Cursor();
                            }
                        }
                        SP_VB_ptr->VB_Operator(VB_INSERT, temp);
                    }
                    break;
                }
            }
        }
    }
    return 0;
}
void Superviser::ReInit()									//������ʾ���ж���
{
    int temp = exiting_number - 1;
    //�������þ�����ȷ��ʾ
    system("cls");
    for (temp; temp >= 0; temp--)
    {
        if (SP_VB_object[temp] != NULL)
            SP_VB_object[temp]->VB_Init();
    }
}

//���ݵõ��������ָ��
int Superviser::SP_ComTranslate(SET_OF_COMMEND COM, std::string commend)
{
    //�����ַ���������ʽת������
    std::stringstream iss(commend);
    //���õ�һ��
    std::string THROW = "";
    iss >> THROW;
    switch (COM)
    {
    case YES: { SWITCH::AGREE(); }; break;
    case NO: {SWITCH::DISAGREE(); }; break;
    case CO_OPEN: {
        std::string file_name = "";
        iss >> file_name;
        if (exiting_number < 32)
            SP_VB_object[exiting_number] = new VirtualBuffer(file_name, exiting_number);
        exiting_number++;
        if (3 < exiting_number && exiting_number < 6)
            ReInit();
    }
                break;
    case CO_INT_DELTETE_INT: {
        int object = 0; std::string temp = ""; int number = 0;
        iss >> object >> temp >> number;
        TaskQueue::TQ_GetTask(OP_DELETE, object, temp, number);
        TaskQueue::TQ_GiveCV(OP_DELETE).notify_all();

    }
   break;
    case CO_INT_DELTETE_ALL: {
        int object = 0; std::string temp = ""; int number = 99999;
        iss >> object >> temp;
        TaskQueue::TQ_GetTask(OP_DELETE, object, temp, number);
        TaskQueue::TQ_GiveCV(OP_DELETE).notify_all();

    }
    break;
    case CO_ALL_DELTETE_INT:
    {
        std::string THROW = ""; std::string temp = ""; int number = 0;
        iss >> THROW >> temp >> number;
        int object = 1;
        for (object; object < exiting_number; object++) {
            if (SP_VB_object[object] != NULL) {
                TaskQueue::TQ_GetTask(OP_DELETE, object, temp, number);
                TaskQueue::TQ_GiveCV(OP_DELETE).notify_all();
            }
        }
    }
    break;
    case CO_ALL_DELTETE_ALL:
    {
        std::string THROW = ""; std::string temp = ""; int number = 99999;
        iss >> THROW >> temp;
        int object = 1;
        for (object; object < exiting_number; object++) {
            if (SP_VB_object[object] != NULL) {
                TaskQueue::TQ_GetTask(OP_DELETE, object, temp, number);
                TaskQueue::TQ_GiveCV(OP_DELETE).notify_all();
            }
        }
    }
    break;
    case CO_INT_INSERT:
    {
        int object = 0, row = 0, col = 0;
        std::string insert_s = "";
        iss >> object >> row >> col >> insert_s;
        TaskQueue::TQ_GetTask(OP_INSERT, object, row, col, insert_s);
        TaskQueue::TQ_GiveCV(OP_INSERT).notify_all();
    }
    break;
    case CO_ALL_INSERT:
    {
        std::string THROW = "";
        int row = 0, col = 0;
        std::string insert_s = "";
        iss >> THROW >> row >> col >> insert_s;
        int object = 1;
        for (object; object < exiting_number; object++) {
            if (SP_VB_object[object] != NULL) {
                TaskQueue::TQ_GetTask(OP_INSERT, object, row, col, insert_s);
                TaskQueue::TQ_GiveCV(OP_INSERT).notify_all();
            }
        }
    }
    break;
    case CO_INT_MODIFY:
    {
        int object = 0;
        std::string original_s = "", replace_s = "";
        iss >> object >> original_s >> replace_s;
        TaskQueue::TQ_GetTask(OP_MODIFY, object, original_s, replace_s);
        TaskQueue::TQ_GiveCV(OP_MODIFY).notify_all();
    }
    break;
    case CO_ALL_MODIFY:
    {
        std::string THROW = "";
        std::string original_s = "", replace_s = "";
        iss >> THROW >> original_s >> replace_s;
        int object = 1;
        for (object; object < exiting_number; object++) {
            if (SP_VB_object[object] != NULL) {
                TaskQueue::TQ_GetTask(OP_MODIFY, object, original_s, replace_s);
                TaskQueue::TQ_GiveCV(OP_MODIFY).notify_all();
            }
        }
    }
    break;
    case CO_INT_FIND:
    {
        int object = 0;
        std::string find_s = "";
        iss >> object >> find_s;
        TaskQueue::TQ_GetTask(OP_FIND, object, find_s);
        TaskQueue::TQ_GiveCV(OP_FIND).notify_all();
    }
    break;
    case CO_ALL_FIND:
    {
        std::string THROW = "";
        std::string find_s = "";
        iss >> THROW >> find_s;
        int object = 1;
        for (object; object < exiting_number; object++) {
            if (SP_VB_object[object] != NULL) {
                TaskQueue::TQ_GetTask(OP_FIND, object, find_s);
                TaskQueue::TQ_GiveCV(OP_FIND).notify_all();
            }
        }
    }
    break;
    case CO_SAVE:
    {
        for (int object = 1; object <= exiting_number; object++) {
            if (SP_VB_object[object] != NULL) {
                TaskQueue::TQ_GetTask(OP_SAVE, object);
                TaskQueue::TQ_GiveCV(OP_SAVE).notify_all();
            }
        }
    }
    break;
    case CO_INFOR:
    {
        for (int object = 1; object <= exiting_number; object++) {
            if (SP_VB_object[object] != NULL) {
                SP_VB_object[object]->VB_CharNumShow();
            }
        }
    }
    break;
    default:
        break;
    }
    return 0;
}

void Superviser::VB_Show()
{
    while (true&& flag) {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(VB_SHOW));
        while (TaskQueue::empty(VB_SHOW) && flag)
        {
            TaskQueue::TQ_GiveCV(VB_SHOW).wait(lk);
        }
        if (!flag) {
            lk.unlock();
            break;
        }
        while (!TaskQueue::empty(VB_SHOW)) {
            int object = TaskQueue::TQ_GiveTask<int>(VB_SHOW);
            std::unique_lock<std::mutex> SP_lk(SP_mtx[object]);
            SP_VB_object[object]->VB_Show();
        }
    }
}

void Superviser::VB_Color()
{
    while (true&& flag) {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(DC_MODIFY));
        std::queue<DC_MODIFY_NEED> queue;
        while (TaskQueue::empty(DC_MODIFY) && flag)
        {
            TaskQueue::TQ_GiveCV(DC_MODIFY).wait(lk);
        }
        if (!flag) { 
            lk.unlock();
            break; 
        }
        while (!TaskQueue::empty(DC_MODIFY))
        {

            DC_MODIFY_NEED temp = TaskQueue::TQ_GiveTask<DC_MODIFY_NEED>(DC_MODIFY);
            int object = temp.first;
            std::vector<std::tuple<int, int, int>> color_vec = temp.second;
            SP_VB_object[object]->VB_Color(color_vec);
            queue.push({ object, color_vec });
        }
        TaskQueue::QueueCopy(queue);
    }
}
void Superviser::OP_Find()
{
    while (true&& flag)
    {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(OP_FIND));
        while (TaskQueue::empty(OP_FIND) && flag)
        {
            TaskQueue::TQ_GiveCV(OP_FIND).wait(lk);
        }
        if (!flag) {
            std::cout << "ending1" << std::endl;
            lk.unlock();
            break;
        }
        while (!TaskQueue::empty(OP_FIND)) {
            OP_FIND_NEED temp = TaskQueue::TQ_GiveTask<OP_FIND_NEED>(OP_FIND);
            int object = std::get<0>(temp);
            std::string find_s = std::get<1>(temp);
            std::unique_lock<std::mutex> SP_lk(SP_mtx[object]);
            SP_VB_object[object]->VB_Operator(VB_OP_FIND, find_s);
        }
    }
}
void Superviser::OP_Save()
{
    while (true&& flag)
    {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(OP_SAVE));
        while (TaskQueue::empty(OP_SAVE) && flag)
        {
            TaskQueue::TQ_GiveCV(OP_SAVE).wait(lk);
        }
        if (!flag) {
            lk.unlock();
            break;
        }
        while (!TaskQueue::empty(OP_SAVE)) {
            int object = TaskQueue::TQ_GiveTask<int>(OP_SAVE);
            std::unique_lock<std::mutex> SP_lk(SP_mtx[object]);
            SP_VB_object[object]->VB_Operator(VB_OP_SAVE);
        }
    }
}
void Superviser::OP_Insert()
{
    while (true&& flag)
    {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(OP_INSERT));
        while (TaskQueue::empty(OP_INSERT) && flag)
        {
            TaskQueue::TQ_GiveCV(OP_INSERT).wait(lk);
        }
        if (!flag) {
            lk.unlock();
            break;
        }
        OP_INSERT_NEED temp = TaskQueue::TQ_GiveTask<OP_INSERT_NEED>(OP_INSERT);
        lk.unlock();
        int object = std::get<0>(temp);
        int row = std::get<1>(temp);
        int col = std::get<2>(temp);
        std::string insert_s = std::get<3>(temp);
        std::unique_lock<std::mutex> SP_lk(SP_mtx[object]);
        SP_VB_object[object]->VB_Operator(VB_OP_INSERT, row, col, insert_s);
    }
}
void Superviser::OP_Modify()
{
    while (true&& flag)
    {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(OP_MODIFY));
        while (TaskQueue::empty(OP_MODIFY) && flag)
        {
            TaskQueue::TQ_GiveCV(OP_MODIFY).wait(lk);
        }
        if (!flag) {
            lk.unlock();
            break;
        }
        OP_MODIFY_NEED temp = TaskQueue::TQ_GiveTask<OP_MODIFY_NEED>(OP_MODIFY);
        lk.unlock();
        int object = std::get<0>(temp);
        std::string  original_s = std::get<1>(temp);
        std::string  replace_s = std::get<2>(temp);
        std::unique_lock<std::mutex> SP_lk(SP_mtx[object]);
        SP_VB_object[object]->VB_Operator(VB_OP_MODIFY, original_s, replace_s);
    }
}
void Superviser::OP_Delete()
{
    while (true&& flag)
    {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(OP_DELETE));
        while (TaskQueue::empty(OP_DELETE) && flag)
        {
            TaskQueue::TQ_GiveCV(OP_DELETE).wait(lk);
        }
        if (!flag) {
            lk.unlock();
            break;
        }
        OP_DELETE_NEED temp = TaskQueue::TQ_GiveTask<OP_DELETE_NEED>(OP_DELETE);
        lk.unlock();
        int object = std::get<0>(temp);
        std::string  delete_s = std::get<1>(temp);
        int num = std::get<2>(temp);
        std::unique_lock<std::mutex> SP_lk(SP_mtx[object]);
        SP_VB_object[object]->VB_Operator(VB_OP_DELETE, delete_s, num);
    }
}
void Superviser::OP_SubDelete()							//OP��Delete������
{
    while (true&& flag)
    {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(DELETE_S));
        while (TaskQueue::empty(DELETE_S) && flag)
        {
            TaskQueue::TQ_GiveCV(DELETE_S).wait(lk);
        }
        if (!flag) {
            lk.unlock();
            break;
        }
        SWITCH::CLOSE();
        while (!TaskQueue::empty(DELETE_S)) {
            int object = 0;
            DELETE_QE temp = TaskQueue::TQ_OutSubQueue<DELETE_QE>(object);
            std::unique_lock<std::mutex> SP_lk(SP_mtx[object]);
            SP_VB_object[object]->VB_SubDelete(temp);
            TaskQueue::TQ_GetTask(VB_SHOW, object);
            TaskQueue::TQ_GiveCV(VB_SHOW).notify_one();
        }
    }
}
void Superviser::OP_SubModify()						//OP��Modify����
{
    while (true&& flag)
    {
        std::unique_lock<std::mutex> lk(TaskQueue::TQ_GiveLock(MODIFY_S));
        while (TaskQueue::empty(MODIFY_S) && flag)
        {
            TaskQueue::TQ_GiveCV(MODIFY_S).wait(lk);
        }
        if (!flag) {
            lk.unlock();
            break;
        }
        SWITCH::CLOSE();
        while (!TaskQueue::empty(MODIFY_S)) {
            int object = 0; std::string replace_s = "";
            MODIFY_QE temp = TaskQueue::TQ_OutSubQueue<MODIFY_QE>(replace_s, object);
            lk.unlock();
            std::unique_lock<std::mutex> SP_lk(SP_mtx[object]);
            SP_VB_object[object]->VB_SubModify(temp, replace_s);
            TaskQueue::TQ_GetTask(VB_SHOW, object);
        }
        TaskQueue::TQ_GiveCV(VB_SHOW).notify_all();
    }
}

int main()
{
    Superviser a;
    a.SP_Console();
}