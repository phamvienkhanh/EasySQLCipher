#ifndef DBHELPER_FETCHBYQUERY_WITHRELATION_INL
#define DBHELPER_FETCHBYQUERY_WITHRELATION_INL

#include <sqlcipher/sqlite3.h>

#include "dbhelper.h"
#include "dbhelper_clausebuilder.inl"
#include "types.h"
#include "columndata.h"
#include "sqltemplate.h"


namespace DBHelper
{

struct ProcessQueryStmtResult {
    qint32 mainId = 0;
    QMultiMap<QString, ColumnData> rowData; // <table name, columndata>
};

auto processQueryStmt(sqlite3_stmt* stmt, QString mainTableName) {
    ProcessQueryStmtResult rs;
    int nCol = sqlite3_column_count(stmt);
    QString mainTableColId = QString("%1_id").arg(mainTableName);
    for(int i = 0; i < nCol; i++) {
        QString fullColName = QString(sqlite3_column_name(stmt, i));
        auto colInfo = DBHelper::extractColName(fullColName);
        ColumnData value(i, colInfo.second, stmt);
        if(fullColName == mainTableColId) {
            rs.mainId = value;
        }
        rs.rowData.insert(colInfo.first, value);
    }

    return rs;
}

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

            auto it = mapData.find(stmtData.mainId);
            if(it != mapData.end()) {
                T& obj = it.value();
                obj.registerMember();
                auto listColsData = stmtData.rowData.values(param.tableName);
                auto* abstractDboRelation = obj.getRegister().getRelation(it.key());
                if(abstractDboRelation == nullptr) {
                    result.retCode = DBCode::RelationNotFound;
                    return result;
                }
            }

            T data;
            data.registerMember();

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
            result.value = mapData;
            return result;
        }
    }
    
    return result;
    
    return Result<QVector<T>, DBCode>{};
}
}

#endif // DBHELPER_FETCHBYQUERY_WITHRELATION_INL
