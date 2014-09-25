/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SETTINGS_AUDIO_H
#define BLACKMISC_SETTINGS_AUDIO_H

#include "valueobject.h"
#include "statusmessagelist.h"
#include "settingutilities.h"
#include "notificationsounds.h"
#include <QJsonObject>

namespace BlackMisc
{
    namespace Settings
    {
        //! Value object encapsulating information of audio related settings.
        class CSettingsAudio : public BlackMisc::CValueObject
        {
        public:
            //! Default constructor.
            CSettingsAudio();

            //! Destructor.
            virtual ~CSettingsAudio() {}

            //! Path
            static const QString &ValueNotificationFlag()
            {
                static const QString value("notificationflag");
                return value;
            }

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Notification flag (play notification?)
            bool getNotificationFlag(BlackSound::CNotificationSounds::Notification notification) const;

            //! Notification flag (play notification?)
            void setNotificationFlag(BlackSound::CNotificationSounds::Notification notification, bool value);

            //! Equal operator ==
            bool operator ==(const CSettingsAudio &other) const;

            //! Unequal operator !=
            bool operator !=(const CSettingsAudio &other) const;

            //! \copydoc BlackCore::IContextSettings::value
            virtual BlackMisc::CStatusMessage value(const QString &path, const QString &command, const BlackMisc::CVariant &value, bool &changedFlag);

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! Init with meaningful default values
            void initDefaultValues();

            //! \copydoc CValueObject::registerMetadata
            static void registerMetadata();

            //! JSON member names
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
            BLACK_ENABLE_TUPLE_CONVERSION(CSettingsAudio)
            QString m_notificationFlags;  //!< play notification for notification x, a little trick to use a string here (streamable, hashable, ..)
            void initNotificationFlags(); //!< init flags
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Settings::CSettingsAudio)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Settings::CSettingsAudio, (o.m_notificationFlags))

#endif // guard
