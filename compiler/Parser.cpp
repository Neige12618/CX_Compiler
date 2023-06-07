#include "Parser.h"

#include <utility>
#include <iostream>
#include <iomanip>
#include <thread>



/// 将输入当作源码
Parser::Parser(std::string input) {
    this->input = std::move(input);
    init();
}

Parser::Parser(std::string &&input) {
    this->input = input;
    init();
}

/// 初始化所有集合
    void Parser::init() {
    declBegSys.insert(Symbol::ConstSym);
    declBegSys.insert(Symbol::IntSym);
    declBegSys.insert(Symbol::BoolSym);

    statBegSys.insert(Symbol::Ident);
    statBegSys.insert(Symbol::WriteSym);
    statBegSys.insert(Symbol::ReadSym);
    statBegSys.insert(Symbol::IfSym);
    statBegSys.insert(Symbol::WhileSym);
    statBegSys.insert(Symbol::LBrace);

    aFacBegSys.insert(Symbol::Ident);
    aFacBegSys.insert(Symbol::Number);
    aFacBegSys.insert(Symbol::LParen);
    aFacBegSys.insert(Symbol::Minus);

    bFacBegSys.insert(Symbol::TrueSym);
    bFacBegSys.insert(Symbol::FalseSym);
    bFacBegSys.insert(Symbol::Ident);
    bFacBegSys.insert(Symbol::Not);
    bFacBegSys.insert(Symbol::LParen);
    bFacBegSys.insert(Symbol::Number);

    blockBegSys.insert(Symbol::LBrace);

    compareSys.insert(Symbol::Eql);
    compareSys.insert(Symbol::Lss);
    compareSys.insert(Symbol::Leq);
    compareSys.insert(Symbol::Geq);
    compareSys.insert(Symbol::Gtr);
    compareSys.insert(Symbol::Neq);


    WSym["const"] = Symbol::ConstSym;
    WSym["if"] = Symbol::IfSym;
    WSym["else"] = Symbol::ElseSym;
    WSym["int"] = Symbol::IntSym;
    WSym["bool"] = Symbol::BoolSym;
    WSym["while"] = Symbol::WhileSym;
    WSym["write"] = Symbol::WriteSym;
    WSym["read"] = Symbol::ReadSym;
    WSym["true"] = Symbol::TrueSym;
    WSym["false"] = Symbol::FalseSym;

    SSym['+'] = Symbol::Plus;
    SSym['-'] = Symbol::Minus;
    SSym['('] = Symbol::LParen;
    SSym[')'] = Symbol::RParen;
    SSym['{'] = Symbol::LBrace;
    SSym['}'] = Symbol::RBrace;
    SSym[','] = Symbol::Comma;
    SSym['.'] = Symbol::Period;
    SSym['!'] = Symbol::Not;
    SSym[';'] = Symbol::Semicolon;
    SSym['%'] = Symbol::Mod;
    SSym['^'] = Symbol::Xor;

}

void Parser::error(ParserError err, bool before) {
    std::string err_msg("*****");
    usize count = before ? last_char_count: char_count;
    for (int i = 1; i < count; ++i) {
        err_msg.push_back(' ');
    }

    err_msg.append(" ^ ");
    std::string err_info;


    switch (err) {
        case ParserError::ConstAssign:
            err_info = "can not assign const twice";
            break;
        case ParserError::NumOverflow:
            err_info = "number should be less than 99999999";
            break;
        case ParserError::AFactorTest:
            err_info = "a factor test failed";
            break;
        case ParserError::UndeclaredIdent:
            err_info = "undeclared ident, ident should be declared before using";
            break;
        case ParserError::OutOfBlock:
            err_info = "this variable has been dropped in previous block";
            break;
        case ParserError::ExpectedRParen:
            err_info = "there should be a ')'";
            break;
        case ParserError::BecomesButEql:
            err_info = "expect = but ==";
            break;
        case ParserError::ExpectedSemicolon:
            err_info = "there should be a ';'";
            break;
        case ParserError::ExpectedBecomes:
            err_info = "there should be a '='";
            break;
        case ParserError::ExpectedLParen:
            err_info = "there should be a '('";
            break;
        case ParserError::ExpectedIdent:
            err_info = "there should be a identifier";
            break;
        case ParserError::StatementTest:
            err_info = "a statement test failed";
            break;
        case ParserError::LevelOverflow:
            err_info = "too much block";
            break;
        case ParserError::ExpectedRBrace:
            err_info = "there should be a '}'";
            break;
        case ParserError::DeclareTest:
            err_info = "a declare test failed";
            break;
        case ParserError::ExpectedCompare:
            err_info = "there should be a compare operator";
            break;
        case ParserError::ExpectedIntOrBool:
            err_info = "there should be an int or bool";
            break;
        case ParserError::BFactorTest:
            err_info = "b factor test failed";
            break;
        case ParserError::ExpectedLBrace:
            err_info = "there should be a '{'";
            break;
        case ParserError::ExpectedInitial:
            err_info = "const should be initial at declaration";
            break;
        case ParserError::ExpectedLogicOr:
            err_info = "there should be a '|'";
            break;
        case ParserError::ExpectedLogicAnd:
            err_info = "there should be a '&'";
            break;
    }
    error_count += 1;
    Error error1;
    error1.description = std::move(err_msg + err_info);
    error1.line_num = last_line_num;
    err_msgs.emplace_back(error1);
}

/// 获得下一个字符
/// 如果存在错误, 会在下一行的开始打印
void Parser::get_char() {
    current_char = input.front();
    if (!input.empty()) {
        input.erase(input.begin());
    } else {
        if (current_symbol != Symbol::RBrace) {
            current_symbol = Symbol::Nul;
        }
        current_char = '\n';
        std::this_thread::yield();
    }

    if (line_start) {
        line_num += 1;
        char_count = 0;
        source_code << line_num << std::setw(4);
    } else {
        char_count += 1;
    }
    source_code << current_char;
    line_start = (current_char == '\n');
}


Symbol Parser::get_symbol_from_string(const std::string& s) {
    if (WSym.count(s)) {
        return WSym[s];
    } else {
        return Symbol::Ident;
    }
}

/// 获得下一个符号, 存入当前符号
void Parser::get_symbol() {
    auto is_identifier = [](char c) {
        return isalpha(c) || isdigit(c);
    };

    last_line_num = line_num == 0 ? 1: line_num;
    last_char_count = char_count;
    while (isspace(current_char)) {
        get_char();
    }
    std::string identifier{};
    usize number{};
    std::string num_string{};


    switch(current_char) {
        case 'a' ... 'z':
        case 'A' ... 'Z': {
            while (is_identifier(current_char)) {
                if (identifier.length() < MAX_IDENTIFIER_LEN) {
                    identifier.push_back(current_char);
                }
                get_char();
            }

            current_symbol = get_symbol_from_string(identifier);
            // 如果是true或者false这种, 则不是标识符
            if (current_symbol == Symbol::TrueSym || current_symbol == Symbol::FalseSym) {
                last_num = (current_symbol == Symbol::TrueSym);
            } else {
                // 记录上一个标识符
                last_id = identifier;
            }
            break;
        }
        case '0' ... '9': {
            current_symbol = Symbol::Number;

            while (isdigit(current_char)) {
                num_string.push_back(current_char);
                get_char();
            }
            // 不应该比 99999999 大
            if (num_string.length() <= 8) {
                number = stoi(num_string);
            } else {
                error(ParserError::NumOverflow, false);
            }

            last_num = (i64)number;
            break;
        }
        case '<': {
            get_char();
            if (current_char == '=') {
                current_symbol = Symbol::Leq;
                get_char();
            } else {
                current_symbol = Symbol::Lss;
            }
            break;
        }
        case '=': {
            get_char();
            if (current_char == '=') {
                current_symbol = Symbol::Eql;
                get_char();
            } else {
                current_symbol = Symbol::Becomes;
            }
            break;
        }
        case '>': {
            get_char();
            if (current_char == '=') {
                current_symbol = Symbol::Geq;
                get_char();
            } else {
                current_symbol = Symbol::Gtr;
            }
            break;
        }
        case '!': {
            get_char();
            if (current_char == '=') {
                current_symbol = Symbol::Neq;
                get_char();
            } else {
                current_symbol = Symbol::Not;
            }
            break;
        }
        case '&': {
            get_char();
            if (current_char == '&') {
                current_symbol = Symbol::LogicAnd;
                get_char();
            } else {
                current_symbol = Symbol::And;
            }
            break;
        }
        case '|': {
            get_char();
            if (current_char == '|') {
                current_symbol = Symbol::LogicOr;
                get_char();
            } else {
                current_symbol = Symbol::Or;
            }
            break;
        }
        case '/': {
            get_char();
            if (current_char == '*') {
                current_symbol = Symbol::LAnnotate;
                get_char();
                while (current_symbol != Symbol::RAnnotate && !input.empty()) {
                    get_symbol();
                }
                get_symbol();
            } else if (current_char == '/') {
                while (current_char != '\n') {
                    get_char();
                }
                get_symbol();
            } else {
                current_symbol = Symbol::Slash;
            }
            break;
        }
        case '*': {
            get_char();
            if (current_char == '/') {
                current_symbol = Symbol::RAnnotate;
                get_char();
            } else {
                current_symbol = Symbol::Times;
            }
            break;
        }
        default: {
            // 如果不是上面的符号, 则在单个字符集中寻找
            if (SSym.count(current_char)) {
                current_symbol = SSym[current_char];
            }

            get_char();
            break;
        }
    }
}


void Parser::generate_instruction(Fct f, usize l, usize a) {
    if (code_index > CXMAX) {
        exit(2);
    } else {
        code_words.emplace_back(
                f, l, a
                );
        code_index += 1;
    }
}

/// 列出当前block生成的代码, 调试生成的代码
void Parser::list_code(usize code_start) {
    usize i;
    for (i = code_start; i < code_words.size(); ++i) {
        virtual_code << std::setw(3) << i << "  "
            << Fct2string(code_words[i].function)  << "  "
            <<  code_words[i].level << "  "
            <<   code_words[i].address << "\n";
    }
}

void Parser::enter(Obj k) {
    table_index += 1;
    usize data_ind = 0;
    switch (k) {
        case Obj::Constant: {
            if (last_num > INT_MAX) {
                error(ParserError::NumOverflow, false);
            }
            ident_table.emplace_back(
                    last_id, k, last_num, 0, 0
            );
            break;
        }

        case Obj::Int:
        case Obj::Bool: {
            data_ind = data_indices.back();
            data_indices.pop_back();
            ident_table.emplace_back(
                    last_id, k, last_num, current_level, data_ind
            );
            data_indices.push_back(data_ind + 1);
            break;
        }

        case Obj::Block: {
            ident_table.emplace_back(
                    "block", k, 0, current_level, data_ind
            );
            break;
        }
    }
    last_num = 0;
}


usize Parser::position(const std::string &id) {
    usize i;
    usize ans = ident_table.size();
    for (i = 0; i < ident_table.size(); ++i) {
        if (ident_table[i].name == id && current_level >= ident_table[i].level) {
            ans = i;
        }
    }
    return ans;
}

void Parser::aFactor(const SymSet& fSys) {
    SymSet nextLev= fSys;
    bool neg = false;
    nextLev.insert(aFacBegSys.begin(), aFacBegSys.end());
    test(aFacBegSys, nextLev, ParserError::AFactorTest);

    if (current_symbol == Symbol::Minus) {
        neg = true;
        get_symbol();
    }

    switch (current_symbol) {
        case Symbol::Number: {
            generate_instruction(Fct::Lit, 0, last_num);
            get_symbol();
            break;
        }
        case Symbol::Ident: {
            usize loc = position(last_id);
            if (loc == ident_table.size()) {
                error(ParserError::UndeclaredIdent, false);
            } else {
                if (ident_table[loc].kind == Obj::Constant) {
                    generate_instruction(Fct::Lit, 0, ident_table[loc].val);
                } else {
                    if (current_level - ident_table[loc].level < 0) {
                        error(ParserError::OutOfBlock, false);
                    } else {
                        generate_instruction(Fct::Lod, current_level - ident_table[loc].level, ident_table[loc].adr);
                    }
                }
            }
            get_symbol();
            break;
        }

        case Symbol::LParen: {
            aExpr(fSys);
            if (current_symbol == Symbol::RParen) {
                get_symbol();
            } else {
                error(ParserError::ExpectedRParen, false);
            }
            break;
        }
        default: {
            break;
        }
    }
    if (neg) {
        generate_instruction(Fct::Opr, 0, Opr::Neg);
    }
}



void Parser::aTerm(const SymSet& fSys) {
    aFactor(fSys);

    switch (current_symbol) {
        case Symbol::And: {
            get_symbol();
            aTerm(fSys);
            generate_instruction(Fct::Opr, 0, Opr::And);
            break;
        }
        case Symbol::Or: {
            get_symbol();
            aTerm(fSys);
            generate_instruction(Fct::Opr, 0, Opr::Or);
            break;
        }
        case Symbol::Xor: {
            get_symbol();
            aTerm(fSys);
            generate_instruction(Fct::Opr, 0, Opr::Xor);
            break;
        }
        case Symbol::Times: {
            get_symbol();
            aFactor(fSys);
            generate_instruction(Fct::Opr, 0, Opr::Mul);
            break;
        }
        case Symbol::Slash: {
            get_symbol();
            aFactor(fSys);
            generate_instruction(Fct::Opr, 0, Opr::Div);
            break;
        }
        case Symbol::Mod: {
            get_symbol();
            aFactor(fSys);
            generate_instruction(Fct::Opr, 0, Opr::Mod);
            break;
        }
        default:{
            break;
        }
    }
}

void Parser::aExpr(const SymSet& fSys) {
    aTerm(fSys);

    switch (current_symbol) {
        case Symbol::Plus: {
            get_symbol();
            aTerm(fSys);
            generate_instruction(Fct::Opr, 0, Opr::Add);
            break;
        }
        case Symbol::Minus: {
            get_symbol();
            aTerm(fSys);
            generate_instruction(Fct::Opr, 0, Opr::Sub);
            break;
        }
        default:{
            break;
        }
    }
}


void Parser::statement(SymSet &fSys) {
    SymSet nextLev = fSys;
    nextLev.insert(statBegSys.begin(), statBegSys.end());
    if (statBegSys.count(current_symbol)) {
        switch (current_symbol) {
            case Symbol::Ident: {
                usize loc = position(last_id);
                if (loc == ident_table.size()) {
                    get_symbol();
                    error(ParserError::UndeclaredIdent, true);
                    break;
                }


                Obj kind = ident_table[loc].kind;

                get_symbol();
                if (current_symbol == Symbol::Becomes || current_symbol == Symbol::Eql) {
                    if (current_symbol == Symbol::Eql) {
                        error(ParserError::BecomesButEql, true);
                    }
                    get_symbol();
                    if (kind == Obj::Int) {
                        aExpr(fSys);
                    } else if (kind == Obj::Bool){
                        bExpr(fSys);
                    } else {
                        aExpr(fSys);
                        error(ParserError::ConstAssign, true);
                    }
                    generate_instruction(Fct::Sto,
                                         current_level - ident_table[loc].level,
                                         ident_table[loc].adr);

                    if (current_symbol == Symbol::Semicolon) {
                        get_symbol();
                    } else {
                        error(ParserError::ExpectedSemicolon, false);
                    }
                }
                else {
                    error(ParserError::ExpectedBecomes, false);
                }
                break;
            }
            case Symbol::IfSym: {
                get_symbol();

                if (current_symbol == Symbol::LParen) {
                    get_symbol();
                    bExpr(fSys);

                    if (current_symbol == Symbol::RParen) {
                        get_symbol();
                    } else {
                        error(ParserError::ExpectedRParen, false);
                    }

                    usize index = code_index;
                    generate_instruction(Fct::Jpc, 0, 0);


                    SymSet nextLev1 = fSys;
                    nextLev1.insert(Symbol::ElseSym);
                    statement(nextLev1);

                    code_words[index].address = (i64)code_index;

                    if (current_symbol == Symbol::ElseSym) {
                        usize index1 = code_index;
                        get_symbol();
                        generate_instruction(Fct::Jpc, 0, 0);
                        code_words[index].address = (i64)code_index;
                        statement(fSys);
                        code_words[index1].address = (i64)code_index;
                    }
                } else {
                    error(ParserError::ExpectedLParen, true);
                }
                break;
            }

            case Symbol::WhileSym:{
                get_symbol();

                if (current_symbol == Symbol::LParen) {
                    get_symbol();
                    usize index1 = code_index;
                    bExpr(fSys);

                    usize index2 = code_index;
                    generate_instruction(Fct::Jpc, 0, 0);

                    if (current_symbol == Symbol::RParen) {
                        get_symbol();
                    } else {
                        error(ParserError::ExpectedRParen, true);
                    }
                    statement(fSys);
                    generate_instruction(Fct::Jmp, 0, index1);
                    code_words[index2].address = (i64)code_index;
                } else {
                    error(ParserError::ExpectedLParen, true);
                }

                break;

            }
            case Symbol::WriteSym: {
                get_symbol();
                if (current_symbol == Symbol::LParen) {
                    get_symbol();
                    aExpr(fSys);

                    generate_instruction(Fct::Opr, 0, Opr::Write);
                    if (current_symbol == Symbol::RParen) {
                        get_symbol();
                        if (current_symbol == Symbol::Semicolon) {
                            get_symbol();
                        } else {
                            error(ParserError::ExpectedSemicolon, true);
                        }
                    } else {
                        error(ParserError::ExpectedRParen, true);
                    }

                } else {
                    error(ParserError::ExpectedLParen, true);
                }

                break;
            }
            case Symbol::ReadSym: {
                get_symbol();

                if (current_symbol == Symbol::LParen) {
                    get_symbol();
                    if (current_symbol == Symbol::Ident) {
                        usize loc = position(last_id);
                        if (loc == ident_table.size()) {
                            error(ParserError::UndeclaredIdent, false);
                            break;
                        }
                        aFactor(fSys);
                        generate_instruction(Fct::Opr, 0, Opr::Read);
                        generate_instruction(Fct::Sto,
                                             current_level - ident_table[loc].level,
                                             ident_table[loc].adr);
                        if (current_symbol == Symbol::RParen) {
                            get_symbol();
                            if (current_symbol == Symbol::Semicolon) {
                                get_symbol();
                            } else {
                                error(ParserError::ExpectedSemicolon, true);
                            }
                        } else {
                            error(ParserError::ExpectedRParen, true);
                        }
                    } else {
                        error(ParserError::ExpectedIdent, true);
                    }


                } else {
                    error(ParserError::ExpectedLParen, true);
                }

                break;
            }
            case Symbol::LBrace: {
                block(fSys);
                break;
            }
            default: {
                nextLev = fSys;
                nextLev.insert(Symbol::RBrace);
                block(nextLev);
                break;
            }
        }
    }
    nextLev.insert(Symbol::Nul);
    fSys.insert(Symbol::RAnnotate);
    fSys.insert(Symbol::RBrace);
    test(fSys, nextLev, ParserError::StatementTest);
}




/// 进入block
void Parser::block(SymSet &fSys) {
    current_level += 1;
    data_indices.push_back(1);


    if (table_index < ident_table.size()) {
        ident_table[table_index].adr = code_index;
    }


    if (current_level > LEVMAX) {
        error(ParserError::LevelOverflow, true);
    }


    test(blockBegSys, fSys, ParserError::ExpectedLBrace);

    if (current_symbol == Symbol::LBrace) {
        enter(Obj::Block);
        get_symbol();
    }
    while (declBegSys.count(current_symbol)) {
        declare();
    }


    usize data_index = data_indices.back();
    data_indices.pop_back();
    generate_instruction(Fct::Int, 0, data_index);
    while (statBegSys.count(current_symbol)) {
        statement(fSys);
    }
    if (current_symbol == Symbol::RBrace) {
        if (current_level == 1) {
            current_level = 0;
            generate_instruction(Fct::Ret, 0, 0);
            return ;
        } else {
            generate_instruction(Fct::Lev, 0, 0);
        }
        get_symbol();
    } else {
        error(ParserError::ExpectedRBrace, true);
    }
    current_level -= 1;
}

/// 判断当前的符号是否符合当前块的要求
void Parser::test(const SymSet& s1, const SymSet& s2, ParserError e) {
    if (!s1.count(current_symbol)) {
        // 如果 s1 里没有当前的符号 则报错
        error(e, true);
        // 跳出当前这部分
        while (!s1.count(current_symbol) && !s2.count(current_symbol)) {
            get_symbol();
        }
    }
}


void Parser::declare() {
    while (declBegSys.count(current_symbol)) {
        if (current_symbol == Symbol::IntSym || current_symbol == Symbol::BoolSym) {
            var_declare();
        } else if (current_symbol == Symbol::ConstSym) {
            const_declare();
        }
    }
    SymSet nextLev = declBegSys;
    nextLev.insert(statBegSys.begin(), statBegSys.end());
    nextLev.insert(Symbol::RBrace);
    nextLev.insert(Symbol::Nul);
    nextLev.insert(Symbol::RAnnotate);
    test(nextLev, nextLev, ParserError::DeclareTest);
}



void Parser::bTerm(const SymSet &fSys) {
    bFactor(fSys);


    while (current_symbol == Symbol::LogicAnd || current_symbol == Symbol::And) {
        if (current_symbol == Symbol::And) {
            error(ParserError::ExpectedLogicAnd, false);
        }
        get_symbol();
        bTerm(fSys);
        generate_instruction(Fct::Opr, 0, Opr::LogicAnd);
    }
}

void Parser::bExpr(const SymSet &fSys) {
    bTerm(fSys);

    while (current_symbol == Symbol::LogicOr || current_symbol == Symbol::Or) {
        if (current_symbol == Symbol::Or) {
            error(ParserError::ExpectedLogicOr, false);
        }
        get_symbol();
        bTerm(fSys);
        generate_instruction(Fct::Opr, 0, Opr::LogicOr);
    }
}

void Parser::rel(const SymSet &fSys) {
    get_symbol();
    if (compareSys.count(current_symbol)) {
        Symbol symbol = current_symbol;
        get_symbol();
        aExpr(fSys);
        compareInt(symbol);
    } else {
        error(ParserError::ExpectedCompare, true);
    }
}


void Parser::bFactor(const SymSet &fSys) {
    SymSet nextLev = fSys;
    nextLev.insert(bFacBegSys.begin(), bFacBegSys.end());
    test(nextLev, nextLev, ParserError::BFactorTest);


    while (bFacBegSys.count(current_symbol)) {
        switch (current_symbol) {

            case Symbol::Ident: {
                usize loc = position(last_id);
                if (loc == ident_table.size()) {
                    get_symbol();
                    error(ParserError::UndeclaredIdent, true);
                } else {
                    if (current_level - ident_table[loc].level < 0) {
                        error(ParserError::OutOfBlock, true);
                    } else {
                        generate_instruction(Fct::Lod, current_level - ident_table[loc].level, ident_table[loc].adr);
                    }
                    if (ident_table[loc].kind == Obj::Int || ident_table[loc].kind == Obj::Constant) {
                        rel(fSys);
                    }
                }
                break;
            }
            case Symbol::TrueSym: {
                generate_instruction(Fct::Lit, 0, true);
                get_symbol();
                break;
            }
            case Symbol::FalseSym: {
                generate_instruction(Fct::Lit, 0, false);
                get_symbol();
                break;
            }
            case Symbol::Not: {
                get_symbol();
                bFactor(fSys);
                generate_instruction(Fct::Opr, 0, Opr::Not);
                break;
            }
            case Symbol::LParen: {
                get_symbol();
                bExpr(fSys);
                if (current_symbol == Symbol::RParen) {
                    get_symbol();
                } else {
                    error(ParserError::ExpectedRParen, true);
                }
                break;
            }
            case Symbol::Number: {
                rel(fSys);
                break;
            }
            default: {
                break;
            }
        }
    }
}

void Parser::compareInt(Symbol symbol) {
    switch (symbol) {
        case Symbol::Lss: {
            generate_instruction(Fct::Opr, 0, Opr::Lt);
            break;
        }
        case Symbol::Leq: {
            generate_instruction(Fct::Opr, 0, Opr::Le);
            break;
        }
        case Symbol::Gtr: {
            generate_instruction(Fct::Opr, 0, Opr::Gt);
            break;
        }
        case Symbol::Geq: {
            generate_instruction(Fct::Opr, 0, Opr::Ge);
            break;
        }
        case Symbol::Eql: {
            generate_instruction(Fct::Opr, 0, Opr::Eq);
            break;
        }
        case Symbol::Neq: {
            generate_instruction(Fct::Opr, 0, Opr::Neq);
            break;
        }
        default: {
            error(ParserError::ExpectedCompare, true);
        }
    }
}





void Parser::const_declare() {
    get_symbol();

    if (current_symbol == Symbol::IntSym || current_symbol == Symbol::BoolSym) {
        get_symbol();

        if (current_symbol == Symbol::Ident) {
            get_symbol();
            if (current_symbol == Symbol::Becomes || current_symbol == Symbol::Eql) {
                if (current_symbol == Symbol::Eql) {
                    error(ParserError::BecomesButEql, false);
                }
                get_symbol();
                enter(Obj::Constant);
                get_symbol();
                if (current_symbol == Symbol::Semicolon) {
                    get_symbol();
                } else {
                    error(ParserError::ExpectedSemicolon, false);
                }
            } else {
                error(ParserError::ExpectedInitial, false);
            }
        } else {
            error(ParserError::ExpectedIdent, true);
        }
    } else {
        error(ParserError::ExpectedIntOrBool, true);
    }

}

void Parser::var_declare() {
    Symbol symbol = current_symbol;
    get_symbol();

    if (current_symbol == Symbol::Ident) {
        if (symbol == Symbol::IntSym) {
            enter(Obj::Int);
        } else {
            enter(Obj::Bool);
        }
        get_symbol();

        while (current_symbol == Symbol::Comma) {
            get_symbol();
            if (current_symbol == Symbol::Ident) {
                if (symbol == Symbol::IntSym) {
                    enter(Obj::Int);
                } else {
                    enter(Obj::Bool);
                }
            } else {
                error(ParserError::ExpectedIdent, true);
            }
            get_symbol();
        }

        if (current_symbol == Symbol::Semicolon) {
            get_symbol();
        } else {
            error(ParserError::ExpectedSemicolon, true);
        }
    } else {
        error(ParserError::ExpectedIdent, true);
    }
}

[[maybe_unused]] void Parser::set_input(std::string s) {
    input = std::move(s);
}

void Parser::compile() {
    SymSet begin = declBegSys;
    begin.insert(statBegSys.begin(), statBegSys.end());
    get_symbol();
    block(begin);
    get_char();
    success = (error_count == 0);
    if (current_symbol == Symbol::Nul) {
        line_num = last_line_num;
    }
    if (!success) {
        std::string final_s;
        std::string s;
        for (int i = 1; i <= line_num; ++i) {
            if (!source_code.eof()) {
                std::getline(source_code, s);
                final_s.append(s + "\n");
            }
            while (!err_msgs.empty() && err_msgs.front().line_num == i) {
                final_s.append(err_msgs.front().description + "\n");
                err_msgs.erase(err_msgs.begin());
            }
        }
        source_code.str(std::move(final_s));
    }
}

Parser::Parser() {
    init();
}

std::string Obj2String(Obj kind) {
    switch (kind) {
        case Obj::Constant:
            return "const";
        case Obj::Int:
            return "int";
        case Obj::Bool:
            return "bool";
        case Obj::Block:
            break;
    }
    return "";
}

[[maybe_unused]] void Parser::list_table() {
    auto p = ident_table[0];
    for (int i = 0; i < table_index; i ++) {
        p = ident_table[i];
        switch (p.kind) {
            case Obj::Constant: {
                ident_table_s << i << " "
                      << Obj2String(p.kind) << " "
                      << p.name << " "
                      << "val=" << p.val << std::endl;
                break;
            }
            case Obj::Bool:
            case Obj::Int: {
                ident_table_s << i << " "
                      << Obj2String(p.kind) << " "
                      << p.name << " "
                      << "lev = " << p.level
                      << "adr = " << p.adr << std::endl;
                break;
            }
            case Obj::Block:
                break;
        }
    }
}













