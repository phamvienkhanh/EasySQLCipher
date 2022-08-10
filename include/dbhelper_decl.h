#ifndef DBHELPER_DECL_H
#define DBHELPER_DECL_H

#include <sqlcipher/sqlite3.h>

#include "types.h"
#include "columndata.h"

class DboRegister;

namespace DBHelper
{

    struct ProcessQueryStmtResult {
        qint32 mainId = 0;
        QMultiMap<QString, ColumnData> rowData; // <table name, columndata>
    };

    struct ComplexQueryParams {
        QString query;
        std::function<bool(sqlite3_stmt*)> cbFuncBind;
        std::function<bool(sqlite3_stmt*)> cbFuncStep;
        std::function<void(DBCode)> cbFuncError;
        std::function<void()> cbFuncFinished;
        sqlite3* connection = nullptr;
    };

    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, qint32 value);
    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, const QString& value);
    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, const QByteArray& value);
    
    QPair<QString, QString> extractColName(const QString& fullColName); // <table name, col name>
    ProcessQueryStmtResult processQueryStmt(sqlite3_stmt* stmt, const QString& mainTableName);
    
    DBCode execQuery(const QString& query, sqlite3* connection);
    void execQuery(const ComplexQueryParams& params);
    bool createTable(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection);

    DBCode insert(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection);
    DBCode insert(const QString& tableName, const QVector<DboRegister*>& listDboRegister, sqlite3* connection);
    
    DBCode update(const QString& tableName, const DboRegister& dboRegister, const QStringList& updateCols,
                  sqlite3* connection);
    DBCode update(const QString& tableName, const QVector<DboRegister*>& listDboRegister, const QStringList& updateCols,
                  sqlite3* connection);
    
    DBCode remove(const QString& tableName, const QString& query, sqlite3* connection);

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

#endif // DBHELPER_DECL_H
