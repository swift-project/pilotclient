/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "stringutils.h"

#include <QChar>

QString BlackMisc::boolToOnOff(bool v, bool i18n)
{
    Q_UNUSED(i18n);
    return v ? "on" : "off";
}

QString BlackMisc::boolToYesNo(bool v, bool i18n)
{
    Q_UNUSED(i18n);
    return v ? "yes" : "no";
}

QString BlackMisc::boolToTrueFalse(bool v, bool i18n)
{
    Q_UNUSED(i18n);
    return v ? "true" : "false";
}

bool BlackMisc::stringToBool(const QString &string)
{
    QString s(string.trimmed().toLower());
    if (s.isEmpty()) { return false; }
    QChar c = s.at(0);

    // explicit values
    if (c == '1' || c == 't' || c == 'y' || c == 'x') { return true; }
    if (c == '0' || c == 'f' || c == 'n' || c == '_') { return false; }
    return false;
}

QString BlackMisc::intToHex(int value, int digits)
{
    QString hex(QString::number(value, 16).toUpper());
    int l = hex.length();
    if (l >= digits) { return hex.right(digits); }
    int d = digits - l;
    return QString(d, '0') + hex;
}

QString BlackMisc::bytesToHexString(const QByteArray &bytes)
{
    QString h;
    for (int i = 0; i < bytes.size(); i++)
    {
        int b = static_cast<int>(bytes.at(i));
        h.append(intToHex(b, 2));
    }
    return h;
}

QByteArray BlackMisc::byteArrayFromHexString(const QString &hexString)
{
    QByteArray ba;
    int pos = 0;
    while (pos + 1 < hexString.length())
    {
        bool ok;
        QString h = hexString.mid(pos, 2);
        int hex = h.toInt(&ok, 16);
        Q_ASSERT_X(ok, Q_FUNC_INFO, "Invalid hex");
        if (!ok) { return QByteArray(); }
        ba.push_back(static_cast<char>(hex));
        pos += 2;
    }
    return ba;
}

QString BlackMisc::stripDesignatorFromCompleterString(const QString &candidate)
{
    const QString s(candidate.trimmed().toUpper());
    if (s.isEmpty()) { return QString(); }
    if (s.contains(' '))
    {
        return s.left(s.indexOf(' '));
    }
    else
    {
        return s;
    }
}
