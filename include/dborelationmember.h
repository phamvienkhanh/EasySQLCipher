#ifndef DBORELATIONMEMBER_H
#define DBORELATIONMEMBER_H

#include "columndata.h"
#include "dbhelper.h"

class AbstractDboRelationMember
{
public:
    AbstractDboRelationMember(){}
    virtual ~AbstractDboRelationMember(){}
    
    virtual QString getRelation() = 0;
    virtual QString buildSelectClause(bool prefixTableName = false) = 0;
    virtual QString buildSelectClause(QString cols, bool prefixTableName = false) = 0;
    virtual QString buildJoinClause() = 0;
};

template<typename T>
class DboRelationMember : public AbstractDboRelationMember
{
public:
    DboRelationMember(T* value, const QString& relationOn, const QString tableLeft) {        
        m_value = value;
        m_listValues = nullptr;
        m_relationOn = relationOn;
        m_tableLeft = tableLeft;
    }
    DboRelationMember(QVector<T>* listVals, const QString& relationOn, const QString tableLeft) {
        m_listValues = listVals;
        m_value = nullptr;
        m_relationOn = relationOn;
        m_tableLeft = tableLeft;
    }
    
    QString getRelation() override {
        return m_relationOn;
    }
    
    QString buildJoinClause() override {
        QStringList onCols = m_relationOn.split("=");
        if(onCols.size() != 2) {
            return "";
        }
        
        T obj;
        QString tableRight = obj.getTableName();
        return QString("JOIN %1 ON %2.%3 = %1.%4 ")
                .arg(tableRight, m_tableLeft, onCols[0], onCols[1]);
    }
    
    QString buildSelectClause(bool prefixTableName = false) override {
        return DBHelper::buildSelectClause<T>(prefixTableName);
    }
    
    QString buildSelectClause(QString cols, bool prefixTableName = false) override {
        return DBHelper::buildSelectClause<T>(cols, prefixTableName);
    } 

private:
    T* m_value;
    QVector<T>* m_listValues;
    QString m_relationOn;
    QString m_tableLeft;
};

#endif // DBORELATIONMEMBER_H
