import os

#该脚本用来统计文件夹下c语言文件的行数

# 说明：UTF兼容ISO8859-1和ASCII，GB18030兼容GBK，GBK兼容GB2312，GB2312兼容ASCII
CODES = ['UTF-8', 'GB18030', 'BIG5']
# UTF-8 BOM前缀字节
UTF_8_BOM = b'\xef\xbb\xbf'
 
def file_encoding(file_path: str):
    """
    获取文件编码类型
    
    :param file_path: 文件路径
    :return: 
    """
    with open(file_path, 'rb') as f:
        return string_encoding(f.read())
 
 
def string_encoding(data: bytes):
    """
    获取字符编码类型
    
    :param data: 字节数据
    :return: 
    """
    # 遍历编码类型
    for code in CODES:
        try:
            data.decode(encoding=code)
            if 'UTF-8' == code and data.startswith(UTF_8_BOM):
                return 'UTF-8-SIG'
            return code
        except UnicodeDecodeError:
            continue
    return 'unknown'
  

def findAllFilesWithSpecifiedSuffix(target_dir):
    # print(target_dir)
    find_res = []
    # target_suffix_dot = "." + target_suffix
    walk_generator = os.walk(target_dir)
    for root_path, dir, files in walk_generator:
        if len(files) < 1:
            continue
        for file in files:
            # file_name, suffix_name = os.path.splitext(file)
            # if suffix_name == target_suffix_dot:
            find_res.append(os.path.join(root_path, file))
        for pdir in dir:
            if pdir=="." or pdir == '..':
                # print(pdir)
                continue
            find_res+= findAllFilesWithSpecifiedSuffix(os.path.join(root_path,pdir))
    return find_res

# 统计路径中的文件数量

def countLinesInFiles(files):
    count =0
    paths=set()
    for filePath in files:
        fileName,suffix=os.path.splitext(filePath)
        if suffix!='.c' and suffix!='.h':
            continue
        if filePath in paths:
            continue
        paths.add(filePath)
        print(filePath,end="\t")
        f=open(filePath,'r',encoding= file_encoding(filePath))
        lines=f.readlines()
        lines=list(filter(lambda x: x!='\n',lines))
        # for str in lines:
        #     print(str)
        # lines=list(filter(lambda x: ))
        add=len(lines)
        print(add)
        count+=add
    return count


# 首先从控制台获取系统路径
dir="."
files=findAllFilesWithSpecifiedSuffix(dir)
n=countLinesInFiles(files)
print(n)
