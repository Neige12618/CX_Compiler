//
// Created by Saatus on 2023/5/26.
//

#ifndef QT_GUI_INTERPRETER_H
#define QT_GUI_INTERPRETER_H
#include "Parser.h"

class Interpreter {
public:
    // 指向编译器，用于获得编译器编译后的内容
    std::shared_ptr<Parser> parser;
    // 当前的栈顶所在位置。
    usize stack_index = -1;
    // 当前基地址
    usize base = 1;
    // 当前的block嵌套层数
    usize current_level = 0;
    // 用于模拟栈
    std::array<i64, STACKSIZE> stack{};
    // 输入的虚拟代码
    std::stringstream input;
    // 用于指向当前所执行的虚拟代码
    Instruction *i = nullptr;
    // 构造函数， 从一个编译器构造
    explicit Interpreter(std::shared_ptr<Parser> &p): parser(p) {}

    void run(usize &address);
};
usize find_base(usize level, usize base, std::array<i64, STACKSIZE> &stack);

#endif //QT_GUI_INTERPRETER_H
