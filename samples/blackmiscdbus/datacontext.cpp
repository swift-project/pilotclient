#include "datacontext.h"

namespace BlackMiscTest
{

    const QString CDataContext::ServiceName = QString(BLACKMISCKTEST_DATACONTEXT_INTERFACENAME);
    const QString CDataContext::ServicePath = QString(BLACKMISCKTEST_DATACONTEXT_SERVICEPATH);

    /*
     * Init this context
     */
    CDataContext::CDataContext(BlackCore::CDBusServer *server)
    {
        // 1. Register with the server
        server->addObject(CDataContext::ServicePath, this);

        // 2. Next I would wire all signals and slots of this context
        //    belonging together
    }

    /*
     * FooSlot
     */
    void CDataContext::fooSlot(const QString &baz)
    {
        qDebug() << "foo slot" << baz;
    }


    /*
     * FooSlot
     * \return
     */
    QString CDataContext::fooSlotRet(const QString &baz)
    {
        qDebug() << "foo slot" << baz;
        return baz;
    }
}
