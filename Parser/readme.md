# 编译原理实验-文法分析器（Parser）![](https://img.shields.io/badge/language-cpp-green.svg)

## Table of Contents

- [Background](#background)
- [Install](*install)
- [Usage](*usage)
- [Related Efforts](#related-efforts)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)



## Background

**语法分析器**（Parser）通常是作为[编译器](https://baike.baidu.com/item/编译器/8853067)或[解释器](https://baike.baidu.com/item/解释器/10418965)的组件出现的，它的作用是进行语法检查、并构建由输入的单词组成的数据结构（一般是[语法分析树](https://baike.baidu.com/item/语法分析树/20452541)、[抽象语法树](https://baike.baidu.com/item/抽象语法树/6129952)等层次化的数据结构）。语法分析器通常使用一个独立的[词法分析器](https://baike.baidu.com/item/词法分析器/4336210)从输入字符流中分离出一个个的“单词”，并将单词流作为其输入。实际开发中，语法分析器可以手工编写，也可以使用工具（半）自动生成。

现要求设计一个LL(1)语法分析器，能够根据用户所输入的二型文法构造出其对应的LL(1)分析表，并能够判断出用户所输入的某个串是否是该文法所产生的句子

### 要求

**1**、用户在一个TXT文件中输入一个二型文法，要求如下：

- 1）文法中的终结符和非终结符各不少于2个
- 2）必须至少有1条文法规则的右部出现ε（用户以输入“@”代表输入“ε”）
- 3）文法共计至少4条规则，且至少1条规则的右部必须有2个及以上的符号
- 4）必须至少有1条规则不是“左线性”或“右线性”文法
- 5）必须至少有1个非终结符有2个或2个以上的产生式
- 6）没有冗余的文法规则，文法要能够完整地推导出某个语言

**2**、系统读取该TXT文件，展示出每条文法右部符号串的**First**集（重复的只需展示**1**次）以及每个   **非终结符**的**Follow**集。

**3、**展示出该文法所对应的**LL(1)分析表**

**4**、用户输入一个由终结符组成的字符串，系统展示出分析过程，并给出分析结论（该串“是”或“不是”文法的句子）。请分别使用**2**个字符串进行测试，分别让系统显示出“该串是文法的句子”和“该串不是文法的句子”的结论

### 流程设计

##### 设计步骤流程描述

1、读入原程序；

2、从原程序中提取规则、非终结符、终结符、右部，并输出；

3、对于提取出的右部、非终结符，分别对他们求First集和Follow集；

4、根据提取出来的终结符、非终结符、规则以及相应的First集和Follow集构建LL（1）分析表；

5、根据构建好的分析表结构，要求用户输入字符串，并分析是否符合文法； 

##### 设计步骤流程图

![img](https://i.loli.net/2021/05/01/3cStjIFw2J4ykXP.png)

### 设计过程

设计过程将从数据需求着手，分析制定合理简约的数据结构，以确保代码易读、优雅。

**数据结构：**

- 1、规则-结构体：考虑到数据存储和输出，需要为常用数据—规则定义专用数据结构，主要分为两部分，一部分是左部，由于至多一个字符，故用字符类型定义；一部分是右部，由于右部有可能有好几种选择，故用一个字符串动态数组定义，又考虑到规则需要经常被输出，于是重载输出流操作符；

  ```cpp
  struct rule {
  	char left;                 //规则左部 
  	vector<string> right;        //规则右部
  	friend ostream& operator << (ostream& out, rule rule);//重载输出流操作符
  };
  ```

- 2、F（irst/ollow）集-结构体：考虑到要格式化输出First/Follow集，又考虑到这两类集数据结构基本相似，于是为这两类数据定义一个通用的数据结构，并通过一个判别符is_first加以区分，并在重载的输出流操作符函数里根据该判别符输出不同的提示信息；

  ```cpp
  struct F_set {
  	bool is_first;              //First集 or Follow集判别符
  	string left;              //F集左部
  	set<char> right;          //F集右部
  	friend ostream& operator << (ostream& out, F_set f_set);//重载输出流操作符
  };
  ```

- 3、LL（1）分析表-结构体：考虑到要格式化输出分析表，仅用一个动态数组存放规则显然是不够的，于是定义如下结构体， 在数组之外，指明表格的横纵表头，并通过重载输出流操作符进行格式化输出，也使得主函数简单、易读；

  ```cpp
  struct LL_1_table {
  	vector<char> row;          //行表头数组
  	vector<char> col;           //列表头数组
      vector<rule> rules;          //规则数组
  	friend ostream& operator << (ostream& out, LL_1_table table);//重载输出流操作符
  };
  ```

##### 功能实现描述

功能实现主要分为文件读写模块、主要元素提取模块、First集求取模块、Follow集求取模块、LL(1)分析表构建模块、文法合规判断模块这六大模块，分别完成程序预处理功能、求First、Follow集、构建分析表、文法合规判断的任务。单词-类别信息数据结构：

![img](https://i.loli.net/2021/05/01/r7uQi1jWfyRF53A.png)

## Usage

- 测试数据**1**：

  文法：

  > E::=a|b|(T)  T::=EG  G::=,EG|@

  待检字符串：

  > (a,a)

- 运行结果：

  ![img](https://i.loli.net/2021/05/01/oeZy3LhUAunOEJ6.png)

- 测试**2**：

  测试数据：

  > E::=aH    H::=aMd|d    M::=Ab|@   A::=aM|e

  待检字符串：

  > aaabd

- 测试**2**运行结果：

  ![img](file:///C:/Users/ddycf/AppData/Local/Temp/msohtmlclip1/01/clip_image008.png)

## Related Efforts

## Maintainers

[@DenryDu](https://github.com/DenryDu)

## Contributors

## License

***

Support from you is my greatest encouragement! (您的支持是对我的最大鼓励！)       
