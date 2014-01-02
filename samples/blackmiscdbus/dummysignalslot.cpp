#include "dummysignalslot.h"

namespace BlackMiscTest
{

    /*
     * Constructor
     */
    CDummySignalSlot::CDummySignalSlot(const QString &name, QObject *parent) : QObject(parent), m_name(name)
    {}

    /*
     * Dummy slot
     */
    void CDummySignalSlot::slotCDummy(const QString &saySomething)
    {
        qDebug() << Q_FUNC_INFO << "name:" << this->m_name << "received:" << saySomething;
    }

}
