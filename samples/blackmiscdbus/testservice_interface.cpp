#include "testservice_interface.h"

namespace BlackMiscTest
{
    TestServiceInterface::TestServiceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
        : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {}

    TestServiceInterface::~TestServiceInterface()
    {}
}
