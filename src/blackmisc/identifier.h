/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_IDENTIFIER_H
#define BLACKMISC_IDENTIFIER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"

#include <QByteArray>
#include <QMetaType>
#include <QString>
#include <QObject>
#include <QUuid>
#include <QtGlobal>

namespace BlackMisc
{
    /*!
     * Value object encapsulating information identifying a component of a modular distributed swift process (core, GUI, audio)
     */
    class BLACKMISC_EXPORT CIdentifier : public CValueObject<CIdentifier>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = CPropertyIndexRef::GlobalIndexCIdentifier,
            IndexMachineId,
            IndexMachineIdBase64,
            IndexMachineName,
            IndexProcessId,
            IndexProcessName,
            IndexIsFromLocalMachine,
            IndexIsFromSameProcess,
            IndexIsFromSameProcessName
        };

        //! Constructor.
        //! \private use CIdentifier::anonymous() if you need an identifier without name and CIdentifier::null() for an empty identifier
        CIdentifier() : CIdentifier("") {}

        //! Constructor.
        CIdentifier(const QString &name);

        //! Constructor using object's name
        //! \remark automatically changes if object's name changes
        CIdentifier(const QString &name, QObject *object);

        //! Returns an anonymous identifier, which is a valid identifier without name
        //! \remark same as CIdentifier()
        static const CIdentifier &anonymous();

        //! Null (empty) identifier
        static const CIdentifier &null();

        //! Returns a fake identifier.
        static const CIdentifier &fake();

        //! Produces a UUID generated from the identifier.
        QUuid toUuid() const;

        //! UUID string
        QString toUuidString() const;

        //! Produces a DBus object path from the identifier.
        QString toDBusObjectPath(const QString &root = {}) const;

        //! Reconstruct an identifier from a DBus object path.
        static CIdentifier fromDBusObjectPath(const QString &path, const QString &root = {});

        //! Name
        const QString &getName() const { return m_name; }

        //! Has name
        bool hasName() const { return !m_name.isEmpty(); }

        //! Set the name
        void setName(const QString &name) { m_name = name; }

        //! Set name or append name
        //! \remark append makes sense if an object name changes after some time (like for UI components when the whole UI is setup)
        void appendName(const QString &name);

        //! Reflect changes of QObject::
        void linkWithQObjectName(QObject *object);

        //! Get machine id
        QByteArray getMachineId() const;

        //! Machine base64 encoded
        const QString &getMachineIdBase64() const { return m_machineIdBase64; }

        //! Machine name
        const QString &getMachineName() const { return m_machineName; }

        //! Check if the other identifier has the same machine name
        bool hasSameMachineName(const CIdentifier &other) const;

        //! Check if other identifier is from the same machine id
        bool hasSameMachineId(const CIdentifier &other) const;

        //! Same machine or id?
        bool hasSameMachineNameOrId(const CIdentifier &other) const;

        //! Get process id
        qint64 getProcessId() const {return m_processId;}

        //! Get process name
        const QString &getProcessName() const {return m_processName;}

        //! Check if originating from the same local machine
        bool isFromLocalMachine() const;

        //! Check if originating from the same process id
        bool hasApplicationProcessId() const;

        //! Check if originating from the same process name
        bool hasApplicationProcessName() const;

        //! Check if it is anonymous identifier
        bool isAnonymous() const;

        //! Null identifier (no name, ids etc)
        bool isNull() const;

        //! Update to current machine
        void updateToCurrentMachine();

        //! Update to current process
        void updateToCurrentProcess();

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CIdentifier &compareValue) const;

    private:
        //! Constructor.
        CIdentifier(const QString &name, const QString &machineId, const QString &machineName,
                    const QString &processName, qint64 processId = 0);

        QString m_name;            //!< object name
        QString m_machineIdBase64; //!< base 64 encoded machine id
        QString m_machineName;     //!< human readable machine name
        QString m_processName;     //!< process name
        qint64 m_processId;        //!< PID

        BLACK_METACLASS(
            CIdentifier,
            BLACK_METAMEMBER(name),
            BLACK_METAMEMBER(machineIdBase64),
            BLACK_METAMEMBER(machineName, 0, DisabledForComparison | DisabledForHashing),
            BLACK_METAMEMBER(processName),
            BLACK_METAMEMBER(processId)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CIdentifier)

#endif // guard
