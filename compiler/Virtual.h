//
// Created by Saatus on 2023/5/21.
//

#ifndef COMPILER_VIRTUAL_H
#define COMPILER_VIRTUAL_H

#include <string>

using i64 = int64_t;
using usize = size_t;

enum class Obj {
    Constant,
    Int,
    Bool,
    Block,
};

enum class Fct {
    /// `lit 0,a`: 将一个常量a放到栈顶
    Lit,
    /// `opr 0,a`: 执行opr a
    Opr,
    /// `lod l,a`: 加载一个位于l，a的变量
    Lod,
    /// `sto l,a`: 将当前栈顶存入l，a所对应的地址
    Sto,
    /// `lev 0,0`: 离开当前block
    Lev,
    /// `ret 0,0`: 结束程序
    Ret,
    /// `int l,a`: 初始化变量空间
    Int,
    /// `jmp 0,a`: 跳转到第a条指令
    Jmp,
    /// `jpc 0,a`: 如果栈顶为0则跳转到第a条指令
    Jpc,
};

enum Opr {
    Neg = 1,
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Eq,
    Neq,
    Lt,
    Ge,
    Gt,
    Le,
    Or,
    And,
    Xor,
    Not,
    LogicAnd,
    LogicOr,
    Write,
    Br,
    Read,
};

struct Instruction {
    /// 指令类型
    Fct function;
    /// 当前所处的level
    i64 level;
    /// opr是操作码，其他为地址偏移
    i64 address;

    Instruction(Fct f, usize l, usize a):
    function(f), level(l), address(a) {}
};


struct Entry {
    std::string name{};
    Obj kind = Obj::Constant;
    i64 val  = 0;
    i64 level = 0;
    usize adr = 0;
    Entry(std::string name, Obj kind, i64 val, usize level, usize adr):
    name(std::move(name)), kind(kind), val(val), level(level), adr(adr)
    {}
    Entry() = default;
};


inline std::string Fct2string(Fct f) {
    switch (f) {
        case Fct::Lit:
            return "lit";
        case Fct::Opr:
            return "opr";
        case Fct::Lod:
            return "lod";
        case Fct::Sto:
            return "sto";
        case Fct::Lev:
            return "lev";
        case Fct::Ret:
            return "ret";
        case Fct::Int:
            return "int";
        case Fct::Jmp:
            return "jmp";
        case Fct::Jpc:
            return "jpc";
    }
    return "lit";
}

#endif //COMPILER_VIRTUAL_H
