#ifndef DBSET_H
#define DBSET_H

#include "types.h"
#include "abstractdbo.h"
#include "dbhelper_query.h"
#include "easysqlcipher.h"

template<typename T>
class DbSet
{
public:
    DbSet() {
        static_assert(std::is_base_of_v<AbstractDBO, T>, "Error: dbo type must base on AbstractDBO");
    }

public:
    bool createTable() {
        T obj;
        obj.registerMember();
        QString tableName = obj.getTableName();
        DboRegister& regist = obj.getRegister();

        return DBHelper::createTable(tableName, regist, m_dbContext->getConnection());
    }

    DBCode insert(T& obj) {
        obj.registerMember();
        QString tableName = obj.getTableName();
        DboRegister& regist = obj.getRegister();

        return DBHelper::insert(tableName, regist, m_dbContext->getConnection());
    }

    DBCode insert(QVector<T>& listObj) {
        if(listObj.empty())
            return DBCode::OK;

        QString tableName = listObj[0].getTableName();
        QVector<DboRegister*> listRegists;
        for(auto& iObj : listObj) {
            iObj.registerMember();
            listRegists.push_back(&iObj.getRegister());
        }

        return DBHelper::insert(tableName, listRegists, m_dbContext->getConnection());
    }

    auto asyncInsert(T& obj) {
        return QtConcurrent::run([obj, this] () mutable {
            DBCode rs = insert(obj);
            if(rs != DBCode::OK)
                throw EZException(rs);

            return obj;
        });
    }

    auto asyncInsert(QVector<T>& listObj) {
        return QtConcurrent::run([listObj, this] () mutable {
            DBCode rs = insert(listObj);
            if(rs != DBCode::OK)
                throw rs;

            return listObj;
        });
    }

    DBCode update(T& obj, const QStringList& updateCols = {}) {
        if(obj.getId() == 0)
            return DBCode::Failed;

        obj.registerMember();
        QString tableName = obj.getTableName();
        DboRegister& regist = obj.getRegister();

        return DBHelper::update(tableName, regist, updateCols, m_dbContext->getConnection());
    }
    
    DBCode update(QVector<T>& listObj, const QStringList& updateCols = {}) {
        if(listObj.empty())
            return DBCode::OK;

        QString tableName = listObj[0].getTableName();
        QVector<DboRegister*> listRegists;
        for(auto& iObj : listObj) {
            if(iObj.getId() == 0)
                return DBCode::Failed;

            iObj.registerMember();
            listRegists.push_back(&iObj.getRegister());
        }

        return DBHelper::update(tableName, listRegists, updateCols, m_dbContext->getConnection());
    }

    DBCode update(const QString& query) {
        T obj;
        return DBHelper::update(obj.getTableName(), query, m_dbContext->getConnection());
    }

    auto asyncUpdate(T& obj, const QStringList& updateCols = {}) {
        return QtConcurrent::run([obj, updateCols, this] () mutable {
            DBCode rs = update(obj, updateCols);
            if(rs != DBCode::OK)
                throw EZException(rs);

            return obj;
        });
    }

    auto asyncUpdate(QVector<T>& listObj, const QStringList& updateCols = {}) {
        return QtConcurrent::run([listObj, updateCols, this] () mutable {
            DBCode rs = update(listObj, updateCols);
            if(rs != DBCode::OK)
                throw EZException(rs);

            return listObj;
        });
    }

    auto asyncUpdate(const QString& query) {
        return QtConcurrent::run([query, this] () {
            DBCode rs = update(query);
            if(rs != DBCode::OK)
                throw EZException(rs);
        });
    }

    DBCode save(T& obj) {
        qint32 id = obj.getId();
        if(id == 0) {
            return insert(obj);
        }
        
        return update(obj);
    }

    auto asyncSave(T& obj) {
        qint32 id = obj.getId();
        if(id == 0) {
            return asyncInsert(obj);
        }

        return asyncUpdate(obj);
    }
    
    DBCode remove(const QString& query) {
        T obj;
        return DBHelper::remove(obj.getTableName(), query, m_dbContext->getConnection());
    }

    DBCode remove(const T& obj) {
        QString whereClause = QString("WHERE id = %1").arg(obj.getId());
        return DBHelper::remove(obj.getTableName(), whereClause, m_dbContext->getConnection());
    }
    
    DBCode remove(const QVector<T>& objs) {
        QString strListIds = "";
        for(auto& iObj : objs) {
            strListIds += QString("%1, ").arg(iObj.getId());
        }
        strListIds.chop(2);
        
        QString whereClause = QString("WHERE id IN (%1)").arg(strListIds);
        T obj;
        return DBHelper::remove(obj.getTableName(), whereClause, m_dbContext->getConnection());
    }

    auto asyncRemove(const QString& query) {
        return QtConcurrent::run([query, this] () {
            DBCode rs = remove(query);
            if(rs != DBCode::OK)
                throw EZException(rs);
        });
    }

    auto asyncRemove(const T& obj) {
        return QtConcurrent::run([obj, this] () {
            DBCode rs = remove(obj);
            if(rs != DBCode::OK)
                throw EZException(rs);
        });
    }

    auto asyncRemove(const QVector<T>& objs) {
        return QtConcurrent::run([objs, this] () {
            DBCode rs = remove(objs);
            if(rs != DBCode::OK)
                throw EZException(rs);
        });
    }

    DBHelper::Query<T> query(const QString& query) {
        return DBHelper::Query<T>(query, m_dbContext->getConnection());
    }

    void setDbContext(EasySQLCipher* dbContext) {
        m_dbContext = dbContext;
    }
    
private:
    EasySQLCipher* m_dbContext = nullptr;
};

#endif // DBSET_H
