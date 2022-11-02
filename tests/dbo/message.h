#ifndef MESSAGE_H
#define MESSAGE_H

#include "abstractdbo.h"

class User;

class Message : public AbstractDBO
{
public:
    Message(){}

    QString getTableName() const override {
        return "Message";
    }

    void registerMember() override {
        AbstractDBO::registerMember();

        bind("body", "text", &m_body);
        bind("md5", "BLOB", &m_md5);
        bind("identity", "text", &m_identity);
        bind("author", "text", &m_author);

        relation("author=sip_id", m_user, getTableName());
    }

public:
    QString     m_body;
    QByteArray  m_md5;
    QString     m_identity;
    QString     m_author;

    std::shared_ptr<User> m_user;
};

#endif // MESSAGE_H
