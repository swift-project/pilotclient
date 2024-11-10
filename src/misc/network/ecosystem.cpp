// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/ecosystem.h"
#include "misc/iconlist.h"
#include "misc/verify.h"
#include "misc/comparefunctions.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CEcosystem)

namespace swift::misc::network
{
    QString CEcosystem::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return this->getSystemString();
    }

    const CEcosystem &CEcosystem::unspecified()
    {
        static const CEcosystem e(Unspecified);
        return e;
    }

    const CEcosystem &CEcosystem::vatsim()
    {
        static const CEcosystem e(VATSIM);
        return e;
    }

    const CEcosystem &CEcosystem::swift()
    {
        static const CEcosystem e(Swift);
        return e;
    }

    const CEcosystem &CEcosystem::swiftTest()
    {
        static const CEcosystem e(SwiftTest);
        return e;
    }

    const CEcosystem &CEcosystem::privateFsd()
    {
        static const CEcosystem e(PrivateFSD);
        return e;
    }

    const QString &CEcosystem::getSystemString() const
    {
        static const QString u("unknown");
        static const QString v("VATSIM");
        static const QString s("swift");
        static const QString st("swift (testing)");
        static const QString fsd("FSD (private)");
        static const QString no("no system");

        switch (this->getSystem())
        {
        case VATSIM: return v;
        case Swift: return s;
        case SwiftTest: return st;
        case PrivateFSD: return fsd;
        case NoSystem: return no;
        case Unspecified:
        default: return u;
        }
    }

    QVariant CEcosystem::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexSystem: return QVariant::fromValue(m_system);
        case IndexSystemString: return QVariant::fromValue(this->getSystemString());
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CEcosystem::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CEcosystem>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexSystem: m_system = variant.toInt(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CEcosystem::comparePropertyByIndex(CPropertyIndexRef index, const CEcosystem &compareValue) const
    {
        if (index.isMyself()) { return Compare::compare(m_system, compareValue.m_system); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexSystem: return Compare::compare(m_system, compareValue.m_system);
        default: break;
        }
        SWIFT_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable("No comparison for index " + index.toQString()));
        return 0;
    }
} // namespace
