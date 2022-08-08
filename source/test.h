#ifndef TEST_H
#define TEST_H

#include "easysqlcipher.h"

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

class Email;

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
        
        relation("sip_id=author", &m_messages, getTableName());
        relation("sip_id=sip_id", &m_emails, getTableName());
    }

public:
    QString m_name;
    QString m_sip_id;
    QByteArray m_data;
    
    QVector<Message> m_messages;
    QVector<Email>   m_emails;
    
};

class Email : public AbstractDBO
{
public:
    Email(){}

    QString getTableName() const override {
        return "Email";
    }

    void registerMember() override {
        AbstractDBO::registerMember();
        
        bind("sip_id", "text", &m_sip_id);
        bind("address", "text", &m_address);
        
        relation("sip_id=sip_id", &m_user, getTableName());
    }

public:
    QString m_address;
    QString m_sip_id;
    
    User m_user;
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
        emails.setFnConProvider(getConnection);
    }

    DbSet<User>    users;
    DbSet<Message> messages;
    DbSet<Email>   emails;
};

#endif // TEST_H
