# ExperimentOfCompiling
# Course Experiment of compiling. Including:
1. C++ Lexical Analysis(Scanner)  
2. Thompson way to generate NFA from regular expression and transform it to DFA, then minimize the DFA and turn it into source codes. 
3. Tiny Parser
4. LL(1) Analyzer

# C++编译原理实验，主要包括：
1. C语言词法分析（代码扫描器）  
2. 用递归方法实现的Thompson算法将正则表达式转换为NFA、DFA、最小化DFA并生成C语言代码。
3. 递归向下子过程实现的tiny语言语法树生成
4. LL(1)分析表的构造

## 一、C语言词法分析器
实验要求：
>- 打开一个C++源文件，进行词法分析
>- 要求应用程序应为Windows界面
>- 为了提高C++源程序的可读性，C++程序在书写过程中加入了空行、空格、缩进、注释等。假设你想牺牲可读性，以节省磁盘空间，那么你可以存贮一个删除了所有不必要空格和注释的C++源程序的压缩文本。因此，程序中还应该有这样的压缩功能
>- 实现减小源文件大小的压缩功能

![运行程序显示Windows界面](https://github.com/SeanLiaoy/ExperimentOfCompiling/blob/master/sample/1_1.png)

![打开文件1](https://github.com/SeanLiaoy/ExperimentOfCompiling/blob/master/sample/1_2.png)

![词法分析](https://github.com/SeanLiaoy/ExperimentOfCompiling/blob/master/sample/1_3.png)

![代码压缩](https://github.com/SeanLiaoy/ExperimentOfCompiling/blob/master/sample/1_4.png)



## 二、正则表达式代码生成XLEX
>设计一个应用软件，以实现将正则表达式-->NFA--->DFA-->DFA最小化-->词法分析程序。本实验基于Qt设计Windows界面并进行可视化操作。

![程序执行流程图](https://github.com/SeanLiaoy/ExperimentOfCompiling/blob/master/sample/2_1.png)

![程序执行流程图](https://github.com/SeanLiaoy/ExperimentOfCompiling/blob/master/sample/2_8png.png)

## 三、Tiny语言语法树生成
>扩充的语法规则有：实现 while、do while、for语句、大于>比较运算符号以及求余计算式子，具体文法规则自行构造。

>可参考：P97及P136的文法规则。

>- While-stmt --> while  exp  do  stmt-sequence  endwhile
>- Dowhile-stmt-->do  stmt-sequence  while(exp); 
>- for-stmt-->for identifier:=simple-exp  to  simple-exp  do  stmt-sequence enddo    步长递增1
>- for-stmt-->for identifier:=simple-exp  downto  simple-exp  do  stmt-sequence enddo    步长递减1
>- 大于>比较运算符号以及求余计算式子的文法规则请自行组织。
>- 把TINY语言原有的if语句书写格式

>>- if_stmt-->if exp then stmt-sequence end  |  | if exp then stmt-sequence else stmt-sequence end 
>>    
>改写为：

>>- if_stmt-->if(exp) stmt-sequence else stmt-sequence | if(exp) stmt-sequence

![程序效果图](https://github.com/SeanLiaoy/ExperimentOfCompiling/blob/master/sample/3_1.png)

## 四、LL(1)分析表的构造
>设计一个应用软件，以实现LL(1)分析器的生成。包括文法规则输入、消除左递归、消除左公因子、生成LL(1)分析表、输入分析句子并展示分析过程。
>>- 要提供一个源程序编辑界面，让用户输入文法规则（可保存、打开存有文法规则的文件）
>>- 检查该文法是否存在着左递归，如果有，则消除该文法的左递归。因此需要提供窗口以便用户可以查看消除左递归后的结果。
>>- 检查该文法是否存在着左公共因子，如果有，则消除该文法的左公共因子。因此需要提供窗口以便用户可以查看消除左公共因子的结果。
>>- 需要提供窗口以便用户可以查看文法对应的LL(1)分析表，并报告该文法是否为LL(1)文法。
>>- 需要提供窗口以便用户输入需要分析的句子。
>>- 需要提供窗口以便用户查看使用LL(1)分析该句子的过程。

![程序效果图](https://github.com/SeanLiaoy/ExperimentOfCompiling/blob/master/sample/4_1.png)
