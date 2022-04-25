#ifndef DBOREGISTER_H
#define DBOREGISTER_H

#include "dbomemmber.h"

class DboRegister
{
public:

    DboRegister(){};

    DboRegister(const DboRegister&) {}
    DboRegister& operator=(const DboRegister& regist) {
        return *this;
    }

    virtual ~DboRegister() {
        for(auto& iMemmber : m_listMemmber) {
            delete iMemmber;
        }

        m_hashMemmber.clear();
        m_listMemmber.clear();
    }

    template<typename T>
    void bind(QString name, QString sqlType, T* memmber) {
        if(!m_hashMemmber.contains(name)) {
            auto dboMemmber = new DboMemmber(memmber, name, sqlType);
            m_hashMemmber.insert(name, dboMemmber);
            m_listMemmber.push_back(dboMemmber);
        }
    }

    template<typename T>
    void setValue(QString name, ColumnData value) {
        if(m_hashMemmber.contains(name))
            m_hashMemmber[name]->setValue(value);
    }

    template<typename T>
    bool bindValue(QString name, sqlite3_stmt* stmt) {
        if(m_hashMemmber.contains(name))
            return m_hashMemmber[name]->bindValue(stmt);
        return false;
    }

    template<typename T>
    bool bindValue(QString name, sqlite3_stmt* stmt, qint32 idx) {
        if(m_hashMemmber.contains(name))
            return m_hashMemmber[name]->bindValue(stmt, idx);
        return false;
    }

    const QVector<AbstractDboMemmber*>& getListMemmber() const {
        return m_listMemmber;
    }

private:
    QHash<QString, AbstractDboMemmber*> m_hashMemmber;
    QVector<AbstractDboMemmber*>        m_listMemmber;
};

#endif // DBOREGISTER_H
