/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_ORIGINATOR_H
#define BLACKMISC_ORIGINATOR_H

//! \file

#include "blackmiscexport.h"
#include "valueobject.h"
#include "blackmiscfreefunctions.h"
#include <QByteArray>
#include <QString>

class QObject;

namespace BlackMisc
{
    //! Value object encapsulating information about the originiator
    class BLACKMISC_EXPORT COriginator : public CValueObject<COriginator>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexName = BlackMisc::CPropertyIndex::GlobalIndexOriginator,
            IndexMachineId,
            IndexMachineIdBase64,
            IndexProcessId,
            IndexProcessName,
            IndexUtcTimestamp,
            IndexIsFromLocalMachine,
            IndexIsFromSameProcess,
            IndexIsFromSameProcessName
        };

        //! Constructor.
        COriginator(const QString &name = QString());

        //! Constructor using the objectName of object as name
        COriginator(const QObject *object);

        //! Name
        QString getName() const { return m_name; }

        //! Get machine id
        QByteArray getMachineId() const;

        //! Machine 64 base64 encoded
        QString getMachineIdBase64() const { return m_machineIdBase64; }

        //! Get process id
        qint64 getProcessId() const {return m_processId;}

        //! Get process name
        QString getProcessName() const {return m_processName;}

        //! When created
        QDateTime getTimestamp() const { return QDateTime::fromMSecsSinceEpoch(m_timestampMsEpoch); }

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
        BLACK_ENABLE_TUPLE_CONVERSION(COriginator)
        QString m_name;
        QString m_machineIdBase64; // base 64 encoded
        QString m_processName;
        qint64 m_processId;
        qint64 m_timestampMsEpoch;
    };
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::COriginator, (
    attr(o.m_name),
    attr(o.m_machineIdBase64),
    attr(o.m_processName),
    attr(o.m_processId),
    attr(o.m_timestampMsEpoch, flags <DisabledForComparison | DisabledForHashing> ())
))

Q_DECLARE_METATYPE(BlackMisc::COriginator)

#endif // guard
