// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/simulatorinternals.h"

#include <QDBusMetaType>
#include <QJsonObject>

#include "misc/dbus.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation, CSimulatorInternals)

namespace swift::misc::simulation
{
    void CSimulatorInternals::setValue(const QString &name, const QString &value)
    {
        m_data.addOrReplaceValue(name, value);
    }

    void CSimulatorInternals::setValue(const QString &name, int value) { this->setValue(name, QString::number(value)); }

    CVariant CSimulatorInternals::getVariantValue(const QString &name) const { return m_data.getVariantValue(name); }

    QString CSimulatorInternals::getStringValue(const QString &name) const { return m_data.getValueAsString(name); }

    QStringList CSimulatorInternals::getSortedNames() const { return m_data.getNames(true); }

    void CSimulatorInternals::setSimulatorVersion(const QString &versionInfo)
    {
        this->setValue("all/versionInfo", versionInfo);
    }

    void CSimulatorInternals::setSimulatorInstallationDirectory(const QString &fullFilePath)
    {
        this->setValue("all/installDir", fullFilePath);
    }

    QString CSimulatorInternals::getSimulatorName() const { return this->getStringValue("all/simulatorName"); }

    void CSimulatorInternals::setSimulatorName(const QString &name) { this->setValue("all/simulatorName", name); }

    QString CSimulatorInternals::getSimulatorSwiftPluginName() const { return this->getStringValue("all/pluginName"); }

    void CSimulatorInternals::setSwiftPluginName(const QString &name) { this->setValue("all/pluginName", name); }

    QString CSimulatorInternals::getSimulatorVersion() const { return this->getStringValue("all/versionInfo"); }

    QString CSimulatorInternals::getSimulatorInstallationDirectory() const
    {
        return this->getStringValue("all/installDir");
    }

    void CSimulatorInternals::registerMetadata()
    {
        qRegisterMetaType<swift::misc::simulation::CSimulatorInternals>();
        qDBusRegisterMetaType<swift::misc::simulation::CSimulatorInternals>();
        registerMetaValueType<swift::misc::simulation::CSimulatorInternals>();
    }

    QString CSimulatorInternals::convertToQString(bool i18n) const { return m_data.toQString(i18n); }

    QVariant CSimulatorInternals::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexData: return QVariant::fromValue(m_data);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CSimulatorInternals::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CSimulatorInternals>();
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexData: m_data = variant.value<CNameVariantPairList>(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::misc::simulation
