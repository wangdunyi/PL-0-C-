#pragma once
#include <iostream>
#include <vector>
#include "utils.hh"

class LexicalAnalysis{
public:
    LexicalAnalysis(std::string code_file_path);
    bool do_lexical_analysis(); //词法分析
    void get_file_content(); // 获取代码文件的内容
    struct RValue get_a_RValue(); // 获取一个RValue
    void get_char(); // 得到一个字符到cur_ch中
    void filter_out_space(); // 过滤掉空格和换行
    int is_key_word(std::string); // 若为保留字，返回保留字的下标，否则为标识符，返回0
    void showError(); 

private:
    std::vector<struct RValue> all_rv;
    std::string code_file_path; // PL0代码的文件路径
    std::string lex_out_file_path; // 词法分析结果的输出路径
    std::string buffer; // 整体存储文件的内容
    int line; // 当前行
    int len_of_buffer; // buffer的总长度
    int cur_index; // buffer 当前的下标
    char cur_ch; // buffer当前的字符
    bool error_happen; // 词法分析的时候是否发生错误
};