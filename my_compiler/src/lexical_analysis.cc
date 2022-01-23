#include "lexical_analysis.hh"
#include <fstream>
#include <sstream>
#include <ctype.h>

LexicalAnalysis::LexicalAnalysis(std::string code_file_path): code_file_path(code_file_path) {
    this->get_file_content(); //得到文件的内容到this->buffer中
    lex_out_file_path = "lex.txt";
    line = 1; // 初始化行为1
    len_of_buffer = buffer.length(); // buffer的总长度
    printf("源代码总长度为 %d\n", len_of_buffer);
    cur_index = 0; // 初始化buffer的当前的下标为0
    cur_ch = buffer[0];
    error_happen = false;
}

bool LexicalAnalysis::do_lexical_analysis() {
    printf("开始词法分析！\n");
    struct RValue rv = get_a_RValue();
    while(!error_happen) {
        all_rv.push_back(rv);
        if(rv.id==-1) break;
        rv = get_a_RValue();
    }
    if(error_happen) {
        showError();
        return false;
    }
    // 将all_rv的内容写入到lex_out_file_path文件中
    printf("将词法分析的结果写到 lex_out_file_path 文件中！\n");
    std::ofstream ofile(lex_out_file_path);
    for (auto rv: all_rv) {
        ofile<<rv.id<<" "<<rv.name<<" "<<rv.line<<std::endl;
    }
    return true;
}

void LexicalAnalysis::get_file_content() {
    std::cout<<"获取"<<code_file_path<<"的内容"<<std::endl;
    std::ifstream ifile(code_file_path, std::ios::in );
    //将文件读入到ostringstream对象buf中
    std::ostringstream buf;
    char ch;
    //返回与流对象buf关联的字符串
    while(buf&&ifile.get(ch)) buf.put(ch);
    this->buffer = buf.str();
    printf("获取文件结果结束！\n");
}

struct RValue LexicalAnalysis::get_a_RValue() {
    struct RValue result;
    get_char();
    filter_out_space();                     
    // 文件到达末尾                                     
    if(cur_ch==' ' || cur_ch=='\n' || cur_ch=='\t' || cur_index==len_of_buffer) {
        result.id = -1;
        result.name = "-1";
        result.line = line;
        return result;
    }
    if(isalpha(cur_ch)) { // 1. 字母开头
        printf("(1)字母开头\n");
        std::string concat = ""; // 拼接字符串
        while(isalpha(cur_ch) || isdigit(cur_ch) || cur_ch=='_') {
            concat += cur_ch;
            if(cur_index==len_of_buffer) break;
            get_char();
        }
        std::cout<<"拼出的变量为 "<<concat<<std::endl;
        cur_index--; // index 回退一格
        int id = is_key_word(concat);
        if(id==0) {  // 非保留字，为标识符
            printf("(2)非保留字，为标识符\n");
            result.id = word_type.SYMBOL;
            result.name = concat;
            result.line = line;
            return result;
        }else {  // 为保留字
            printf("(3)为保留字\n");
            result.id = id;
            result.name = key_words[id-1];
            result.line = line;
            return result;
        }
    }else if(isdigit(cur_ch)) {  // 2. 数字开头
        printf("(4)数字开头\n");
        std::string concat = ""; // 拼接字符串
        while(isdigit(cur_ch)) {
            concat += cur_ch;
            if(cur_index==len_of_buffer) break;
            get_char();
        }
        cur_index--; // index 回退一格
        printf("(5)数字常量\n");
        result.id = word_type.CONST;  // 数字常量
        result.name = concat;
        result.line = line;
        return result;
    } else if(cur_ch=='=') {  // 3. 等号 =
        printf("(6)等号 = 开头\n");
        result.id = word_type.EQU;
        result.name = "=";
        result.line = line;
        return result;
    } else if(cur_ch=='+') {  //4. 加号 +
        printf("(7)加号 + 开头\n");
        result.id = word_type.ADD;
        result.name = "+";
        result.line = line;
        return result;
    } else if(cur_ch=='-') {  // 5. 减号 -
        printf("(8)减号 - 开头\n");
        result.id = word_type.SUB;
        result.name = "-";
        result.line = line;
        return result;
    } else if(cur_ch=='*') {  // 6. 乘号 *
        printf("（9)乘号 （ 开头\n");
        result.id = word_type.MUL;
        result.name = "*";
        result.line = line;
        return result;
    } else if(cur_ch=='/') {  // 7. 除号 /
        printf("(10)除号 / 开头\n");
        result.id = word_type.DIV;
        result.name = "/";
        result.line = line;
        return result;
    } else if(cur_ch=='<') {  // 8. < 开头
        printf("(11)小于号 < 开头\n");
        get_char();
        if(cur_ch=='=') {  // <=
            printf("(12)为 <= \n");
            result.id = word_type.LESE;
            result.name = "<=";
            result.line = line;
            return result;
        } else if(cur_ch=='>') {// <>
            printf("(13)为 <> 开头\n");
            result.id = word_type.NEQU;
            result.name = "<>";
            result.line = line;
            return result;
        } else {  // <
            printf("(14)为 < \n");
            cur_index--;  // 回退一格
            result.id = word_type.LES;
            result.name = "<";
            result.line = line;
            return result;
        }
    } else if(cur_ch=='>') {  // 9. > 开头
        printf("(15)大于号 > 开头\n");
        get_char();
        if(cur_ch=='=') {  // >=
            printf("(16)为 >=\n");
            result.id = word_type.LARE;
            result.name = ">=";
            result.line = line;
            return result;
        } else { // >
            printf("(17)为 > \n");
            cur_index--;  // 回退一格
            result.id = word_type.LAR;
            result.name = ">";
            result.line = line;
            return result;
        }
    } else if(cur_ch==',') {  // 10. ,
        printf("(18)逗号 , 开头\n");
        result.id = word_type.COMMA;
        result.name = ",";
        result.line = line;
        return result;
    } else if(cur_ch==';') {  // 11. ;
        printf("(19)分号 ; 开头\n");
        result.id = word_type.SEMIC;
        result.name = ";";
        result.line = line;
        return result;
    } else if(cur_ch=='.') {  // 12. .
        printf("(20)点 . 开头\n");
        result.id = word_type.POI;
        result.name = ".";
        result.line = line;
        return result;
    } else if(cur_ch=='(') {  // 13. (
        printf("(21)左括号 ( 开头\n");
        result.id = word_type.LBR;
        result.name = "(";
        result.line = line;
        return result;
    } else if(cur_ch==')') {  // 14. )
        printf("(22)右括号 ) 开头\n");
        result.id = word_type.RBR;
        result.name = ")";
        result.line = line;
        return result;
    } else if(cur_ch==':') {  // 15. :
        printf("(23)冒号 : 开头\n");
        get_char();
        if(cur_ch=='=') {  // 16. :=
            printf("(24)为 :=\n");
            result.id = word_type.CEQU;
            result.name = ":=";
            result.line = line;
            return result;
        } else {
            cur_index--;  // 回退一格
        }
    } else {  // 16. 这时候发生错误
        printf("(25)错误发生！！！\n");
        error_happen = true;
        return result;
    }
}

void LexicalAnalysis::get_char() {
    if(cur_index<len_of_buffer) {
        cur_ch = buffer[cur_index];
        cur_index++;
    }
}

void LexicalAnalysis::filter_out_space() {
    while(cur_index<len_of_buffer&&(cur_ch==' '||cur_ch=='\n' || cur_ch=='\t')) {
        if(cur_ch=='\n') line+=1;
        get_char();
    }
}

int LexicalAnalysis::is_key_word(std::string concat) {
    for(int i=0;i<len_key_words;i++) {
        if(key_words[i]==concat) return i+1;
    }
    return 0;
}

void LexicalAnalysis::showError() {
    std::cout<<"词法分析阶段第"<<line<<"行，字符"<<cur_ch<<"发生错误！"<<std::endl;
}