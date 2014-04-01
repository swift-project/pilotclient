#ifndef BLACKMISC_VALUEOBJECT_H
#define BLACKMISC_VALUEOBJECT_H

#include "dbus.h"
#include "tuple.h"
#include "json.h"
#include <QtDBus/QDBusMetaType>
#include <QString>
#include <QtGlobal>
#include <QDataStream>
#include <QDebug>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <type_traits>
#include <iostream>

namespace BlackMisc
{
    // forward declaration
    class CValueMap;

    /*!
     * \brief Base class for value objects.
     * Public non-virtual interface with protected virtual implementation.
     */
    class CValueObject
    {

        /*!
         * \brief Stream << overload to be used in debugging messages
         * \param debug
         * \param uc
         * \return
         */
        friend QDebug operator<<(QDebug debug, const CValueObject &uc)
        {
            debug << uc.stringForStreaming();
            return debug;
        }

        /*!
         * \brief Operator << based on text stream
         * \param textStream
         * \param uc
         * \return
         */
        friend QTextStream &operator<<(QTextStream &textStream, const CValueObject &uc)
        {
            textStream << uc.stringForStreaming();
            return textStream;
        }

        /*!
         * \brief Operator << when there is no debug stream
         * \param nodebug
         * \param valueObject
         * \return
         */
        friend QNoDebug operator<<(QNoDebug nodebug, const CValueObject &valueObject)
        {
            Q_UNUSED(valueObject);
            return nodebug;
        }

        /*!
         * \brief Stream operator << for QDataStream
         * \param stream
         * \param valueObject
         * \return
         */
        friend QDataStream &operator<<(QDataStream &stream, const CValueObject &valueObject)
        {
            stream << valueObject.stringForStreaming();
            return stream;
        }

        /*!
         * \brief Stream operator << for std::cout
         * \param ostr
         * \param uc
         * \return
         */
        friend std::ostream &operator<<(std::ostream &ostr, const CValueObject &uc)
        {
            ostr << uc.stringForStreaming().toStdString();
            return ostr;
        }

        /*!
         * \brief Unmarshalling operator >>, DBus to object
         * \param argument
         * \param valueObject
         * \return
         */
        friend const QDBusArgument &operator>>(const QDBusArgument &argument, CValueObject &valueObject);

        /*!
         * \brief Marshalling operator <<, object to DBus
         * \param argument
         * \param valueObject
         * \return
         */
        friend QDBusArgument &operator<<(QDBusArgument &argument, const CValueObject &valueObject);

        /*!
         * \brief Operator == with value map
         * \param valueMap
         * \param valueObject
         * \return
         */
        friend bool operator==(const CValueMap &valueMap, const CValueObject &valueObject);

        /*!
         * \brief Operator != with value map
         * \param valueMap
         * \param valueObject
         * \return
         */
        friend bool operator!=(const CValueMap &valueMap, const CValueObject &valueObject);

        //! \brief Operator == with value map

        /*!
         * \brief operator == with value map
         * \param valueObject
         * \param valueMap
         * \return
         */
        friend bool operator==(const CValueObject &valueObject, const CValueMap &valueMap);

        /*!
         * \brief Operator != with value map
         * \param valueObject
         * \param valueMap
         * \return
         */
        friend bool operator!=(const CValueObject &valueObject, const CValueMap &valueMap);

        /*!
         * Compares two instances of related classes
         * and returns an integer less than, equal to, or greater than zero
         * if v1 is less than, equal to, or greater than v2.
         * \return
         * \pre The runtime types of the two objects must be the same or related by inheritance.
         */
        friend int compare(const CValueObject &v1, const CValueObject &v2);

    public:
        /*!
         * \brief Virtual destructor
         */
        virtual ~CValueObject() {}

        /*!
         * \brief Cast as QString
         */
        QString toQString(bool i18n = false) const;

        /*!
         * \brief Cast to pretty-printed QString
         */
        virtual QString toFormattedQString(bool i18n = false) const;

        /*!
         * \brief To std string
         */
        std::string toStdString(bool i18n = false) const;

        /*!
         * \brief Update by variant map
         */
        int apply(const BlackMisc::CValueMap &valueMap);

        /*!
         * \brief Value hash, allows comparisons between QVariants
         */
        virtual uint getValueHash() const = 0;

        /*!
         * \brief Virtual method to return QVariant, used with DBus QVariant lists
         */
        virtual QVariant toQVariant() const = 0;

        /*!
         * \brief Contribute to JSON object
         * \return updated JSON object
         */
        virtual QJsonObject toJson() const { QJsonObject json; return json;}

        /*!
         * \brief Initialize from JSOn object
         * \param json
         */
        virtual void fromJson(const QJsonObject &json) { Q_UNUSED(json); }

        /*!
         * \brief As icon, not implement by all classes
         * \return
         */
        virtual const QPixmap &toIcon() const { static const QPixmap p; return p; }

        /*!
         * \brief Set property by index
         * \remarks Intentionally not abstract, avoiding all classes need to implement this method
         * \param variant
         * \param index as used in Qt table views
         */
        virtual void setPropertyByIndex(const QVariant &variant, int index);

        /*!
         * \brief Property by index
         * \remarks Intentionally not abstract, avoiding all classes need to implement this method
         * \param index as used in Qt table views
         * \return
         */
        virtual QVariant propertyByIndex(int index) const;

        /*!
         * \brief Property by index as String
         * \remarks Intentionally not abstract, avoiding all classes need to implement this method
         * \param index as used in Qt table views
         * \param i18n
         * \return
         */
        virtual QString propertyByIndexAsString(int index, bool i18n = false) const;

        /*!
         * \brief The stored object as CValueObject
         */
        static const CValueObject *fromQVariant(const QVariant &qv);

    protected:
        /*!
         * \brief Default constructor
         */
        CValueObject() {}

        /*!
         * \brief Copy constructor
         */
        CValueObject(const CValueObject &) {}

        /*!
         * \brief Copy assignment operator =
         */
        CValueObject &operator=(const CValueObject &) { return *this; }

        /*!
         * \brief String for streaming operators
         */
        virtual QString stringForStreaming() const;

        /*!
         * \brief String for QString conversion
         * \param i18n
         * \return
         */
        virtual QString convertToQString(bool i18n = false) const = 0;

        /*!
         * \brief Returns the Qt meta type ID of this object.
         * \return
         */
        virtual int getMetaTypeId() const = 0;

        /*!
         * \brief Returns true if this object is an instance of the class with the given meta type ID,
         *        or one of its subclasses.
         * \param metaTypeId
         * \return
         */
        virtual bool isA(int metaTypeId) const { Q_UNUSED(metaTypeId); return false; }

        /*!
         * \brief Compare this value with another value of the same type
         * \param other
         * \return Less than, equal to, or greater than zero if this is
         *         less than, equal to, or greather than other.
         * \pre Other must have the same runtime type as the this object.
         * \remark It is usually safer to use the friend function compare() instead.
         */
        virtual int compareImpl(const CValueObject &other) const = 0;

        /*!
         * \brief Marshall to DBus
         */
        virtual void marshallToDbus(QDBusArgument &) const = 0;

        /*!
         * \brief Unmarshall from DBus
         */
        virtual void unmarshallFromDbus(const QDBusArgument &) = 0;

    };

    /*!
     * Non-member non-friend operator for streaming T objects to QDBusArgument.
     * Needed because we can't rely on the friend operator in some cases due to
     * an unrelated template for streaming Container<T> in QtDBus/qdbusargument.h
     * which matches more types than it can actually handle.
     *
     * <a href="https://dev.vatsim-germany.org/boards/15/topics/26?r=891#message-891">Forum</a>
     * <a href="https://dev.vatsim-germany.org/boards/15/topics/26?r=865#message-865">Forum</a>
     *
     * \param argument
     * \param valueObject
     * \return
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, QDBusArgument>::type const &
    operator>>(const QDBusArgument &argument, T &valueObject)
    {
        return argument >> static_cast<CValueObject &>(valueObject);
    }

    /*!
     * Non-member non-friend operator for streaming T objects from QDBusArgument.
     * Needed because we can't rely on the friend operator in some cases due to
     * an unrelated template for streaming Container<T> in QtDBus/qdbusargument.h
     * which matches more types than it can actually handle.
     *
     * <a href="https://dev.vatsim-germany.org/boards/15/topics/26?r=891#message-891">Forum</a>
     * <a href="https://dev.vatsim-germany.org/boards/15/topics/26?r=865#message-865">Forum</a>
     *
     * \param argument
     * \param valueObject
     * \return
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, QDBusArgument>::type &
    operator<<(QDBusArgument &argument, const T &valueObject)
    {
        return argument << static_cast<CValueObject const &>(valueObject);
    }

    /*!
     * \brief Non member, non friend operator >> for JSON
     * \param json
     * \param valueObject
     * \return
     */
    inline const QJsonObject &operator>>(const QJsonObject &json, CValueObject &valueObject)
    {
        valueObject.fromJson(json);
        return json;
    }

    /*!
     * \brief Non member, non friend operator >> for JSON
     * \param json
     * \param valueObject
     * \return
     */
    inline const QJsonValue &operator>>(const QJsonValue &json, CValueObject &valueObject)
    {
        valueObject.fromJson(json.toObject());
        return json;
    }

    /*!
     * \brief Non member, non friend operator >> for JSON
     * \param json
     * \param valueObject
     * \return
     */
    inline const QJsonValueRef &operator>>(const QJsonValueRef &json, CValueObject &valueObject)
    {
        valueObject.fromJson(json.toObject());
        return json;
    }

    /*!
     * \brief Non member, non friend operator >> for JSON
     * \param json
     * \param value
     * \return
     */
    inline QJsonArray &operator<<(QJsonArray &json, const CValueObject &value)
    {
        json.append(value.toJson());
        return json;
    }

    /*!
     * \brief Non member, non friend operator >> for JSON
     * \param json
     * \param value
     * \return
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, QJsonObject>::type &
    operator<<(QJsonObject &json, const std::pair<QString, T> &value)
    {
        json.insert(value.first, QJsonValue(value.second.toJson()));
        return json;
    }

    /*!
     * Allow comparison with QVariant, e.g. QVariant == CFrequency ?
     * \param variant
     * \param valueObject
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator==(const QVariant &variant, const T &valueObject)
    {
        if (!variant.canConvert<T>()) return false;
        T vuc = variant.value<T>();
        return vuc == valueObject;
    }

    /*!
     * \brief Allow comparison with QVariant, e.g. QVariant != CFrequency ?
     * \param variant
     * \param valueObject
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator!=(const QVariant &variant, const T &valueObject)
    {
        return !(variant == valueObject);
    }

    /*!
     * \brief Allow comparison with QVariant, e.g. QVariant == CFrequency ?
     * \param valueObject
     * \param variant
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator==(const T &valueObject, const QVariant &variant)
    {
        return variant == valueObject;
    }

    /*!
     * \brief Allow comparison with QVariant, e.g. QVariant != CFrequency ?
     * \param valueObject
     * \param variant
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator!=(const T &valueObject, const QVariant &variant)
    {
        return variant != valueObject;
    }

    /*!
     * qHash overload, needed for storing CValueObject in a QSet.
     * \param value
     * \param seed
     * \return
     */
    inline uint qHash(const BlackMisc::CValueObject &value, uint seed = 0)
    {
        return ::qHash(value.getValueHash(), seed);
    }

    // Needed so that our qHash overload doesn't hide the qHash overloads in the global namespace.
    // This will be safe as long as no global qHash has the same signature as ours.
    // Alternative would be to qualify all our invokations of the global qHash as ::qHash.
    using ::qHash;

} // namespace

#endif // guard
