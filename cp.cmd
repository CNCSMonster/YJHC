::该文件定义了编译过程的cmd使用方法
pause
ggb
cls
@REM 使用doc_del把注释和多余空格换行去除
exe\dd res\code.yjhc out\out1.txt      
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
exe\fbp out/func_body.txt out/func_tokens.txt

chdir yjhc_func_translate
func_translate>0.txt
chdir ..

