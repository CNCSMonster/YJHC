::往里面写入编译指令,固定格式,避免每次都要重复输入
gcc yjhc_preProcess\doc_del.c -o exe\dd
gcc yjhc_preProcess\pound_remove.c -o exe\pr
gcc yjhc_preProcess\pound_substitute.c -o exe\ps
gcc yjhc_preProcess\global_remove.c -o exe\gr
gcc yjhc_preProcess\type_remove.c -o exe\tr
gcc yjhc_preProcess\func_split.c -o exe\fs
gcc yjhc_preProcess\func_body_parser.c -o exe\fbp
@REM 准备read工具到out中
gcc tool\token_readable.c -o out\read

@REM 准备gtg工具到exe中
gcc gtg\main.c -o exe\gtg

@REM 准备refector工具到exe中,该工具主要使用gtg生成的语法动作表以及token_reader模块
@REM 能够根据函数的token列表获得函数的格式化
gcc yjhc_func_translate\refector.c -o exe\refector



