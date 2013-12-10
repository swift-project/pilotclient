#include "valueobject.h"
#include "valuemap.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{

    /*
     * Constructor
     */
    CValueObject::CValueObject() {}

    /*
     * Stringify
     */
    QString CValueObject::toQString(bool i18n) const
    {
        return this->convertToQString(i18n);
    }

    /*
     * Stringify
     */
    QString CValueObject::toFormattedQString(bool i18n) const
    {
        return this->toQString(i18n);
    }

    /*
     * Stringify
     */
    std::string CValueObject::toStdString(bool i18n) const
    {
        return this->convertToQString(i18n).toStdString();
    }

    /*
     * Streaming
     */
    QString CValueObject::stringForStreaming() const
    {
        // simplest default implementation requires only one method
        return this->convertToQString();
    }

    /*
     * from DBus
     */
    const QDBusArgument &operator>>(const QDBusArgument &argument, CValueObject &uc)
    {
        argument.beginStructure();
        uc.unmarshallFromDbus(argument);
        argument.endStructure();
        return argument;
    }

    /*
     * to DBus
     */
    QDBusArgument &operator<<(QDBusArgument &argument, const CValueObject &uc)
    {
        argument.beginStructure();
        uc.marshallToDbus(argument);
        argument.endStructure();
        return argument;
    }
}
