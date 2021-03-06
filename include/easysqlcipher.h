#ifndef EASYSQLCIPHER_H
#define EASYSQLCIPHER_H

#include <sqlite3.h>

#include "dbset.h"
#include "types.h"
#include "abstractdbo.h"

class EasySQLCipher
{
public:
    EasySQLCipher();
    DBCode init(const DBInitParam& param);
    void close();

protected:
    sqlite3* m_connection = nullptr;
};

#endif // EASYSQLCIPHER_H
