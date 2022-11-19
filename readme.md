# 用面向对象的方式打开纯c语言的一种方法

### 意义:

可以说几乎没有意义，唯一的意义可能是让笔者心情愉快吧。

消耗了相比较于天生为对象的语言来说更多的空间,而且效果还没有那么好。

还有很多面向对象的性质没有实现.

本质上是对c的语法的一种封装。

底层依赖于mingw64来编译代码

### 前景:可以说没有前景，起码笔者没看到

### 原理

把这种语法的代码转化为c语言的代码

### 开发过程

1. 完成doc_del,完成代码注释去除，和多余空格去除，多余换行去除(11.19完成)

2. 预编译包含指令提取和宏定义常量提取:在文件前面部分的内容。

   pound_remove

3. 语块划分器:使用;对语块进行划分，每个语块可能是函数也可能是定义

   1. 结构体，枚举类型，共用体提取

   2. 函数提取

   3. 全局变量提取

```
//加入this关键字,会使用this来显式地调用自身参数
//如果没有this关键字,会先检查是否使用了全局变量,如果使用了全局变量,则优先使用
//全局变量
//如果没有使用全局变量
//则使用的是自身的变量,进行语句替换
//以行为分隔进行分析。
//点号前面当且仅当为id的时候是正确的
//id包括变量名和函数名
```