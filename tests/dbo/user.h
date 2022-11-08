#ifndef USER_H
#define USER_H

#include "abstractdbo.h"
#include "message.h"
#include "email.h"

class User : public AbstractDBO
{
public:
    User(){}

    QString getTableName() const override {
        return "User";
    }

    void registerMember() override {
        AbstractDBO::registerMember();

        bind("sip_id", "text", &m_sip_id);
        bind("name", "text", &m_name);
        bind("data", "BLOB", &m_data);

        relation("sip_id=author", &m_messages);
        relation("sip_id=sip_id", &m_emails);
    }

public:
    QString m_name;
    QString m_sip_id;
    QByteArray m_data;

    QVector<Message> m_messages;
    QVector<Email>   m_emails;

};

#endif // USER_H
