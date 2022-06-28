#ifndef DBHELPER_H
#define DBHELPER_H

#include <sqlcipher/sqlite3.h>

#include "types.h"

class DboRegister;

namespace DBHelper
{
    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, qint32 value);
    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, const QString& value);
    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, const QByteArray& value);

    DBCode execQuery(const QString& query, sqlite3* connection);
    bool createTable(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection);

    DBCode insert(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection);
    DBCode insert(const QString& tableName, const QVector<DboRegister*>& listDboRegister, sqlite3* connection);
    
    DBCode update(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection);
    DBCode update(const QString& tableName, const QVector<DboRegister*>& listDboRegister, sqlite3* connection);
}

#endif // DBHELPER_H
