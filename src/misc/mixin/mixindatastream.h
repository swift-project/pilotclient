// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_MIXIN_MIXINDATASTREAM_H
#define SWIFT_MISC_MIXIN_MIXINDATASTREAM_H

#include "misc/metaclass.h"
#include "misc/inheritancetraits.h"
#include <QDataStream>

namespace swift::misc
{
    class CEmpty;

    namespace mixin
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
            friend QDataStream &operator<<(QDataStream &stream, const Derived &value)
            {
                value.marshalToDataStream(stream);
                return stream;
            }

            //! Unmarshal a value from a QDataStream.
            friend QDataStream &operator>>(QDataStream &stream, Derived &value)
            {
                value.unmarshalFromDataStream(stream);
                return stream;
            }
        };

        /*!
         * CRTP class template to generate QDataStream marshalling methods using CMetaClass.
         *
         * \see SWIFT_MISC_DECLARE_USING_MIXIN_DATASTREAM
         */
        template <class Derived>
        class DataStreamByMetaClass : public DataStreamOperators<Derived>
        {
        public:
            //! Marshal a value to a QDataStream.
            void marshalToDataStream(QDataStream &stream) const;

            //! Unmarshal a value from a QDataStream.
            void unmarshalFromDataStream(QDataStream &stream);

        private:
            const Derived *derived() const;
            Derived *derived();

            template <typename T>
            static void baseMarshal(const T *base, QDataStream &stream);
            template <typename T>
            static void baseUnmarshal(T *base, QDataStream &stream);
            static void baseMarshal(const void *, QDataStream &);
            static void baseUnmarshal(void *, QDataStream &);
            static void baseMarshal(const CEmpty *, QDataStream &);
            static void baseUnmarshal(CEmpty *, QDataStream &);
        };

        template <class Derived>
        void DataStreamByMetaClass<Derived>::marshalToDataStream(QDataStream &stream) const
        {
            baseMarshal(static_cast<const TBaseOfT<Derived> *>(derived()), stream);
            introspect<Derived>().forEachMember([&, this](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForMarshalling>()))
                {
                    stream << member.in(*this->derived());
                }
            });
        }

        template <class Derived>
        void DataStreamByMetaClass<Derived>::unmarshalFromDataStream(QDataStream &stream)
        {
            baseUnmarshal(static_cast<TBaseOfT<Derived> *>(derived()), stream);
            introspect<Derived>().forEachMember([&, this](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForMarshalling>()))
                {
                    stream >> member.in(*this->derived());
                }
            });
        }

        template <class Derived>
        const Derived *DataStreamByMetaClass<Derived>::derived() const
        {
            return static_cast<const Derived *>(this);
        }

        template <class Derived>
        Derived *DataStreamByMetaClass<Derived>::derived()
        {
            return static_cast<Derived *>(this);
        }

        template <class Derived>
        template <typename T>
        void DataStreamByMetaClass<Derived>::baseMarshal(const T *base, QDataStream &stream)
        {
            base->marshalToDataStream(stream);
        }

        template <class Derived>
        template <typename T>
        void DataStreamByMetaClass<Derived>::baseUnmarshal(T *base, QDataStream &stream)
        {
            base->unmarshalFromDataStream(stream);
        }

        template <class Derived>
        void DataStreamByMetaClass<Derived>::baseMarshal(const void *, QDataStream &)
        {}

        template <class Derived>
        void DataStreamByMetaClass<Derived>::baseUnmarshal(void *, QDataStream &)
        {}

        template <class Derived>
        void DataStreamByMetaClass<Derived>::baseMarshal(const CEmpty *, QDataStream &)
        {}

        template <class Derived>
        void DataStreamByMetaClass<Derived>::baseUnmarshal(CEmpty *, QDataStream &)
        {}

        /*!
         * When a derived class and a base class both inherit from mixin::DataStreamByMetaClass,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#define SWIFT_MISC_DECLARE_USING_MIXIN_DATASTREAM(DERIVED)                          \
    using ::swift::misc::mixin::DataStreamByMetaClass<DERIVED>::marshalToDataStream; \
    using ::swift::misc::mixin::DataStreamByMetaClass<DERIVED>::unmarshalFromDataStream;
    }
}

#endif
