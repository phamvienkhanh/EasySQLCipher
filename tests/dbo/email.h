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

    void registerMember() override;

public:
    QString m_address;
    QString m_sip_id;

    User* m_user;
};

#endif // EMAIL_H
