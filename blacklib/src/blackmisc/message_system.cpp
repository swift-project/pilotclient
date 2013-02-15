#include "blackmisc/message_system.h"

namespace BlackMisc
{


CMessageSystem::CMessageSystem()
{
    init();
}

void CMessageSystem::init()
{
    CMessageFactory::registerMessages();
}

} // namespace BlackMisc
