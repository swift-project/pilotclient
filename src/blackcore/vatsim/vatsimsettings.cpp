/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "vatsimsettings.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{
    namespace Vatsim
    {
        CReaderSettings::CReaderSettings()
        { }

        CReaderSettings::CReaderSettings(const CTime &initialTime, const CTime &periodicTime, bool neverUpdate) :
            m_initialTime(initialTime), m_periodicTime(periodicTime), m_neverUpdate(neverUpdate)
        { }

        QString CReaderSettings::convertToQString(bool i18n) const
        {
            QString s("CReaderSettings");
            s.append(" ").append(this->m_initialTime.convertToQString(i18n));
            s.append(" ").append(this->m_periodicTime.convertToQString(i18n));
            return s;
        }

        const CReaderSettings &CReaderSettings::neverUpdateSettings()
        {
            static const CReaderSettings s(CTime{ 1.0, CTimeUnit::d()}, CTime{ 1.0, CTimeUnit::d()}, true);
            return s;
        }

        CVariant CReaderSettings::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexInitialTime:
                return this->m_initialTime.propertyByIndex(index.copyFrontRemoved());
            case IndexPeriodicTime:
                return this->m_periodicTime.propertyByIndex(index.copyFrontRemoved());
            case IndexNeverUpdate:
                return CVariant::fromValue(this->m_neverUpdate);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CReaderSettings::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CReaderSettings>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexInitialTime:
                this->m_initialTime.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexPeriodicTime:
                this->m_periodicTime.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexNeverUpdate:
                this->m_neverUpdate = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        CRawFsdMessageSettings::CRawFsdMessageSettings()
        { }

        CRawFsdMessageSettings::CRawFsdMessageSettings(bool enabled, const QString &FileDir) :
            m_fileWritingEnabled(enabled), m_FileDir(FileDir)
        { }

        QString CRawFsdMessageSettings::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s("CRawFsdMessageSettings");
            s.append(" enabled: ").append(boolToYesNo(m_fileWritingEnabled));
            s.append(" dir: ").append(m_FileDir);
            return s;
        }

        CVariant CRawFsdMessageSettings::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexWriteEnabled: return CVariant::fromValue(this->m_fileWritingEnabled);
            case IndexFileDir: return CVariant::fromValue(this->m_FileDir);
            case IndexFileWriteMode: return CVariant::fromValue(this->m_fileWriteMode);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CRawFsdMessageSettings::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CRawFsdMessageSettings>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexWriteEnabled: this->m_fileWritingEnabled = variant.toBool(); break;
            case IndexFileDir: this->m_FileDir = variant.toQString(); break;
            case IndexFileWriteMode: this->m_fileWriteMode = variant.to<FileWriteMode>(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }
    } // ns
} // ns
