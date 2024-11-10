// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/callsign.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/units.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/registermetadata.h"
#include "misc/variantlist.h"
#include "misc/variant.h"
#include "test.h"

#include <QDBusArgument>
#include <QObject>
#include <QDateTime>
#include <QTest>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;

namespace MiscTest
{
    //! Variant and map related tests
    class CTestVariantAndMap : public QObject
    {
        Q_OBJECT

    private slots:
        //! Init test case data
        void initTestCase();

        //! Basic unit tests for value objects and variants
        void variant();

        //! Unit tests for variant lists
        void variantList();

        //! Unit tests for matches feature
        void matches();

        //! Unit tests for value maps and value objects
        void valueMap();
    };

    //! \private
    class CTestMatcher : public swift::misc::mixin::MetaType<CTestMatcher>
    {
    public:
        bool matches(const swift::misc::CVariant &) const { return true; }

        // methods needed for metatype registration
        QString toQString(bool) const { return {}; }
        friend QDBusArgument &operator<<(QDBusArgument &arg, const CTestMatcher &) { return arg; }
        friend const QDBusArgument &operator>>(const QDBusArgument &arg, CTestMatcher &) { return arg; }
        friend QDataStream &operator<<(QDataStream &ds, const CTestMatcher &) { return ds; }
        friend QDataStream &operator>>(QDataStream &ds, CTestMatcher &) { return ds; }
    };
} // namespace

Q_DECLARE_METATYPE(MiscTest::CTestMatcher)

namespace MiscTest
{
    void CTestVariantAndMap::initTestCase()
    {
        swift::misc::registerMetadata();
    }

    void CTestVariantAndMap::variant()
    {
        // ATC station
        CCoordinateGeodetic geoPos =
            CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
        CAtcStation station1(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                             CFrequency(118.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(50, CLengthUnit::km()), false);
        CAtcStation station2(station1);
        CAtcStation station3(CCallsign("eddm_app"), CUser("654321", "Jen Doe"),
                             CFrequency(120.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(100, CLengthUnit::km()), false);

        // compare
        CLength l1(0, nullptr);
        CLength l2(0, nullptr);
        QVERIFY2(l1 == l2, "Null lengths should be equal");

        CLength l3(0, CLengthUnit::m());
        CLength l4(-1, CLengthUnit::m());
        QVERIFY2(l1 != l3, "Null length and non-null length should not be equal");
        QVERIFY2(l1 != l4, "Null length and non-null length should not be equal");
        QVERIFY2((l1 < l4) != (l1 > l4), "Null length and non-null length should be comparable");
        QVERIFY2(compare(l1, l4) != 0, "Null length and non-null length should be sortable");

        CVariant station1qv = CVariant::fromValue(station1);
        QVERIFY2(station1 == station1, "Station should be equal");

        QVERIFY(station1.getController() == station2.getController());
        QVERIFY(station1.getRelativeDistance() == station2.getRelativeDistance());

        QVERIFY2(station1 == station2, "Station should be equal");
        QVERIFY2(station1 != station3, "Station should not be equal");
        QVERIFY2(station1qv == CVariant::from(station1), "Station should be equal (CVariant)");
        QVERIFY2(CVariant::from(station1) == station1qv, "Station should be equal (CVariant)");
        QVERIFY2(CVariant::from(station2) == station1qv, "Station should be equal (CVariant)");
        QVERIFY2(CVariant::from(station3) != station1qv, "Station should be equal (CVariant)");

        QVERIFY2(compare(station1, station1) == 0, "Station should be equal");
        QVERIFY2(compare(station1, station2) == 0, "Station should be equal");
        QVERIFY2(compare(station1, station3) != 0, "Station should not be equal");
    }

    void CTestVariantAndMap::variantList()
    {
        const CSequence<int> ints { 1, 2, 3 };
        CVariant variant = CVariant::from(ints);
        QVERIFY2(variant.canConvert<CVariantList>(), "Variant containing list can convert to CVariantList");
        QVERIFY2(variant.convert(qMetaTypeId<CVariantList>()), "Variant containing list can convert to CVariantList");
        const CVariantList variantInts = variant.to<CVariantList>();
        QVERIFY2(ints.size() == variantInts.size(), "Variant list has same size as original list");
        QVERIFY2(ints[0] == variantInts[0].to<int>(), "Variant list has same element");
        QVERIFY2(variant.canConvert<CSequence<int>>(), "Variant containing can convert back");
        QVERIFY2(ints == variant.to<CSequence<int>>(), "Variant list converted back compares equal");

        const CAirlineIcaoCodeList list { CAirlineIcaoCode("BAW"), CAirlineIcaoCode("DLH"), CAirlineIcaoCode("AAL") };
        variant = CVariant::from(list);
        QVERIFY2(variant.canConvert<CVariantList>(), "Variant containing list can convert to CVariantList");
        QVERIFY2(variant.convert(qMetaTypeId<CVariantList>()), "Variant containing list can convert to CVariantList");
        CVariantList variantList = variant.to<CVariantList>();
        QVERIFY2(list.size() == variantList.size(), "Variant list has same size as original list");
        QVERIFY2(list[0] == variantList[0].to<CAirlineIcaoCode>(), "Variant list has same element");
        QVERIFY2(variant.canConvert<CAirlineIcaoCodeList>(), "Variant containing can convert back");
        QVERIFY2(list == variant.to<CAirlineIcaoCodeList>(), "Variant list converted back compares equal");
    }

    void CTestVariantAndMap::matches()
    {
        CTestMatcher::registerMetadata();
        const CTestMatcher matcher {};
        const CVariant variant = CVariant::from(matcher);
        QVERIFY2(variant.matches(CVariant()), "Variant provides access to stored object's matches() method");
    }

    void CTestVariantAndMap::valueMap()
    {
        // ATC station
        CCoordinateGeodetic geoPos =
            CCoordinateGeodetic::fromWgs84("48° 21′ 13″ N", "11° 47′ 09″ E", { 1487, CLengthUnit::ft() });
        CAtcStation station1(CCallsign("eddm_twr"), CUser("123456", "Joe Doe"),
                             CFrequency(118.7, CFrequencyUnit::MHz()),
                             geoPos, CLength(50, CLengthUnit::km()), false);

        // value maps
        CPropertyIndexVariantMap vmWildcard(true);
        CPropertyIndexVariantMap vmNoWildcard(false);
        CPropertyIndexVariantMap vm;
        CPropertyIndexVariantMap vmCopy(vmWildcard);

        // Remark: Shortcoming here, as the callsign will be automatically set for user in station
        // I have to set this as well, otherwise, no match.
        vm.addValue(CAtcStation::IndexController, CUser("123456", "Joe Doe", CCallsign("EDDM_TWR")));

        // compare

        QVERIFY2(vmWildcard.matches(station1), "Station should be equal to wildcard");
        QVERIFY2(!vmNoWildcard.matches(station1), "Station should not be equal to empty list");
        QVERIFY2(vm.matches(station1), "Controller should match");
        QVERIFY2(vmWildcard == vmCopy, "Maps should be equal");
        QVERIFY2(qHash(vmWildcard) == qHash(vmCopy), "Hashs should be equal (simple)");

        vm.addValue(CAtcStation::IndexFrequency, CFrequency(118.7, CFrequencyUnit::MHz()));
        vm.addValue(CAtcStation::IndexPosition, geoPos);
        vmCopy = vm;
        QVERIFY2(qHash(vm) == qHash(vmCopy), "Hashs should be equal (detailed)");
        vmCopy.setWildcard(!vm.isWildcard());
        QVERIFY2(qHash(vm) != qHash(vmCopy), "Hashs should not be equal (detailed)");
    }
} // namespace

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CTestVariantAndMap);

#include "testvariantandmap.moc"

//! \endcond
