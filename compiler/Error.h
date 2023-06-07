//
// Created by Saatus on 2023/5/21.
//

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H


#include "Virtual.h"

enum class ParserError {
    NumOverflow,
    AFactorTest,
    UndeclaredIdent,
    OutOfBlock,
    ExpectedRParen,
    BecomesButEql,
    ExpectedSemicolon,
    ExpectedBecomes,
    ExpectedLParen,
    ExpectedIdent,
    StatementTest,
    LevelOverflow,
    ExpectedRBrace,
    DeclareTest,
    ExpectedCompare,
    ExpectedIntOrBool,
    BFactorTest,
    ExpectedLBrace,
    ExpectedInitial,
    ConstAssign,
    ExpectedLogicOr,
    ExpectedLogicAnd,
};

struct Error {
    std::string description{};
    usize line_num = 0;

    Error() = default;
};


#endif //COMPILER_ERROR_H
