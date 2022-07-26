#ifndef SQLTEMPLATE_H
#define SQLTEMPLATE_H

namespace DBHelper
{

Q_GLOBAL_STATIC_WITH_ARGS(QString, templateSelect, ("SELECT %1 FROM %2 %3;"))

}
#endif // SQLTEMPLATE_H
