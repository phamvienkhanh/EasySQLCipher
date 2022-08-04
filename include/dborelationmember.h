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
    virtual void    setValue(const QList<ColumnData>& listColsData) = 0;
    virtual QString buildSelectClause(bool prefixTableName = false) = 0;
    virtual QString buildSelectClause(const QString& cols, bool prefixTableName = false) = 0;
    virtual QString buildJoinClause() = 0;
};

template<typename T>
class DboRelationMember : public AbstractDboRelationMember
{
public:
    DboRelationMember(T* value, const QString& relationOn, const QString& tableLeft) {             
        m_value = value;
        m_listValues = nullptr;
        m_relationOn = relationOn;
        m_tableLeft = tableLeft;
    }
    DboRelationMember(QVector<T>* listVals, const QString& relationOn, const QString& tableLeft) {
        m_listValues = listVals;
        m_value = nullptr;
        m_relationOn = relationOn;
        m_tableLeft = tableLeft;
    }
    
    QString getRelation() override {
        return m_relationOn;
    }
    
    void setValue(const QList<ColumnData>& listColsData) override {
        if(m_value) {
            m_value->registerMember();
            for(auto& iColsData : listColsData) {
                m_value->getRegister().setValue(iColsData.getColName(), iColsData);                
            }
        }
        else if(m_listValues) {
            T obj;
            obj.registerMember();
            for(auto& iColsData : listColsData) {
                obj.getRegister().setValue(iColsData.getColName(), iColsData);                
            }
            
            qint32 id = obj.getId();
            if(id != 0) {
                if(!m_idsAdded.contains(id)) {
                    m_idsAdded.insert(id);                    
                    m_listValues->push_back(obj);
                }
            }
        }
    }
    
    QString buildJoinClause() override {
        QStringList onCols = m_relationOn.split("=");
        if(onCols.size() != 2) {
            return "";
        }
        
        T obj;
        QString tableRight = obj.getTableName();
        return QString("LEFT JOIN %1 ON %2.%3 = %1.%4 ")
                .arg(tableRight, m_tableLeft, onCols[0], onCols[1]);
    }
    
    QString buildSelectClause(bool prefixTableName = false) override {
        return DBHelper::buildSelectClause<T>(prefixTableName);
    }
    
    QString buildSelectClause(const QString& cols, bool prefixTableName = false) override {
        return DBHelper::buildSelectClause<T>(cols, prefixTableName);
    } 

private:
    T* m_value;
    QSet<qint32> m_idsAdded;
    QVector<T>* m_listValues;
    QString m_relationOn;
    QString m_tableLeft;
};

#endif // DBORELATIONMEMBER_H
