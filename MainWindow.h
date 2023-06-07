//
// Created by Saatus on 2023/5/21.
//

#ifndef QT_GUI_MAINWINDOW_H
#define QT_GUI_MAINWINDOW_H
#include "Parser.h"
#include "Interpreter.h"

#include <QMainWindow>
#include <QLayout>
#include "MyTextEdit.h"
#include <QTabWidget>
#include <QPushButton>
#include "StackDialog.h"


class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;
public:
    QHBoxLayout mainLayout;
    QWidget mainWidget;
    QVBoxLayout buttonBox;
    QWidget sideBar;
    QTabWidget tabWidget;
    std::array<MyTextEdit*, TextItem::Count> tabBox;
    std::shared_ptr<QPushButton> open_file;
    std::shared_ptr<QPushButton> compile;
    std::shared_ptr<QPushButton> save;
    std::shared_ptr<QPushButton> save_all;
    std::shared_ptr<QPushButton> run_btn;
    std::shared_ptr<QPushButton> debug_btn;
    std::shared_ptr<QPushButton> next_btn;
    std::shared_ptr<QPushButton> continue_btn;
    std::shared_ptr<QLineEdit> read_buf;
    std::shared_ptr<Parser> parser;
    std::shared_ptr<Interpreter> interpreter;
    std::shared_ptr<StackDialog> stack_info;
    QTextCharFormat err_info_fmt;
    QTextCharFormat debug_fmt;
    QTextCharFormat default_fmt;
    bool in_debug = false;
    bool has_compiled = false;
    usize debug_address = 0;
    usize last_debug_address = 0;


private slots:
    void openFile();
    void notSaved();
    void saveAll();
    void saveFile();
    void compileFile();
    void run();
    void debug();
    void saved();
    void next();
    void until();


private:
    void initTabWidget();

    void initSideBar();

    void initFont();

    void initLayout();

    void initFormat();

    void setErrorColor();

    void setDebugColor();

    void backToDefault();

    void closeEvent(QCloseEvent*event) override;

    void getResult();
};


#endif //QT_GUI_MAINWINDOW_H
