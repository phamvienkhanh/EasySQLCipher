#ifndef DBSET_H
#define DBSET_H

#include "types.h"
#include "abstractdbo.h"
#include "dbhelpertemplate.h"

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

        if(m_fnConProvider)
            return DBHelper::createTable(tableName, regist, m_fnConProvider());

        return false;
    }

    DBCode insert(T& obj) {
        obj.registerMember();
        QString tableName = obj.getTableName();
        DboRegister& regist = obj.getRegister();

        if(m_fnConProvider)
            return DBHelper::insert(tableName, regist, m_fnConProvider());
        return DBCode::Failed;
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

        if(m_fnConProvider)
            return DBHelper::insert(tableName, listRegists, m_fnConProvider());
        return DBCode::Failed;
    }

    DBCode update(T& obj) {
        return DBCode::Failed;
    }

    DBCode save(T& obj) {
        return DBCode::Failed;
    }

    DBCode remove(T& obj) {
        return DBCode::Failed;
    }

    DBHelper::Query<T> query(QString query) {
        return DBHelper::Query<T>{query, m_fnConProvider};
    }

    DBCode fetchById(T& obj) {
        return DBCode::Failed;
    }

    
    void setFnConProvider(std::function<sqlite3* (void)> fn) {
        m_fnConProvider = fn;
    }
    
private:
    std::function<sqlite3* (void)> m_fnConProvider;
};

#endif // DBSET_H
