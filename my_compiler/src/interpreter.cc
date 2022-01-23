#include "interpreter.hh"
void Interpreter::interpreter() {
    P = 0;
    B = 0;
    T = 0;
    IP = PCode->get_PCode_array()[P];
    do {
        IP = PCode->get_PCode_array()[P];
        P++;
        switch(IP.getF()){ 
            case 0: // LIT 0 a : 取常量a放入数据栈栈顶
                data_stack[T] = IP.getA();
                T++;
                break;
            case 1: // OPR 相关
                switch(IP.getA()) {
                    case 0: // OPR 0 0 : 过程调用结束后,返回调用点并退栈
                        T = B;
                        P = data_stack[B+2];
                        B = data_stack[B];
                        break;
                    case 1: // OPR 0 1 : 栈顶元素取反
                        data_stack[T-1] = -data_stack[T-1];
                        break;
                    case 2: // OPR 0 2 : 次栈顶与栈顶相加，退两个栈元素，结果值进栈
                        data_stack[T-2] = data_stack[T-2] + data_stack[T-1];
                        T--;
                        break;
                    case 3: // OPR 0 3 : 次栈顶减去栈顶，退两个栈元素，结果值进栈
                        data_stack[T-2] = data_stack[T-2] - data_stack[T-1];
                        T--;
                        break;
                    case 4: // OPR 0 4 : 次栈顶乘以栈顶，退两个栈元素，结果值进栈
                        data_stack[T-2] = data_stack[T-2] * data_stack[T-1];
                        T--;
                        break;
                    case 5: // OPR 0 5 : 次栈顶除以栈顶，退两个栈元素，结果值进栈
                        data_stack[T-2] = data_stack[T-2] / data_stack[T-1];
                        T--;
                        break;
                    case 6: // OPR 0 6 : 栈顶元素的奇偶判断，结果值在栈顶
                        data_stack[T-1] = data_stack[T-1] % 2; // 奇数为1
                        break; 
                    case 7: // OPR 0 7 : 暂时为空
                        break;
                    case 8: // OPR 0 8 : 次栈顶与栈顶是否相等，退两个栈元素，结果值进栈
                        data_stack[T-2] = !data_stack[T-2]^data_stack[T-1];
                        T--;
                        break;
                    case 9: // OPR 0 9 : 次栈顶与栈顶是否不等，退两个栈元素，结果值进栈
                        data_stack[T-2] = data_stack[T-2]^data_stack[T-1];
                        T--;
                        break;
                    case 10: // OPR 0 10 : 次栈顶是否小于栈顶，退两个栈元素，结果值进栈
                        if(data_stack[T-2] < data_stack[T-1]) data_stack[T-2]=1;
                        else data_stack[T-2] = 0;
                        T--;
                        break;
                    case 11: // OPR 0 11 : 次栈顶是否大于等于栈顶，退两个栈元素，结果值进栈
                        if(data_stack[T-2] >= data_stack[T-1]) data_stack[T-2]=1;
                        else data_stack[T-2] = 0;
                        T--;
                        break;
                    case 12: // OPR 0 12 : 次栈顶是否大于栈顶，退两个栈元素，结果值进栈
                        if(data_stack[T-2] > data_stack[T-1]) data_stack[T-2]=1;
                        else data_stack[T-2] = 0;
                        T--;
                        break;
                    case 13:// OPR 0 13 : 次栈顶是否小于等于栈顶，退两个栈元素，结果值进栈
                        if(data_stack[T-2] <= data_stack[T-1]) data_stack[T-2]=1;
                        else data_stack[T-2] = 0;
                        T--;
                        break;
                    case 14: // OPR 0 14 : 栈顶值输出至屏幕
                        std::cout<<data_stack[T-1]<<" ";
                        break;
                    case 15: // OPR 0 15 : 屏幕输出换行
                        std::cout<<std::endl;
                        break;
                    case 16: // OPR 0 16 : 从命令行读入一个输入置于栈顶
                        std::cin>>data_stack[T];
                        T++;
                        break;
                }
                break;
            case 2: // LOD L a : 取变量（相对地址为a，层差为L）放到数据栈的栈顶
                data_stack[T] = data_stack[IP.getA() + get_base(B, IP.getL())];
                T++;
                break;
            case 3: // STO L a : 将数据栈栈顶的内容存入变量（相对地址为a，层次差为L）
                data_stack[IP.getA() + get_base(B, IP.getL())] = data_stack[T-1];
                T--;
                break;
            case 4: // CAL L a : 调用过程（转子指令）（入口地址为a，层次差为L）
                data_stack[T] = B; // DL 动态链，它是指向调用该过程前正在运行过程的数据段的基地址。
                data_stack[T+1] = get_base(B, IP.getL()); // SL 静态链，它是指向定义该过程的直接外过程运行时数据段的基地址。
                data_stack[T+2] = P;
                B = T;
                P = IP.getA();
                break;
            case 5: // INT 0 a : 数据栈栈顶指针增加a
                T = T + IP.getA();
                break;
            case 6: // JMP 0 a : 无条件转移到地址为a的指令
                P = IP.getA();
                break;
            case 7: // JPC 0 a : 条件转移指令，转移到地址为a的指令
                if(data_stack[T-1] == 0) P = IP.getA();
                break;
        }
    } while(P<PCode->get_code_ptr());
}

int Interpreter::get_base(int B, int L) {
    int old_B = B;
    while(L > 0) {
        old_B = data_stack[old_B+1];
        L--;
    }
    return old_B;
}