#ifndef DBO_INC_TYPES_H
#define DBO_INC_TYPES_H

struct DBInitParam {
    QString dbPath;
    int     openMode;
};

struct ColumnInfo {
    QString colName;
    QString sqlType;
};

enum class DBCode {
    OK,
    Failed,

    PrepareFailed,
    BindValueFailed,
    CreateIdFailed,

};

template<typename T, typename R>
struct Result {
    T value;
    R retCode;
};

#endif // DBO_INC_TYPES_H
