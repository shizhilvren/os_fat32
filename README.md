# os_fat32
操作系统实验  
按照FAT32格式简单实现一个文件系统  
特点：可以挂载到本地  
运行 make.bat 编译。  

## 注意
由于windows 使用GBK2312编码，而linux使用UTF-8编码。这会导致中文乱码。  
对win系统，请修改编译命令 -fexec-charset=GB2312。
或使用 -fexec-charset=UTF-8 编译，但在运行前设置powershell 代码页为UTF8。
如命令 
``` cmd
chcp 936    # 设置 GBK2312
chcp 65001  # 设置 UTF8
```
