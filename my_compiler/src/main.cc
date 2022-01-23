// g++ -std=c++11  -o my_compiler/out/main my_compiler/src/*.cc
// my_compiler/out/main 
// /home/wangzi06/learn/compile/PL-0-C++/test_PL0/testPL1.txt
#include<iostream>
#include<string>
#include<fstream>
#include "syntax_analysis.hh"
int main(int argc, char **argv){
    std::string code_file_path;
    std::cout<<">Please input code_file_path:\n>>";
    std::cin>>code_file_path;
    SyntaxAnalysis* syntax = new SyntaxAnalysis(code_file_path);
    if(!syntax->get_lex_result()) { // 词法分析，顺带完成语法分析
        std::cout<<"compile succeed！"<<std::endl;
    }
    int choice;
    printf(">please input your choice:\n");
    printf(">1.run now  2.show PCode  3.show Symbol Table\n");
    printf(">>");
    std::cin>>choice;
    if(choice==1) {
        printf("running...\n");
        syntax->interpreter();
    } else if(choice==2) {
        printf("The PCode is:\n");
        syntax->show_PCode();
    } else {
        printf("The Symbol table is:\n");
        syntax->show_symbol_table();
    }
    delete syntax;
    return 0;
}