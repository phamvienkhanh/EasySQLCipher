#ifndef SQLTEMPLATE_H
#define SQLTEMPLATE_H

#ifndef ENABLE_PRECOMPILED_HEADER
#include "precompiled.h"
#endif

namespace DBHelper
{

Q_GLOBAL_STATIC_WITH_ARGS(QString, templateSelect, ("SELECT %1 FROM %2 %3;"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, templateCreateTable, ("CREATE TABLE IF NOT EXISTS %1(%2);"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, templateInsert, ("INSERT INTO %1(%2) VALUES %3 RETURNING %4;"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, templateDelete, ("DELETE FROM %1 %2;"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, templateUpdate, ("UPDATE %1 SET %2 WHERE %3;"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, templateUpdate2, ("UPDATE %1 %2;"))

}
#endif // SQLTEMPLATE_H
