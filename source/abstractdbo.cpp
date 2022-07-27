#include "abstractdbo.h"

AbstractDBO::AbstractDBO()
{
}

AbstractDBO::~AbstractDBO()
{
}

DboRegister& AbstractDBO::getRegister()
{
    return m_dboRegister;
}

void AbstractDBO::registerMember()
{
    m_dboRegister.bind("id", "integer", &m_id);
}
