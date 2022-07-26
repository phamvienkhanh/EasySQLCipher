#ifndef DBHELPER_FETCHBYQUERY_INL
#define DBHELPER_FETCHBYQUERY_INL

#include <sqlcipher/sqlite3.h>

#include "types.h"
#include "columndata.h"
#include "sqltemplate.h"

namespace DBHelper
{

template<typename T>
Result<QVector<T>, DBCode> fetchByQuery(const QString& tableName, const QString& query, 
                                        const QString& colSelect, sqlite3* connection) {
    Result<QVector<T>, DBCode> result;
    result.retCode = DBCode::OK;
    
    if(!connection) {
        result.retCode = DBCode::Failed;
        return result;
    }
    
    QString selects = "*";
    if(!colSelect.isEmpty()) {
        selects = colSelect;
    }
    
    QString strQuery = templateSelect->arg(selects, tableName, query);
    QByteArray utf8Query = strQuery.toUtf8();
    sqlite3_stmt* stmt = nullptr;
    if(sqlite3_prepare_v2(connection, utf8Query, utf8Query.size(), &stmt, 0) != SQLITE_OK) {
        result.retCode = DBCode::PrepareFailed;
        return result;
    }
    
    QVector<T> listData;
    int rsStep = 0;
    while (1) {
        rsStep = sqlite3_step(stmt);
        switch (rsStep) {
        case SQLITE_ROW: {
            int nCol = sqlite3_column_count(stmt);
            T data;
            data.registerMember();
            for(int i = 0; i < nCol; i++) {
                QString colName = QString(sqlite3_column_name(stmt, i));
                ColumnData value(i, stmt);
                data.getRegister().setValue(colName, value);                    
            }
            listData.push_back(data);
            
            break;
        }
        case SQLITE_BUSY:
            sqlite3_finalize(stmt);
            result.retCode = DBCode::Busy;
            return result;
            
        case SQLITE_MISUSE:
        case SQLITE_CONSTRAINT:
        case SQLITE_ERROR:
            sqlite3_finalize(stmt);
            result.retCode = DBCode::Unknown;
            return result;
            
        case SQLITE_DONE:
        default:
            sqlite3_finalize(stmt);
            
            if(listData.empty())
                result.retCode = DBCode::Empty;
            else
                result.retCode = DBCode::OK;
            result.value = listData;
            return result;
        }
    }
    
    return result;
}
}

#endif // DBHELPER_FETCHBYQUERY_INL
