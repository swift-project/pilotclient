// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_DATAREFS_H
#define SWIFT_SIM_XSWIFTBUS_DATAREFS_H

//! \file

#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMUtilities.h>

#include <array>
#include <cassert>
#include <string>

// Avoid checking large auto-generated header with cppcheck
#ifndef CPPCHECK
#    include "datarefs.inc"
#endif

namespace XSwiftBus
{

    //! \private
    class DataRefImpl
    {
    public:
        DataRefImpl(char const *name) : m_ref(XPLMFindDataRef(name))
        {
            if (!m_ref)
            {
                XPLMDebugString("Missing dataref:");
                XPLMDebugString(name);
                XPLMDebugString("\n");
            }
        }

        bool isValid() const { return m_ref; }

        template <typename T>
        void implSet(T);

        template <typename T>
        T implGet() const;

    private:
        XPLMDataRef m_ref;
    };

    //! \private
    class ArrayDataRefImpl
    {
    public:
        ArrayDataRefImpl(char const *name, int size) : m_ref(XPLMFindDataRef(name)), m_size(size)
        {
            if (!m_ref)
            {
                XPLMDebugString("Missing dataref:");
                XPLMDebugString(name);
                XPLMDebugString("\n");
            }
        }

        bool isValid() const { return m_ref; }

        template <typename T>
        void implSetAll(T *const);

        template <typename T>
        void implGetAll(T *) const;

        template <typename T>
        void implSetAt(int index, T);

        template <typename T>
        T implGetAt(int index) const;

    private:
        XPLMDataRef m_ref;
        int const m_size;
    };

    /*!
     * Class providing access to a single X-Plane dataref
     * \tparam DataRefTraits The trait class representing the dataref.
     * See the xplane::data namespace and http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html
     */
    template <class DataRefTraits>
    class DataRef : private DataRefImpl
    {
        static_assert(!DataRefTraits::is_array, "this is an array dataref");

    public:
        //! Constructor
        DataRef() : DataRefImpl(DataRefTraits::name()) {}

        //! Traits type
        using TraitsType = DataRefTraits;

        //! Dataref type
        using DataRefType = typename DataRefTraits::type;

        //! Set the value of the dataref (if it is writable)
        void set(DataRefType d)
        {
            static_assert(DataRefTraits::writable, "read-only dataref");
            DataRefImpl::implSet(d);
        }

        //! Set as integer, avoids cast warnings such as "possible loss of data"
        void setAsInt(int d) { this->set(static_cast<DataRefType>(d)); }

        //! Set as integer, avoids cast warnings such as "possible loss of data"
        void setAsDouble(double d) { this->set(static_cast<DataRefType>(d)); }

        //! Get the value of the dataref
        DataRefType get() const { return DataRefImpl::implGet<DataRefType>(); }

        using DataRefImpl::isValid;
    };

    /*!
     * Class providing access to a single X-Plane array dataref
     * \tparam DataRefTraits The trait class representing the dataref.
     * See the xplane::data namespace and http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html
     */
    template <class DataRefTraits>
    class ArrayDataRef : private ArrayDataRefImpl
    {
        static_assert(DataRefTraits::is_array, "not an array dataref");

    public:
        //! Constructor
        ArrayDataRef() : ArrayDataRefImpl(DataRefTraits::name(), DataRefTraits::size) {}

        //! Traits type
        using TraitsType = DataRefTraits;

        //! Dataref type
        using DataRefType = typename DataRefTraits::type;

        //! Array dataref size
        static constexpr auto DataRefSize = DataRefTraits::size;

        //! Set the value of the whole array (if it is writable)
        void setAll(std::array<DataRefType, DataRefSize> const &a)
        {
            static_assert(DataRefTraits::writable, "read-only dataref");
            ArrayDataRefImpl::implSetAll<DataRefType>(a.data());
        }

        //! Get the value of the whole array
        std::array<DataRefType, DataRefSize> getAll() const
        {
            std::array<DataRefType, DataRefSize> result;
            ArrayDataRefImpl::implGetAll<DataRefType>(result.data());
            return result;
        }

        //! Set the value of a single element (if it is writable)
        void setAt(int index, DataRefType d)
        {
            static_assert(DataRefTraits::writable, "read-only dataref");
            ArrayDataRefImpl::implSetAt(index, d);
        }

        //! Get the value of a single element
        DataRefType getAt(int index) const { return ArrayDataRefImpl::implGetAt<DataRefType>(index); }

        using ArrayDataRefImpl::isValid;
    };

    /*!
     * Class providing access to a single X-Plane string dataref
     * \tparam DataRefTraits The trait class representing the dataref.
     * See the xplane::data namespace and http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html
     */
    template <class DataRefTraits>
    class StringDataRef
    {
        static_assert(DataRefTraits::is_array, "not an array dataref");

    public:
        //! Constructor
        StringDataRef() : m_ref(XPLMFindDataRef(DataRefTraits::name()))
        {
            if (!m_ref)
            {
                XPLMDebugString("Missing dataref:");
                XPLMDebugString(DataRefTraits::name());
                XPLMDebugString("\n");
            }
        }

        //! True if the dataref exists
        bool isValid() const { return m_ref; }

        //! Set the value of the whole string (if it is writable)
        void set(std::string const &s) { setSubstr(0, s); }

        //! Get the value of the whole string
        std::string get() const { return getSubstr(0, DataRefTraits::size); }

        //! Set the value of part of the string (if it is writable)
        void setSubstr(size_t offset, std::string const &s)
        {
            static_assert(DataRefTraits::writable, "read-only dataref");
            assert((s.size() + 1) <= (DataRefTraits::size - offset));
            XPLMSetDatab(m_ref, (void *)s.c_str(), (int)offset, (int)s.size() + 1);
        }

        //! Get the value of part of the string
        std::string getSubstr(size_t offset, size_t size) const
        {
            std::string s(size, 0);
            XPLMGetDatab(m_ref, &s[0], (int)offset, (int)size);
            size = s.find(char(0));
            if (size != std::string::npos) s.resize(size);
            return s;
        }

    private:
        XPLMDataRef m_ref;
    };

    /*!
     * Class providing a custom variable + dataref
     * \tparam DataRefTraits The trait class representing the dataref.
     */
    template <class DataRefTraits>
    class CustomDataRef
    {
    public:
        //! Constructor
        CustomDataRef()
        {
            if constexpr (std::is_same_v<typename DataRefTraits::type, int>)
            {
                m_ref = XPLMRegisterDataAccessor(DataRefTraits::name(), xplmType_Int, 0, read, NULL, NULL, NULL, NULL,
                                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL, this, NULL);
            }
            else { XPLMDebugString("Unsupported custom dataref type\n"); }
            if (!m_ref)
            {
                XPLMDebugString("Missing dataref:");
                XPLMDebugString(DataRefTraits::name());
                XPLMDebugString("\n");
            }
        }

        CustomDataRef(const CustomDataRef &) = delete;
        CustomDataRef &operator=(const CustomDataRef &) = delete;
        CustomDataRef(CustomDataRef &&other) = delete;
        CustomDataRef &operator=(CustomDataRef &&other) = delete;
        ~CustomDataRef()
        {
            if (m_ref) { XPLMUnregisterDataAccessor(m_ref); }
        }

        static typename DataRefTraits::type read(void *refcon)
        {
            return reinterpret_cast<CustomDataRef *>(refcon)->get();
        }

        //! True if the dataref exists
        bool isValid() const { return m_ref != nullptr; }

        //! Set the value
        void set(typename DataRefTraits::type val) { m_datarefVal = val; }

        //! Get the value
        typename DataRefTraits::type get() const { return m_datarefVal; }

        XPLMDataRef m_ref;
        typename DataRefTraits::type m_datarefVal;
    };

    template <>
    inline void DataRefImpl::implSet<int>(int d)
    {
        XPLMSetDatai(m_ref, d);
    }
    template <>
    inline void DataRefImpl::implSet<float>(float d)
    {
        XPLMSetDataf(m_ref, d);
    }
    template <>
    inline void DataRefImpl::implSet<double>(double d)
    {
        XPLMSetDatad(m_ref, d);
    }
    template <>
    inline int DataRefImpl::implGet<int>() const
    {
        return XPLMGetDatai(m_ref);
    }
    template <>
    inline float DataRefImpl::implGet<float>() const
    {
        return XPLMGetDataf(m_ref);
    }
    template <>
    inline double DataRefImpl::implGet<double>() const
    {
        return XPLMGetDatad(m_ref);
    }

    template <>
    inline void ArrayDataRefImpl::implSetAll(int const *v)
    {
        XPLMSetDatavi(m_ref, const_cast<int *>(v), 0, m_size);
    }
    template <>
    inline void ArrayDataRefImpl::implSetAll(float const *v)
    {
        XPLMSetDatavf(m_ref, const_cast<float *>(v), 0, m_size);
    }
    template <>
    inline void ArrayDataRefImpl::implGetAll(int *v) const
    {
        XPLMGetDatavi(m_ref, &v[0], 0, m_size);
    }
    template <>
    inline void ArrayDataRefImpl::implGetAll(float *v) const
    {
        XPLMGetDatavf(m_ref, &v[0], 0, m_size);
    }

    template <>
    inline void ArrayDataRefImpl::implSetAt<int>(int i, int d)
    {
        assert(i <= m_size);
        XPLMSetDatavi(m_ref, &d, i, 1);
    }
    template <>
    inline void ArrayDataRefImpl::implSetAt<float>(int i, float d)
    {
        assert(i <= m_size);
        XPLMSetDatavf(m_ref, &d, i, 1);
    }
    template <>
    inline int ArrayDataRefImpl::implGetAt<int>(int i) const
    {
        assert(i <= m_size);
        int d;
        XPLMGetDatavi(m_ref, &d, i, 1);
        return d;
    }
    template <>
    inline float ArrayDataRefImpl::implGetAt<float>(int i) const
    {
        assert(i <= m_size);
        float d;
        XPLMGetDatavf(m_ref, &d, i, 1);
        return d;
    }

} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_DATAREFS_H
