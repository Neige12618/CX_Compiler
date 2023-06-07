//
// Created by Saatus on 2023/5/21.
//

#include "MyTextEdit.h"
#include <QFileDialog>
#include <QTextBlockFormat>
#include <QTextFrame>
#include <QPainter>
#include <QAbstractTextDocumentLayout>

MyTextEdit::MyTextEdit(TextItem it){
    item = it;
    setTabStopDistance(40);
    breakpointFormat.setBackground(Qt::yellow);
}


MyTextEdit::~MyTextEdit() {
    if (f != nullptr) {
        if (f->isOpen()) {
            f->close();
        }
        delete f;
    }
}

QString MyTextEdit::ReadFile() {
    QFileDialog dialog(this);
    QList<QString> fileName;
    QString code;

    // 设为打开文件的dialog
    dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    // 只能读取后缀名为cx的文件
    dialog.setNameFilter("sourcecode(*.cx *.txt)");


    if (dialog.exec()) {
        fileName = dialog.selectedFiles();
    }
    if (!fileName.length()) {
        return "";
    }


    f = new QFile(fileName[0]);
    f->open(QFile::ReadOnly | QFile::Text);
    code = QString::fromStdString(f->readAll().toStdString());
    this->setText(code);
    f->close();

    isSaved = true;
    return code;
}

void MyTextEdit::SaveFile(const QString *s) {
    if (f != nullptr) {
        f->open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
        f->write(this->toPlainText().toLocal8Bit());
        f->close();
    } else {
        auto path = QFileDialog::getSaveFileName(this,
                                                 tr("Save File"),
                                                 *s,
                                                 tr("CX (*.cx) txt (*.txt)"));

        if (path.isEmpty()) {
            return ;
        }
        QFile file(path);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        file.write(this->toPlainText().toLocal8Bit());
        file.close();
    }
    isSaved = true;
    emit hasSaved();
}


void MyTextEdit::mouseDoubleClickEvent(QMouseEvent *event) {
    // 调用基类的实现，以便处理其他默认操作
    QTextEdit::mouseDoubleClickEvent(event);

    // 获取双击的行号
    int lineNumber = cursorForPosition(event->pos()).blockNumber();

    // 切换断点
    toggleBreakpoint(lineNumber);

}



void MyTextEdit::toggleBreakpoint(int lineNumber) {
    QTextBlock block = document()->findBlockByNumber(lineNumber);
    QTextCursor cursor(block);

    if (breakpoints.contains(lineNumber)) {
        breakpoints.remove(lineNumber);


        cursor.select(QTextCursor::SelectionType::LineUnderCursor);
        cursor.setCharFormat(QTextCharFormat());
        cursor.clearSelection();
    } else {
        breakpoints.insert(lineNumber);


        cursor.select(QTextCursor::LineUnderCursor);
        cursor.setCharFormat(breakpointFormat);
        cursor.clearSelection();
    }

}

