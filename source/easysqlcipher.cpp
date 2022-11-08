#include "easysqlcipher.h"

EasySQLCipher::EasySQLCipher()
{
    m_workerPool = new QThreadPool(qApp);
}

DBCode EasySQLCipher::init(const DBInitParam& param)
{
    int rs = sqlite3_open_v2(param.dbPath.toUtf8(), &m_connection, param.openMode, nullptr);
    if(rs == SQLITE_OK) {
        rs = sqlite3_exec(m_connection, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    }

    m_workerPool->setMaxThreadCount(param.maxThreadCount);

    return rs == SQLITE_OK ? DBCode::OK : DBCode::Failed;
}

void EasySQLCipher::close()
{
    if(m_workerPool) {
//        m_workerPool->clear();
        m_workerPool->waitForDone();
        m_workerPool->deleteLater();
    }

    if(m_connection) {
        sqlite3_close(m_connection);
        m_connection = nullptr;
    }
}
