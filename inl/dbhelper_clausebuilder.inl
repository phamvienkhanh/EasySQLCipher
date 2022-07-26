#ifndef DBHELPER_CLAUSEBUILDER_INL
#define DBHELPER_CLAUSEBUILDER_INL

namespace DBHelper {

template<typename T>
QString buildSelectClause(bool prefixTableName /*= false*/) {
    QString rsStr;
    T obj;
    obj.registerMember();
    
    QString tableName = obj.getTableName();
    const auto& listMemmber = obj.getRegister().getListMember();
    for(auto i = 0; i < listMemmber.size(); i++) {
        auto& colInfo = listMemmber[i]->getColumnInfo();
        if(prefixTableName)
            rsStr += QString("%1.%2 as %1_%2, ").arg(tableName, colInfo.colName);
        else
            rsStr += QString("%1.%2, ").arg(tableName, colInfo.colName);
    }
    rsStr.chop(2);
    
    return rsStr;
}

template<typename T>
QString buildSelectClause(QString cols, bool prefixTableName /*= false*/) {
    QString rsStr;
    cols = cols.trimmed();
    if(cols == "*") {
        return DBHelper::buildSelectClause<T>(prefixTableName);
    }
    
    T obj;
    obj.registerMember();        
    QString tableName = obj.getTableName();
    QStringList listColsSelect = cols.split(",");
    for(auto& iCol : listColsSelect) {
        if(prefixTableName)
            rsStr += QString("%1.%2 as %1_%2, ").arg(tableName, iCol.trimmed());
        else
            rsStr += QString("%1.%2, ").arg(tableName, iCol.trimmed());
    }
    rsStr.chop(2);
    
    return rsStr;
}

}

#endif // DBHELPER_CLAUSEBUILDER_INL
