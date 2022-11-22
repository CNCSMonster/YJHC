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

6. 函数分析(func_anaylize):

   主要做两件事情:函数头信息提取,以及函数代码段token序列化

   输入为type_remove后提取出来的函数文件，输出为两种

   输入:经过type_remove分离出的函数定义文件。

   输出:格式化的两个函数信息文件,一个是函数类型信息表,一个是函数代码词法分析表

   函数类型信息表内容为:函数头分行

   ```
   函数名|函数主人|函数返回值|函数参数表
   eat|struct human|int|(int a)
   //表示结构体human具有的方法eat
   //如果是全局函数而不是某个结构体的方法
   add|NULL|struct word|(int a,int b)
   //则函数主人部分分配类型为NULL
   //实现方法,先统计出
   ```

   函数代码词法分析表内容为函数代码段的token序列,每个函数的token序列用一对花括号包裹

   * 输入

     ```
     int main(){
     int i=1;
     for(i=0;i<100;i++){
         printf("%d",i);
     }
     if(i==1) i++;
     else if(i<100) i--;
     else i+=2;
     while(i>100) i/=2;
     do{
         i+=1000;
     }while(i<10000);
     return 10;
     }
     ```

   * 输出(函数代码块的词法分析序列):因为我们不用完成具体的汇编语言翻译工作，那个交给编译器，我们要做的只是对c语言进行翻译,输出为(token类型,token属性)序列

     ```
     {
     (type,int)
     (var,i)
     (op,=)
     (const,1)
     (sep,;)
     (key,for)
     (sep,()
     ...
     }函数参数信息文件:第一行为读取的文件信息的行数,以便后面准备代码,第一行为函数总数,第二行为全局函数数量，5
     ```

   * token分类表

     |类型|码值|允许的字符字面值|
     |-|-|-|
     |关键字|0|for,if,else if, else, while,do,|
     |常量|1|1,2.15,"nihao"（包括整数小数字符串等)|
     |运算符|2|各种算术运算符,逻辑运算符和位运算符|
     |边界符|3|逗号，花括号，小括号，以及分号(不包括中括号,包括点符(用来取成员)),|
     |量名|4|比如结构体或者共用体的成员id,或者全局变量名或者全局常量名|
     |函数名|5|函数的名称,可能是自定义的,也可能是库包含的|
     |类型|6|类型名称:如int,short,double,struct student以及通过typedef定义的类型别名等|

     分析方法:使用缓冲区进行流分析,里面关键字和运算符以及边界符的长度都是确定的。关键字前面和后面应该是界限符，常量是数字和可能存在的小数点组成的,或者是由字符串包围的量,边界符可以直接识别,在字符串外面所有在边界符字符字面值集合中的都是边界符token.运算符也是同理。关于量名和函数名区分。在上面分析的token成分以外的字符串，如果后面跟着边界符中的小括号,则是函数,否则是变量名。

7. 函数翻译:

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

8. code_draw_back

   代码回填,把所有的定义声明赋值填装回代码之中,

   ```
   //按照一定格式回填成c语言代码,同时方便阅读
   //首先填入预编译包含语句
   //然后填入常量和全局变量。按照原本的相对顺序填入
   //然后填入所有自定义类型的信息,按照相对定义填入
   //
   ```

9. 组装得到最终编译器，提供各种附加命令选项(yjhc)