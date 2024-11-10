// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SETTINGS_XSWIFTBUSSETTINGS_H
#define SWIFT_MISC_SIMULATION_SETTINGS_XSWIFTBUSSETTINGS_H

#include "misc/simulation/settings/xswiftbussettingsqtfree.h"
#include "misc/statusmessagelist.h"
#include "misc/settingscache.h"
#include "misc/dbusserver.h"
#include "misc/valueobject.h"
#include "misc/swiftmiscexport.h"

#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::Settings, CXSwiftBusSettings)

namespace swift::misc::simulation::settings
{
    //! xswiftbus settings
    class SWIFT_MISC_EXPORT CXSwiftBusSettings final :
        public CValueObject<CXSwiftBusSettings>,
        public CXSwiftBusSettingsQtFree,
        public ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDBusServerAddress = CPropertyIndexRef::GlobalIndexCXSwiftBusSettings,
            IndexMaxPlanes,
            IndexDrawingLabels,
            IndexBundleTaxiLandingLights,
            IndexMessageBoxMargins,
            IndexMaxDrawingDistance,
            IndexFollowAircraftDistance,
            IndexNightTextureMode
        };

        //! Default constructor
        CXSwiftBusSettings();

        //! From JSON constructor
        CXSwiftBusSettings(const QString &json);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc CXSwiftBusSettingsQtFree::getDBusServerAddress
        QString getDBusServerAddressQt() const { return QString::fromStdString(this->getDBusServerAddress()); }

        //! \copydoc CXSwiftBusSettingsQtFree::setDBusServerAddress
        void setDBusServerAddressQt(const QString &dBusAddress) { this->setDBusServerAddress(dBusAddress.toStdString()); }

        //! \copydoc CXSwiftBusSettingsQtFree::getNightTextureMode
        QString getNightTextureModeQt() const { return QString::fromStdString(this->getNightTextureMode()); }

        //! \copydoc CXSwiftBusSettingsQtFree::setNightTextureMode
        void setNightTextureModeQt(const QString &nightTexture) { this->setNightTextureMode(nightTexture.toStdString()); }

        //! \copydoc CXSwiftBusSettingsQtFree::toXSwiftBusJsonString
        QString toXSwiftBusJsonStringQt() const { return QString::fromStdString(this->toXSwiftBusJsonString()); }

        //! \copydoc CXSwiftBusSettingsQtFree::parseXSwiftBusString
        void parseXSwiftBusStringQt(const QString &json) { this->parseXSwiftBusString(json.toStdString()); }

        //! Sets both timestamps
        virtual void setCurrentUtcTime() override;

        //! Valid settings?
        CStatusMessageList validate() const;

        //! Default value for settings
        static const CXSwiftBusSettings &defaultValue();

    protected:
        //! \copydoc CXSwiftBusSettingsQtFree::objectUpdated
        virtual void objectUpdated() override final;

    private:
        BLACK_METACLASS(
            CXSwiftBusSettings,
            BLACK_METAMEMBER(dBusServerAddress),
            BLACK_METAMEMBER(msgBox),
            BLACK_METAMEMBER(nightTextureMode),
            BLACK_METAMEMBER(maxPlanes),
            BLACK_METAMEMBER(drawingLabels),
            BLACK_METAMEMBER(labelColor),
            BLACK_METAMEMBER(bundleTaxiLandingLights),
            BLACK_METAMEMBER(maxDrawDistanceNM),
            BLACK_METAMEMBER(followAircraftDistanceM),
            BLACK_METAMEMBER(logRenderPhases),
            BLACK_METAMEMBER(tcasEnabled),
            BLACK_METAMEMBER(terrainProbeEnabled),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch, 0, DisabledForComparison | DisabledForHashing)
        );
    };

    /*!
     * Setting for xswiftbus.
     */
    struct TXSwiftBusSettings : public TSettingTrait<CXSwiftBusSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "xswiftbus/settings"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("xswiftbus");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static CXSwiftBusSettings defaultValue() { return CXSwiftBusSettings::defaultValue(); }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const CXSwiftBusSettings &settings, QString &reason)
        {
            const CStatusMessageList msgs = settings.validate();
            reason = msgs.toQString(true);
            return msgs.isSuccess();
        }
    };
} // ns

Q_DECLARE_METATYPE(swift::misc::simulation::settings::CXSwiftBusSettings)

#endif // guard
