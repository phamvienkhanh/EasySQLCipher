#ifndef DBO_INC_TYPES_H
#define DBO_INC_TYPES_H

#ifndef ENABLE_PRECOMPILED_HEADER
#include "precompiled.h"
#endif

#include <sqlcipher/sqlite3.h>

struct DBInitParam {
    QString dbPath;
    int     openMode;
    qint32  maxThreadCount = 1;
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

class EZException : public QException
{
public:
    EZException(DBCode code): _code(code){}
    void raise() const override { throw *this; }
    EZException *clone() const override { return new EZException(*this); }
    const DBCode& code() const {return _code;}

private:
    DBCode _code = DBCode::Unknown;
};

#endif // DBO_INC_TYPES_H
