#include "aviobase.h"

namespace BlackMisc
{
    namespace Aviation
    {
        bool CAvionicsBase::operator ==(const CAvionicsBase &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAvionicsBase>::toTuple(*this) == TupleConverter<CAvionicsBase>::toTuple(other);
        }

        int CAvionicsBase::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAvionicsBase &>(otherBase);
            return compare(TupleConverter<CAvionicsBase>::toTuple(*this), TupleConverter<CAvionicsBase>::toTuple(other));
        }

        void CAvionicsBase::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAvionicsBase>::toTuple(*this);
        }

        void CAvionicsBase::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAvionicsBase>::toTuple(*this);
        }

        uint CAvionicsBase::getValueHash() const
        {
            return qHash(TupleConverter<CAvionicsBase>::toTuple(*this));
        }
    }
}
