#ifndef MESSAGE_H
#define MESSAGE_H

#include "abstractdbo.h"

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
    }

public:
    QString     m_body;
    QByteArray  m_md5;
    QString     m_identity;
    QString     m_author;
};

#endif // MESSAGE_H
