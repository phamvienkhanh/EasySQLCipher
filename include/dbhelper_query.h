#ifndef DBHELPER_QUERY_H
#define DBHELPER_QUERY_H

#include <sqlcipher/sqlite3.h>

#include "types.h"
#include "dbhelper.h"

namespace DBHelper
{

template<typename T>
class Query {
public:
    Result<QVector<T>, DBCode> select(const QString& colSelect) {
        Result<QVector<T>, DBCode> rs;
        T obj;
        QString tableName = obj.getTableName();
        if(m_fnConProvider) {
            if(m_withTables.isEmpty()) {
                return DBHelper::fetchByQuery<T>(tableName, m_query, colSelect, m_fnConProvider());
            }
            else {
                FetchWithRelationParams params;
                params.tableName = tableName;
                params.query = m_query;
                params.colSelect = colSelect;
                params.withTables = m_withTables;
                params.connection = m_fnConProvider();
                
                return DBHelper::fetchByQueryWithRelation<T>(params); 
            }
        }
        
        rs.retCode = DBCode::Failed;
        return rs;
    }
    
    template<typename R>
    Query<T>& with(const QString& selectCols) {
        R objJoin;
        if(!m_withTables.contains(objJoin.getTableName())) {
            m_withTables.insert(objJoin.getTableName(), selectCols);
        }
        
        return *this;
    } 
    
    Query(const QString& query, std::function<sqlite3* (void)> fnCon) {
        m_query = query;
        m_fnConProvider = fnCon;
    }
    
private:
    QString m_query;
    QHash<QString, QString> m_withTables;
    std::function<sqlite3* (void)> m_fnConProvider;
};
}

#endif // DBHELPER_QUERY_H
