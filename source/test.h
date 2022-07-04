#ifndef TEST_H
#define TEST_H

#include "easysqlcipher.h"

class Message : public AbstractDBO
{
public:
    Message(){}

    QString getTableName() override {
        return "Message";
    }

    void registerMember() override {
        bind("id", "integer", &m_id);
        bind("body", "text", &m_body);
        bind("md5", "BLOB", &m_md5);
        bind("identity", "text", &m_identity);
        bind("author", "text", &m_author);
    }

public:
    qint32      m_id;
    QString     m_body;
    QByteArray  m_md5;
    QString     m_identity;
    QString     m_author;
};

class User : public AbstractDBO
{
public:
    User(){}

    QString getTableName() override {
        return "User";
    }

    void registerMember() override {
        bind("id", "integer", &m_id);
        bind("sip_id", "text", &m_sip_id);
        bind("name", "text", &m_name);
        bind("data", "BLOB", &m_data);
        
        relation("sip_id=author", &m_messages);
    }

public:
    qint32  m_id;
    QString m_name;
    QString m_sip_id;
    QByteArray m_data;
    
    QVector<Message> m_messages;
    Message mesg;
};

class TestDB : public EasySQLCipher
{
public:
    TestDB() {
        auto getConnection = [this](){
            return m_connection;
        };

        users.setFnConProvider(getConnection);
        messages.setFnConProvider(getConnection);
    }

    DbSet<User>    users;
    DbSet<Message> messages;
};

#endif // TEST_H
