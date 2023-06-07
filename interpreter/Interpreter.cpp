#include "Interpreter.h"


// 找到对应 level 的base
usize find_base(usize level, usize base, std::array<i64, STACKSIZE> &stack) {
    while (level > 0) {
        base = stack[base];
        level -= 1;
    }
    return base;
}

void Interpreter::run(usize &address) {
    if (parser == nullptr) {
        return ;
    }

    i = &parser->code_words[address];
    address += 1;
    usize sto_base = 0;


    switch (i->function) {
        case Fct::Lit: {
            stack_index += 1;
            stack[stack_index] = i->address;
            break;
        }
        case Fct::Opr: {
            switch (i->address) {
                case Opr::Neg : {
                    stack[stack_index] = -stack[stack_index];
                    break;
                }
                case Opr::Add : {
                    stack_index -= 1;
                    stack[stack_index] += stack[stack_index + 1];
                    break;
                }
                case Opr::Sub : {
                    stack_index -= 1;
                    stack[stack_index] -= stack[stack_index + 1];
                    break;
                }
                case Opr::Mul : {
                    stack_index -= 1;
                    stack[stack_index] *= stack[stack_index + 1];
                    break;
                }
                case Opr::Div : {
                    stack_index -= 1;
                    stack[stack_index] /= stack[stack_index + 1];
                    break;
                }
                case Opr::Mod : {
                    stack_index -= 1;
                    stack[stack_index] %= stack[stack_index + 1];
                    break;
                }
                case Opr::Eq : {
                    stack_index -= 1;
                    stack[stack_index] = (stack[stack_index] == stack[stack_index + 1]);
                    break;
                }
                case Opr::Neq : {
                    stack_index -= 1;
                    stack[stack_index] = (stack[stack_index] != stack[stack_index + 1]);
                    break;
                }
                case Opr::Lt : {
                    stack_index -= 1;
                    stack[stack_index] = (stack[stack_index] < stack[stack_index + 1]);
                    break;
                }
                case Opr::Gt : {
                    stack_index -= 1;
                    stack[stack_index] = (stack[stack_index] > stack[stack_index + 1]);
                    break;
                }
                case Opr::Ge : {
                    stack_index -= 1;
                    stack[stack_index] = (stack[stack_index] >= stack[stack_index + 1]);
                    break;
                }
                case Opr::Le : {
                    stack_index -= 1;
                    stack[stack_index] = (stack[stack_index] <= stack[stack_index + 1]);
                    break;
                }
                case Opr::LogicOr:
                case Opr::Or : {
                    stack_index -= 1;
                    stack[stack_index] |= stack[stack_index + 1];
                    break;
                }
                case Opr::LogicAnd:
                case Opr::And : {
                    stack_index -= 1;
                    stack[stack_index] &= stack[stack_index + 1];
                    break;
                }
                case Opr::Xor: {
                    stack_index -= 1;
                    stack[stack_index] ^= stack[stack_index + 1];
                    break;
                }
                case Opr::Not : {
                    stack[stack_index] = !stack[stack_index];
                    break;
                }
                case Opr::Write : {
                    parser->program_result << stack[stack_index] << std::endl;
                    stack_index -= 1;
                    break;
                }
                case Opr::Br : {
                    parser->program_result << std::endl;
                    break;
                }
                case Opr::Read : {
                    input >> stack[stack_index];
                    break;
                }
            }
            break;

        }
        case Fct::Lod: {
            stack_index += 1;
            stack[stack_index] = stack[find_base(i->level, base, stack) + i->address];
            break;
        }
        case Fct::Sto: {
            sto_base = find_base(i->level, base, stack);
            stack[sto_base + i->address] = stack[stack_index];
            stack_index -= 1;
            break;
        }
        case Fct::Lev: {
            current_level -= 1;
            stack_index = base - 1;
            base = stack[base];
            break;
        }
        case Fct::Ret: {
            address = 0;
            break;
        }
        case Fct::Int: {
            current_level += 1;
            stack_index += 1;
            stack[stack_index] = (i64) base;
            base = stack_index;
            stack_index += i->address;
            break;
        }
        case Fct::Jmp: {
            address = i->address;
            break;
        }
        case Fct::Jpc: {
            if (stack[stack_index] == 0) {
                address = i->address;
            }
            stack_index -= 1;
            break;
        }
    }


}
