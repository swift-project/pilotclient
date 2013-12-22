#ifndef BLACKMISC_VALUEOBJECT_H
#define BLACKMISC_VALUEOBJECT_H

#include "blackmisc/debug.h"
#include <QtDBus/QDBusMetaType>
#include <QString>
#include <QtGlobal>
#include <QDataStream>
#include <QDebug>
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
         * \param uc
         * \return
         */
        friend QNoDebug operator<<(QNoDebug nodebug, const CValueObject & /* uc */)
        {
            return nodebug;
        }

        /*!
         * \brief Stream operator << for QDataStream
         * \param stream
         * \param uc
         * \return
         */
        friend QDataStream &operator<<(QDataStream &stream, const CValueObject &uc)
        {
            stream << uc.stringForStreaming();
            return stream;
        }

        /*!
         * \brief Stream operator << for log messages
         * \param log
         * \param uc
         * \return
         */
        friend CLogMessage operator<<(CLogMessage log, const CValueObject &uc)
        {
            log << uc.stringForStreaming();
            return log;
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
         * \param uc
         * \return
         */
        friend const QDBusArgument &operator>>(const QDBusArgument &argument, CValueObject &uc);

        /*!
         * \brief Marshalling operator <<, object to DBus
         * \param argument
         * \param pq
         * \return
         */
        friend QDBusArgument &operator<<(QDBusArgument &argument, const CValueObject &uc);

    public:
        /*!
         * \brief Virtual destructor
         */
        virtual ~CValueObject() {}

        /*!
         * \brief Cast as QString
         * \bool i18n
         */
        QString toQString(bool i18n = false) const;

        /*!
         * \brief Cast to pretty-printed QString
         * \return
         */
        virtual QString toFormattedQString(bool i18n = false) const;

        /*!
         * \brief To std string
         * \param i18n
         * \return
         */
        std::string toStdString(bool i18n = false) const;

        /*!
         * \brief Value hash, allows comparisons between QVariants
         * \return
         */
        virtual uint getValueHash() const = 0;

        /*!
         * Compares with QVariant with this object
         * and returns an integer less than, equal to, or greater than zero
         * if this is less than, equal to, or greater than QVariant.
         * \remarks allows sorting among QVariants, not all classes implement this
         * \return
         */
        virtual int compare(const QVariant &qv) const;

        /*!
         * \brief Virtual method to return QVariant, used with DBUS QVariant lists
         * \return
         */
        virtual QVariant toQVariant() const = 0;

        /*!
         * \brief The stored object as CValueObject
         * \param qv
         * \return
         */
        static const CValueObject *fromQVariant(const QVariant &qv);

    protected:
        /*!
         * \brief Default constructor
         */
        CValueObject();

        /*!
         * \brief String for streaming operators
         * \return
         */
        virtual QString stringForStreaming() const;

        /*!
         * \brief String for QString conversion
         * \param i18n
         * \return
         */
        virtual QString convertToQString(bool i18n = false) const = 0;

        /*!
         * \brief Marshall to DBus
         * \param argument
         */
        virtual void marshallToDbus(QDBusArgument &) const = 0;

        /*!
         * \brief Unmarshall from DBus
         * \param argument
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
     * \param uc
     * \return
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, QDBusArgument>::type const &
    operator>>(const QDBusArgument &argument, T &uc)
    {
        return argument >> static_cast<CValueObject &>(uc);
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
     * \param uc
     * \return
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, QDBusArgument>::type &
    operator<<(QDBusArgument &argument, const T &uc)
    {
        return argument << static_cast<CValueObject const &>(uc);
    }

    /*!
     * Allow comparison with QVariant, e.g.
     * QVariant == CFrequency ?
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator==(const QVariant &variant, const T &uc)
    {
        if (!variant.canConvert<T>()) return false;
        T vuc = variant.value<T>();
        return vuc == uc;
    }

    /*!
     * Allow comparison with QVariant, e.g.
     * QVariant != CFrequency ?
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator!=(const QVariant &variant, const T &uc)
    {
        return !(variant == uc);
    }

    /*!
     * Allow comparison with QVariant, e.g.
     * QVariant == CFrequency ?
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator==(const T &uc, const QVariant &variant)
    {
        return variant == uc;
    }

    /*!
     * Allow comparison with QVariant, e.g.
     * QVariant != CFrequency ?
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator!=(const T &uc, const QVariant &variant)
    {
        return variant != uc;
    }

} // namespace

#endif // guard
