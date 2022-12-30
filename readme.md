# 用面向对象的方式打开纯c语言的一种方法

### 功能说明

笔者自定义了一种面向对象语法，命名为yjhc。

该工程编译器用c编写，能够把yjhc代码翻译成c语言代码.从而执行

在环境配置了mingw64，有gcc工具可以依赖的情况下，还可以直接把yjhc代码翻译成机器码

### 工程说明

exe文件夹放编译出来的可执行文件

yjhc文件夹放项目代码

res放使用到的资源，比如测试用的yjhc语言源代码

out文件夹存放编译输出的结果

cp.cmd命令行文件里面存放编译的指令

```
cp
```

在项目文件夹目录下输入该命令会自动编译res中名为code.yjhc文件，并且把编译最终结果和中间结果存放到

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

 2. 预编译包含指令提取和宏定义常量提取:在文件前面部分的内容。(11.20完成)

    pound_remove

 3. pound_substitute ,(输入经过提取宏后的源文件以及宏文件)

    ```
    ps out2.txt pound.txt out3.txt
    ```

    宏常量替换。读取前面读取出来的包含指令提取和宏定义常量提取的文件中的宏定义内容，对原始代码进行替换。消除所有#define语句定义的常量。

 4. 全局常量全局变量提取，global_remove(11.20完成)

 5. 类型提取，type_remove(在全局常量变量提取后提取在全局定义的变量类型)(11.20)

    该功能会把函数声明给忽略掉

    ```
    //编译成tr.exe
    tr input.txt func.txt type.txt
    //按照该格式命令行参数调用程序把input.txt中的代码(这里input.txt为步骤global_remove后的结果)
    //func.txt为提取出的函数代码的输出路径
    //type.txt为提取出的类型定义代码的输出路径
    ```

 6. 函数头体分离信息提取(func_split.c)

    主要做三件事情:函数头信息提取,函数代码块的词法分析，根据token序列进行格式化

    输入为type_remove后提取出来的函数文件，输出为两种

    输入:经过type_remove分离出的函数定义文件。

    输出:格式化的两个函数信息文件,一个是函数类型信息表,一个是函数代码的初步词法分析得到的token序列

    函数类型信息表内容为:函数头分行

    ```
    函数名|函数主人|函数返回值|函数参数表
    swap|NULL|void|(int* a,int * b)
    getScore|struct student|double|()
    selfIntroduce|struct student|void|()
    ```

    函数代码表如:

    ```c
    { void (*p)(int); *a=*a+*b; *b=*a-*b; *a=*a-*b; }
    { return score; }
    { printf("my name is:%s,my socre is:%f,my age is:%d",name,getScore(),age); }
    
    ```

 7. 函数体词法分析

    函数代码词法分析表内容为函数代码段的token序列,每个函数的token序列开头和结尾分别是Sep类型的左花括号和右花括号

    * 输入为进行func_split后的函数体

      ```
      { void (*p)(int); *a=*a+*b; *b=*a-*b; *a=*a-*b; }
      { return score; }
      { printf("my name is:%s,my socre is:%f,my age is:%d",name,getScore(),age); }
      
      ```

    * 输出(词法序列),完成类型匹配,通过编码进行匹配输出每行是一个token

      每行token的输出是token编码以及对应字符串字面值，二者之间空一个空格

      与输入对应的输出为

      ```c
      19 {
      10 void
      15 (
      21 *
      11 p
      16 )
      15 (
      10 int
      16 )
      13 ;
      21 *
      11 a
      21 =
      21 *
      11 a
      21 +
      21 *
      11 b
      13 ;
      21 *
      11 b
      21 =
      21 *
      11 a
      21 -
      21 *
      11 b
      13 ;
      21 *
      11 a
      21 =
      21 *
      11 a
      21 -
      21 *
      11 b
      13 ;
      20 }
      19 {
      9 return
      11 score
      13 ;
      20 }
      19 {
      23 printf
      15 (
      12 "my name is:%s,my socre is:%f,my age is:%d"
      14 ,
      11 name
      14 ,
      23 getScore
      15 (
      16 )
      14 ,
      11 age
      16 )
      13 ;
      20 }
      ```

    * 这一阶段要完成的另一个任务是代码补全，补充上流程控制中多余的空格以方便建立局部变量表,下面是一个大概说明对应关系的表格

      ```
      Sep {
      Type int
      Id a
      Op =
      Const 1
      Sep ;
      Key if
      Sep (
      Id a
      op >
      Const 1
      Sep )
      Id a
      Op ++
      Sep ;
      Key return
      Id a
      Sep ;
      Sep }
      ```

    token类型表，对应类型编号和名称见token_kind.h文件

 8. 准备代码语义分析和格式化动作知道表编写与生成器(grammar_table_generator,gtg)

    规定了一种编辑语法表的语言，设计并完成了一种能够执行这种脚本语言并导出对应的

    语义分析动作指导表的执行器，代码见gtg文件夹

 9. 准备token读取器(提供函数翻译需要的api,以及缓冲保存token的数据结构)

    关于为什么不一次读完所有token来处理。是为了避免可能的内存溢出,或者说避免源代码很长的情况下内存空间不足以容纳所有token。所以我们以一种类似流处理的方式进行，每次只读取一部分token，分析完之后立刻释放这部分空间,把分析结果写入文件,然后再读取后面的内容。换句话说，我们把更多的存储管理的任务交给了操作系统的文件系统去完成，于是就不用担心内存容量的限制。

    就算我们用一个程序往文件中写入100G的数据而可用硬盘空间只有40G,操作系统也能够正确应对,采取正确的措施

10. 准备局部变量表数据结构(可变结构,能够维护变量的作用域属性,而且能够快速建立和结束作用域)

11. 准备全局变量表和常量表(在函数解释过程中不会改动,要求能够快速根据名查找)

12. 准备自定义类型表(读取后不会改动,yjhc中放弃支持函数内结构体定义),能够快速根据类型名或者类型指针名查找到该结构

13. 函数翻译,把yjhc的语法翻译成c的语法

    输入为序列分析后的函数信息以及

    加载之前分析出的预编译包含指令与宏常量表中的宏常量,加载全局量信息表,加载自定义类型信息表,加载函数类型信息表，读取分析出的函数token串对函数进行翻译。把yjhc语法的函数翻译成c语言的语法的函数定义

    具体措施一:结构体方法内结构体成员直接引用替换。

    ```
    比如函数内使用了不在形参列表也不在全局量列表页中的定义,
    则这个量只能是结构体类型中的成员量,如果对应结构体类型中没有该成员量,则报语法异常
    //
    struct student{
        char name[20];
        int age;
    }
    //对于方法
    struct student->void study(int book){
        printf("%s is reading book %d",name,book);
        name[20]='2';
        this.book=1;
        printf("his age is %d",age);
    }
    //里面调用了结构体自身的成员变量age,这个是没有在形参里面出现也没有在全局变量全局常量和宏定义中出现的
    //应该修改成
    void _yjhc_student_study(struct student* this,int book){
        printf("%s is reading book %d",this->name,book);
        this->name[20]='2';
        this->book=1;    //如果是使用this关键字替换,说明是自身替换,则把.符号改成->符号
        printf("his age is %d",this->age);
    }
    //因为this作为关键字,所以不允许有变量或者函数命名为this
    ```

    具体措施二:函数调用方式替换:把面向对象的方法调用形式修改成c语言的函数调用形式:

    ```
    int main(){
        struct student mm;
        int book=1;
        mm.age=11;
        strcpy(mm.name,"ggb");
        mm.study(book);
    }
    //mm为struct student结构体类型的变量,在main方法中调用了自身的study方法.
    //修改为
    int main(){
        struct student mm;
        int book=1;
        mm.age=11;
        strcpy(mm.name,"ggb");
        _yjhc_mm_study(&mm,book);
    }
    //第二种可能,如果结构体方法内调用结构体方法,那么也要进行重构
    struct student->void study2(int book){
        study(book);
        printf("in study2 way");
    }
    //在结构体方法内直接调用而不是结构体类型变量调用的函数,有两种可能,一种是全局函数,另一种是该结构体的其他方法
    //翻译成
    void _yjhc_student_study2(struct student* this,int book){
        _study_student_study(this,book);
        printf("in study2 way");
    }
    ```

14. code_draw_back

    代码回填,把所有的定义声明赋值填装回代码之中,

    ```
    //按照一定格式回填成c语言代码,同时方便阅读
    //首先填入预编译包含语句
    //然后填入常量和全局变量。按照原本的相对顺序填入
    //然后填入所有自定义类型的信息,按照相对定义填入
    //
    ```

15. 组装得到最终编译器，提供各种附加命令选项(yjhc)