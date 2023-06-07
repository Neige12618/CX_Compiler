//
// Created by Saatus on 2023/5/21.
//

#ifndef QT_GUI_TEXTITEM_H
#define QT_GUI_TEXTITEM_H


enum TextItem {
    SourceCode,
    ErrCode,
    VirtualCode,
    Result,
    Count,
};
const std::array<QString, TextItem::Count> TextName = {
        "SourceCode",
        "ErrCode",
        "VirtualCode",
        "Result",
};

#endif //QT_GUI_TEXTITEM_H
