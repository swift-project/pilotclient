#ifndef BLACKMISCTEST_DUMMYSIGNALSLOT_H
#define BLACKMISCTEST_DUMMYSIGNALSLOT_H

#include <QObject>
#include <QString>
#include <QDebug>

namespace BlackMiscTest
{

    /*!
     * \brief Dummy class to hook a signal or slot
     */
    class CDummySignalSlot : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * \brief CDummySignalSlot
         * \param parent
         */
        CDummySignalSlot(const QString &name, QObject *parent = 0);

    signals:
        /*!
         * \brief Test signal
         * \param saySomething
         */
        void signalCDummy(const QString &saySomething);
    public slots:
        /*!
         * \brief Test slot
         * \param saySomething
         */
        void slotCDummy(const QString &saySomething);

    private:
        QString m_name;

    };
}
#endif // BLACKMISCTEST_DUMMYSIGNALSLOT_H
