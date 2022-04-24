#include "columndata.h"

ColumnData::ColumnData(const qint32& idx, sqlite3_stmt* stmt)
{
    m_colIdx = idx;
    m_stmt = stmt;
}

ColumnData::operator qint32() const
{
    return sqlite3_column_int(m_stmt, m_colIdx);
}

ColumnData::operator QString() const
{
    return QString::fromUtf8((char*)sqlite3_column_text(m_stmt, m_colIdx)
                             , sqlite3_column_bytes(m_stmt, m_colIdx));
}

ColumnData::operator QByteArray() const
{
    return QByteArray((char*)sqlite3_column_blob(m_stmt, m_colIdx)
                      , sqlite3_column_bytes(m_stmt, m_colIdx));
}
