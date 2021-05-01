# 编译原理实验-词法分析器（Lexical Analyzer）![](https://img.shields.io/badge/language-cpp-green.svg)

## Table of Contents

- [Background](#background)
- [Install](*install)
- [Usage](*usage)
- [Related Efforts](#related-efforts)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)



## Background

词法分析（lexical analysis）是计算机科学中将字符序列转换为单词（Token）序列的过程。进行词法分析的程序或者函数叫作词法分析器（Lexical analyzer，简称Lexer），也叫[扫描器](https://baike.baidu.com/item/扫描器)（Scanner）。词法分析器一般以函数的形式存在，供[语法分析器](https://baike.baidu.com/item/语法分析器/10598664)调用。

现要求编写一个面向高级程序设计语言的简单**词法分析**程序，能够识别出标识符、关键字、十进制整数和运算符

### 要求

**1**、用户在一个txt文件中输入一段有意义的高级语言源程序，要求如下：

- 1）整个程序不少于50个字符（不算空格）

- 2）程序中需包括合法的标识符、关键字、十进制整数和运算符

- 3）语言类型自选

- 4）程序中必须有回车换行和注释。允许输入“/* */”和“//”两种注释，且前者可跨行

- 5）程序中需含有至少一个非法的标识符

- 6）程序中需出现多余的空格

**2**、词法分析程序读入上述文件，并进行预处理（删除回车换行、注释），要求如下：

- 1）将预处理后的源程序以字符流的形式显示出来

- 2）以一个新txt文件的形式，保存在和原txt文件相同的文件夹内

**3**、词法分析器扫描预处理后的源程序，识别各单词。要求如下：

- 1）识别出的单词以<序号，类别码，值>的形式显示，并以一个新txt文件的形式，保存在和原txt文件相同的文件夹内。序号是指，该单词是预处理后的源程序中的第几个单词。

- 2） 对于非法标识符，跳过不显示，但需在最后给出出错信息。出错信息包括：出错标识符的序（预处理后的源程序中的第几个单词）、出错的标识符

### 流程设计

##### 设计步骤流程描述

1、读入原程序，进行预处理，包括去除注释、多余空格、制表符、换行符，再将预处理过后的代
码存入新文件中； 
2、从文件中读取预处理过后的代码，进行分词操作，分词的同时对单个单词进行词法分析，存入
结构体容器中； 
3、从容器中输出合规单词，并同时输出单词-类型表到控制台和文件中；、 
4、从容器中挑出不合规单词，根据单词的值进行去重复，读取原始程序文件，通过逐行读入字符
串，进行不合规单词的定位工作，最后将错误定位信息同时输出到控制台和文件中。 

##### 设计步骤流程图

![总体流程图](https://i.loli.net/2021/05/01/wkG9sNVZzu7rmjO.png)

### 设计过程

设计过程将从主要数据结构和主要映射关系进行阐述，并对重要部分，尤其是词法分析模块中的
逻辑通过状态转换图和有穷自动机的形式进行描述：

##### 单词-类别信息数据结构：

```cpp
struct wtPair{ 
  int idx; 
  int type; 
  string value; 
  wtPair(int a, int b, string c) :idx(a), type(b), value(c) {} 
}; 
```

##### 单词-类型关系表：

| 单词   | 类型 | 单词       | 类型 |
| ------ | ---- | ---------- | ---- |
| void   | 1    | <<         | 10   |
| main   | 2    | ;          | 11   |
| int    | 3    | ,          | 12   |
| cout   | 4    | =          | 13   |
| return | 5    | *          | 14   |
| (      | 6    | /          | 15   |
| )      | 7    | 标识符     | 16   |
| {      | 8    | 十进制整数 | 17   |
| }      | 9    | Install    |      |

##### 状态转换图

- 识别标识符的状态转换图

  ![变量名状态转换图](https://i.loli.net/2021/05/01/dXuCcx5LwRYyVhe.png)

- 识别十进制整数的状态转换图

  ![识别整常数的状态转换图](https://i.loli.net/2021/05/01/uqDQ2s9fCTX3Gy1.png)

- 词法分析器的总体状态转换图

  ![词法分析器状态转换图](https://i.loli.net/2021/05/01/Bpz4YgJkeNIKr6R.png)

##### 功能实现描述

功能实现主要分为预处理模块、词法分析模块以及不合规单词定位模块这三大模块，分别完成程
序预处理功能、单词分割和类型匹配、原程序中单词定位的任务。

![模块结构图](https://i.loli.net/2021/05/01/CQrcgBNyeT39sWA.png)

## Usage

- 先将源代码写入代码同级文件testCpp.txt中

  > ​	此处以以下代码为例
  >
  > ```cpp
  > void main() 
  > { 
  >         int a=3, b=201, res1, 2res;    //这里有一个错误的标识符 
  >         res1=b/a;                                /*计算 res1 
  >                                                  的值*/ 
  >         2res=        b*c; 
  >         cout<<res1; 
  >         return 0; 
  > } 
  > ```

- 运行LexicalAnalysis.cpp

- 预处理结果：

  ![img](https://i.loli.net/2021/05/01/fWdzhjLp4O6vBX5.png)

- 词法分析结果：

  ![img](https://i.loli.net/2021/05/01/Deu7398WgO5CJQs.png)

- 不合规单词分析结果：

  ![img](https://i.loli.net/2021/05/01/wj4zVnyLakS9UP3.png)

## Related Efforts

## Maintainers

[@DenryDu](https://github.com/DenryDu)

## Contributors

## License

***

Support from you is my greatest encouragement! (您的支持是对我的最大鼓励！)       
