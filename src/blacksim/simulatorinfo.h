#ifndef BLACKSIM_SIMULATORINFO_H
#define BLACKSIM_SIMULATORINFO_H

#include "blackmisc/indexvariantmap.h"
#include "blackmisc/valueobject.h"

namespace BlackSim
{
    //! \brief Describing a simulator
    class CSimulatorInfo : public BlackMisc::CValueObject
    {
    public:
        //! \brief Default constructor
        CSimulatorInfo();

        //! \brief Constructor
        CSimulatorInfo(const QString &shortname, const QString &fullname);

        //! \brief Unspecified simulator
        bool isUnspecified() const { return this->m_shortname.isEmpty() || this->m_shortname.startsWith("Unspecified", Qt::CaseInsensitive); }

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override
        {
            return QVariant::fromValue(*this);
        }

        //! \brief Equal operator ==
        bool operator ==(const CSimulatorInfo &other) const;

        //! \brief Unequal operator !=
        bool operator !=(const CSimulatorInfo &other) const;

        //! \copydoc CValueObject::getValueHash()
        virtual uint getValueHash() const override;

        //! Single setting value
        QVariant getSimulatorSetupValue(int index) const;

        //! Single setting value
        QString getSimulatorSetupValueAsString(int index) const;

        //! Set single settings
        void setSimulatorSetup(const BlackMisc::CIndexVariantMap &setup);

        //! \brief Simulator is FS9 - Microsoft Flight Simulator 2004
        static const CSimulatorInfo &FS9()
        {
            static CSimulatorInfo sim("FS9", "Microsoft Flight Simulator 2004");
            return sim;
        }

        //! \brief Simulator is FSX Microsoft Flight Simulator X (2006)
        static const CSimulatorInfo &FSX()
        {
            static CSimulatorInfo sim("FSX", "Microsoft Flight Simulator X (2006)");
            return sim;
        }

        //! \brief Simulator is XPlane, unspecified version
        static const CSimulatorInfo &XP()
        {
            static CSimulatorInfo sim("XP", "X-Plane");
            return sim;
        }

        //! \brief Simulator is unspecified
        static const CSimulatorInfo &UnspecifiedSim()
        {
            static CSimulatorInfo sim("Unspecified", "Unspecified");
            return sim;
        }

        //! \brief Register the metatypes
        static void registerMetadata();

    protected:
        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

        //! \copydoc CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override;

        //! \copydoc CValueObject::compareImpl
        virtual int compareImpl(const CValueObject &otherBase) const override;

        //! \copydoc CValueObject::marshallToDbus()
        virtual void marshallToDbus(QDBusArgument &argument) const override;

        //! \copydoc CValueObject::unmarshallFromDbus()
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CSimulatorInfo)
        QString m_fullname;
        QString m_shortname;
        BlackMisc::CIndexVariantMap m_simsetup; //!< allows to access simulator keys requried on remote side
    };
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackSim::CSimulatorInfo, (o.m_fullname, o.m_shortname, o.m_simsetup))
Q_DECLARE_METATYPE(BlackSim::CSimulatorInfo)

#endif // guard
