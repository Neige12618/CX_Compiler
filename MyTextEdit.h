//
// Created by Saatus on 2023/5/21.
//

#ifndef QT_GUI_MYTEXTEDIT_H
#define QT_GUI_MYTEXTEDIT_H
#include <QTextEdit>
#include <QFile>
#include "TextItem.h"

class MyTextEdit: public QTextEdit {
    Q_OBJECT
public:
    explicit MyTextEdit(TextItem it) ;
    ~MyTextEdit() override;
    QFile *f = nullptr;
    bool isSaved = true;
    QSet<int> breakpoints;
    TextItem item = TextItem::SourceCode;
    QString ReadFile();
    void SaveFile(const QString *s);
signals:
    void hasSaved();

public:
    void toggleBreakpoint(int lineNumber);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QTextCharFormat breakpointFormat;
};


#endif //QT_GUI_MYTEXTEDIT_H
