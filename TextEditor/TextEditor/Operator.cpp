#include "Operator.h"
Operator::Operator()
{

}

Operator::~Operator()
{

}

TempCommend tree[32] =
{
	{NOBODY,-1,-1,NOBODY},//第0个弃用
	{CO_OPEN,-1,-1,CO_OPEN},//第一个
	{CO_DELETE,6,7,NOBODY},{CO_INSERT,8,9,NOBODY},{CO_MODIFY,10,11,NOBODY},{CO_FIND,12,13,NOBODY},{INT_M,14,-1,NOBODY},
	{ALL ,15,-1,NOBODY},{INT_M,16,-1,NOBODY},{ALL,17,-1,NOBODY},{INT_M,18,-1,NOBODY},{ALL,19,-1,NOBODY},
	{INT_M,20,-1,NOBODY},{ALL,21,-1,NOBODY},
	{STRING,22,23,NOBODY},{STRING,24,25,NOBODY},{INT_M,26,-1,NOBODY},{INT_M,27,-1,NOBODY},{STRING,28,-1,NOBODY},{STRING,29,-1,NOBODY},
	{STRING,-1,-1,CO_INT_FIND},{STRING,-1,-1,CO_ALL_FIND},
	{INT_M,-1,-1,CO_INT_DELTETE_INT},{ALL,-1,-1,CO_INT_DELTETE_ALL},{INT_M,-1,-1,CO_ALL_DELTETE_INT},{ALL,-1,-1,CO_ALL_DELTETE_ALL},
	{INT_M,30,-1,NOBODY},{INT_M,31,-1,NOBODY},{STRING,-1,-1,CO_INT_MODIFY},{STRING,-1,-1,CO_ALL_MODIFY},
	{STRING,-1,-1,CO_INT_INSERT},{STRING,-1,-1,CO_ALL_INSERT}
};
//检测是否满足字符串的特质
bool Operator::OP_StringAnalysis(std::string commend)
{
	for (auto i = commend.begin(); i < commend.end(); i++)
	{
		if (('a' < *i && *i < 'z')|| ('A' < *i && *i < 'Z'))
			continue;
		else 
			return false;
	}
	return true;
}

bool Operator::OP_IntAnalysis(std::string commend)
{
	for (auto i = commend.begin(); i < commend.end(); i++)
	{
		if (('0' > *i || *i > '9'))
			return false;
	}
	return true;
}
//分解子结构，检查类型，命令
SET_OF_COMMEND Operator::OP_TypeAnalysis(std::string commend)
{
	if (commend == set_of_commend.CO_OPEN) {
		return CO_OPEN;
	}
	else if (commend == set_of_commend.CO_DELETE) {
		return CO_DELETE;
	}
	else if (commend == set_of_commend.CO_INSERT) {
		return CO_INSERT;
	}
	else if (commend == set_of_commend.CO_MODIFY) {
		return CO_MODIFY;
	}
	else if (commend == set_of_commend.CO_FIND) {
		return CO_FIND;
	}
	else if (commend == set_of_commend.CO_SAVE) {
		return CO_SAVE;
	}
	else if (commend == set_of_commend.CO_INFOR) {
		return CO_INFOR;
	}
	else if (OP_IntAnalysis(commend)) {
		return INT_M;
	}
	else if (OP_StringAnalysis(commend)) {
		return STRING;
	}
	else if (commend == "ALL") {
		return ALL;
	}
	return NOBODY;
}
SET_OF_COMMEND Operator::sub_back(std::vector<std::string>& commend_vec, int i,int j)
{
	SET_OF_COMMEND temp = OP_TypeAnalysis(commend_vec[j]);
	if (temp == tree[i].commend && tree[i].end_commend != NOBODY) {
		return tree[i].end_commend;
	}
	SET_OF_COMMEND end = NOBODY;
	if (temp == tree[i].commend && tree[i].next_left != -1) {
		end = sub_back(commend_vec, tree[i].next_left, j + 1);
		if (end != NOBODY) return end;
	}
	if (temp == tree[i].commend && tree[i].next_right != -1)
		end = sub_back(commend_vec, tree[i].next_right,j + 1);
	return end;
}	
//本质是回溯，但为了实现简单，砍掉了许多分支
SET_OF_COMMEND Operator::BackingFind(std::vector<std::string>& commend_vec)
{
	SET_OF_COMMEND temp = OP_TypeAnalysis(commend_vec[0]);
	if (temp == CO_SAVE || temp == CO_INFOR) {
		return temp;
	}
	for (int j = 1; j <= 5; j++)
	{
		if (tree[j].commend == temp)
			return sub_back(commend_vec,j,0);
	}
	return NOBODY;
}

//对命令进行分解处理,不要忘了，commend是主控中心传入的，所以只需要负责检测语法正确性
//以及将对应命令传出即可，然后主控根据命令处理参数
SET_OF_COMMEND Operator::OP_control(std::string commend)
{
	std::stringstream iss(commend);
	//因为命令设定最长就五段，所以只要流的前五个
	std::vector<std::string> Commend_Vec = std::vector<std::string>(5);
	int i = 0; std::string temp = "";
	while (i < 5 && iss >> temp)
	{
		Commend_Vec[i] = temp;
		i++;
	}
	if (commend == "Y") return YES;
	if (commend == "N") return NO;
	return BackingFind(Commend_Vec);
}