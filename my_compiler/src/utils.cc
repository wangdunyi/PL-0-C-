#include<iostream>
#include<string>
#include"utils.hh"
//word_type={"program","begin","end","if","then","else","const","procedure","var","do",
//         "while","call","read","write","repeat","until", "=", "<", "<=", ">=", ">", "<>"
//          "+", "-", "*", "/", 标识符, 常量， ":=", ",", ";", ".", "(", ")"};
// 识别的单词类型


// PL0程序用到的保留字
std::string key_words[] = {"program","begin","end","if","then","else","const","procedure","var","do","while","call","read","write","repeat","odd"};
int len_key_words = sizeof(key_words) / sizeof(key_words[0]);
struct WORD_TYPE word_type;