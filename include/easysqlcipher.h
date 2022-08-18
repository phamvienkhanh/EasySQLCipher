#ifndef EASYSQLCIPHER_H
#define EASYSQLCIPHER_H

#include <sqlcipher/sqlite3.h>
#include "types.h"

class EasySQLCipher
{
public:
    EasySQLCipher();
    DBCode init(const DBInitParam& param);
    void close();

    sqlite3* getConnection() {return m_connection;}

protected:
    sqlite3* m_connection = nullptr;
};

#endif // EASYSQLCIPHER_H
