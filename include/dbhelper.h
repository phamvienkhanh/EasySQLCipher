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
    
    QPair<QString, QString> extractColName(QString fullColName); // <table name, col name>

    DBCode execQuery(const QString& query, sqlite3* connection);
    bool createTable(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection);

    DBCode insert(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection);
    DBCode insert(const QString& tableName, const QVector<DboRegister*>& listDboRegister, sqlite3* connection);
    
    DBCode update(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection);
    DBCode update(const QString& tableName, const QVector<DboRegister*>& listDboRegister, sqlite3* connection);
    
    template<typename T>
    Result<QVector<T>, DBCode> fetchByQuery(const QString& tableName, const QString& query, const QString& colSelect, 
                                            sqlite3* connection);
    
    template<typename T>
    Result<QVector<T>, DBCode> fetchByQueryWithRelation(FetchWithRelationParams& param);
    
    template<typename T>
    QString buildSelectClause(bool prefixTableName = false);
    
    template<typename T>
    QString buildSelectClause(QString cols, bool prefixTableName = false);
}

#include "dbhelper_fetchbyquery.inl"
#include "dbhelper_fetchbyquery_withrelation.inl"
#include "dbhelper_clausebuilder.inl"

#endif // DBHELPER_H
