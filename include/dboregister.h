#ifndef DBOREGISTER_H
#define DBOREGISTER_H

#include "dbomember.h"
#include "dborelationmember.h"

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
        for(auto& iRelationMember : m_hashRelationMember) {
            delete iRelationMember;
        }

        m_hashMember.clear();
        m_listMember.clear();
        m_hashRelationMember.clear();
    }

    template<typename T>
    void bind(const QString& name, const QString& sqlType, T* memmber) {
        if(!m_hashMember.contains(name)) {
            auto dboMember = new DboMember(memmber, name, sqlType);
            m_hashMember.insert(name, dboMember);
            m_listMember.push_back(dboMember);
        }
    }
    
    template<typename T>
    void relation(const QString& relationOn, T* memmber, const QString& tableLeft) {
        QString tableName = memmber->getTableName();
        if(!m_hashRelationMember.contains(tableName)) {
            auto dboRelationMember = new DboRelationMember(memmber, relationOn, tableLeft);
            m_hashRelationMember.insert(tableName, dboRelationMember);
        }
    }
    
    template<typename T>
    void relation(const QString& relationOn, QVector<T>* memmber, const QString& tableLeft) {
        T obj;
        QString tableName = obj.getTableName();
        if(!m_hashRelationMember.contains(tableName)) {
            auto dboRelationMember = new DboRelationMember(memmber, relationOn, tableLeft);
            m_hashRelationMember.insert(tableName, dboRelationMember);
        }
    }

    void setValue(const QString& name, ColumnData value) {
        if(m_hashMember.contains(name))
            m_hashMember[name]->setValue(value);
    }
    
    void setValue(const QList<ColumnData>& values) {
        for(auto& iValue : values) {
            if(m_hashMember.contains(iValue.getColName()))
                m_hashMember[iValue.getColName()]->setValue(iValue);            
        }
    }

    bool bindValue(const QString& name, sqlite3_stmt* stmt) const {
        if(m_hashMember.contains(name))
            return m_hashMember[name]->bindValue(stmt);
        return false;
    }

    bool bindValue(const QString& name, sqlite3_stmt* stmt, qint32 idx) const {
        if(m_hashMember.contains(name))
            return m_hashMember[name]->bindValue(stmt, idx);
        return false;
    }

    const QVector<AbstractDboMember*>& getListMember() const {
        return m_listMember;
    }
    
    AbstractDboRelationMember* getRelation(const QString& tableName) const {
        if(m_hashRelationMember.contains(tableName)) {
            return m_hashRelationMember[tableName];
        }
        
        return nullptr;
    }

private:
    QHash<QString, AbstractDboMember*> m_hashMember;
    QVector<AbstractDboMember*>        m_listMember;
    
    QHash<QString, AbstractDboRelationMember*> m_hashRelationMember;
};

#endif // DBOREGISTER_H
