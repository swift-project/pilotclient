/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_EVENT_ORIGINATOR_H
#define BLACKMISC_EVENT_ORIGINATOR_H

//! \file

#include "valueobject.h"
#include "blackmiscfreefunctions.h"
#include <QByteArray>
#include <QString>

namespace BlackMisc
{
    namespace Event
    {

        //! Value object encapsulating information about the originiator
        class COriginator : public BlackMisc::CValueObject
        {
        public:
            //! Default constructor.
            COriginator();

            //! Get machine id
            QByteArray getMachineId() const {return m_machineId;}

            //! Get process id
            qint32 getProcessId() const {return m_processId;}

            //! Get process name
            QString getProcessName() const {return m_processName;}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \brief Register metadata
            static void registerMetadata();

            //! Check if originating from the same local machine
            bool isFromLocalMachine() const;

            //! Check if originating from the same process id
            bool isFromSameProcess() const;

            //! Check if originating from the same process name
            bool isFromSameProcessName() const;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(COriginator)
            QString m_originatorName;
            QByteArray m_machineId;
            QByteArray m_primaryIpAddress;
            QByteArray m_objectId;
            qint32 m_processId;
            QString m_processName;
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Event::COriginator, (o.m_originatorName, o.m_machineId, o.m_primaryIpAddress, o.m_objectId, o.m_processId, o.m_processName))
Q_DECLARE_METATYPE(BlackMisc::Event::COriginator)

#endif // BLACKMISC_EVENT_ORIGINATOR_H
