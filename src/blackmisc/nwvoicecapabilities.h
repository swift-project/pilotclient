/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_VOICECAPABILITIES_H
#define BLACKMISC_VOICECAPABILITIES_H

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

            //! \brief Equal operator ==
            bool operator ==(const CVoiceCapabilities &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CVoiceCapabilities &other) const;

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! \brief Register metadata
            static void registerMetadata();

            //! \brief Members
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
