/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_SIMULATORINFO_H
#define BLACKSIM_SIMULATORINFO_H

#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/valueobject.h"

namespace BlackSim
{
    //! Describing a simulator
    class CSimulatorInfo : public BlackMisc::CValueObjectStdTuple<CSimulatorInfo>
    {
    public:
        //! Default constructor
        CSimulatorInfo();

        //! Constructor
        CSimulatorInfo(const QString &shortname, const QString &fullname);

        //! Unspecified simulator
        bool isUnspecified() const { return this->m_shortName.isEmpty() || this->m_shortName.startsWith("Unspecified", Qt::CaseInsensitive); }

        //! Single setting value
        BlackMisc::CVariant getSimulatorSetupValue(int index) const;

        //! Single setting value
        QString getSimulatorSetupValueAsString(int index) const;

        //! Set single settings
        void setSimulatorSetup(const BlackMisc::CPropertyIndexVariantMap &setup);

        //! Short name
        const QString &getShortName() const { return m_shortName; }

        //! Short name
        const QString &getFullName() const { return m_fullName; }

        //! Compare on names only, ignore setup
        bool isSameSimulator(const CSimulatorInfo &otherSimulator) const;

        //! Simulator is FS9 - Microsoft Flight Simulator 2004
        static const CSimulatorInfo &FS9()
        {
            static CSimulatorInfo sim("FS9", "Microsoft Flight Simulator 2004");
            return sim;
        }

        //! Simulator is FSX Microsoft Flight Simulator X (2006)
        static const CSimulatorInfo &FSX()
        {
            static CSimulatorInfo sim("FSX", "Microsoft Flight Simulator X (2006)");
            return sim;
        }

        //! Simulator is XPlane, unspecified version
        static const CSimulatorInfo &XP()
        {
            static CSimulatorInfo sim("XP", "X-Plane");
            return sim;
        }

        //! Simulator is unspecified
        static const CSimulatorInfo &UnspecifiedSim()
        {
            static CSimulatorInfo sim("Unspecified", "Unspecified");
            return sim;
        }

    protected:
        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CSimulatorInfo)
        QString m_fullName;
        QString m_shortName;
        BlackMisc::CPropertyIndexVariantMap m_simsetup; //!< allows to access simulator keys requried on remote side
    };
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackSim::CSimulatorInfo, (
    o.m_fullName,
    o.m_shortName,
    attr(o.m_simsetup, flags<DisabledForComparison>())
))
Q_DECLARE_METATYPE(BlackSim::CSimulatorInfo)

#endif // guard
