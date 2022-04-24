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
