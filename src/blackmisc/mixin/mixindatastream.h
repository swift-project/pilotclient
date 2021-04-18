/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MIXIN_MIXINDATASTREAM_H
#define BLACKMISC_MIXIN_MIXINDATASTREAM_H

#include "blackmisc/metaclass.h"
#include "blackmisc/inheritancetraits.h"
#include <QDataStream>

namespace BlackMisc
{
    class CEmpty;

    namespace Mixin
    {
        /*!
         * CRTP class template to generate non-member QDataStream streaming operators.
         *
         * \tparam Derived Must implement public methods marshalToDataStream(QDataStream &) and unmarshalFromDataStream(QDataStream &).
         */
        template <class Derived>
        class DataStreamOperators
        {
        public:
            //! Marshal a value to a QDataStream.
            friend QDataStream &operator <<(QDataStream &stream, const Derived &value)
            {
                value.marshalToDataStream(stream);
                return stream;
            }

            //! Unmarshal a value from a QDataStream.
            friend QDataStream &operator >>(QDataStream &stream, Derived &value)
            {
                value.unmarshalFromDataStream(stream);
                return stream;
            }
        };

        /*!
         * CRTP class template to generate QDataStream marshalling methods using CMetaClass.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_DATASTREAM
         */
        template <class Derived>
        class DataStreamByMetaClass : public DataStreamOperators<Derived>
        {
        public:
            //! Marshal a value to a QDataStream.
            void marshalToDataStream(QDataStream &stream) const
            {
                baseMarshal(static_cast<const TBaseOfT<Derived> *>(derived()), stream);
                introspect<Derived>().forEachMember([ &, this ](auto member)
                {
                    if constexpr (!decltype(member)::has(MetaFlags<DisabledForMarshalling>()))
                    {
                        stream << member.in(*this->derived());
                    }
                });
            }

            //! Unmarshal a value from a QDataStream.
            void unmarshalFromDataStream(QDataStream &stream)
            {
                baseUnmarshal(static_cast<TBaseOfT<Derived> *>(derived()), stream);
                introspect<Derived>().forEachMember([ &, this ](auto member)
                {
                    if constexpr (!decltype(member)::has(MetaFlags<DisabledForMarshalling>()))
                    {
                        stream >> member.in(*this->derived());
                    }
                });
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T> static void baseMarshal(const T *base, QDataStream &stream) { base->marshalToDataStream(stream); }
            template <typename T> static void baseUnmarshal(T *base, QDataStream &stream) { base->unmarshalFromDataStream(stream); }
            static void baseMarshal(const void *, QDataStream &) {}
            static void baseUnmarshal(void *, QDataStream &) {}
            static void baseMarshal(const CEmpty *, QDataStream &) {}
            static void baseUnmarshal(CEmpty *, QDataStream &) {}
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::DataStreamByMetaClass,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_DATASTREAM(DERIVED)                            \
            using ::BlackMisc::Mixin::DataStreamByMetaClass<DERIVED>::marshalToDataStream;  \
            using ::BlackMisc::Mixin::DataStreamByMetaClass<DERIVED>::unmarshalFromDataStream;
    }
}

#endif
