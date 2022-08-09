#ifndef DBO_INC_TYPES_H
#define DBO_INC_TYPES_H

#ifndef ENABLE_PRECOMPILED_HEADER
#include "precompiled.h"
#endif

#include <sqlcipher/sqlite3.h>

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

    Empty,
    Busy,
    Unknown,
    
    PrepareFailed,
    BindValueFailed,
    CreateIdFailed,
    
    RelationNotFound,
    FieldIdNotFound,
};

template<typename T, typename R>
struct Result {
    T value;
    R retCode;
};

struct FetchWithRelationParams {
    QString tableName;
    QString query;
    QString colSelect;
    QHash<QString, QString> withTables;
    sqlite3* connection = nullptr;
};

#endif // DBO_INC_TYPES_H
