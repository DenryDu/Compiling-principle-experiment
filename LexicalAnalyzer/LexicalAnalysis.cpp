// LexicalAnalysis.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include <set>
#include <iomanip>
#include <map>
using namespace std;


//美化输出
void printTask(const string tip,const string placeholder) {
	int len = 80 - tip.length();
	int left = len / 2;
	int right = len - left;
	cout << "|";
	for (int i = 0; i < left; i++) {
		cout << placeholder;
	}
	cout << tip;
	for (int i = 0; i < right; i++) {
		cout << placeholder;
	}
	cout << "|" << endl;
}
void printTips(const string tip) {
	cout << "|********************************************************************************|" << endl;
	printTask(tip,"*");
	cout << "|********************************************************************************|" << endl;
	cout << endl;
}

//单词类别信息
struct wtPair{
	int idx;
	int type;
	string value;
	wtPair(int a, int b, string c) :idx(a), type(b), value(c) {}
};

//预处理程序
bool preProcess(const string fileName, const string newFileName) {
	printTips("开始程序预处理");
	//定义文件流 
	fstream fs;
	//供读使用
	fs.open(fileName.c_str(), ios::in);
	//判空 
	if (!fs) {
		cout << "file not exsist!";
		return false;
	}
	//将文件中字符串写入code字符串中
	istreambuf_iterator<char> beg(fs), end;
	string code(beg, end);
	printTask("读入原程序：","-");
	cout << endl << code << endl;
	fs.close();

	//对缓冲字符串做去行注释的处理 
	int idxBeg = -1;
	int idxEnd = -1;
	//去除单行注释
	while ( code.find("//") != code.npos) {
		idxBeg = code.find("//");
		idxEnd = code.find('\n', idxBeg);
		code.erase(idxBeg, idxEnd - idxBeg);
	}
	//去除多行注释
	while (code.find("/*") != code.npos) {
		idxBeg = code.find("/*");
		idxEnd = code.find("*/", idxBeg);
		code.erase(idxBeg, idxEnd - idxBeg+2);
	}
	//去除换行
	while (code.find('\n') != code.npos) {
		code.erase(code.find('\n'),1);
	}
	//去除制表符
	while (code.find('\t') != code.npos) {
		code.erase(code.find('\t'), 1);
	}
	//去除多余的空格
	int flag = 0;
	while (code.find(' ',flag) != code.npos) {
		idxBeg = code.find(' ', flag);
		idxEnd = code.find_first_not_of(" ", idxBeg);
		int len = idxEnd - idxBeg - 1;
		code.erase(idxBeg, len);
		flag = idxEnd-len;	
	}
	printTask("预处理完成后代码：","-");
	cout << endl << code << endl << endl;

	fs.open(newFileName.c_str(), ios::out);
	fs << code.c_str();
	fs.close();
	printTask("预处理后代码写入文件成功","-");
	cout << endl;

	return true;
}

//判断单词是否是变量名
bool IsIdentifier(const string word) {
	bool flagStatus=false;
	if (isalpha(word[0]) || word[0] == '_') {
		for (int i = 1; i < int(word.length()); i++) {
			if ( !(isdigit(word[i]) || isalpha(word[i]) || word[i] == '_') ) {
				return flagStatus;
			}
		}
		flagStatus = true;
	}
	return flagStatus;
}
//判断单词是否是数字
bool IsNum(const string word) {
	bool flagStatus = false;
	for (int i = 1; i < word.length(); i++) {
		if (!(isdigit(word[i]))) {
			return flagStatus;
		}
	}
	flagStatus = true;
	return flagStatus;
}
//判断单词类型
int wtMatch(const string word) {
	//单词-类型映射：word-type map
	map<string, int> wtMap;
	map<string, int>::iterator iter;
	wtMap["void"] = 1;
	wtMap["main"] = 2;
	wtMap["int"] = 3;
	wtMap["cout"] = 4;
	wtMap["return"] = 5;
	wtMap["("] = 6;
	wtMap[")"] = 7;
	wtMap["{"] = 8;
	wtMap["}"] = 9;
	wtMap["<<"] = 10;
	wtMap[";"] = 11;
	wtMap[","] = 12;
	wtMap["="] = 13;
	wtMap["*"] = 14;
	wtMap["/"] = 15;

	iter = wtMap.find(word.c_str());
	if (iter != wtMap.end())
		return iter->second;
	else {
		if (IsIdentifier(word))	return 16;
		else if (IsNum(word)) return 17;
		else return 18;
	}

}
string findError(const string fileName, const string value) {
	//定义文件流 
	fstream fs;
	//供读使用
	fs.open(fileName.c_str(), ios::in);

	//判空 
	if (!fs) {
		cout << "file not exsist!";
		return "";
	}
	//子串定位
	int row = 0;
	int column = 0;
	string ret;
	char rowBuff[81];
	while (fs.getline(rowBuff, 81)) {
		row++;
		string temp(rowBuff);
		if (temp.find(value) != temp.npos) {
			column = temp.find(value);
			ostringstream oss;
			oss << "第" << row << "行" << "第" << column << "列";
			ret = ret + oss.str();
		}
	}
	fs.close();
	return ret;
}
bool mainProcess(const string fileName, const string fileNameProcessed) {
	printTips("开始后续处理");
	//定义文件流 
	fstream fs;
	//供读使用
	fs.open(fileName.c_str(), ios::in);
	//判空 
	if (!fs) {
		cout << "file not exsist!";
		return false;
	}
	//将文件中字符串写入code字符串中
	istreambuf_iterator<char> beg(fs), end;
	string code(beg, end);
	printTask("读入预处理后程序：","-");
	cout << endl << code << endl;
	fs.close();

	vector<wtPair> wtSeq;
	vector<string> errorSeq;
	int idx = 1;
	//进行单词分割
	int idxBeg = 0;
	int idxEnd = 0;
	while(idxEnd < code.length()) {
		//遇到空格直接跳
		if (code[idxBeg] == ' ') {
			idxBeg++;
			idxEnd++;
		}
		//如果首位遇到符号(非下划线)
		else if (ispunct(code[idxBeg]) && code[idxBeg]!='_') {
			if (code[idxBeg] != '<') {
				wtSeq.push_back(wtPair(idx, wtMatch(string(1, code[idxBeg])), string(1, code[idxBeg])));
				idx++;
				idxBeg++;
				idxEnd++;
			}
			else {
				if (code[idxBeg + 1] == '<') {
					wtSeq.push_back(wtPair(idx, wtMatch( string(code).substr(idxBeg, 2) ), string(code).substr(idxBeg, 2)));
					idx++;
					idxBeg += 2;
					idxEnd += 2;
				}
			}
		}
		//如果首位非符号
		else{
			while (idxEnd < code.length()) {
				idxEnd++;
				//字符串终止判断
				if ( (ispunct(code[idxEnd]) && code[idxEnd] != '_') || code[idxEnd]==' ') {
					wtSeq.push_back(wtPair(idx, wtMatch( string(code).substr(idxBeg, idxEnd - idxBeg)), string(code).substr(idxBeg, idxEnd - idxBeg)));
					idx++;
					idxBeg = idxEnd;
					break;
				}
			}
		}
	}
	printTask("单词类别表：", "-");
	cout << setfill(' ') << setw(8) << "单词序号"
		<< setfill(' ') << setw(12) << "单词类别"
		<< setfill(' ') << setw(12) << "单词" << '\n';
	for (size_t i = 0; i < wtSeq.size(); i++)
	{
		if (wtSeq[i].type != 18) {
			cout << setfill(' ') << setw(8) << wtSeq[i].idx
				<< setfill(' ') << setw(12) << wtSeq[i].type
				<< setfill(' ') << setw(12) << wtSeq[i].value << '\n';
		}
		else {
			errorSeq.push_back(wtSeq[i].value);
		}
	}
	//去重复
	set<string>s(errorSeq.begin(), errorSeq.end());
	errorSeq.assign(s.begin(), s.end());

	//输出不合规单词
	string originFile = "./testCpp.txt";
	cout << "不合规单词：";
	for (size_t i = 0; i < errorSeq.size(); i++)
	{
		cout << endl << "  " << errorSeq[i] << ": " << endl << "    ";
		for (size_t j = 0; j < wtSeq.size(); j++)
		{
			if (wtSeq[j].value ==errorSeq[i]) {
				cout <<  "第" << wtSeq[j].idx << "个单词;";
			}
		} 
		cout<<endl<<"    "<<"位于" << findError(originFile, errorSeq[i]) << endl;
	}

	//同样的内容写入文件
	fs.open(fileNameProcessed.c_str(), ios::out);
	fs << setfill(' ') << setw(8) << "单词序号"
		<< setfill(' ') << setw(12) << "单词类别"
		<< setfill(' ') << setw(12) << "单词" << '\n';
	for (size_t i = 0; i < wtSeq.size(); i++)
	{
		if (wtSeq[i].type != 18) {
			fs << setfill(' ') << setw(8) << wtSeq[i].idx
				<< setfill(' ') << setw(12) << wtSeq[i].type
				<< setfill(' ') << setw(12) << wtSeq[i].value << '\n';
		}
	}
	//输出不合规单词到txt文件
	fs << "不合规单词：";
	for (size_t i = 0; i < errorSeq.size(); i++)
	{
		fs << endl << "  " << errorSeq[i] << ": " << endl << "    ";
		for (size_t j = 0; j < wtSeq.size(); j++)
		{
			if (wtSeq[j].value == errorSeq[i]) {
				fs << "第" << wtSeq[j].idx << "个单词;";
			}
		}
		fs << endl << "    " << "位于" << findError(originFile, errorSeq[i]) << endl;
	}
	fs.close();
	printTask("单词-类别表写入文件成功", "-");
	return true;
}


int main()
{
	string fileName = "./testCpp.txt";
	string afterPreProcess = "./afterPreProcess.txt";
	string processed = "./processed.txt";
	printTips("开始词法分析");
	if (preProcess(fileName, afterPreProcess)) {
		if (mainProcess(afterPreProcess, processed)) {
			printTips("词法分析已完成");
		}
	}
}
