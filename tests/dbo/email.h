#ifndef EMAIL_H
#define EMAIL_H

#include "abstractdbo.h"

class User;

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

        relation("sip_id=sip_id", m_user);
    }

public:
    QString m_address;
    QString m_sip_id;

    std::shared_ptr<User> m_user;
};

#endif // EMAIL_H
