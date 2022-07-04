#ifndef ABSTRACTDBO_H
#define ABSTRACTDBO_H

#include "dboregister.h"

class AbstractDBO
{
public:
    AbstractDBO();
    ~AbstractDBO();

    virtual QString getTableName() = 0;
    virtual void registerMember() = 0;

    DboRegister& getRegister();

protected:
    template<typename T>
    void bind(QString name, QString sqlType, T* value) {
        m_dboRegister.bind(name, sqlType, value);
    }
    
    template<typename T>
    void relation(QString relation, T* value) {
        static_assert(std::is_base_of_v<AbstractDBO, T>, "Error: relation type must base on AbstractDBO");        
        m_dboRegister.relation(relation, value);
    }
    
    template<typename T>
    void relation(QString relation, QVector<T>* value) {
        static_assert(std::is_base_of_v<AbstractDBO, T>, "Error: relation type must base on AbstractDBO");
        m_dboRegister.relation(relation, value);
    }

protected:
    DboRegister m_dboRegister;
};

#endif // ABSTRACTDBO_H
