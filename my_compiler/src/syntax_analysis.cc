#include "syntax_analysis.hh"
#include "lexical_analysis.hh"
#include "interpreter.hh"
#include<iostream>
#include<sstream>
#include <fstream>
SyntaxAnalysis::SyntaxAnalysis(std::string code_file_path): code_file_path(code_file_path) {
}
SyntaxAnalysis::~SyntaxAnalysis(){
    //printf("调用 ~SyntaxAnalysis\n");
    delete PCode;
    delete symbol_table;
}

bool SyntaxAnalysis::get_lex_result() {
    LexicalAnalysis* lex = new LexicalAnalysis(code_file_path);
    if(lex->do_lexical_analysis()) {
        read_lex_result_file(); // 获取词法分析结果
        //printf("开始语法分析\n");
        prog(); // 语法分析开始
        return error_happen;
    } else {
        return true;
    }
}

void SyntaxAnalysis::read_lex_result_file() {
    //std::cout<<"获取lex.txt的结果"<<std::endl;
    std::ifstream ifile(lex_out_file_path, std::ios::in);
    std::string every_line;
    std::string id;   //对应key_word
    std::string name; //值
    std::string line; //程序所在行
    if(ifile) {
        while(getline(ifile, every_line)){
            std::stringstream rv(every_line);
            struct RValue tmp;
            rv>>id;
            rv>>name;
            rv>>line;
            tmp.id = atoi(id.c_str());
            tmp.name = name;
            tmp.line = atoi(line.c_str());
            all_rv.push_back(tmp);
        }
    }
    len_of_all_rv = all_rv.size();
}

void SyntaxAnalysis::prog() {
    //printf("prog\n");
    if(all_rv[rv_ptr].id==word_type.PROGRAM) {
        rv_ptr++;
        if(all_rv[rv_ptr].id!=word_type.SYMBOL){
            error_happen = true;
            show_error(1, "");
            return;
        }else {
            rv_ptr++;
            if(all_rv[rv_ptr].id!=word_type.SEMIC) {
                error_happen = true;
                show_error(0, "");
                return;
            }else {
                rv_ptr++;
                block(0, 0);
            }
        }
    }else {
        error_happen = true;
        show_error(2, "");
        return;
    }
}

void SyntaxAnalysis::block(int proc_pose, int count) {
    //printf("block\n");
    // count 是此过程的参数个数
    int pre_address = address; //记录本层之前的数据量，以便恢复时返回
    address = 3 + count;
    //暂存当前PCode.code_ptr的值，即jmp,0,0在code_ptr中的位置，用来一会回填
    int tmp_code_ptr = PCode->get_code_ptr();
    //std::cout<<"block先: JMP 0 0"<<std::endl;
    PCode->gen(PCode->get_JMP(), 0, 0);
    if(all_rv[rv_ptr].id==word_type.CON) condecl();
    if(all_rv[rv_ptr].id==word_type.VAR) vardecl();
    if (all_rv[rv_ptr].id==word_type.PROCEDURE) {
        proc();
        level--;
    }
    if(count>0) {
        for(int i=count;i>=1;i--){
            PCode->gen(PCode->get_STO(), 0, i+3-1);  // 栈内生成proc过程的形参变量
        }
    }
    //printf("tmp_code_ptr: %d\n", tmp_code_ptr);
    //std::cout<<PCode->get_code_ptr()<<std::endl;
    PCode->get_PCode_array()[tmp_code_ptr].setA(PCode->get_code_ptr());
    //std::cout<<"INT: "<<" address:"<<address<<std::endl;
    PCode->gen(PCode->get_INT(), 0, address);
    // 设置过程proc的值value, 其实就是proc在PCode中开始的下标，便于call过程可以找到
    if(symbol_table->get_table_row(proc_pose).get_type()==symbol_table->get_proc()){
        //std::cout<<"设置位置"<<proc_pose<<"的过程的 value 为"<<PCode->get_code_ptr()-1-count<<std::endl;
        symbol_table->get_table_row(proc_pose).set_value(PCode->get_code_ptr()-1-count);
    }
    body();
    //std::cout<<"退出过程的代码: "<<"OPR 0 0"<<std::endl;
    PCode->gen(PCode->get_OPR(), 0, 0); // 生成退出过程的代码，若是主程序，则直接退出程序
    address = pre_address;  // 分程序结束，恢复相关的值
    //symbol_table->set_table_ptr(init_table_ptr);
}

void SyntaxAnalysis::condecl() {
    //printf("condecl\n");
    if(all_rv[rv_ptr].id==word_type.CON) {
        rv_ptr++;
        myconst();
        while(all_rv[rv_ptr].id==word_type.COMMA) {
            rv_ptr++;
            myconst();
        }
        if(all_rv[rv_ptr].id!=word_type.SEMIC) {
            error_happen = true;
            show_error(0, "");
            return;
        }else {
            rv_ptr++;
        }
    }else {
        //printf("error in condecl: -1\n");
        error_happen = true;
        show_error(-1, "");
        return;
    }
}

void SyntaxAnalysis::myconst() {
    //printf("myconst\n");
    std::string name;
    int value;
    if(all_rv[rv_ptr].id==word_type.SYMBOL) {
        name = all_rv[rv_ptr].name;
        rv_ptr++;
        if(all_rv[rv_ptr].id==word_type.CEQU) {
            rv_ptr++;
            if(all_rv[rv_ptr].id==word_type.CONST) {
                value = atoi(all_rv[rv_ptr].name.c_str());
                if(symbol_table->is_now_exists(name, level)) {
                    //printf("error in myconst: 15\n");
                    error_happen = true;
                    show_error(15, name);
                }
                symbol_table->enter_const(name, level, value, address);
                //注意, const变量不需要addree+1, 因为其为固定量，不会进行相关指令运算。
                rv_ptr++;
            }else {
                //printf("error in myconst: 17\n");
                error_happen = true;
                show_error(17, all_rv[rv_ptr].name);
                return;
            }
        }else {
            //printf("error in myconst: 3\n");
            error_happen = true;
            show_error(3, "");
            return;
        }
    }else {
        //printf("error in myconst: 1\n");
        error_happen = true;
        show_error(1, "");
        return;
    }
}

void SyntaxAnalysis::vardecl() {
    //printf("vardecl\n");
    std::string name;
    if(all_rv[rv_ptr].id==word_type.VAR) {
        rv_ptr++;
        if(all_rv[rv_ptr].id==word_type.SYMBOL) {
            name = all_rv[rv_ptr].name;
            if(symbol_table->is_now_exists(name, level)) {
                //printf("error in vardecl: 15\n");
                error_happen = true;
                show_error(15, name);
            }
            symbol_table->enter_var(name, level, address);
            address += increament;  // 地址加1
            rv_ptr++;
            while(all_rv[rv_ptr].id==word_type.COMMA) {
                rv_ptr++;
                if(all_rv[rv_ptr].id==word_type.SYMBOL) {
                    name = all_rv[rv_ptr].name;
                    if(symbol_table->is_now_exists(name, level)) {
                        //printf("error in vardecl: 15-2\n");
                        error_happen = true;
                        show_error(15, name);
                    }
                    symbol_table->enter_var(name, level, address);
                    address += increament;  // 地址加1
                    rv_ptr++;
                }else{
                    //printf("error in vardecl: 1\n");
                    error_happen = true;
                    show_error(1, "");
                    return;
                }
            }
            if(all_rv[rv_ptr].id!=word_type.SEMIC) {
                //printf("error in vardecl: 0\n");
                error_happen = true;
                show_error(0, "");
                return;
            }else {
                rv_ptr++;
            }
        }else {
            //printf("error in vardecl: 1-2\n");
            error_happen = true;
            show_error(1, "");
            return;
        }
    }else {
        //printf("error in vardecl: -1\n");
        error_happen = true;
        show_error(-1, "");
        return;
    }
}

void SyntaxAnalysis::proc() {
    //printf("proc\n");
    if(all_rv[rv_ptr].id==word_type.PROCEDURE) {
        rv_ptr++;
        int count = 0; // 过程的形参个数
        int proc_pos = 0; // 过程在symbol_table中的位置
        std::string name;
        if(all_rv[rv_ptr].id==word_type.SYMBOL) {
            name = all_rv[rv_ptr].name;
            if(symbol_table->is_now_exists(name, level)) {
                //printf("error in proc: 15\n");
                error_happen = true;
                show_error(15, "");
            }
            proc_pos = symbol_table->get_table_ptr();
            symbol_table->enter_proc(name, level, address);
            level++;
            rv_ptr++;
            if(all_rv[rv_ptr].id==word_type.LBR) {
                rv_ptr++;
                if(all_rv[rv_ptr].id==word_type.SYMBOL) {
                    name = all_rv[rv_ptr].name;
                    symbol_table->enter_var(name, level, 3+count);
                    count++;
                    rv_ptr++;
                    while(all_rv[rv_ptr].id==word_type.COMMA) {
                        rv_ptr++;
                        if(all_rv[rv_ptr].id==word_type.SYMBOL) {
                            name = all_rv[rv_ptr].name;
                            symbol_table->enter_var(name, level, 3+count);
                            count++;
                            rv_ptr++;
                        }else {
                            //printf("error in proc: 1\n");
                            error_happen = true;
                            show_error(1, "");
                            return;
                        }
                    }
                    symbol_table->get_table_row(proc_pos).set_size(count);
                }
                if(all_rv[rv_ptr].id==word_type.RBR) {
                    rv_ptr++;
                    if(all_rv[rv_ptr].id!=word_type.SEMIC) {
                        //printf("error in proc: 0\n");
                        error_happen = true;
                        show_error(0, "");
                        return;
                    }else {
                        rv_ptr++;
                        block(proc_pos, count);
                        while(all_rv[rv_ptr].id==word_type.SEMIC) {
                            rv_ptr++;
                            proc();
                        }
                    }
                }else {
                    //printf("error in proc: 5\n");
                    error_happen = true;
                    show_error(5, "");
                    return;
                }
            }else {
                //printf("error in proc: 4\n");
                error_happen = true;
                show_error(4, "");
                return;
            }

        }else {
            //printf("error in proc: 1\n");
            error_happen = true;
            show_error(1, "");
            return;
        }
    }else {
        //printf("error in proc: -1\n");
        error_happen = true;
        show_error(-1, "");
        return;
    }
}

void SyntaxAnalysis::body() {
    //printf("body\n");
    if(all_rv[rv_ptr].id==word_type.BEGIN) {
        rv_ptr++;
        statement();
        while(all_rv[rv_ptr].id==word_type.SEMIC) {
            rv_ptr++;
            statement();
        }
        if(all_rv[rv_ptr].id==word_type.END) {
            rv_ptr++;
        }else {
            //printf("error in body: 7\n");
            error_happen = true;
            show_error(7, "");
            return;
        }
    }else {
        //printf("error in body: 6\n");
        error_happen = true;
        show_error(6, "");
        return;
    }
    //std::cout<<"body end!"<<std::endl;
}

void SyntaxAnalysis::statement() {
    //printf("statement\n");
    if(all_rv[rv_ptr].id==word_type.IF) {
        //std::cout<<"statement: IF"<<std::endl;
        rv_ptr++;
        lexp();
        if(all_rv[rv_ptr].id==word_type.THEN) {
            int code_ptr1 = PCode->get_code_ptr();
            PCode->gen(PCode->get_JPC(), 0, 0); // 这个是跳到else的内容中
            rv_ptr++;
            statement();
            int code_ptr2 = PCode->get_code_ptr();
            PCode->gen(PCode->get_JMP(), 0, 0); // 这个是跳到else结束的内容中
            //std::cout<<"statement-IF: JPC 回填地址"<<PCode->get_code_ptr()<<std::endl;
            PCode->get_PCode_array()[code_ptr1].setA(PCode->get_code_ptr());
            PCode->get_PCode_array()[code_ptr2].setA(PCode->get_code_ptr());
            if(all_rv[rv_ptr].id==word_type.ELSE) {  // 这个else也可能没有
                rv_ptr++;
                statement();
                PCode->get_PCode_array()[code_ptr2].setA(PCode->get_code_ptr());
            }
            //std::cout<<"statement-IF: JMP 回填地址"<<PCode->get_code_ptr()<<std::endl;
        }else {
            //printf("error in statement: 8\n");
            error_happen = true;
            show_error(8, "");
            return;
        }
    }else if(all_rv[rv_ptr].id==word_type.WHILE) {
        //std::cout<<"statement: while"<<std::endl;
        int code_ptr1 = PCode->get_code_ptr();  // 记住 while 的初始地址
        rv_ptr++;
        lexp();
        if(all_rv[rv_ptr].id==word_type.DO) {
            int code_ptr2 = PCode->get_code_ptr();
            PCode->gen(PCode->get_JPC(), 0, 0);
            rv_ptr++;
            body();
            PCode->gen(PCode->get_JMP(), 0, code_ptr1);  // 每次循环都得过一次lexp()
            PCode->get_PCode_array()[code_ptr2].setA(PCode->get_code_ptr());  // 回填while条件不满足时的转移地址
        }else {
            //printf("error in statement: 9\n");
            error_happen = true;
            show_error(9, "");
            return;
        }
    }else if(all_rv[rv_ptr].id==word_type.CALL) {
        //std::cout<<"statement: CALL"<<std::endl;
        int count=0; // 调用函数的参数和函数本身的参数相比对
        rv_ptr++;
        TableRow tmp_row; 
        if(all_rv[rv_ptr].id==word_type.SYMBOL) {
            if(symbol_table->is_pre_exists(all_rv[rv_ptr].name, level)) {
                tmp_row = symbol_table->get_table_row_by_name(all_rv[rv_ptr].name);
                if(tmp_row.get_type()!=symbol_table->get_proc()){
                    //printf("error in statement: 11\n");
                    error_happen = true;
                    show_error(11, "");
                    return;
                }

            }else {
                //printf("error in statement: 10\n");
                error_happen = true;
                show_error(10, "");
                return;
            }
            rv_ptr++;
            if(all_rv[rv_ptr].id==word_type.LBR) {
                rv_ptr++;
                if(all_rv[rv_ptr].id==word_type.RBR) {
                    rv_ptr++;
                    //std::cout<<"CALL "<<level-tmp_row.get_level()<<" "<<tmp_row.get_value()<<std::endl;
                    PCode->gen(PCode->get_CAL(), level-tmp_row.get_level(), tmp_row.get_value());
                }else {
                    exp();
                    count++;
                    while(all_rv[rv_ptr].id==word_type.COMMA) {
                        rv_ptr++;
                        exp();
                        count++;
                    }
                    if(count != tmp_row.get_size()) {
                        error_happen = true;
                        show_error(16, tmp_row.get_name());
                        return;
                    }
                    PCode->gen(PCode->get_CAL(), level-tmp_row.get_level(), tmp_row.get_value());
                    if(all_rv[rv_ptr].id==word_type.RBR) {
                        rv_ptr++;
                    }else {
                        //printf("error in statement: 5\n");
                        error_happen = true;
                        show_error(5, "");
                        return;
                    }
                }
            }else {
                //printf("error in statement: 4\n");
                error_happen = true;
                show_error(4, "");
                return;
            }
        }else {
            //printf("error in statement: 1\n");
            error_happen = true;
            show_error(1, "");
            return;
        }
    }else if(all_rv[rv_ptr].id==word_type.READ) {
        //std::cout<<"statement: READ"<<std::endl;
        rv_ptr++;
        if(all_rv[rv_ptr].id==word_type.LBR) {
            rv_ptr++;
            if(all_rv[rv_ptr].id==word_type.SYMBOL) {
                if(!symbol_table->is_pre_exists(all_rv[rv_ptr].name, level)) {
                    //printf("error in statement: 10\n");
                    error_happen = true;
                    show_error(10, "");
                    return;
                }else {
                    TableRow temp_row = symbol_table->get_table_row_by_name(all_rv[rv_ptr].name);
                    if(temp_row.get_type()==symbol_table->get_var()) {
                        PCode->gen(PCode->get_OPR(), 0, 16);
                        PCode->gen(PCode->get_STO(), level-temp_row.get_level(), temp_row.get_address());
                    }else {
                        //printf("error in statement: 12\n");
                        error_happen = true;
                        show_error(12, "");
                        return;
                    }
                }
                rv_ptr++;
                while(all_rv[rv_ptr].id==word_type.COMMA) {
                    rv_ptr++;
                    if(all_rv[rv_ptr].id==word_type.SYMBOL) {
                        if(!symbol_table->is_pre_exists(all_rv[rv_ptr].name, level)) {
                            printf("error in statement: 10\n");
                            error_happen = true;
                            show_error(10, "");
                            return;
                        }else {
                            TableRow temp_row = symbol_table->get_table_row_by_name(all_rv[rv_ptr].name);
                            if(temp_row.get_type()==symbol_table->get_var()) {
                                PCode->gen(PCode->get_OPR(), 0, 16);  // 从标准输入输入数字到栈顶
                                PCode->gen(PCode->get_STO(), level-temp_row.get_level(), temp_row.get_address());
                            }else {
                                printf("error in statement: 12-2\n");
                                error_happen = true;
                                show_error(12, "");
                                return;
                            }
                        }
                        rv_ptr++;
                    }else {
                        printf("error in statement: 1-2\n");
                        error_happen = true;
                        show_error(1, "");
                        return;
                    }
                }
                if(all_rv[rv_ptr].id==word_type.RBR) {
                    rv_ptr++;
                }else {
                    //printf("error in statement: 5\n");
                    error_happen = true;
                    show_error(5, "");
                    return;
                }
            }else {
                //printf("error in statement: 1-3\n");
                error_happen = true;
                show_error(1, "");
                return;
            }
        }else {
            //printf("error in statement: 4-2\n");
            error_happen = true;
            show_error(4, "");
            return;
        }
    }else if(all_rv[rv_ptr].id==word_type.WRITE) {
        //std::cout<<"statement: WRITE"<<std::endl;
        rv_ptr++;
        if(all_rv[rv_ptr].id==word_type.LBR) {
            rv_ptr++;
            exp();
            PCode->gen(PCode->get_OPR(), 0, 14); //输出栈顶的值到屏幕
            while(all_rv[rv_ptr].id==word_type.COMMA) {
                rv_ptr++;
                exp();
                PCode->gen(PCode->get_OPR(), 0, 14); //输出栈顶的值到屏幕
            }
            PCode->gen(PCode->get_OPR(), 0, 15); //输出换行
            if(all_rv[rv_ptr].id==word_type.RBR) {
                rv_ptr++;
            }else {
                //printf("error in statement: 5-2\n");
                error_happen = true;
                show_error(5, "");
                return;  
            }
        }else {
            //printf("error in statement: 4-3\n");
            error_happen = true;
            show_error(4, "");
            return;
        }

    }else if(all_rv[rv_ptr].id==word_type.SYMBOL) {  // 赋值语句
        //std::cout<<"statement: symbol"<<std::endl;
        std::string name = all_rv[rv_ptr].name;
        rv_ptr++;
        if(all_rv[rv_ptr].id==word_type.CEQU) {
            rv_ptr++;
            exp();
            if(!symbol_table->is_pre_exists(name, level)) {
                //printf("error in statement: 14\n");
                error_happen = true;
                show_error(14, name);
                return;
            }else {
                TableRow temp_row = symbol_table->get_table_row_by_name(name);
                if(temp_row.get_type()==symbol_table->get_var()) {
                    PCode->gen(PCode->get_STO(), level-temp_row.get_level(), temp_row.get_address());
                }else {
                    //printf("error in statement: 12-3\n");
                    error_happen = true;
                    show_error(12, "");
                    return;
                }
            }
        }else {
            //printf("error in statement: 3\n");
            error_happen = true;
            show_error(3, "");
            return;
        }
    }else if(all_rv[rv_ptr].id==word_type.BEGIN) {
        //std::cout<<"statement: BEGIN"<<std::endl;
        body();
    }else {
        //printf("error in statement: 1-5\n");
        error_happen = true;
        show_error(1, "");
        return;
    }
}

void SyntaxAnalysis::lexp() {
    //printf("lexp\n");
    if(all_rv[rv_ptr].id==word_type.ODD) {  // 奇偶判断
        rv_ptr++;
        exp();
        PCode->gen(PCode->get_OPR(), 0, 6);  //OPR 0 6	栈顶元素的奇偶判断，结果值在栈顶
    }else {
        exp();
        int lop_operator = lop();
        exp();
        if(lop_operator==word_type.EQU) {
            PCode->gen(PCode->get_OPR(), 0, 8);  //OPR 0 8	次栈顶与栈顶是否相等，退两个栈元素，结果值进栈
        }else if(lop_operator==word_type.NEQU) {
            PCode->gen(PCode->get_OPR(), 0, 9);  //OPR 0 9	次栈顶与栈顶是否不等，退两个栈元素，结果值进栈
        }else if(lop_operator==word_type.LES) {
            PCode->gen(PCode->get_OPR(), 0, 10);  //OPR 0 10	次栈顶是否小于栈顶，退两个栈元素，结果值进栈
        }else if(lop_operator==word_type.LESE) {
            PCode->gen(PCode->get_OPR(), 0, 13);  // OPR 0 13	次栈顶是否小于等于栈顶，退两个栈元素，结果值进栈
        }else if(lop_operator==word_type.LAR) {
            PCode->gen(PCode->get_OPR(), 0, 12);  //OPR 0 12	次栈顶是否大于栈顶，退两个栈元素，结果值进栈
        }else if(lop_operator==word_type.LARE) {
            PCode->gen(PCode->get_OPR(), 0, 11);  //OPR 0 11	次栈顶是否大于等于栈顶，退两个栈元素，结果值进栈
        }
    }
}

void SyntaxAnalysis::exp() {
    //printf("exp\n");
    int temp_id = all_rv[rv_ptr].id;
    if(temp_id==word_type.ADD) {
        rv_ptr++;
    }else if(temp_id==word_type.SUB) {
        rv_ptr++;
    }
    term();
    if(temp_id==word_type.SUB) {
        PCode->gen(PCode->get_OPR(), 0, 11); // OPR 0 1	栈顶元素取反
    }

    while(all_rv[rv_ptr].id==word_type.ADD || all_rv[rv_ptr].id==word_type.SUB) {
        temp_id = all_rv[rv_ptr].id;
        rv_ptr++;
        term();
        if(temp_id==word_type.ADD) {
            PCode->gen(PCode->get_OPR(), 0, 2);  //OPR 0 2	次栈顶与栈顶相加，退两个栈元素，结果值进栈
        }else if(temp_id==word_type.SUB) {
            PCode->gen(PCode->get_OPR(), 0, 3);  //OPR 0 3	次栈顶减去栈顶，退两个栈元素，结果值进栈
        }
    }
}

void SyntaxAnalysis::term() {
    //printf("term\n");
    factor();
    while(all_rv[rv_ptr].id==word_type.MUL || all_rv[rv_ptr].id==word_type.DIV) {
        int temp_id = all_rv[rv_ptr].id;
        rv_ptr++;
        factor();
        if(temp_id==word_type.MUL) {
            PCode->gen(PCode->get_OPR(), 0, 4);  //OPR 0 4	次栈顶乘以栈顶，退两个栈元素，结果值进栈
        }else if(temp_id==word_type.DIV) {
            PCode->gen(PCode->get_OPR(), 0, 5);  //OPR 0 5	次栈顶除以栈顶，退两个栈元素，结果值进栈
        }
    }
}

void SyntaxAnalysis::factor() {
    //printf("factor\n");
    if(all_rv[rv_ptr].id==word_type.CONST) {
        //std::cout<<"向 stack 中 LIT 常值, "<<"value: "<<atoi(all_rv[rv_ptr].name.c_str())<<std::endl;
        PCode->gen(PCode->get_LIT(), 0, atoi(all_rv[rv_ptr].name.c_str()));
        rv_ptr++;
    }else if(all_rv[rv_ptr].id==word_type.LBR) {
        rv_ptr++;
        exp();
        if(all_rv[rv_ptr].id==word_type.RBR) {
            rv_ptr++;
        }else {
            //printf("error in factor: 5\n");
            error_happen = true;
            show_error(5, "");
            return;
        }
    }else if(all_rv[rv_ptr].id==word_type.SYMBOL) {
        std::string name = all_rv[rv_ptr].name;
        if(!symbol_table->is_pre_exists(name, level)) {
            //printf("error in factor: 10\n");
            error_happen = true;
            show_error(10, "");
            return;
        }else {
            TableRow temp_row = symbol_table->get_table_row_by_name(name);
            if(temp_row.get_type()==symbol_table->get_my_const()) {
                //std::cout<<"向 stack 中 LIT 常量, "<<"name: "<<temp_row.get_name()<<"value: "<<temp_row.get_value()<<std::endl;
                PCode->gen(PCode->get_LIT(), 0, temp_row.get_value());
            }else if(temp_row.get_type()==symbol_table->get_var()) {
                //std::cout<<"向 stack 中 LOD 变量, "<<"level差: "<<level-temp_row.get_level()<<" address: "<<temp_row.get_address()<<std::endl;
                PCode->gen(PCode->get_LOD(), level-temp_row.get_level(), temp_row.get_address());
            }
        }
        rv_ptr++;
    }else {
        //printf("error in factor: 1\n");
        error_happen = true;
        show_error(1, "");
        return;
    }
    //std::cout<<"factor end!"<<std::endl;
}

int SyntaxAnalysis::lop() {
    //printf("lop\n");
    if(all_rv[rv_ptr].id == word_type.EQU) {
        //std::cout<<"lop: ="<<std::endl;
        rv_ptr++;
        return word_type.EQU;
    }else if(all_rv[rv_ptr].id == word_type.NEQU) {
        //std::cout<<"lop: <>"<<std::endl;
        rv_ptr++;
        return word_type.NEQU;
    }else if(all_rv[rv_ptr].id == word_type.LAR) {
        //std::cout<<"lop: >"<<std::endl;
        rv_ptr++;
        return  word_type.LAR;
    }else if(all_rv[rv_ptr].id == word_type.LARE) {
        //std::cout<<"lop: >="<<std::endl;
        rv_ptr++;
        return  word_type.LARE;
    }else if(all_rv[rv_ptr].id == word_type.LES) {
        //std::cout<<"lop: <"<<std::endl;
        rv_ptr++;
        return  word_type.LES;
    }else if(all_rv[rv_ptr].id == word_type.LESE) {
        //std::cout<<"lop: <="<<std::endl;
        rv_ptr++;
        return  word_type.LESE;
    }
    return -1;
}

void SyntaxAnalysis::show_error(int i, std::string name) {
    switch(i){
        case -1:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"wrong token"<<std::endl;  //常量定义不是const开头，变量定义不是var开头, 过程不是procedure开头
            break;
        case 0:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Missing semicolon"<<std::endl;  //缺少分号
            break;
        case 1:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Identifier illegal"<<std::endl;  //标识符不合法
            break;
        case 2:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"The beginning of program must be 'program'"<<std::endl;
            break;
        case 3:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Assign must be ':='"<<std::endl;
            break;
        case 4:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Missing '('"<<std::endl;
            break; 
        case 5:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Missing ')'"<<std::endl;
            break; 
        case 6:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Missing 'begin'"<<std::endl;
            break; 
        case 7:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Missing 'end'"<<std::endl;
            break; 
        case 8:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Missing 'then'"<<std::endl;
            break; 
        case 9:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Missing 'do'"<<std::endl;
            break; 
        case 10:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Not exist '"<<all_rv[rv_ptr].name<<"'"<<std::endl;
            break; 
        case 11:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<all_rv[rv_ptr].name<<" is not a procedure"<<std::endl;
            break; 
        case 12:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"'"<<all_rv[rv_ptr].name<<"' is not a variable"<<std::endl;
            break; 

        case 15:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Already exist '"<<name<<"'"<<std::endl; // 变量已经存在
            break;
        case 16:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<"Number of parameters of procedure "<<name<<" is incorrect!"<<std::endl; // 变量已经存在
            break;
        case 17:
            std::cout<<"ERROR "<<i<<" in line "<<all_rv[rv_ptr].line<<":";
            std::cout<<name<<"is not a CONST!"<<std::endl; // 常量值不合法
            break;
        default:
            std::cout<<"未知名错误类型"<<std::endl;
            break;
    }
}

void SyntaxAnalysis::show_PCode() {
    PerPCode* PCode_array = PCode->get_PCode_array();
    int pcode_ptr = PCode->get_code_ptr();
    int F, L, A;
    for(int i=0;i<pcode_ptr;i++) {
        F = PCode_array[i].getF();
        L = PCode_array[i].getL();
        A = PCode_array[i].getA();
        switch(F) {
            case 0:
                printf("LIT  ");
                break;
            case 1:
                printf("OPR  ");
                break;
            case 2:
                printf("LOD  ");
                break;
            case 3:
                printf("STO  ");
                break;
            case 4:
                printf("CALL  ");
                break;
            case 5:
                printf("INT  ");
                break;
            case 6:
                printf("JMP  ");
                break;
            case 7:
                printf("JPC  ");
                break;
            case 8:
                printf("RED  ");
                break;
            case 9:
                printf("WRT  ");
                break;
        }
        printf("%d  %d\n", L, A);
    }
}

void SyntaxAnalysis::show_symbol_table() {
    printf("type,name,level,address,value,size\n");
    TableRow* table = symbol_table->get_table();
    int table_ptr = symbol_table->get_table_ptr();
    for(int i=0;i<table_ptr;i++) {
        std::cout<<table[i].get_type()<<" "<<table[i].get_name()<<" ";
        std::cout<<table[i].get_level()<<" "<<table[i].get_address()<<" ";
        std::cout<<table[i].get_value()<<" "<<table[i].get_size()<<"\n";
    }
}

void SyntaxAnalysis::interpreter() {
    if(error_happen) return;
    Interpreter *inter = new Interpreter(this->PCode);
    inter->interpreter();
}

