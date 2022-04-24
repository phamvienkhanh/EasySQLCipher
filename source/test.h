#ifndef TEST_H
#define TEST_H

#include "easysqlcipher.h"

class User : public AbstractDBO
{
public:
    User(){}

    virtual QString getTableName() override {
        return "User";
    }

    virtual void registerMemmber() override {
        bind("id", "integer", &m_id);
        bind("name", "text", &m_name);
        bind("data", "BLOB", &m_data);
    }

public:
    qint32  m_id;
    QString m_name;
    QByteArray m_data;
};

class Message : public AbstractDBO
{
public:
    Message(){}

    virtual QString getTableName() override {
        return "Message";
    }

    virtual void registerMemmber() override {
        bind("id", "integer", &m_id);
        bind("body", "text", &m_body);
        bind("md5", "BLOB", &m_md5);
        bind("identity", "text", &m_identity);
    }

public:
    qint32      m_id;
    QString     m_body;
    QByteArray  m_md5;
    QString     m_identity;
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
