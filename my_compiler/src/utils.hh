#pragma once
#include<iostream>
#include<string>
#include<vector>


// 写入词法分析结果文件的每行的格式
struct RValue{
    int id; // 对应key_word
    std::string name; // 
    int line; // 在程序中的所在行
};

struct WORD_TYPE {
    int PROGRAM=1; // "program"
    int BEGIN=2; // "begin"
    int END=3; // "end"
    int IF=4; // "if"
    int THEN=5; // "then"
    int ELSE=6; // "else"
    int CON=7; // "const"
    int PROCEDURE=8; // "procedure"
    int VAR=9; // "var"
    int DO=10; // "do"
    int WHILE=11; // "while"
    int CALL=12; // "call"
    int READ=13; // "read"
    int WRITE=14; // "write"
    int REPEAT=15; // "repeat"
    int ODD=16; // "odd"  // 和 key_words中相对应
    int EQU=17; // "="
    int LES=18; // "<"
    int LESE=19; // "<="
    int LARE=20; // ">="
    int LAR=21; // ">"
    int NEQU=22; // "<>"
    int ADD=23; // "+"
    int SUB=24; // "-"
    int MUL=25; // "*"
    int DIV=26; // "/"
    int SYMBOL=27; // 标识符
    int CONST=28; // 常量
    int CEQU=29; // ":="
    int COMMA=30; // ","
    int SEMIC=31; // ";"
    int POI=32; // "."
    int LBR=33; // "("
    int RBR=34; // ")"
};

class PerPCode {
public:
    PerPCode(int F, int L, int A):F(F), L(L), A(A){
    }
    PerPCode(){
    }
    PerPCode(const PerPCode &a){
        this->F = a.F;
        this->L = a.L;
        this->A = a.A;
    }
    PerPCode& operator=(const PerPCode &a){
        this->F = a.F;
        this->L = a.L;
        this->A = a.A;
    }
    int getF() {return this->F;}
    int getL() {return this->L;}
    int getA() {return this->A;}
    void setF(int F) {this->F = F;}
    void setL(int L) {this->L = L;}
    void setA(int A) {this->A = A;}
private:
    int F; // PCode伪操作码
    int L; // 代表调用层与说明层的层差值
    int A; // 代表位移量（相对地址）
};

class AllPCode {
    /*
    *    代码的具体形式：
    *    FLA
    *    其中：F段代表伪操作码
    *    L段代表调用层与说明层的层差值
    *    A段代表位移量（相对地址）
    *    进一步说明：
    *    INT：为被调用的过程（包括主过程）在运行栈S中开辟数据区，这时A段为所需数据单元个数（包括三个连接数据）；L段恒为0。
    *    CAL：调用过程，这时A段为被调用过程的过程体（过程体之前一条指令）在目标程序区的入口地址。
    *    LIT：将常量送到运行栈S的栈顶，这时A段为常量值。
    *    LOD：将变量送到运行栈S的栈顶，这时A段为变量所在说明层中的相对位置。
    *    STO：将运行栈S的栈顶内容送入某个变量单元中，A段为变量所在说明层中的相对位置。
    *    JMP：无条件转移，这时A段为转向地址（目标程序）。
    *    JPC：条件转移，当运行栈S的栈顶的布尔值为假（0）时，则转向A段所指目标程序地址；否则顺序执行。
    *    OPR：关系或算术运算，A段指明具体运算，例如A=2代表算术运算“＋”；A＝12代表关系运算“>”等等。运算对象取自运行栈S的栈顶及次栈顶。
    *
    *    OPR 0 0	过程调用结束后,返回调用点并退栈
    *    OPR 0 1	栈顶元素取反
    *    OPR 0 2	次栈顶与栈顶相加，退两个栈元素，结果值进栈
    *    OPR 0 3	次栈顶减去栈顶，退两个栈元素，结果值进栈
    *    OPR 0 4	次栈顶乘以栈顶，退两个栈元素，结果值进栈
    *    OPR 0 5	次栈顶除以栈顶，退两个栈元素，结果值进栈
    *    OPR 0 6	栈顶元素的奇偶判断，结果值在栈顶
    *    OPR 0 7
    *    OPR 0 8	次栈顶与栈顶是否相等，退两个栈元素，结果值进栈
    *    OPR 0 9	次栈顶与栈顶是否不等，退两个栈元素，结果值进栈
    *    OPR 0 10	次栈顶是否小于栈顶，退两个栈元素，结果值进栈
    *    OPR 0 11	次栈顶是否大于等于栈顶，退两个栈元素，结果值进栈
    *    OPR 0 12	次栈顶是否大于栈顶，退两个栈元素，结果值进栈
    *    OPR 0 13	次栈顶是否小于等于栈顶，退两个栈元素，结果值进栈
    *    OPR 0 14	栈顶值输出至屏幕
    *    OPR 0 15	屏幕输出换行
    *    OPR 0 16	从命令行读入一个输入置于栈顶
    */
public:
    AllPCode(){}
    void gen(int F, int L, int A) {
        // 生成一个PerPCode记录
        PCode_array[pcode_ptr].setF(F);
        PCode_array[pcode_ptr].setL(L);
        PCode_array[pcode_ptr].setA(A);
        this->pcode_ptr++;
    }
    int get_code_ptr() {return pcode_ptr;}
    PerPCode* get_PCode_array() {return PCode_array;}
    int get_LIT() {return LIT;}
    int get_OPR() {return OPR;}
    int get_LOD() {return LOD;}
    int get_STO() {return STO;}
    int get_CAL() {return CAL;}
    int get_INT() {return INT;}
    int get_JMP() {return JMP;}
    int get_JPC() {return JPC;}
    int get_RED() {return RED;}
    int get_WRT() {return WRT;}
private:
    int LIT = 0;        //LIT 0 a: 取常量a放入数据栈栈顶
    int OPR = 1;        //OPR 0 a: 执行运算，a表示执行某种运算，具体是何种运算见上面的注释
    int LOD = 2;        //LOD L a: 取变量（相对地址为a，层差为L）放到数据栈的栈顶
    int STO = 3;        //STO L a: 将数据栈栈顶的内容存入变量（相对地址为a，层次差为L）
    int CAL = 4;        //CAL L a: 调用过程（转子指令）（入口地址为a，层次差为L）
    int INT = 5;        //INT 0 a: 数据栈栈顶指针增加a
    int JMP = 6;        //JMP 0 a: 无条件转移到地址为a的指令
    int JPC = 7;        //JPC 0 a: 条件转移指令，转移到地址为a的指令
    int RED = 8;        //RED L a: 读数据并存入变量（相对地址为a，层次差为L）
    int WRT = 9;        //WRT 0 0: 将栈顶内容输出

    int pcode_ptr=0;      //指向下一条将要产生的代码的在AllPCode中的地址
    static const int array_size = 1000;
    PerPCode PCode_array[array_size];
};

class TableRow {
    // 表示符号表中的每一行
public:
    TableRow(){}
    TableRow(int type, int value, int level, int address, int size, std::string name){
        this->type = type;
        this->value = value;
        this->level = level;
        this->address = address;
        this->size = size;
        this->name = name;
    }
    int get_type() {return type;}
    int get_value() {return value;}
    int get_level() {return level;}
    int get_address() {return address;}
    int get_size() {return size;}
    std::string get_name() {return name;}
    void set_type(int type) {this->type = type;}
    void set_value(int value) {this->value = value;}
    void set_level(int level) {this->level = level;}
    void set_address(int address) {this->address = address;}
    void set_size(int size) {this->size = size;}
    void set_name(std::string name) {this->name = name;}
private:
    int type;           //表示常量、变量或过程
    int value;          //表示常量或变量的值
    int level;          //嵌套层次，最大应为3，不在这里检查其是否小于等于，在SymbolTable中检查
    int address;        //相对于所在嵌套过程基地址的地址
    int size;           //表示常量，变量，过程所占的大小，此变量和具体硬件有关，实际上在本编译器中为了方便，统一设为4了,设置过程在SymTable中的三个enter函数中
    std::string name;   //变量、常量或过程名
};

class SymbolTable {
public:
    SymbolTable(){}
    int get_my_const() {return my_const;}
    int get_var() {return var;}
    int get_proc() {return proc;}
    int get_table_ptr() {return table_ptr;}
    int get_len_of_table() {return len_of_table;}
    TableRow * get_table() {return table;}

    void enter_const(std::string name, int level, int value, int address) {
        std::cout<<"enter_const:"<<"name, level, value, address: "<<name<<" "<<level<<" "<<value<<" "<<address<<std::endl;
        table[table_ptr].set_type(my_const);
        table[table_ptr].set_value(value);
        table[table_ptr].set_level(level);
        table[table_ptr].set_address(address);
        table[table_ptr].set_size(4);
        table[table_ptr].set_name(name);
        table_ptr++;
        len_of_table++;
    }

    void enter_var(std::string name, int level, int address) {
        std::cout<<"enter_var:"<<"name, level, address: "<<name<<" "<<level<<" "<<address<<std::endl;
        TableRow tmp(var, 0, level, address, 0, name); // value 和 size 都默认为0
        table[table_ptr].set_type(var);
        table[table_ptr].set_value(0);
        table[table_ptr].set_level(level);
        table[table_ptr].set_address(address);
        table[table_ptr].set_size(0);
        table[table_ptr].set_name(name);
        table_ptr++;
        len_of_table++;
    }

    void enter_proc(std::string name, int level, int address) {
        std::cout<<"enter_proc:"<<"name, level, address:"<<name<<" "<<level<<" "<<address<<std::endl;
        table[table_ptr].set_type(proc);
        table[table_ptr].set_value(0);
        table[table_ptr].set_level(level);
        table[table_ptr].set_address(address);
        table[table_ptr].set_size(0);
        table[table_ptr].set_name(name);
        table_ptr++;
        len_of_table++;
    }

    TableRow& get_table_row(int i) {return table[i];}  // 获取符号表中的第i行
    TableRow& get_table_row_by_name(std::string name) {
        // 根据名字获取table_row, name: 名字
        for (int i=len_of_table-1; i>=0; i--) {
            if(table[i].get_name()==name){
                std::cout<<"i: "<<i<<std::endl;
                return table[i];
            }
        }
    }
    //std::vector<TableRow> get_table() return table;  // 获取所有的符号表
    int get_level_proc(int level){
        //查找本层的过程在符号表中的位置, 其实从后往前查找到的第一个就是所需的过程
        for(int i=len_of_table-1;i>=0;i--) if(table[i].get_type()==proc) return i;
        return -1;
    }
    void set_table_ptr(int table_ptr) {this->table_ptr = table_ptr;}
    
    bool is_now_exists(std::string name, int level) {
        for(int i=0;i<len_of_table;i++) {
            if(table[i].get_name()==name && table[i].get_level()==level){
                return true;
            }
        }
        return false;
    }

    bool is_pre_exists(std::string name, int level) {
        for(int i=0;i<len_of_table;i++) {
            if(table[i].get_name()==name && table[i].get_level()<=level){
                return true;
            }
        }
        return false;
    }

private:
    static const int table_size = 100;
    TableRow table[table_size];
    int my_const = 1;  // 常量用1表示
    int var = 2;  // 变量用2表示
    int proc = 3;  // 过程用3表示
    int table_ptr = 0;  // 符号表迭代器
    int len_of_table;  // 符号表长度
};
extern std::string key_words[];
extern int len_key_words;
extern struct WORD_TYPE word_type;
