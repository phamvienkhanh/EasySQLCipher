#ifndef DBOREGISTER_H
#define DBOREGISTER_H

#include "dbomember.h"

class DboRegister
{
public:

    DboRegister(){};

    DboRegister(const DboRegister&) {}
    DboRegister& operator=(const DboRegister& regist) {
        return *this;
    }

    virtual ~DboRegister() {
        for(auto& iMember : m_listMember) {
            delete iMember;
        }

        m_hashMember.clear();
        m_listMember.clear();
        m_hashRelationMember.clear();
    }

    template<typename T>
    void bind(QString name, QString sqlType, T* memmber) {
        if(!m_hashMember.contains(name)) {
            auto dboMember = new DboMember(memmber, name, sqlType);
            m_hashMember.insert(name, dboMember);
            m_listMember.push_back(dboMember);
        }
    }
    
    template<typename T>
    void relation(QString relation, T* memmber) {
        QString tableName = memmber->getTableName();
        if(!m_hashRelationMember.contains(tableName)) {
            auto dboRelationMember = new DboRelationMember(memmber, relation);
            m_hashRelationMember.insert(tableName, dboRelationMember);
        }
    }
    
    template<typename T>
    void relation(QString relation, QVector<T>* memmber) {
        T obj;
        QString tableName = obj.getTableName();
        if(!m_hashRelationMember.contains(tableName)) {
            auto dboRelationMember = new DboRelationMember(memmber, relation);
            m_hashRelationMember.insert(tableName, dboRelationMember);
        }
    }

    void setValue(QString name, ColumnData value) {
        if(m_hashMember.contains(name))
            m_hashMember[name]->setValue(value);
    }

    bool bindValue(QString name, sqlite3_stmt* stmt) {
        if(m_hashMember.contains(name))
            return m_hashMember[name]->bindValue(stmt);
        return false;
    }

    bool bindValue(QString name, sqlite3_stmt* stmt, qint32 idx) {
        if(m_hashMember.contains(name))
            return m_hashMember[name]->bindValue(stmt, idx);
        return false;
    }

    const QVector<AbstractDboMember*>& getListMember() const {
        return m_listMember;
    }
    
    QString getRelation(QString tableName) const {
        if(m_hashRelationMember.contains(tableName)) {
            return m_hashRelationMember[tableName]->getRelation();
        }
        
        return "";
    }

private:
    QHash<QString, AbstractDboMember*> m_hashMember;
    QVector<AbstractDboMember*>        m_listMember;
    
    QHash<QString, AbstractDboRelationMember*> m_hashRelationMember;
};

#endif // DBOREGISTER_H
