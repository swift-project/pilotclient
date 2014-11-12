/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VARIANT_H
#define BLACKMISC_VARIANT_H

#include <QVariant>
#include <QDateTime>
#include <QJsonValueRef>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

class QDBusArgument;

namespace BlackMisc
{

    /*!
     * Wrapper class for QVariant, for more natural and transparent DBus integration.
     */
    class CVariant
    {
    public:
        //! Default constructor.
        CVariant() {}

        //! Copy constructor.
        CVariant(const CVariant &other) : m_v(other.m_v) {}

        //! Move constructor.
        CVariant(CVariant &&other) : m_v(std::move(other.m_v)) {}

        //! Construct from a QVariant.
        CVariant(const QVariant &var) : m_v(var) {}

        //! Move-construct from a QVariant.
        CVariant(QVariant &&var) : m_v(std::move(var)) {}

        //! Construct a null variant of the given type.
        CVariant(QVariant::Type type) : m_v(type) {}

        //! Construct a variant from the given type and opaque pointer.
        CVariant(int typeId, const void *copy) : m_v(typeId, copy) {}

        //! Value hash
        uint getValueHash() const;

        //! Change the internal QVariant.
        void reset(const QVariant &var) { m_v = var; }

        //! Change the internal QVariant.
        void reset(QVariant &&var) { m_v = std::move(var); }

        //! Copy assignment operator.
        CVariant &operator =(const CVariant &other) { m_v = other.m_v; return *this; }

        //! Move assignment operatior.
        CVariant &operator =(CVariant && other) { m_v = std::move(other.m_v); return *this; }

        //! Change the internal QVariant
        CVariant &operator =(const QVariant &var) { m_v = var; return *this; }

        //! Change the internal QVariant
        CVariant &operator =(QVariant && var) { m_v = std::move(var); return *this; }

        //! Swap this variant with another.
        void swap(CVariant &other) { m_v.swap(other.m_v); }

        //! Swap the internal QVariant with another.
        void swap(QVariant &other) { m_v.swap(other); }

        //! Construct a variant from a value.
        template <typename T> static CVariant fromValue(T &&value) { return CVariant(QVariant::fromValue(std::forward<T>(value))); }

        //! Synonym for fromValue().
        template <typename T> static CVariant from(T &&value) { return CVariant(QVariant::fromValue(std::forward<T>(value))); }

        //! Change the value.
        template <typename T> void setValue(T &&value) { m_v.setValue(std::forward<T>(value)); }

        //! Synonym for setValue().
        template <typename T> void set(T &&value) { m_v.setValue(std::forward<T>(value)); }

        //! Return the value converted to the type T.
        template <typename T> T value() const { return m_v.value<T>(); }

        //! Synonym for value().
        template <typename T> T to() const { return m_v.value<T>(); }

        //! Return the internal QVariant.
        const QVariant &toQVariant() const { return m_v; }

        //! True if this variant can be converted to the type with the given metatype ID.
        bool canConvert(int typeId) const { return m_v.canConvert(typeId); }

        //! True if this variant can be converted to the type T.
        template <typename T> bool canConvert() const { return m_v.canConvert<T>(); }

        //! Convert this variant to the type with the given metatype ID and return true if successful.
        bool convert(int typeId) { return m_v.convert(typeId); }

        //! Convert this variant to a QString.
        QString toString(bool i18n = false) const;

        //! Convert this variant to a bool.
        bool toBool() const { return m_v.toBool(); }

        //! Convert this variant to an integer.
        int toInt() const { return m_v.toInt(); }

        //! Convert this variant to double.
        double toDouble() const { return m_v.toDouble(); }

        //! Convert this variant to QDateTime.
        QDateTime toDateTime() const { return m_v.toDateTime(); }

        //! Set the variant to null.
        void clear() { m_v.clear(); }

        //! True if this variant is null.
        bool isNull() const { return m_v.isNull(); }

        //! True if this variant is valid.
        bool isValid() const { return m_v.isValid(); }

        //! Return the metatype ID of the value in this variant, or QMetaType::User if it is a user type.
        // KB we should think about returning QMetaType::Type directly, as QVariant::Type is deprecated
        QVariant::Type type() const { return m_v.type(); }

        //! Return the typename of the value in this variant.
        const char *typeName() const { return m_v.typeName(); }

        //! Return the metatype ID of the value in this variant.
        int userType() const { return m_v.userType(); }

        //! To JSON
        QJsonObject toJson() const;

        //! From JSON
        void fromJson(const QJsonObject &json);

        //! Equal operator.
        bool operator ==(const CVariant &other) const { return m_v == other.m_v; }

        //! Not equal operator.
        bool operator !=(const CVariant &other) const { return m_v != other.m_v; }

        //! Less than operator.
        bool operator <(const CVariant &other) const { return m_v < other.m_v; }

        //! Less than or equal operator.
        bool operator <=(const CVariant &other) const { return m_v <= other.m_v; }

        //! Greater than operator.
        bool operator >(const CVariant &other) const { return m_v > other.m_v; }

        //! Greater than or equal operator.
        bool operator >=(const CVariant &other) const { return m_v >= other.m_v; }

        //! Register metadata.
        static void registerMetadata();

    private:
        QVariant m_v;
    };

    //! Marshall a variant to DBus.
    QDBusArgument &operator <<(QDBusArgument &arg, const CVariant &var);

    //! Unmarshall a variant from DBus.
    const QDBusArgument &operator >>(const QDBusArgument &arg, CVariant &var);

    //! Non member, non friend operator >> for JSON
    inline QJsonArray &operator<<(QJsonArray &json, const BlackMisc::CVariant &variant)
    {
        json.append(variant.toJson());
        return json;
    }

    /*!
     * \brief Non member, non friend operator << for JSON
     * \param json
     * \param value as pair name/value
     * \return
     */
    inline QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, BlackMisc::CVariant> &value)
    {
        json.insert(value.first, QJsonValue(value.second.toJson()));
        return json;
    }

    //! Non member, non friend operator >> for JSON
    inline const QJsonObject &operator>>(const QJsonObject &json, BlackMisc::CVariant &variant)
    {
        variant.fromJson(json);
        return json;
    }

    //! Non member, non friend operator >> for JSON
    inline const QJsonValue &operator>>(const QJsonValue &json, BlackMisc::CVariant &variant)
    {
        variant.fromJson(json.toObject());
        return json;
    }

    //! Non member, non friend operator >> for JSON
    inline const QJsonValueRef &operator>>(const QJsonValueRef &json, BlackMisc::CVariant &variant)
    {
        variant.fromJson(json.toObject());
        return json;
    }

    //! qHash overload, needed for storing CVariant with tupel system
    inline uint qHash(const BlackMisc::CVariant &value, uint seed = 0)
    {
        return ::qHash(value.getValueHash(), seed);
    }

    // Needed so that our qHash overload doesn't hide the qHash overloads in the global namespace.
    // This will be safe as long as no global qHash has the same signature as ours.
    // Alternative would be to qualify all our invokations of the global qHash as ::qHash.
    using ::qHash;

} // namespace

Q_DECLARE_METATYPE(BlackMisc::CVariant)

#endif // guard
