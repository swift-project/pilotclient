/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_VOICECAPABILITIES_H
#define BLACKMISC_VOICECAPABILITIES_H

//! \file

#include "valueobject.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Network
    {

        /*!
         * Value object encapsulating information for voice capabilities.
         */
        class CVoiceCapabilities : public BlackMisc::CValueObject
        {
        public:

            //! Voice capabilities
            enum VoiceCapabilities
            {
                Unknown,
                Voice,
                VoiceReceivingOnly,
                TextOnly,
            };

            //! Default constructor.
            CVoiceCapabilities() {}

            //! Constructor by callsign
            CVoiceCapabilities(VoiceCapabilities capabilities) : m_voiceCapabilities(static_cast<int>(capabilities)) {}

            //! Constructor.
            CVoiceCapabilities(const QString &flightPlanRemarks);

            //! Get capabilities
            VoiceCapabilities getCapabilities() const { return static_cast<VoiceCapabilities>(m_voiceCapabilities); }

            //! Set capabilites
            void setCapabilities(VoiceCapabilities capabilites) { m_voiceCapabilities = static_cast<int>(capabilites); }

            //! \copydoc CValueObject::toIcon()
            virtual CIcon toIcon() const override;

            //! Equal operator ==
            bool operator ==(const CVoiceCapabilities &other) const;

            //! Unequal operator !=
            bool operator !=(const CVoiceCapabilities &other) const;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! Register metadata
            static void registerMetadata();

            //! Members
            static const QStringList &jsonMembers();

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
            BLACK_ENABLE_TUPLE_CONVERSION(CVoiceCapabilities)
            int m_voiceCapabilities = Unknown;

            //! Capabilites from flight plans remarks such as "/V/"
            void fromFlightPlanRemarks(const QString &flightPlanRemarks);
        };

    } // namespace

} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CVoiceCapabilities, (o.m_voiceCapabilities))
Q_DECLARE_METATYPE(BlackMisc::Network::CVoiceCapabilities)

#endif // guard
