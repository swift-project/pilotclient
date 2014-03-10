#include "blackmisc/coordinatened.h"

using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace Geo
    {

        bool CCoordinateNed::operator ==(const CCoordinateNed &other) const
        {
            if (this == &other) return true;
            if (!CVector3DBase::operator ==(other)) return false;
            return TupleConverter<CCoordinateNed>::toTuple(*this) == TupleConverter<CCoordinateNed>::toTuple(other);
        }

        /*
         * Marshall
         */
        void CCoordinateNed::marshallToDbus(QDBusArgument &argument) const
        {
            CVector3DBase::marshallToDbus(argument);
            argument << TupleConverter<CCoordinateNed>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CCoordinateNed::unmarshallFromDbus(const QDBusArgument &argument)
        {
            CVector3DBase::unmarshallFromDbus(argument);
            argument >> TupleConverter<CCoordinateNed>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CCoordinateNed::getValueHash() const
        {
            QList<uint> hashs;
            hashs << CVector3DBase::getValueHash();
            hashs << qHash(TupleConverter<CCoordinateNed>::toTuple(*this));
            return BlackMisc::calculateHash(hashs, "CCoordinateNed");
        }

        /*
         * Compare
         */
        int CCoordinateNed::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CCoordinateNed &>(otherBase);
            int result = compare(TupleConverter<CCoordinateNed>::toTuple(*this), TupleConverter<CCoordinateNed>::toTuple(other));
            return result == 0 ? CVector3DBase::compareImpl(otherBase) : result;
        }
    }
}
