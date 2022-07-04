#ifndef DBHELPERTEMPLATE_H
#define DBHELPERTEMPLATE_H

#include <sqlcipher/sqlite3.h>

#include "types.h"
#include "columndata.h"
//#include "dboregister.h"

Q_GLOBAL_STATIC_WITH_ARGS(QString, templateSelect, ("SELECT %1 FROM %2 %3;"))

class DboRegister;

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
                DboRegister& regist = data.getRegister();
                for(int i = 0; i < nCol; i++) {
                    QString colName = QString(sqlite3_column_name(stmt, i));
                    ColumnData value(i, stmt);
                    regist.setValue(colName, value);                    
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
    
    template<typename T>
    Result<QVector<T>, DBCode> fetchByQueryWithRelation(const QString& tableName, const QString& query, 
                                                        const QString& colSelect, const QString& tableJoin,
                                                        const QString& relation, sqlite3* connection) {
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
        QStringList colRelation = relation.split("=");
        QString joinStmt = QString("JOIN %1 ON %2.%4 = %5.%6 ")
                           .arg(tableJoin, tableName, colRelation[0], tableJoin, colRelation[1]);
        
        QString strQuery = templateSelect->arg(selects, tableName, joinStmt + query);
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
                DboRegister& regist = data.getRegister();
                for(int i = 0; i < nCol; i++) {
                    QString colName = QString(sqlite3_column_name(stmt, i));
                    ColumnData value(i, stmt);
                    regist.setValue(colName, value);  
                    qDebug() << colName;
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
        
        return Result<QVector<T>, DBCode>{};
    }

    template<typename T>
    class Query {
    public:
        Result<QVector<T>, DBCode> select(QString colSelect) {
            Result<QVector<T>, DBCode> rs;
            T obj;
            QString tableName = obj.getTableName();
            if(m_fnConProvider) {
                if(m_tableJoin.isEmpty()) {
                    return DBHelper::fetchByQuery<T>(tableName, m_query, colSelect, m_fnConProvider());                    
                }
                else {
                    return DBHelper::fetchByQueryWithRelation<T>(tableName, m_query, colSelect, m_tableJoin,
                                                                 m_relation , m_fnConProvider()); 
                }
            }
            
            rs.retCode = DBCode::Failed;
            return rs;
        }
        
        template<typename R>
        Query<T>& with() {
            R objJoin;
            T objQuery;
            objQuery.registerMember();
            m_tableJoin = objJoin.getTableName();
            m_relation = objQuery.getRegister().getRelation(m_tableJoin);
            return *this;
        } 
        
        Query(QString query, std::function<sqlite3* (void)> fnCon) {
            m_query = query;
            m_fnConProvider = fnCon;
            m_tableJoin = "";
            m_relation = "";
        }

    private:
        QString m_query;
        QString m_tableJoin;
        QString m_relation;
        std::function<sqlite3* (void)> m_fnConProvider;
    };
    
}

#endif // DBHELPERTEMPLATE_H
