#ifndef DBOMEMMBER_H
#define DBOMEMMBER_H

#include "columndata.h"
#include "dbhelper.h"
#include "types.h"

class AbstractDboMemmber
{
public:
    AbstractDboMemmber(){}
    virtual ~AbstractDboMemmber(){}

public:
    virtual void setValue(ColumnData) = 0;
    virtual bool bindValue(sqlite3_stmt* stmt, qint32 idx) = 0;
    virtual bool bindValue(sqlite3_stmt* stmt) = 0;

    const QString& getColName() const {
        return m_columnInfo.colName;
    }

    const QString& getSqlType() const {
        return m_columnInfo.sqlType;
    }

    const ColumnInfo& getColumnInfo() const {
        return m_columnInfo;
    }

protected:
    ColumnInfo m_columnInfo;
};

template<typename T>
class DboMemmber : public AbstractDboMemmber
{
public:
    DboMemmber(T* value, const QString& colName, const QString& sqlType) {
        m_value = value;
        m_columnInfo.sqlType = sqlType;
        m_columnInfo.colName = colName;
    }

    virtual void setValue(ColumnData value) override {
        T forward = value;
        *m_value  = forward;
    }

    virtual bool bindValue(sqlite3_stmt* stmt, qint32 idx) override {
        return DBHelper::stmtBindValue(stmt, idx, *m_value);
    }

    virtual bool bindValue(sqlite3_stmt* stmt) override {
        QString bindName = ":" + m_columnInfo.colName;
        int idx = sqlite3_bind_parameter_index(stmt, bindName.toUtf8());
        return DBHelper::stmtBindValue(stmt, idx, *m_value);
    }

private:
    T* m_value;
};

#endif // DBOMEMMBER_H
