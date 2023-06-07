//
// Created by Saatus on 2023/5/21.
//

#include "MainWindow.h"
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QCloseEvent>
#include <QTextBlock>


MainWindow::MainWindow():
        mainLayout(), mainWidget(), buttonBox(), tabWidget(), tabBox()
{
    setWindowIcon(QIcon(":/icon/icon.svg"));
    initFont();
    initTabWidget();
    initSideBar();
    initLayout();
    initFormat();
}

// slot begin

void MainWindow::openFile() {
    tabBox[TextItem::SourceCode]->ReadFile();
    tabBox[TextItem::SourceCode]->isSaved = true;
    emit tabBox[TextItem::SourceCode]->hasSaved();
}

void MainWindow::notSaved() {
    auto f = (MyTextEdit*) sender();
    tabWidget.setTabText(f->item, TextName[f->item] + "*");
    f->isSaved = false;
    has_compiled = false;
}

void MainWindow::saved() {
    auto f = (MyTextEdit*) sender();
    tabWidget.setTabText(f->item, TextName[f->item]);
}

void MainWindow::saveAll() {
    for (int i = TextItem::SourceCode; i < TextItem::Count; i++) {
        tabBox[i]->SaveFile(&TextName[i]);
    }
}

void MainWindow::saveFile(){
    usize index = tabWidget.currentIndex();
    tabBox[index]->SaveFile(&TextName[index]);
}

void MainWindow::compileFile() {
    backToDefault();

    std::string input = tabBox[TextItem::SourceCode]->toPlainText().toStdString();

    parser.reset(new Parser(input));
    if (input.empty()) {
        return ;
    }
    parser->compile();


    if (parser->success) {
        // 获得虚拟机的代码
        parser->list_code(0);
        auto s = parser->virtual_code.rdbuf();
        QString code = QString::fromStdString(s->str());
        tabBox[TextItem::VirtualCode]->setText(code);
        // 设置error
        tabBox[TextItem::ErrCode]->setText("no error");


        // 编译成功
        QMessageBox::information(this, "compile success", "compile success");

        // 切换窗口到虚拟机代码
        tabWidget.setCurrentIndex(TextItem::VirtualCode);

        has_compiled = true;

    } else {
        // 获得报错信息以及源代码
        auto s = parser->source_code.rdbuf();
        QString code = QString::fromStdString(s->str());
        const QString text =
                QString::fromStdString(
                    std::to_string(parser->error_count) +
                    " errors occurred"
                );

        // 弹出报错窗口, 并且切换窗口到报错
        QMessageBox::warning(this, "error!", text);
        tabBox[TextItem::ErrCode]->setText(code);
        tabWidget.setCurrentIndex(TextItem::ErrCode);
        setErrorColor();
    }

}


void MainWindow::run() {
    in_debug = false;
    stack_info->hide();
    if (!has_compiled) {
        compileFile();
    }

    // 只有编译成功才会运行
    if (parser->success) {
        usize address = 0;

        interpreter.reset(new Interpreter(parser));
        auto input = read_buf->text().toStdString();
        interpreter->input << input;
        do {
            interpreter->run(address);
        } while (address != 0);


        getResult();
    }

}


void MainWindow::next() {
    if (!in_debug) {
        QMessageBox::information(this, "next", "please debug first");
        return ;
    }
    last_debug_address = debug_address;
    interpreter->run(debug_address);
    if (tabBox[TextItem::VirtualCode]->breakpoints.contains((int)debug_address)) {
        tabBox[TextItem::VirtualCode]->breakpoints.remove((int)debug_address);
    }
    setDebugColor();
    stack_info->updateView();
    if (debug_address == 0) {
        backToDefault();
        getResult();
    }

}


void MainWindow::debug() {
    if (!has_compiled) {
        compileFile();
    }

    if (parser->success) {
        debug_address = 0;
        resize(1200, 600);

        interpreter.reset(new Interpreter(parser));
        tabWidget.setCurrentIndex(TextItem::VirtualCode);
        setDebugColor();
        in_debug = true;

        stack_info->init(parser->ident_table, *interpreter);
        stack_info->show();
    }
}


void MainWindow::until() {
    if (!in_debug) {
        QMessageBox::information(this, "next", "please debug first");
        return ;
    }
    last_debug_address = debug_address;

    while (!tabBox[TextItem::VirtualCode]->breakpoints.contains((int)debug_address)) {
        interpreter->run(debug_address);
        if (debug_address == 0) {
            break;
        }
    }

    tabBox[TextItem::VirtualCode]->breakpoints.remove((int)debug_address);
    setDebugColor();
    stack_info->updateView();
    if (debug_address == 0) {
        backToDefault();
        getResult();
    }
}

// slot end


void MainWindow::initLayout() {
    resize(800, 600);
    stack_info = std::make_shared<StackDialog>(new StackDialog());
    stack_info->setVisible(false);
    mainLayout.addWidget(&sideBar);
    mainLayout.addWidget(stack_info.get());

    mainLayout.addWidget(&tabWidget);
    mainWidget.setLayout(&mainLayout);
    setCentralWidget(&mainWidget);
}

void MainWindow::initFont() {
    QFont f("FiraCode NF", 13);
    setFont(f);
}


/// 初始化界面上所有的按钮
void MainWindow::initSideBar() {
    // 设置 打开文件 按钮
    open_file = std::make_shared<QPushButton>("open file");
    connect(open_file.get(), SIGNAL(clicked()), this, SLOT(openFile()));
    buttonBox.addWidget(open_file.get());

    // 设置 编译 按钮
    compile = std::make_shared<QPushButton>("compile");
    connect(compile.get(), SIGNAL(clicked()), this, SLOT(compileFile()));
    buttonBox.addWidget(compile.get());

    // 设置 保存 按钮
    save = std::make_shared<QPushButton>("save");
    connect(save.get(), SIGNAL(clicked()), this, SLOT(saveFile()));
    buttonBox.addWidget(save.get());

    // 设置 保存所有 按钮
    save_all = std::make_shared<QPushButton>("save all");
    connect(save_all.get(), SIGNAL(clicked()), this, SLOT(saveAll()));
    buttonBox.addWidget(save_all.get());

    // 设置 运行编译后 按钮
    run_btn = std::make_shared<QPushButton>("run");
    connect(run_btn.get(), SIGNAL(clicked()), this, SLOT(run()));
    buttonBox.addWidget(run_btn.get());

    // 设置 调试 按钮
    debug_btn = std::make_shared<QPushButton>("debug");
    connect(debug_btn.get(), SIGNAL(clicked()), this, SLOT(debug()));
    buttonBox.addWidget(debug_btn.get());

    // 设置 单步调试 按钮
    next_btn = std::make_shared<QPushButton>("next");
    connect(next_btn.get(), SIGNAL(clicked()), this, SLOT(next()));
    buttonBox.addWidget(next_btn.get());

    // 设置 运行到断点 按钮
    continue_btn = std::make_shared<QPushButton>("continue");
    connect(continue_btn.get(), SIGNAL(clicked()), this, SLOT(until()));
    buttonBox.addWidget(continue_btn.get());

    read_buf = std::make_shared<QLineEdit>();
    read_buf->setPlaceholderText("input");
    read_buf->setValidator(new
    QRegularExpressionValidator(QRegularExpression("([0-9]+\\s)*$")));
    buttonBox.addWidget(read_buf.get());

    sideBar.setLayout(&buttonBox);
    sideBar.setMaximumWidth(this->width() / 4);
}


void MainWindow::initTabWidget() {
    for (int i = TextItem::SourceCode; i < TextItem::Count; i++) {
        tabBox[i] = new MyTextEdit((TextItem)i);
        if (i != TextItem::SourceCode) {
            tabBox[i]->setReadOnly(true);
        }
        tabWidget.addTab(tabBox[i], TextName[i]);
        connect(tabBox[i], SIGNAL(textChanged()), this, SLOT(notSaved()));
        connect(tabBox[i], SIGNAL(hasSaved()), this, SLOT(saved()));
    }
}


void MainWindow::setErrorColor() {
    QRegularExpression rx("^\\d");
    QTextCursor cursor1 = tabBox[TextItem::ErrCode]->textCursor();
    while (!cursor1.atEnd()) {
        cursor1.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
        cursor1.select(QTextCursor::LineUnderCursor);
        if (cursor1.selectedText().lastIndexOf(rx) == -1) {
            cursor1.mergeCharFormat(err_info_fmt);
        }
    }
}


void MainWindow::backToDefault() {
    resize(800, 600);

    tabBox[TextItem::VirtualCode]->setCurrentCharFormat(default_fmt);
    in_debug = false;
    stack_info->hide();
}


void MainWindow::setDebugColor() {
    // 获取document
    QTextDocument *doc = tabBox[TextItem::VirtualCode]->document();

    // 获取行号用于设定颜色
    QTextBlock block = doc->findBlockByNumber((int)last_debug_address);
    QTextCursor cursor(block);
    cursor.select(QTextCursor::LineUnderCursor);

    cursor.setCharFormat(default_fmt);
    cursor.clearSelection();
    tabBox[TextItem::VirtualCode]->setTextCursor(cursor);

    block = doc->findBlockByNumber((int)debug_address);
    QTextCursor cursor1(block);

    cursor1.select(QTextCursor::LineUnderCursor);

    cursor1.setCharFormat(debug_fmt);
    cursor1.clearSelection();
    tabBox[TextItem::VirtualCode]->setTextCursor(cursor1);


}


void MainWindow::initFormat() {
    err_info_fmt.setForeground(Qt::red);
    debug_fmt.setBackground(Qt::red);
    default_fmt.setBackground(Qt::white);
}



void MainWindow::getResult() {
    tabWidget.setCurrentIndex(TextItem::Result);
    // 获得程序运行的结果, 如果有的话
    auto s = parser->program_result.rdbuf();
    QString result;
    if (s->str().empty()) {
        result = QString::fromStdString("run success but no output");
    } else {
        result = QString::fromStdString("run success,\n the result is: \n" + s->str());
    }
    tabBox[TextItem::Result]->setText(result);
}


MainWindow::~MainWindow() {
    for (auto i: tabBox) {
        delete i;
    }
}


void MainWindow::closeEvent(QCloseEvent *event) {
    bool exit = true;
    for (int i = TextItem::SourceCode; i < TextItem::Count; i++) {
        if (!tabBox[i]->isSaved) {
            exit = false;
        }
    }
    if (exit) {
        QWidget::closeEvent(event);
    } else {
        switch (
            QMessageBox::information(this,
                                     "退出确定",
                                     "你还有文件未保存, 真的要退出吗?",
                                    QMessageBox::Yes,
                                    QMessageBox::No)){
            case QMessageBox::Yes:
                event->accept();
                break;
            case QMessageBox::No:
            default:
                event->ignore();
                break;
        }


    }
}










