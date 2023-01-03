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
@REM 编译函数编译器
gcc yjhc_func_translate\func_translate.c -o exe\ft
