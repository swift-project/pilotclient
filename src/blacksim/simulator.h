#ifndef BLACKSIM_SIMULATOR_H
#define BLACKSIM_SIMULATOR_H

#include "blackmisc/valueobject.h"

namespace BlackSim
{

    /*!
     * \brief Describing a simulator
     */
    class CSimulator : public BlackMisc::CValueObject
    {
    public:
        //! \brief Default constructor
        CSimulator();

        //! \brief Constructor
        CSimulator(const QString &shortname, const QString &fullname);

        //! \brief Unspecified simulator
        bool isUnspecified() const { return this->m_shortname.isEmpty() || this->m_shortname.startsWith("Unspecified", Qt::CaseInsensitive); }

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override
        {
            return QVariant::fromValue(*this);
        }

        //! \copydoc CValueObject::getValueHash()
        virtual uint getValueHash() const override;

        //! \brief Simulator is FS9 - Microsoft Flight Simulator 2004
        static const CSimulator &FS9()
        {
            static CSimulator sim("FS9", "Microsoft Flight Simulator 2004");
            return sim;
        }

        //! \brief Simulator is FSX Microsoft Flight Simulator X (2006)
        static const CSimulator &FSX()
        {
            static CSimulator sim("FSX", "Microsoft Flight Simulator X (2006)");
            return sim;
        }


        //! \brief Simulator is XPlane 10
        static const CSimulator &XP10()
        {
            static CSimulator sim("XP10", "XPlane 10 (2011)");
            return sim;
        }

        //! \brief Simulator is unspecified
        static const CSimulator &UnspecifiedSim()
        {
            static CSimulator sim("Unspecified", "Unspecified");
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
        virtual int compareImpl(const CValueObject &other) const override;

        //! \copydoc CValueObject::marshallToDbus()
        virtual void marshallToDbus(QDBusArgument &argument) const override;

        //! \copydoc CValueObject::unmarshallFromDbus()
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

    private:
        QString m_fullname;
        QString m_shortname;
    };
}

Q_DECLARE_METATYPE(BlackSim::CSimulator)

#endif // guard
