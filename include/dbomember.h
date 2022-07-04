#ifndef DBOMEMMBER_H
#define DBOMEMMBER_H

#include "columndata.h"
#include "dbhelper.h"
#include "types.h"

class AbstractDboMember
{
public:
    AbstractDboMember(){}
    virtual ~AbstractDboMember(){}

public:
    virtual void setValue(ColumnData value) = 0;
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

class AbstractDboRelationMember
{
public:
    AbstractDboRelationMember(){}
    virtual ~AbstractDboRelationMember(){}
    
    virtual QString getRelation() = 0; 
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

    bool bindValue(sqlite3_stmt* stmt, qint32 idx) override {
        return DBHelper::stmtBindValue(stmt, idx, *m_value);
    }

    bool bindValue(sqlite3_stmt* stmt) override {
        QString bindName = ":" + m_columnInfo.colName;
        int idx = sqlite3_bind_parameter_index(stmt, bindName.toUtf8());
        return DBHelper::stmtBindValue(stmt, idx, *m_value);
    }

private:
    T* m_value;
};

template<typename T>
class DboRelationMember : public AbstractDboRelationMember
{
public:
    DboRelationMember(T* value, const QString& relation) {        
        m_value = value;
        m_listValues = nullptr;
        m_relation = relation;
    }
    DboRelationMember(QVector<T>* listVals, const QString& relation) {
        m_listValues = listVals;
        m_value = nullptr;
        m_relation = relation;
    }
    
    QString getRelation() override {
        return m_relation;
    }

    void setValue(ColumnData value) {};
    bool bindValue(sqlite3_stmt* stmt, qint32 idx) {return false;}
    bool bindValue(sqlite3_stmt* stmt) {return false;}

private:
    T* m_value;
    QVector<T>* m_listValues;
    QString m_relation;
};

#endif // DBOMEMMBER_H
