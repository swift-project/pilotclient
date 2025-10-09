// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/settings/xswiftbussettings.h"

#include "misc/simulation/settings/xswiftbussettingsqtfree.inc"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation::settings, CXSwiftBusSettings)

namespace swift::misc::simulation::settings
{
    CXSwiftBusSettings::CXSwiftBusSettings() {}

    CXSwiftBusSettings::CXSwiftBusSettings(const QString &json) { this->parseXSwiftBusStringQt(json); }

    QVariant CXSwiftBusSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMaxPlanes: return QVariant::fromValue(m_maxPlanes);
        case IndexDBusServerAddress: return QVariant::fromValue(QString::fromStdString(m_dBusServerAddress));
        case IndexNightTextureMode: return QVariant::fromValue(QString::fromStdString(m_nightTextureMode));
        case IndexMessageBoxMargins: return QVariant::fromValue(QString::fromStdString(m_msgBox));
        case IndexDrawingLabels: return QVariant::fromValue(m_drawingLabels);
        case IndexBundleTaxiLandingLights: return QVariant::fromValue(m_bundleTaxiLandingLights);
        case IndexMaxDrawingDistance: return QVariant::fromValue(m_maxDrawDistanceNM);
        case IndexFollowAircraftDistance: return QVariant::fromValue(m_followAircraftDistanceM);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CXSwiftBusSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CXSwiftBusSettings>();
            return;
        }
        if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); }

        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMaxPlanes: m_maxPlanes = variant.toInt(); break;
        case IndexDBusServerAddress: m_dBusServerAddress = variant.toString().toStdString(); break;
        case IndexMessageBoxMargins: m_msgBox = variant.toString().toStdString(); break;
        case IndexNightTextureMode: m_nightTextureMode = variant.toString().toStdString(); break;
        case IndexDrawingLabels: m_drawingLabels = variant.toBool(); break;
        case IndexBundleTaxiLandingLights: m_bundleTaxiLandingLights = variant.toBool(); break;
        case IndexMaxDrawingDistance: m_maxDrawDistanceNM = variant.toDouble(); break;
        case IndexFollowAircraftDistance: m_followAircraftDistanceM = variant.toInt(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    QString CXSwiftBusSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QString::fromStdString(CXSwiftBusSettingsQtFree::convertToString());
    }

    void CXSwiftBusSettings::setCurrentUtcTime()
    {
        ITimestampBased::setCurrentUtcTime();
        m_msSinceEpochQtFree = m_timestampMSecsSinceEpoch;
    }

    CStatusMessageList CXSwiftBusSettings::validate() const
    {
        CStatusMessageList msgs;
        const QString dbus = QString::fromStdString(m_dBusServerAddress);
        if (!CDBusServer::isSessionOrSystemAddress(dbus) && !CDBusServer::isQtDBusAddress(dbus))
        {
            msgs.addValidationMessage(QStringLiteral("Invalid DBus address '%1'").arg(dbus),
                                      CStatusMessage::SeverityError);
        }
        return msgs;
    }

    const CXSwiftBusSettings &CXSwiftBusSettings::defaultValue()
    {
        static const CXSwiftBusSettings s;
        return s;
    }

    void CXSwiftBusSettings::objectUpdated() { m_timestampMSecsSinceEpoch = m_msSinceEpochQtFree; }
} // namespace swift::misc::simulation::settings
