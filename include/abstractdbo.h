#ifndef ABSTRACTDBO_H
#define ABSTRACTDBO_H

#include "dboregister.h"

class AbstractDBO
{
public:
    AbstractDBO();
    ~AbstractDBO();

    virtual QString getTableName() = 0;
    virtual void registerMemmber() = 0;

    DboRegister& getRegister();

protected:
    template<typename T>
    void bind(QString name, QString sqlType, T* value)
    {
        m_dboRegister.bind(name, sqlType, value);
    }

protected:
    DboRegister m_dboRegister;
};

#endif // ABSTRACTDBO_H
