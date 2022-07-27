#ifndef DBHELPER_FETCHBYQUERY_WITHRELATION_INL
#define DBHELPER_FETCHBYQUERY_WITHRELATION_INL

#include <sqlcipher/sqlite3.h>

#include "dbhelper_decl.h"
#include "types.h"
#include "columndata.h"
#include "sqltemplate.h"


namespace DBHelper
{

template<typename T>
Result<QVector<T>, DBCode> fetchByQueryWithRelation(FetchWithRelationParams& param) {
    Result<QVector<T>, DBCode> result;
    result.retCode = DBCode::OK;
    
    if(!param.connection) {
        result.retCode = DBCode::Failed;
        return result;
    }
    
//    QString selects = "*";
//    if(!param.colSelect.isEmpty()) {
//        selects = param.colSelect;
//    }
    
    T obj;
    obj.registerMember();
    QString joinClause = "";
    
    qint32 idxTable = 1;
    param.query = param.query.replace("#1.", QString("%1.").arg(param.tableName));
    QString selectClause = DBHelper::buildSelectClause<T>(param.colSelect, true) + ", ";
    QHashIterator<QString, QString> it(param.withTables);
    
    while (it.hasNext()) {
        it.next();
        idxTable++;
        
        auto* abstractDboRelation = obj.getRegister().getRelation(it.key());
        if(abstractDboRelation == nullptr) {
            result.retCode = DBCode::RelationNotFound;
            return result;
        }
        
        param.query = param.query.replace(QString("#%1.").arg(idxTable), QString("%1.").arg(it.key()));
        joinClause += abstractDboRelation->buildJoinClause();
        selectClause += abstractDboRelation->buildSelectClause(it.value(), true);
        selectClause += ", ";
    }
    selectClause.chop(2);
    
    QString strQuery = templateSelect->arg(selectClause, param.tableName, joinClause + param.query);
    QByteArray utf8Query = strQuery.toUtf8();
    
    qDebug() << utf8Query;
    
    sqlite3_stmt* stmt = nullptr;
    if(sqlite3_prepare_v2(param.connection, utf8Query, utf8Query.size(), &stmt, 0) != SQLITE_OK) {
        result.retCode = DBCode::PrepareFailed;
        return result;
    }
    
    QHash<qint32, T> mapData;
    int rsStep = 0;    
    while (1) {
        rsStep = sqlite3_step(stmt);
        switch (rsStep) {
        case SQLITE_ROW: {
            auto stmtData = DBHelper::processQueryStmt(stmt, param.tableName);
            if(stmtData.mainId == 0) {
                sqlite3_finalize(stmt);
                result.retCode = DBCode::FieldIdNotFound;
                return result;
            }
            
            auto procData = [&](T& obj, bool isOnlyRelationData) -> bool {
                auto tables = stmtData.rowData.uniqueKeys();
                for(auto& iTable : tables) {
                    auto colsData = stmtData.rowData.values(iTable);
                    
                    if(iTable == param.tableName) {
                        if(isOnlyRelationData) {
                            continue;
                        }
                        else {
                            obj.getRegister().setValue(colsData);
                            continue;
                        }
                    }
                    
                    auto* abstractDboRelation = obj.getRegister().getRelation(iTable);
                    if(abstractDboRelation == nullptr) {                        
                        return false;
                    }
                    
                    abstractDboRelation->setValue(colsData);
                }
                
                return true;
            };

            auto it = mapData.find(stmtData.mainId);
            if(it != mapData.end()) {
                T& obj = it.value();
                obj.registerMember();
                
                if(!procData(obj, true)) {
                    sqlite3_finalize(stmt);
                    result.retCode = DBCode::RelationNotFound;
                    return result;
                }                
            }
            else {
                T obj;
                obj.registerMember();
                if(!procData(obj, false)) {
                    sqlite3_finalize(stmt);
                    result.retCode = DBCode::RelationNotFound;
                    return result;
                }
                mapData.insert(stmtData.mainId, obj);
            }

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
            
            if(mapData.empty())
                result.retCode = DBCode::Empty;
            else
                result.retCode = DBCode::OK;
            result.value = mapData.values().toVector();
            return result;
        }
    }
    
    return result;
}
}

#endif // DBHELPER_FETCHBYQUERY_WITHRELATION_INL
