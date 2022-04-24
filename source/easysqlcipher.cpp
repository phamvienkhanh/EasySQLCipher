#include "easysqlcipher.h"

EasySQLCipher::EasySQLCipher()
{
}

DBCode EasySQLCipher::init(const DBInitParam& param)
{
    int rs = sqlite3_open_v2(param.dbPath.toUtf8(), &m_connection, param.openMode, nullptr);
    if(rs == SQLITE_OK) {
        rs = sqlite3_exec(m_connection, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    }

    return rs == SQLITE_OK ? DBCode::OK : DBCode::Failed;
}

void EasySQLCipher::close()
{
    if(m_connection) {
        sqlite3_close(m_connection);
        m_connection = nullptr;
    }
}
