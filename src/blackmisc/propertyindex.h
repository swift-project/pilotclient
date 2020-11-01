/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROPERTYINDEX_H
#define BLACKMISC_PROPERTYINDEX_H

#include "blackmisc/propertyindexref.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/mixin/mixindatastream.h"
#include "blackmisc/mixin/mixinhash.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/typetraits.h"
#include "blackmisc/mixin/mixinmetatype.h"

#include <QList>
#include <QMetaType>
#include <QString>
#include <initializer_list>
#include <type_traits>

namespace BlackMisc
{
    class CPropertyIndex;

    namespace Private
    {
        //! \private
        template <class T, class X>
        int compareByProperty(const T &a, const T &b, const CPropertyIndex &index, std::true_type, X)
        {
            return a.comparePropertyByIndex(index, b);
        }
        //! \private
        template <class T>
        int compareByProperty(const T &a, const T &b, const CPropertyIndex &index, std::false_type, std::true_type)
        {
            return compare(a.propertyByIndex(index), b.propertyByIndex(index));
        }
        //! \private
        template <class T>
        int compareByProperty(const T &, const T &, const CPropertyIndex &, std::false_type, std::false_type)
        {
            qFatal("Not implemented");
            return 0;
        }
    }

    /*!
     * Property index. The index can be nested, that's why it is a sequence
     * (e.g. PropertyIndexPilot, PropertyIndexRealname).
     */
    class BLACKMISC_EXPORT CPropertyIndex :
        public Mixin::MetaType<CPropertyIndex>,
        public Mixin::HashByMetaClass<CPropertyIndex>,
        public Mixin::DBusByMetaClass<CPropertyIndex>,
        public Mixin::DataStreamByMetaClass<CPropertyIndex>,
        public Mixin::JsonOperators<CPropertyIndex>,
        public Mixin::EqualsByMetaClass<CPropertyIndex>,
        public Mixin::LessThanByMetaClass<CPropertyIndex>,
        public Mixin::CompareByMetaClass<CPropertyIndex>,
        public Mixin::String<CPropertyIndex>
    {
        // In the first trial I have used CSequence<int> as base class. This has created too much circular dependencies of the headers
        // CIndexVariantMap is used in CValueObject, CPropertyIndex in CIndexVariantMap

    public:
        //! Global index, make sure the indexes are unqiue (for using them in class hierarchy)
        enum GlobalIndex
        {
            GlobalIndexCValueObject                     =    10,
            GlobalIndexCPhysicalQuantity                =   100,
            GlobalIndexCStatusMessage                   =   200,
            GlobalIndexCNameVariantPair                 =   300,
            GlobalIndexITimestampBased                  =   400,
            GlobalIndexIOrderable                       =   500,
            GlobalIndexINullable                        =   600,
            GlobalIndexCIdentifier                      =   700,
            GlobalIndexCRgbColor                        =   800,
            GlobalIndexCCountry                         =   900,
            GlobalIndexCPlatform                        =  1000,
            GlobalIndexCApplicationInfo                 =  1100,
            GlobalIndexCDirectories                     =  1200,
            GlobalIndexCCrashInfo                       =  1300,
            GlobalIndexCCrashSettings                   =  1400,
            GlobalIndexCCallsign                        =  2000,
            GlobalIndexCAircraftSituation               =  2100,
            GlobalIndexCAircraftSituationChange         =  2200,
            GlobalIndexCAtcStation                      =  2300,
            GlobalIndexCInformationMessage              =  2400,
            GlobalIndexCAirport                         =  2500,
            GlobalIndexCAircraftParts                   =  2600,
            GlobalIndexCAircraftLights                  =  2700,
            GlobalIndexCLivery                          =  2800,
            GlobalIndexCFlightPlan                      =  2900,
            GlobalIndexCSimBriefData                    =  3000,
            GlobalIndexCComSystem                       =  3100,
            GlobalIndexCModulator                       =  3200,
            GlobalIndexCTransponder                     =  3300,
            GlobalIndexCAircraftCategory                =  3500,
            GlobalIndexCAircraftIcaoCode                =  3600,
            GlobalIndexCAirlineIcaoCode                 =  3700,
            GlobalIndexCAirportIcaoCode                 =  3800,
            GlobalIndexCMetar                           =  4000,
            GlobalIndexCCloudLayer                      =  4100,
            GlobalIndexCPresentWeather                  =  4200,
            GlobalIndexCWindLayer                       =  4300,
            GlobalIndexCTemperatureLayer                =  4400,
            GlobalIndexCGridPoint                       =  4500,
            GlobalIndexCVisibilityLayer                 =  4600,
            GlobalIndexCWeatherScenario                 =  4700,
            GlobalIndexICoordinateGeodetic              =  5000,
            GlobalIndexICoordinateWithRelativePosition  =  5100,
            GlobalIndexCCoordinateGeodetic              =  5200,
            GlobalIndexCElevationPlane                  =  5300,
            GlobalIndexCClient                          =  6000,
            GlobalIndexCUser                            =  6100,
            GlobalIndexCAuthenticatedUser               =  6200,
            GlobalIndexCRole                            =  6300,
            GlobalIndexCServer                          =  6400,
            GlobalIndexCFsdSetup                        =  6500,
            GlobalIndexCVoiceSetup                      =  6600,
            GlobalIndexCNetworkSettings                 =  6700,
            GlobalIndexCUrl                             =  6800,
            GlobalIndexCUrlLog                          =  6900,
            GlobalIndexCRemoteFile                      =  7000,
            GlobalIndexCEcosystem                       =  7100,
            GlobalIndexCRawFsdMessage                   =  7200,
            GlobalIndexCAircraftModel                   =  8000,
            GlobalIndexCSimulatedAircraft               =  8100,
            GlobalIndexCTextMessage                     =  8200,
            GlobalIndexCSimulatorInternals              =  8300,
            GlobalIndexCSimulatorSettings               =  8400,
            GlobalIndexCSwiftPluignSettings             =  8500,
            GlobalIndexCSimulatorMessageSettings        =  8600,
            GlobalIndexCModelSettings                   =  8700,
            GlobalIndexCAircraftCfgEntries              =  8800,
            GlobalIndexCDistributor                     =  8900,
            GlobalIndexCVPilotModelRule                 =  9000,
            GlobalIndexCAudioDeviceInfo                 = 10000,
            GlobalIndexCSettingKeyboardHotkey           = 11000,
            GlobalIndexCKeyboardKey                     = 11100,
            GlobalIndexCJoystickButton                  = 11200,
            GlobalIndexIDatastore                       = 12000,
            GlobalIndexIDatastoreInteger                = 12100,
            GlobalIndexIDatastoreString                 = 12200,
            GlobalIndexCDbInfo                          = 12300,
            GlobalIndexCGlobalSetup                     = 13000,
            GlobalIndexCArtifact                        = 13100,
            GlobalIndexCDistribution                    = 13200,
            GlobalIndexCUpdateInfo                      = 13300,
            GlobalIndexCVatsimSetup                     = 13400,
            GlobalIndexCLauncherSetup                   = 13500,
            GlobalIndexCGuiStateDbOwnModelsComponent    = 14000,
            GlobalIndexCGuiStateDbOwnModelSetComponent  = 14100,
            GlobalIndexCDockWidgetSettings              = 14200,
            GlobalIndexCNavigatorSettings               = 14300,
            GlobalIndexCSettingsReaders                 = 14400,
            GlobalIndexCViewUpdateSettings              = 14500,
            GlobalIndexCGeneralGuiSettings              = 14600,
            GlobalIndexCTextMessageSettings             = 14700,
            GlobalIndexCAtcStationsSettings             = 14800,
            GlobalIndexRawFsdMessageSettings            = 14900,
            GlobalIndexCInterpolatioRenderingSetup      = 16000,
            GlobalIndexCMatchingStatisticsEntry         = 16100,
            GlobalIndexCAircraftMatcherSetup            = 16200,
            GlobalIndexCXSwiftBusSettings               = 16300,
            GlobalIndexSwiftPilotClient                 = 17000,
            GlobalIndexSwiftCore                        = 17100,
            GlobalIndexSwiftLauncher                    = 17200,
            GlobalIndexLineNumber                       = 20000, //!< pseudo index for line numbers
            GlobalIndexEmpty                            = 20001
        };

        //! Default constructor.
        CPropertyIndex() = default;

        //! Non nested index
        CPropertyIndex(int singleProperty);

        //! Initializer list constructor
        CPropertyIndex(std::initializer_list<int> il);

        //! Construct from a vector of indexes.
        CPropertyIndex(const QVector<int> &indexes);

        //! Construct from a list of indexes.
        CPropertyIndex(const QList<int> &indexes);

        //! From string
        CPropertyIndex(const QString &indexes);

        //! Return a simplified non-owning reference
        operator CPropertyIndexRef() const;

        //! Copy with first element removed
        CPropertyIndex copyFrontRemoved() const;

        //! Is nested index?
        bool isNested() const;

        //! Myself index, used with nesting
        bool isMyself() const;

        //! Empty?
        bool isEmpty() const;

        //! Index vector
        QVector<int> indexVector() const;

        //! Index list
        QList<int> indexList() const;

        //! Shift existing indexes to right and insert given index at front
        void prepend(int newLeftIndex);

        //! Contains index?
        bool contains(int index) const;

        //! Compare with index given by enum
        template<class EnumType> bool contains(EnumType ev) const
        {
            static_assert(std::is_enum<EnumType>::value, "Argument must be an enum");
            return this->contains(static_cast<int>(ev));
        }

        //! Front to integer
        int frontToInt() const;

        //! Starts with given index?
        bool startsWith(int index) const;

        //! First element casted to given type, usually the PropertIndex enum
        template<class CastType> CastType frontCasted() const
        {
            static_assert(std::is_enum<CastType>::value || std::is_integral<CastType>::value, "CastType must be an enum or integer");
            return static_cast<CastType>(frontToInt());
        }

        //! Compare with index given by enum
        template<class EnumType> bool startsWithPropertyIndexEnum(EnumType ev) const
        {
            static_assert(std::is_enum<EnumType>::value, "Argument must be an enum");
            return this->startsWith(static_cast<int>(ev));
        }

        //! Return a predicate function which can compare two objects based on this index
        auto comparator() const
        {
            return [index = *this](const auto & a, const auto & b)
            {
                using T = std::decay_t<decltype(a)>;
                return Private::compareByProperty(a, b, index, THasComparePropertyByIndex<T>(), THasPropertyByIndex<T>());
            };
        }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::CValueObject::toJson
        QJsonObject toJson() const;

        //! \copydoc BlackMisc::CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! an empty property index
        static const CPropertyIndex &empty() { static const CPropertyIndex pi; return pi; }

    protected:
        //! Parse indexes from string
        void parseFromString(const QString &indexes);

    private:
        QVector<int> m_indexes;

        BLACK_METACLASS(
            CPropertyIndex,
            BLACK_METAMEMBER(indexes)
        );
    };
} //namespace

Q_DECLARE_METATYPE(BlackMisc::CPropertyIndex)

#endif //guard
