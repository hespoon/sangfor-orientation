# 环境使用说明

## 开发环境安装

1. Linux系统（可以是虚拟机或者Win10的WSL环境，实在没有可以是msys2, cygwin环境）
2. 安装gcc，make
3. 安装lcov，lcov是生成覆盖率报告的工具: 
```
sudo apt install lcov
```
4. 安装valgrind，valgrind是动态调试工具，用于查泄露和指针问题: 
```
sudo apt install valgrind
```

apt是ubuntu的命令，其它系统可以用rpm或yum，pacman等安装工具。
或者baidu相关工具的源码，下载后编译安装。

安装完毕后，请执行make;make test;make lcov;make check;命令验证环境工作正常。

## 编译测试过程

1. 编译
```
make
```

2. 运行测试代码
```
make test
```

3. 生成覆盖率数据报告
```
make lcov
```

4. 打开`demo_web`目录下的`index.html`，查看覆盖率数据

5. 使用valgrind运行demo，查看内存泄漏和指针问题
```
make check
```

## 常见问题

1. 使用C++怎么办？

> 将makefile里的`gcc`命令改为`g++`，修改编码命令中的文件名。
> 如，将：
>	gcc -Wall -g -fprofile-arcs -ftest-coverage -c -o json.o json.c
> 修改为：
> 	g++ -Wall -g -fprofile-arcs -ftest-coverage -c -o json.o json.cpp

