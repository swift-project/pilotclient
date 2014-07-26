/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_VOICECAPABILITIES_H
#define BLACKMISC_VOICECAPABILITIES_H

//! \file

#include "valueobject.h"

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

            //!< Voice capabilities
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
            CVoiceCapabilities(VoiceCapabilities capabilities) : m_voiceCapabilities(static_cast<uint>(capabilities)) {}

            //! Constructor.
            CVoiceCapabilities(const QString &flightPlanRemarks);

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Get capabilities
            VoiceCapabilities getCapabilities() const { return static_cast<VoiceCapabilities>(m_voiceCapabilities); }

            //! Set capabilites
            void setCapabilities(VoiceCapabilities capabilites) { m_voiceCapabilities = static_cast<uint>(capabilites); }

            //! \copydoc CValueObject::toIcon()
            virtual const QPixmap &toIcon() const override;

            //! Equal operator ==
            bool operator ==(const CVoiceCapabilities &other) const;

            //! Unequal operator !=
            bool operator !=(const CVoiceCapabilities &other) const;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

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
            uint m_voiceCapabilities;

            //! Capabilites from flight plans remarks such as "/V/"
            void fromFlightPlanRemarks(const QString &flightPlanRemarks);
        };

    } // namespace

} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CVoiceCapabilities, (o.m_voiceCapabilities))
Q_DECLARE_METATYPE(BlackMisc::Network::CVoiceCapabilities)

#endif // guard
