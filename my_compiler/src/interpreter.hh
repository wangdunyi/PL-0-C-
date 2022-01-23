#pragma once
#include<iostream>
#include "utils.hh"

class Interpreter {
public:
    Interpreter(AllPCode* PCode) {
        this->PCode = PCode;
    }
    void interpreter();
    int get_base(int B, int L);
private:
    AllPCode *PCode;  // PCode变量
    PerPCode IP; // 指令寄存器I，存放当前要执行的代码
    int T; // 栈顶指示器T，指向数据栈STACK的栈，栈顶不存放元素？
    int B; // 基址寄存器B，存放当前运行过程的数据区在STACK中的起始地址
    int P; // 程序地址寄存器，存放下一条要执行的指令的地址
    static const int stack_size = 1000;
    static const int stack_increment = 100;
    static const int stack_maxsize = 10000;
    int data_stack[stack_size];
};