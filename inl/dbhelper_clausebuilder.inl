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
    bool hasIdField = false;
    for(auto& iCol : listColsSelect) {
        QString colName = iCol.trimmed();
        if(colName == "id") {
            hasIdField = true;
        }
        
        if(prefixTableName)
            rsStr += QString("%1.%2 as %1_%2, ").arg(tableName, colName);
        else
            rsStr += QString("%1.%2, ").arg(tableName, colName);
    }
    rsStr.chop(2);
    
    if(!hasIdField) {
        if(prefixTableName)
            rsStr = QString("%1.id as %1_id, ").arg(tableName) + rsStr;
        else
            rsStr = QString("%1.id, ").arg(tableName) + rsStr;
    }
    
    return rsStr;
}

}

#endif // DBHELPER_CLAUSEBUILDER_INL
