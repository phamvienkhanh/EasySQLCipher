
#include "email.h"
#include "user.h"

void Email::registerMember() {
    AbstractDBO::registerMember();

    bind("sip_id", "text", &m_sip_id);
    bind("address", "text", &m_address);

    relation("sip_id=sip_id", m_user, getTableName());
}
