/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_IDENTIFIER_H
#define BLACKMISC_IDENTIFIER_H

//! \file

#include "blackmiscexport.h"
#include "valueobject.h"
#include "timestampbased.h"
#include "blackmiscfreefunctions.h"
#include <QByteArray>
#include <QString>
#include <QUuid>

namespace BlackMisc
{
    /*!
     * Value object encapsulating information identifying a component of a modular distributed swift process (core, GUI, audio)
     */
    class BLACKMISC_EXPORT CIdentifier :
        public CValueObject<CIdentifier>,
        public ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = BlackMisc::CPropertyIndex::GlobalIndexCIdentifier,
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
        CIdentifier(const QString &name = QString());

        //! Returns an anonymous identifier.
        static CIdentifier anonymous();

        //! Produces a UUID generated from the identifier.
        QUuid toUuid() const;

        //! Name
        QString getName() const { return m_name; }

        //! Has name
        bool hasName() const { return !m_name.isEmpty(); }

        //! Get machine id
        QByteArray getMachineId() const;

        //! Machine 64 base64 encoded
        QString getMachineIdBase64() const { return m_machineIdBase64; }

        //! Machine name
        QString getMachineName() const { return m_machineName; }

        //! Get process id
        qint64 getProcessId() const {return m_processId;}

        //! Get process name
        QString getProcessName() const {return m_processName;}

        //! Check if originating from the same local machine
        bool isFromLocalMachine() const;

        //! Check if originating from the same process id
        bool isFromSameProcess() const;

        //! Check if originating from the same process name
        bool isFromSameProcessName() const;

        //! \copydoc CValueObject::convertToQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc CValueObject::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc CValueObject::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CIdentifier)
        QString m_name;            //!< object name
        QString m_machineIdBase64; //!< base 64 encoded machine id
        QString m_machineName;     //!< human readable machine name
        QString m_processName;     //!< process name
        qint64 m_processId;        //!< PID
    };
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CIdentifier, (
                                   attr(o.m_name),
                                   attr(o.m_machineIdBase64),
                                   attr(o.m_machineName, flags <DisabledForComparison | DisabledForHashing> ()),
                                   attr(o.m_processName),
                                   attr(o.m_processId),
                                   attr(o.m_timestampMSecsSinceEpoch, flags <DisabledForComparison | DisabledForHashing> ())
                               ))

Q_DECLARE_METATYPE(BlackMisc::CIdentifier)

#endif // guard
