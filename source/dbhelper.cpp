#include "dbhelper.h"

#include "dboregister.h"

Q_GLOBAL_STATIC_WITH_ARGS(QString, templateCreateTable, ("CREATE TABLE IF NOT EXISTS %1(%2);"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, templateInsertTable, ("INSERT INTO %1(%2) VALUES(%3) RETURNING %4;"))

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

    DBCode execQuery(const QString& query, sqlite3* connection)
    {
        qDebug() << query;
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

        const auto& listMemmber = dboRegister.getListMemmber();
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

    DBCode insert(const QString &tableName, const DboRegister &dboRegister, sqlite3 *connection)
    {
        if(!connection)
            return DBCode::Failed;

        const auto& listMemmber = dboRegister.getListMemmber();
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

        QByteArray query = templateInsertTable->arg(tableName, listCols, listBindCols, fieldID).toUtf8();

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


}
