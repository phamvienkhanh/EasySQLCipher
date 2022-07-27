#ifndef COLUMNDATA_H
#define COLUMNDATA_H

#include <sqlcipher/sqlite3.h>

class ColumnData
{
public:
    ColumnData(const qint32& idx, sqlite3_stmt* stmt);
    ColumnData(const qint32& idx, const QString& colName, sqlite3_stmt* stmt);

    operator qint32() const;
    operator QString() const;
    operator QByteArray() const;
    
    QString getColName() const {
        return m_colName;
    }

private:
    qint32 m_colIdx;
    QString m_colName;
    sqlite3_stmt* m_stmt;
};

#endif // COLUMNDATA_H
