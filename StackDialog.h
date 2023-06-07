//
// Created by Saatus on 2023/5/27.
//

#ifndef QT_GUI_STACKDIALOG_H
#define QT_GUI_STACKDIALOG_H
#include <QTableWidget>
#include "Parser.h"
#include "Interpreter.h"

class StackDialog: public QTableWidget {
    Q_OBJECT
public:
    explicit StackDialog(QWidget *parent = nullptr);

    void init(std::vector<Entry> &table, Interpreter &interpreter);
    void updateView();

private:
    std::vector<Entry> *entry = nullptr ;
    Interpreter *interpreter = nullptr;
    int sz = 0;
};


#endif //QT_GUI_STACKDIALOG_H
