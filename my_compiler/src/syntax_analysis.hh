#pragma once
#include<string>
#include<vector>
#include "utils.hh"
class SyntaxAnalysis{
public:
    SyntaxAnalysis(std::string code_file_path);
    ~SyntaxAnalysis();
    // 得到词法分析结果
    bool get_lex_result();  // 获得词法分析结果
    void read_lex_result_file(); // 读取词法分析结果文件
    void prog();
    void block(int proc_pose=0, int count=0);
    void condecl();
    void myconst();
    void vardecl();
    void proc();
    void body();
    void statement();
    void lexp();
    void exp();
    void term();
    void factor();
    int lop();
    void show_error(int, std::string);
    void show_PCode();
    void show_symbol_table();
    void interpreter();
private:
    std::string code_file_path; // 代码存储路径，最好全路径存在
    std::vector<struct RValue> all_rv;  // 词法分析结果
    int rv_ptr = 0; // all_rv的迭代器
    int len_of_all_rv; // all_rv的长度
    std::string lex_out_file_path = "lex.txt"; // 词法分析结果存储路径
    std::string pcode_out_file_path = "pcode.txt"; // pcode结果存储路径
    AllPCode* PCode = new AllPCode();  // PCode变量
    bool error_happen = false;
    SymbolTable* symbol_table = new SymbolTable(); // 符号表变量

    int level = 0; // 主程序为第0层
    int address = 0; // 主程序或变量的定义初始化为0
    int increament = 1; // addreass的增量，只会对var进行增量

};
