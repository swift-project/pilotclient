// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/vatsim/vatsimsettings.h"

using namespace swift::misc;
using namespace swift::misc::physical_quantities;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::core::vatsim, CReaderSettings)
SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::core::vatsim, CRawFsdMessageSettings)

namespace swift::core::vatsim
{
    CReaderSettings::CReaderSettings(const CTime &initialTime, const CTime &periodicTime, bool neverUpdate)
        : m_initialTime(initialTime), m_periodicTime(periodicTime), m_neverUpdate(neverUpdate)
    {}

    QString CReaderSettings::convertToQString(bool i18n) const
    {
        QString s("CReaderSettings");
        s.append(" ").append(this->m_initialTime.convertToQString(i18n));
        s.append(" ").append(this->m_periodicTime.convertToQString(i18n));
        return s;
    }

    const CReaderSettings &CReaderSettings::neverUpdateSettings()
    {
        static const CReaderSettings s(CTime { 1.0, CTimeUnit::d() }, CTime { 1.0, CTimeUnit::d() }, true);
        return s;
    }

    QVariant CReaderSettings::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexInitialTime: return this->m_initialTime.propertyByIndex(index.copyFrontRemoved());
        case IndexPeriodicTime: return this->m_periodicTime.propertyByIndex(index.copyFrontRemoved());
        case IndexNeverUpdate: return QVariant::fromValue(this->m_neverUpdate);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CReaderSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CReaderSettings>();
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexInitialTime: this->m_initialTime.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexPeriodicTime: this->m_periodicTime.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexNeverUpdate: this->m_neverUpdate = variant.toBool(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    CRawFsdMessageSettings::CRawFsdMessageSettings(bool enabled, const QString &FileDir)
        : m_rawFsdMessagesEnabled(enabled), m_FileDir(FileDir)
    {}

    QString CRawFsdMessageSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s("CRawFsdMessageSettings");
        s.append(" enabled: ").append(boolToYesNo(m_rawFsdMessagesEnabled));
        s.append(" dir: ").append(m_FileDir);
        return s;
    }

    QVariant CRawFsdMessageSettings::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexRawFsdMessagesEnabled: return QVariant::fromValue(this->m_rawFsdMessagesEnabled);
        case IndexFileDir: return QVariant::fromValue(this->m_FileDir);
        case IndexFileWriteMode: return QVariant::fromValue(this->m_fileWriteMode);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CRawFsdMessageSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CRawFsdMessageSettings>();
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexRawFsdMessagesEnabled: this->m_rawFsdMessagesEnabled = variant.toBool(); break;
        case IndexFileDir: this->m_FileDir = variant.toString(); break;
        case IndexFileWriteMode: this->m_fileWriteMode = variant.value<FileWriteMode>(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::core::vatsim
