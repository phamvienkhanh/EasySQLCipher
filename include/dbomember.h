#ifndef DBOMEMMBER_H
#define DBOMEMMBER_H

#include "types.h"
#include "columndata.h"
#include "dbhelper.h"

class AbstractDboMember
{
public:
    AbstractDboMember(){}
    virtual ~AbstractDboMember(){}

public:
    virtual void setValue(ColumnData value) = 0;
    virtual bool bindValue(sqlite3_stmt* stmt, qint32 idx) const = 0;
    virtual bool bindValue(sqlite3_stmt* stmt) const = 0;

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
class DboMember : public AbstractDboMember
{
public:
    DboMember(T* value, const QString& colName, const QString& sqlType) {
        m_value = value;
        m_columnInfo.sqlType = sqlType;
        m_columnInfo.colName = colName;
    }

    void setValue(ColumnData value) override {
        T forward = value;
        *m_value  = forward;
    }

    bool bindValue(sqlite3_stmt* stmt, qint32 idx) const override {
        return DBHelper::stmtBindValue(stmt, idx, *m_value);
    }

    bool bindValue(sqlite3_stmt* stmt) const override {
        QString bindName = ":" + m_columnInfo.colName;
        int idx = sqlite3_bind_parameter_index(stmt, bindName.toUtf8());
        return DBHelper::stmtBindValue(stmt, idx, *m_value);
    }

private:
    T* m_value;
};

#endif // DBOMEMMBER_H
