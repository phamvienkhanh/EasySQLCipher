#include "dbhelper.h"

#include "dboregister.h"

namespace DBHelper
{
    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, qint32 value)
    {
        return sqlite3_bind_int(stmt, idx, value) == SQLITE_OK;
    }

    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, const QString& value)
    {
        auto valueData = value.toUtf8();
        return sqlite3_bind_text(stmt, idx, valueData, valueData.size(), SQLITE_TRANSIENT) == SQLITE_OK;
    }

    bool stmtBindValue(sqlite3_stmt* stmt, qint32 idx, const QByteArray& value)
    {
        return sqlite3_bind_blob(stmt, idx, value, value.size(), SQLITE_TRANSIENT) == SQLITE_OK;
    }

    QPair<QString, QString> extractColName(const QString& fullColName)
    {
        auto idxSep = fullColName.indexOf('_');
        if(idxSep <= 0) {
            return {};
        }
        
        QPair<QString, QString> rs{};
        rs.first = fullColName.first(idxSep);
        rs.second = fullColName.right(fullColName.size() - idxSep - 1);
        
        return rs;
    }        
    
    ProcessQueryStmtResult processQueryStmt(sqlite3_stmt* stmt, const QString& mainTableName) {
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
    
    DBCode execQuery(const QString& query, sqlite3* connection)
    {
        char* mesgError = nullptr;
        int rs = sqlite3_exec(connection, query.toUtf8(), nullptr, nullptr, &mesgError);
        if(rs != SQLITE_OK && mesgError != nullptr) {
            qDebug() << mesgError;
            sqlite3_free(mesgError);
        }
        return rs == SQLITE_OK
                ? DBCode::OK
                : DBCode::Failed;
    }

    bool createTable(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection)
    {
        if(!connection)
            return false;

        const auto& listMemmber = dboRegister.getListMember();
        if(listMemmber.size() <= 1)
            return false;

        QString listCols = QString("%1 %2 PRIMARY KEY AUTOINCREMENT, ")
                            .arg(listMemmber[0]->getColName(), listMemmber[0]->getSqlType());

        for(auto i = 1; i < listMemmber.size(); i++) {
            auto& colInfo = listMemmber[i]->getColumnInfo();
            listCols += QString("%1 %2, ").arg(colInfo.colName, colInfo.sqlType);
        }
        listCols.chop(2);

        QString query = templateCreateTable->arg(tableName, listCols);
        return DBHelper::execQuery(query, connection) == DBCode::OK;
    }

    DBCode insert(const QString& tableName, const DboRegister& dboRegister, sqlite3* connection)
    {
        if(!connection)
            return DBCode::Failed;

        const auto& listMemmber = dboRegister.getListMember();
        if(listMemmber.size() <= 1)
            return DBCode::Failed;

        QString fieldID = listMemmber[0]->getColName();
        QString listCols = "";
        QString listBindCols = "";
        for(auto i = 1; i < listMemmber.size(); i++) {
            listCols += QString("%1, ").arg(listMemmber[i]->getColName());
            listBindCols += QString(":%1, ").arg(listMemmber[i]->getColName());
        }
        listCols.chop(2);
        listBindCols.chop(2);
        listBindCols = QString("(%1)").arg(listBindCols);

        QByteArray query = templateInsert->arg(tableName, listCols, listBindCols, fieldID).toUtf8();

        sqlite3_stmt* stmt = nullptr;
        int rs = sqlite3_prepare_v2(connection, query, query.size(), &stmt, nullptr);
        if(rs != SQLITE_OK) {
            return DBCode::PrepareFailed;
        }

        for(auto i = 1; i < listMemmber.size(); i++) {
            if(!listMemmber[i]->bindValue(stmt)) {
                sqlite3_finalize(stmt);
                return DBCode::BindValueFailed;
            }
        }

        rs = sqlite3_step(stmt);
        if(rs == SQLITE_ROW || rs == SQLITE_DONE) {
            int numCols = sqlite3_column_count(stmt);
            if(numCols == 0) {
                sqlite3_finalize(stmt);
                return DBCode::CreateIdFailed;
            }

            ColumnData id(0, stmt);
            listMemmber[0]->setValue(id);

            sqlite3_finalize(stmt);
            return DBCode::OK;
        }

        sqlite3_finalize(stmt);
        return DBCode::Failed;
    }

    DBCode insert(const QString& tableName, const QVector<DboRegister*>& listDboRegister, sqlite3* connection)
    {
        if(!connection)
            return DBCode::Failed;

        if(listDboRegister.empty())
            return DBCode::OK;

        const auto& listMemmber = listDboRegister[0]->getListMember();
        if(listMemmber.size() <= 1)
            return DBCode::Failed;

        QString fieldID = listMemmber[0]->getColName();
        QString listCols = "";
        QString listBindCols = "";
        for(auto i = 1; i < listMemmber.size(); i++) {
            listCols += QString("%1, ").arg(listMemmber[i]->getColName());
            listBindCols += "?, ";
        }
        listCols.chop(2);
        listBindCols.chop(2);
        listBindCols = QString("(%1), ").arg(listBindCols);

        QString listBindValue = "";
        for(auto i = 0; i < listDboRegister.size(); i++) {
            listBindValue += listBindCols;
        }
        listBindValue.chop(2);

        QByteArray query = templateInsert->arg(tableName, listCols, listBindValue, fieldID).toUtf8();

        sqlite3_stmt* stmt = nullptr;
        int rs = sqlite3_prepare_v2(connection, query, query.size(), &stmt, nullptr);
        if(rs != SQLITE_OK) {
            return DBCode::PrepareFailed;
        }

        auto numMemmbers = listMemmber.size() - 1; //skip id field
        for(auto iObj = 0; iObj < listDboRegister.size(); iObj++) {
            const auto& members = listDboRegister[iObj]->getListMember();
            for(auto iMemmber = 1; iMemmber < members.size(); iMemmber++) {
                if(!members[iMemmber]->bindValue(stmt, iObj*numMemmbers + iMemmber)) {
                    sqlite3_finalize(stmt);
                    return DBCode::BindValueFailed;
                }
            }
        }

        int cntResultId = -1;
        while (true) {
            rs = sqlite3_step(stmt);
            if(rs == SQLITE_ROW) {
                int numCols = sqlite3_column_count(stmt);
                if(numCols == 0) {
                    sqlite3_finalize(stmt);
                    return DBCode::CreateIdFailed;
                }

                if(++cntResultId < listDboRegister.size()) {
                    ColumnData id(0, stmt);
                    listDboRegister[cntResultId]->getListMember()[0]->setValue(id);
                }
            }
            else if(rs == SQLITE_DONE) {
                sqlite3_finalize(stmt);
                if(cntResultId == listDboRegister.size() - 1)
                    return DBCode::OK;
                else
                    return DBCode::Failed;
            }
            else {
                sqlite3_finalize(stmt);
                return DBCode::Failed;
            }
        }
    }

    DBCode remove(const QString& tableName, const QString& query, sqlite3* connection)
    {
        QString deleteQuery = templateDelete->arg(tableName, query);
        return DBHelper::execQuery(deleteQuery, connection); 
    }
    
}
