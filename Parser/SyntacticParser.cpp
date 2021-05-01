#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <iomanip>
#include <stack>
using namespace std;

/**
 * Description: 各类函数与结构体定义
 * Author: 杜登宇
 */

//加载文件
string load_file(string file_name);

//定义规则结构体
struct rule {
	char left;
	vector<string> right;
	friend ostream& operator << (ostream& out, rule rule);
};
//从文本提取规则、终结符、非终结符、右部
void split_into_rules(const string text, vector<rule>& rules, set<char>& non_terminal, set<char>& terminal, set<string>& right_parts);
//检查规则情况
void check_rules(const vector<rule> rules);
//检查set集情况
template <class T>
void check_set(set<T> set_, string tip);

//定义F（irst/ollow）集结构体
struct F_set {
	bool is_first;
	string left;
	set<char> right;
	friend ostream& operator << (ostream& out, F_set f_set);
};
//查找First集
F_set findFirst(const string right_part, vector<rule> rules);
//查找Follow集
F_set findFollow(const char non_terminal, vector<rule> rules);
//输出一个set集合里存放的字符对应的First集集合
vector<F_set> checkFirst(set<string> right_parts, vector<rule> rules);
//输出一个set集合里存放的字符对应的Follow集集合
vector<F_set> checkFollow(set<char> non_terminal, vector<rule> rules);

//定义LL（1）分析表结构体
struct LL_1_table {
	vector<char> row;
	vector<char> col;
	vector<rule> rules;
	friend ostream& operator << (ostream& out, LL_1_table table);
};
//分析表构建
LL_1_table LL_1(set<char>& terminal, set<char>& non_terminal, vector<rule> rules);
void print_state(stack<char> analysis_s, stack<char> input_s, rule& rule_temp);

//根据分析表进行语法分析
void syntacticAnalysis(LL_1_table table);

/**
* Description: 主函数，简略显示分析过程
* Author: 杜登宇
*/
int main() {
	string file_name = "./test3.txt";

	//调用函式读取文件
	string text = load_file(file_name);

	//预定义变量存放终结符、非终结符、右部以及文法规则
	set<char> non_terminal;
	set<char> terminal;
	set<string> right_parts;
	vector<rule> rules;

	//调用函式对字符串进行终结符、非终结符、右部以及文法规则的提取
	split_into_rules(text, rules, non_terminal, terminal, right_parts);

	//检查提取出来的终结符、非终结符、右部以及文法规则是否正确
	check_rules(rules);
	check_set(right_parts, "提取规则右部");
	check_set(non_terminal, "提取非终结符");
	check_set(terminal, "提取终结符");

	//求右部的First集
//	F_set f = findFirst("AB", rules);
	vector<F_set> first_sets = checkFirst(right_parts, rules);
	//求非终结符的Follow集
	vector<F_set> follow_sets = checkFollow(non_terminal, rules);

	//进行LL（1）分析表的构建
	LL_1_table table = LL_1(terminal, non_terminal, rules);
	//输出分析表
	cout << table;

	//根据构建好的分析表进行语法分析
	syntacticAnalysis(table);
	
	return 0;
}
/**
 * Description: 函数实现以及各类操作符重载
 * Author: 杜登宇
 **/

string load_file(string file_name) {
	fstream fs;
	fs.open(file_name.c_str(), ios::in);
	if (!fs) {
		cout << "文件不存在!";
		return "";
	}
	istreambuf_iterator<char> beg(fs), end;
	string code(beg, end);
	cout << "读取文件中..." << endl;
	cout << "文件内容如下：" << endl;
	cout << code << endl;
	fs.close();
	return code;
}
ostream& operator << (ostream& out, rule rule) {
	string ret;
	if (rule.left != ' ') {
		ret = ret + rule.left + "::=";
		for (int i = 0; i < rule.right.size(); i++) {
			if (i == 0)
				ret = ret + rule.right[i];
			else {
				ret = ret + ('|' + rule.right[i]);
			}
		}
	}
	out << ret;

	return out;
}
void split_into_rules(const string text, vector<rule> &rules, set<char> &non_terminal, set<char> &terminal, set<string> &right_parts) {
	
	//判断接下来是否是右部
	bool is_right = false;
	string temp_str = "";
	rule temp_rule;
	for (int i = 0; i < text.size(); i++) {
		char temp_ch = text[i];
		//如果在左部
		if (!is_right) {
			//直接跳过空格和冒号
			if (temp_ch == ' ' || temp_ch == ':') {
				continue;
			}
			//遇到等于号切换左右部
			else if (temp_ch == '=') {
				is_right = true;
			}
			//遇到非空字符直接放进rule左部
			else {
				temp_rule.left = temp_ch;
				non_terminal.insert(temp_ch);
			}
		}
		//如果在右部
		else {
			//遇到空格说明rule结束，切换左右部，同时将rule放进rules
			if (temp_ch == ' ') {
				temp_rule.right.push_back(temp_str);
				right_parts.insert(temp_str);
				rules.push_back(temp_rule);

				//对rule进行重新初始化
				temp_rule = rule();
				temp_str = "";
				is_right = false;
			}
			//如果遇到|，则将字符串放入rule右部，同时初始化字符串
			else if (temp_ch == '|') {
				temp_rule.right.push_back(temp_str);
				right_parts.insert(temp_str);
				temp_str = "";
			}
			//其他字符直接加在字符串后
			else {
				temp_str = temp_str + temp_ch;
				if (!isupper(temp_ch))
					terminal.insert(temp_ch);
				if (i + 1 == text.size()) {
					temp_rule.right.push_back(temp_str);
					right_parts.insert(temp_str);
					rules.push_back(temp_rule);
				}
			}
		}
	}
}
void check_rules(const vector<rule> rules) {
	cout << endl;
	cout << "提取规则：" << endl;
	for (int i = 0; i < rules.size(); i++) {
		cout <<std::left <<setw(15) << rules[i];
	}
}
template <class T>
void check_set(set<T> set_, string tip) {
	cout << endl;
	cout << endl << tip << endl;
	for (set<T>::iterator it = set_.begin(); it != set_.end(); it++) {
		cout << *it << " ";
	}

}
ostream& operator << (ostream& out, F_set f_set) {
	string temp_str;
	if (f_set.is_first) {
		temp_str = " First";
	}
	else {
		temp_str = " Follow";
	}
	out << temp_str << "(";
	out << f_set.left;
	out << ")={";
	for (set<char>::iterator it = f_set.right.begin(); it != f_set.right.end(); it++) {
		if (it != f_set.right.begin())
			out << ',';
		out << *it;
	}
	out << "} ";
	return out;
}
F_set findFirst(const string ss, vector<rule> rules) {
	F_set f_set;
	f_set.is_first = true;
	f_set.left = ss;
	//bool flag = false;
	int i = 0;
	while( i < ss.size()) {
		bool flag = false;
		char temp_ch = ss[i];
		//终结符
		if (!isupper(temp_ch)) {
			f_set.right.insert(temp_ch);
			return f_set;
		}
		//非终结符, 如A
		else{
			//遍历查找左部与该大写字母相同的规则
			for (int i = 0; i < rules.size(); i++) {
				if (rules[i].left == temp_ch) {
					//找到匹配的规则，如A::= xxx |xxxx
					//遍历递归规则右部
					for (int j = 0; j < rules[i].right.size(); j++) {
						//xxx
						string temp_str = rules[i].right[j];
						//若xxx非空
						if (temp_str[0] != '@') {
							//继续对右部求First集
							F_set f_set_temp = findFirst(temp_str, rules);
							for (set<char>::iterator it = f_set_temp.right.begin(); it != f_set_temp.right.end(); it++) {
								if (*it == '@') {
									flag = true;
									continue;
								}
								f_set.right.insert(*it);
							}
						}
						//若xxx为空
						else {
							flag = true;
						}
					}
				}
			}
		}
		if (flag) {
			if (i == ss.size() - 1)
				f_set.right.insert('@');
			i++;
		}
		else {
			return f_set;
		}
	}
	return f_set;
}
vector<F_set> checkFirst(set<string> right_parts, vector<rule> rules) {
	vector<F_set> f_sets;
	cout << endl << endl << "First集：" << endl;
	for (set<string>::iterator it = right_parts.begin(); it != right_parts.end(); it++) {
		F_set f_set_temp = findFirst(*it, rules);
		cout << f_set_temp;
		f_sets.push_back(f_set_temp);
	}
	cout << endl;
	return f_sets;
}
F_set findFollow(const char non_terminal, vector<rule> rules) {
	F_set f_set;
	f_set.is_first = false;
	f_set.left = non_terminal;
	bool flag = false;
	for (int i = 0; i < rules.size(); i++) {
		//每次查找首先存放好左部备用
		char temp_ch_n = rules[i].left;
		for (int j = 0; j < rules[i].right.size(); j++) {
			//拿出右部备用
			string temp_str = rules[i].right[j];
			for (int k = 0; k < temp_str.size(); k++) {
				char temp_ch = temp_str[k];
				//如果匹配上了
				if (temp_ch == non_terminal) {
					//判断该终结符是否出现在右部的末尾
					//如果出现在末尾，找左部follow集加上
					if ((k + 1) == temp_str.size()) {
						if (non_terminal != temp_ch_n) {
							F_set f_set_temp = findFollow(temp_ch_n, rules);
							for (set<char>::iterator it = f_set_temp.right.begin(); it != f_set_temp.right.end(); it++) {
								f_set.right.insert(*it);
							}
						}
					}
					//如果不是出现在末尾
					else {
						//如果是非终结符
						if (isupper(temp_str[k + 1])) {
							if (temp_str[k + 1] != non_terminal) {
								string temp_str_2 = string(1, temp_str[k + 1]);
								F_set f_set_temp = findFirst(temp_str_2, rules);
								for (set<char>::iterator it = f_set_temp.right.begin(); it != f_set_temp.right.end(); it++) {
									if (*it == '@') {
										flag = true;
										continue;
									}
									f_set.right.insert(*it);
								}
							}
						}
						//如果是终结符
						else {
							flag = true;
							f_set.right.insert(temp_str[k + 1]);
						}
					}
				}
			}
		}
	}
	f_set.right.insert('#');
	return f_set;
}
vector<F_set> checkFollow(set<char> non_terminal, vector<rule> rules) {
	vector<F_set> f_sets;
	cout << endl << endl << "Follow集：" << endl;
	for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++) {
		F_set f_set_temp = findFollow(*it, rules);
		cout << f_set_temp;
		f_sets.push_back(f_set_temp);
	}
	cout << endl;
	return f_sets;
}
LL_1_table LL_1(set<char>& terminal, set<char>& non_terminal, vector<rule> rules) {
	//从终结符中提取表头
	vector<char> table_head;
	for (set<char>::iterator it = terminal.begin(); it != terminal.end(); it++) {
		if (*it != '@')
			table_head.push_back(*it);
	}
	table_head.push_back('#');

	//定义LL1分析表结构并初始化
	LL_1_table table;
	for (set<char>::iterator it = non_terminal.begin(); it != non_terminal.end(); it++) {
		table.row.push_back(*it);
	}
	for (int i = 0; i < table_head.size(); i++) {
		table.col.push_back(table_head[i]);
	}
	for (int i = 0; i < table.row.size() * table.col.size(); i++) {
		rule rule_temp;
		rule_temp.left = ' ';
		table.rules.push_back(rule_temp);
	}

	//开始逐条分析规则
	for (int i = 0; i < rules.size(); i++) {
		//拿到规则
		rule rule_temp = rules[i];
		//遍历规则的不同右部
		for (int j = 0; j < rule_temp.right.size(); j++) {
			//拿到右部，获取右部的first集
			set<char> first_set = findFirst(rules[i].right[j], rules).right;
			//遍历first集
			for (set<char>::iterator it = first_set.begin(); it != first_set.end(); it++) {
				//如果First集中非空
				if (*it != '@') {
					//定位行号和列号
					int col = distance(table.col.begin(), find(table.col.begin(), table.col.end(), *it));
					int row = distance(table.row.begin(), find(table.row.begin(), table.row.end(), rule_temp.left));
					//将规则左部-》当前部分右部存入表格
					table.rules[row * table.col.size() + col].left = rule_temp.left;
					table.rules[row * table.col.size() + col].right.push_back(rule_temp.right[j]);
				}
				//如果first集中有空
				else {
					//获取规则左部的Follow集
					set<char> follow_set = findFollow(rules[i].left, rules).right;
					//遍历Follow集
					for(set<char>::iterator it_ = follow_set.begin();it_!=follow_set.end();it_++){
						//定位行号和列号
						int col = distance(table.col.begin(), find(table.col.begin(), table.col.end(), *it_));
						int row = distance(table.row.begin(), find(table.row.begin(), table.row.end(), rule_temp.left));
						//将规则左部-》当前部分右部存入表格
						table.rules[row * table.col.size() + col].left = rule_temp.left;
						table.rules[row * table.col.size() + col].right.push_back(rule_temp.right[j]);
					}
				}
			}
		}
		
	}
	return table;
}
ostream& operator << (ostream& out, LL_1_table table) {
	out << endl << "LL(1)分析表如下：" << endl;
	int width = 10;
	out <<setw(3) << " ";
	for (int i = 0; i < table.col.size(); i++) {
		out <<setw(width) << table.col[i];
	}
	out << endl;

	for (int i = 0; i < table.rules.size(); i++) {
		if (i % table.col.size() == 0) {
			out << setw(3) << table.row[i / table.col.size()];
		}
		out <<setw(width) << table.rules[i];
		if ((i + 1) % table.col.size() == 0) {
			out << endl;
		}
	}
	return out;
}
void print_state(stack<char> analysis_s, stack<char> input_s, rule& rule_temp) {
	string left_str, right_str;
	char temp_char;
	int len_left = analysis_s.size();
	int len_right = input_s.size();

	stack<char> temp;
	for (int i = 0; i < len_left; i++) {
		temp.push(analysis_s.top());
		analysis_s.pop();
	}
	for (int i = 0; i < len_left; i++) {
		temp_char = temp.top();
		left_str = left_str + temp_char;
		temp.pop();
	}
	for (int i = 0; i < len_right; i++) {
		temp_char = input_s.top();
		right_str = right_str + temp_char;
		input_s.pop();
	}
	cout <<endl<< std::left << setw(10) << left_str << std::right << setw(10) << right_str << setw(5) <<" "<< std::left << setw(20) << rule_temp;
}
void syntacticAnalysis(LL_1_table table) {
	cout << endl << "接下来进行语法分析" << endl << "请输入字符串：" << endl;
	//等待用户输入字符串以进行分析
	string input;
	cin >> input;

	//将该字符串放入栈中
	stack<char> input_s;
	input_s.push('#');
	for (int i = input.size() - 1; i >= 0; i--) {
		input_s.push(input[i]);
	}
	stack<char> analysis_s;
	analysis_s.push('#');
	analysis_s.push('E');

	cout << std::left << setw(10) << "分析栈" << std::right << setw(10) << "预留输入串栈" << setw(5) << "" << std::left << setw(10) << "产生式";
	bool flag = true;
	while (!(analysis_s.empty() && input_s.empty())) {
		char left_ch = analysis_s.top();
		char right_ch = input_s.top();
		//如果左侧栈顶为非终结符
		if (isupper(left_ch)) {
			//到分析表中查找元素
			int row = distance(table.row.begin(), find(table.row.begin(), table.row.end(), left_ch));
			int col = distance(table.col.begin(), find(table.col.begin(), table.col.end(), right_ch));
			//取出对应规则
			rule rule_temp = table.rules[row * table.col.size() + col];
			//如果规则非空
			if (rule_temp.left != ' ') {
				//输出当前状况
				print_state(analysis_s, input_s, rule_temp);
				//分析栈顶出栈
				analysis_s.pop();
				//取出规则右部进行入栈操作(右部非空情况下）
				if (rule_temp.right[0][0] != '@') {
					string temp_str = rule_temp.right[0];
					for (int i = temp_str.size() - 1; i >= 0; i--) {
						analysis_s.push(temp_str[i]);
					}
				}
			}
			else {
				print_state(analysis_s, input_s, rule_temp);
				flag = false;
				break;
			}
		}
		//如果左侧栈顶为终结符
		else {
			if (left_ch == right_ch) {
				//输出当前状况
				rule rule_temp;
				rule_temp.left = ' ';
				print_state(analysis_s, input_s, rule_temp);

				analysis_s.pop();
				input_s.pop();
			}
			else {
				bool flag = false;
				break;
			}
		}

	}
	cout<<endl << "字符串" << input;
	if (!flag)
		cout << "不";
	cout << "是该文法的句子";
}