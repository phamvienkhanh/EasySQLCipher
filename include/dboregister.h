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
    void setValue(QString name, ColumnData value) {
        if(m_hashMember.contains(name))
            m_hashMember[name]->setValue(value);
    }

    template<typename T>
    bool bindValue(QString name, sqlite3_stmt* stmt) {
        if(m_hashMember.contains(name))
            return m_hashMember[name]->bindValue(stmt);
        return false;
    }

    template<typename T>
    bool bindValue(QString name, sqlite3_stmt* stmt, qint32 idx) {
        if(m_hashMember.contains(name))
            return m_hashMember[name]->bindValue(stmt, idx);
        return false;
    }

    const QVector<AbstractDboMember*>& getListMember() const {
        return m_listMember;
    }

private:
    QHash<QString, AbstractDboMember*> m_hashMember;
    QVector<AbstractDboMember*>        m_listMember;
};

#endif // DBOREGISTER_H
