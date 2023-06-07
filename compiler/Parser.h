//
// Created by Saatus on 2023/5/21.
//

#ifndef QT_GUI_PARSER_H
#define QT_GUI_PARSER_H
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <memory>
#include "Error.h"
#include "Symbol.h"
#include "Virtual.h"

using SymSet = std::unordered_set<Symbol>;
using String2Sym = std::unordered_map<std::string, Symbol>;

template <class T>
using P = std::shared_ptr<T>;

using Char2Sym = std::unordered_map<char, Symbol>;


const usize MAX_IDENTIFIER_LEN = 10;     // length of identifiers
const usize LEVMAX = 10;  // maximum depth of block nesting
const usize CXMAX = 200; // size of code array
const usize STACKSIZE = 500; // interpreter stack size


class Parser {
public:
    // 用于存放虚拟机的代码
    std::vector<Instruction> code_words{  };
    // 用于存放符号表
    std::vector<Entry> ident_table{  };
    // 当前的字符
    char current_char = ' ';
    // 当前的符号
    Symbol current_symbol = Symbol::Nul;
    // 当前的block层数
    i64 current_level = 0;
    // 上一个字符的字符串字面量
    std::string last_id{};
    // 上一个数的值
    i64  last_num = 0;
    // 输入的源代码
    std::string input{};
    // 虚拟机代码的代码行数
    usize  code_index = 0;
    // 用于存放当前block中的变量个数
    std::vector<usize>  data_indices{};
    // 符号表的符号个数
    usize  table_index = 0;
    // 编译过程中错误个数
    usize  error_count = 0;
    // 所有的错误字符串字面量
    std::vector<Error>  err_msgs{};
    // 当前行的字符个数
    usize  char_count = 0;
    // 当前行号
    usize line_num = 0;
    // 上一个当前行的字符个数，用于报错
    usize last_char_count = 0;
    // 上一个行号，用于报错
    usize last_line_num = 1;
    // 是否是行开头
    bool line_start = true;
    // declare的first集合，以下同
    SymSet declBegSys{};
    SymSet statBegSys{};
    SymSet aFacBegSys{};
    SymSet bFacBegSys{};
    SymSet blockBegSys{};
    SymSet compareSys{};
    // 字符串转符号对应集合
    String2Sym WSym{};
    // 单个字符转符号集合
    Char2Sym SSym{};
    // 虚拟机代码
    std::stringstream virtual_code{};
    // 源代码
    std::stringstream source_code{};
    // 程序运行结果
    std::stringstream program_result{};
    // 符号表（未使用）
    std::stringstream ident_table_s{};
    // 是否编译成功
    bool success = false;

    // block decl statement expr term factor rel
public:
    void error(ParserError err, bool before);
    void get_char();
    void get_symbol();
    Symbol get_symbol_from_string(const std::string& s);


    void generate_instruction(Fct f, usize l, usize a);
    void list_code(usize code_start);

    [[maybe_unused]] void list_table();

    void enter(Obj k);
    usize position(const std::string& id);
    // factor
    void aFactor(const SymSet& fSys);
    void bFactor(const SymSet& fSys);
    void rel(const SymSet& fSys);
    // term
    void aTerm(const SymSet& fSys);
    void bTerm(const SymSet& fSys);
    // expression
    void aExpr(const SymSet& fSys);
    void bExpr(const SymSet& fSys);

    void compareInt(Symbol symbol);

    void statement(SymSet &fSys);
    void block(SymSet &fSys);
    void declare();
    void const_declare();
    void var_declare();

    void test(const SymSet& s1, const SymSet& s2, ParserError e);
    void init();


public:
    explicit Parser(std::string input);
    explicit Parser(std::string&& input);
    Parser() ;

    [[maybe_unused]] void set_input(std::string s);
    void compile();
};



#endif //QT_GUI_PARSER_H
