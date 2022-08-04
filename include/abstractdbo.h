#ifndef ABSTRACTDBO_H
#define ABSTRACTDBO_H

#include "dboregister.h"

class AbstractDBO
{
public:
    AbstractDBO();
    ~AbstractDBO();

    virtual QString getTableName() = 0;
    virtual void registerMember();
    
    DboRegister& getRegister();
    qint32 getId() {return m_id;}

protected:
    template<typename T>
    void bind(const QString& name, QString sqlType, T* value) {
        m_dboRegister.bind(name, sqlType, value);
    }
    
    template<typename T>
    void relation(const QString& relation, T* value, const QString& tableLeft) {
        static_assert(std::is_base_of_v<AbstractDBO, T>, "Error: relation type must base on AbstractDBO");        
        m_dboRegister.relation(relation, value, tableLeft);
    }
    
    template<typename T>
    void relation(const QString& relation, QVector<T>* value, const QString& tableLeft) {
        static_assert(std::is_base_of_v<AbstractDBO, T>, "Error: relation type must base on AbstractDBO");
        m_dboRegister.relation(relation, value, tableLeft);
    }
    
public:
    qint32 m_id = 0;

protected:
    DboRegister m_dboRegister;
};

#endif // ABSTRACTDBO_H
