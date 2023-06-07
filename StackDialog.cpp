//
// Created by Saatus on 2023/5/27.
//

#include "StackDialog.h"


StackDialog::StackDialog(QWidget *parent) {
    setParent(parent);
    setWindowTitle("stack top and variable");
    setColumnCount(4);
    setHorizontalHeaderLabels(QStringList() << "type" << "level" << "adr" << "val");
    resize(320, 400);
}

void StackDialog::init(std::vector<Entry> &table, Interpreter &interpreter1) {
    this->interpreter = &interpreter1;
    sz = table.size();
    for (auto &i: table) {
        if (i.kind == Obj::Block) {
            sz -= 1;
        }
    }
    setRowCount(sz + 3);
    entry = &table;
    updateView();
}

QString Obj2String(Obj obj) {
    switch (obj) {
        case Obj::Constant:
            return "const";
        case Obj::Int:
            return "int";
        case Obj::Bool:
            return "bool";
        case Obj::Block:
            return "block";
    }
    return "";
}


void StackDialog::updateView() {
    int line = 0;
    for (auto &i: *entry) {
        if (i.kind == Obj::Block) {
            continue;
        }
        if (i.kind != Obj::Constant) {
            if (interpreter->i != nullptr) {
                if (interpreter->current_level >= i.level) {
                    i.val = interpreter->stack[find_base(interpreter->current_level - i.level, interpreter->base, interpreter->stack) + i.adr];
                }
            }
        }
        auto kind = Obj2String(i.kind);
        auto type = kind + "  " + QString::fromStdString(i.name);
        setItem(line, 0, new QTableWidgetItem(type));
        setItem(line, 1, new QTableWidgetItem(QString::number(i.level)));
        setItem(line, 2, new QTableWidgetItem(QString::number(i.adr)));
        setItem(line, 3, new QTableWidgetItem(QString::number(i.val)));
        line += 1;
    }
    // 设置栈顶
    setItem(sz, 0, new QTableWidgetItem("stack top"));
    setItem(sz, 1, new QTableWidgetItem);
    setItem(sz, 2, new QTableWidgetItem);
    setItem(sz, 3, new QTableWidgetItem(QString::number(interpreter->stack[interpreter->stack_index])));

    // 设置base
    setItem(sz + 1, 0, new QTableWidgetItem("base"));
    setItem(sz + 1, 1, new QTableWidgetItem);
    setItem(sz + 1, 2, new QTableWidgetItem);
    setItem(sz + 1, 3, new QTableWidgetItem(QString::number(interpreter->base)));

    setItem(sz + 2, 0, new QTableWidgetItem("current level"));
    setItem(sz + 2, 1, new QTableWidgetItem);
    setItem(sz + 2, 2, new QTableWidgetItem);
    setItem(sz + 2, 3, new QTableWidgetItem(QString::number(interpreter->current_level)));
}
