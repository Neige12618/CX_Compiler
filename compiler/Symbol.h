//
// Created by Saatus on 2023/5/21.
//

#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H


enum class Symbol{
    Nul,
    Ident,
    Number,
    Plus,
    Minus,
    Times,
    Slash,
    Mod,
    And,
    LogicAnd,
    Or,
    LogicOr,
    Xor,
    LAnnotate,
    RAnnotate,
    Not,
    Eql,
    Neq,
    Lss,
    Leq,
    Gtr,
    Geq,
    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Semicolon,
    Period,
    Becomes,
    TrueSym,
    FalseSym,
    IfSym,
    ElseSym,
    WhileSym,
    WriteSym,
    ReadSym,
    ConstSym,
    IntSym,
    BoolSym,
};


#endif //COMPILER_SYMBOL_H
