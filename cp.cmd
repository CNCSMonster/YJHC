::该文件定义了编译过程的cmd使用方法
pause
ggb
cls
@REM 使用doc_del把注释和多余空格换行去除
exe\dd res\code_example\test.yjhc out\out1.txt    
@REM exe\dd test.c out\out1.txt  
@REM 使用pound_remove 从源程序中分离出pound(#)语句
exe\pr out\out1.txt out/out2.txt out/pound.txt  
@REM 使用pound_substitue使用分离出的pound语句中的宏替换语句对文件进行符合语法要求的宏替换
exe\ps out\out2.txt out/pound.txt out/out3.txt
@REM 使用global_remove去除全局常量和变量定义
exe\gr out/out3.txt out/out4.txt out/global.txt
@REM 使用type_remove从去除宏,完成宏替换,去全局量后的文件中分离出函数以及类型定义
exe\tr out/out4.txt out/func.txt out/type.txt   
::使用func_analyze对func文件中提取出来的函数定义进行一阶段分析:分离函数头到head文件中,分离函数体到body文件中
exe\fs out/func.txt out/func_head.txt out/func_body.txt
::使用func_body_parser对函数体文件func_body.txt的内容进行词法分析,输出token序列到func_tokens中
exe\fbl out/func_body.txt out/func_tokens.txt

::阅读out中的token化结果
out\read out/func_tokens.txt> out/func_read_tokens.txt

@REM 使用exe/gtg运行res中的gtg脚本产生语法分析动作指导表到out中
chdir res/grammar_tbl
..\..\exe\gtg err<yjhc.gtg>0err
chdir ../..

@REM 使用refector工具获得代码的格式化放置到out中
exe\formatter out/func_tokens.txt out/func_format.txt

@REM 使用tokens_split工具tks把token序列进行切割,以检查切割结果是否正确
exe\tks out/func_tokens.txt out/after_tokens_split.txt

@REM 进行翻译工具ft把函数token翻译为c的token
exe\ft out/type.txt out/global.txt out/func_head.txt out/func_tokens.txt out/func_tokens_after_translate.txt

@REM 格式化翻译后的代码
exe\formatter out/func_tokens_after_translate.txt out/func_format2.txt

@REM 使用combine把处理好的各部分代码连接
exe\combine res/yjhclib.txt out/type.txt out/global.txt out/func_head.txt out/func_format2.txt out/final.c

@REM 把翻译后的代码处理成执行程序
gcc out/final.c -o final/a.exe
