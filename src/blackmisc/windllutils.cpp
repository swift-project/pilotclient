// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/windllutils.h"
#include <QDir>
#include <QtGlobal>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#    include <windows.h>
#    include <tlhelp32.h>
#endif

namespace BlackMisc
{
#ifdef Q_OS_WIN

    namespace PrivateWindows
    {
        struct LanguageCodePage
        {
            WORD wLanguage;
            WORD wCodePage;
        };

        QString languageToIsoCode(LanguageCodePage codePage)
        {
            const LCID locale = codePage.wLanguage;
            const int nchars = GetLocaleInfo(locale, LOCALE_SISO639LANGNAME, nullptr, 0);
            std::vector<wchar_t> language(static_cast<size_t>(nchars));
            GetLocaleInfo(locale, LOCALE_SISO639LANGNAME, language.data(), nchars);
            const QString iso = QString::fromWCharArray(language.data(), nchars);
            return iso;
        }

        QString queryStringFileInfo(BYTE *pbVersionInfo, LanguageCodePage codePage, const QString &stringName)
        {
            constexpr int fieldWidth = 4;
            constexpr int base = 16;
            QString subBlockNameBuffer = QStringLiteral("\\StringFileInfo\\%1%2\\%3")
                                             .arg(codePage.wLanguage, fieldWidth, base, QLatin1Char('0'))
                                             .arg(codePage.wCodePage, fieldWidth, base, QLatin1Char('0'))
                                             .arg(stringName);
            UINT dwBytes = 0;
            wchar_t *szQueryString = nullptr;
            VerQueryValue(pbVersionInfo, subBlockNameBuffer.toStdWString().c_str(), reinterpret_cast<LPVOID *>(&szQueryString), &dwBytes);
            const QString queryString = QString::fromWCharArray(szQueryString, dwBytes);
            return queryString;
        }
    } // ns

    CWinDllUtils::DLLInfo CWinDllUtils::getDllInfo(const QString &dllFile)
    {
        // http://stackoverflow.com/questions/940707/how-do-i-programatically-get-the-version-of-a-dll-or-exe-file
        DLLInfo result;
        const QFile dllQFile(dllFile);
        if (!dllQFile.exists())
        {
            result.errorMsg = QStringLiteral("No file '%1'").arg(dllFile);
            return result;
        }
        const QString dll(QDir::toNativeSeparators(dllFile));

        // temp std:string object, disposed at end of function
        const std::wstring dllStr(dll.toStdWString());

        DWORD dwSize = 0;
        UINT puLenFileInfo = 0;
        const TCHAR *pszFilePath = dllStr.c_str();
        VS_FIXEDFILEINFO *pFileInfo = nullptr;

        // Get the version info for the file requested
        dwSize = GetFileVersionInfoSize(pszFilePath, nullptr);
        if (dwSize == 0)
        {
            result.errorMsg = QStringLiteral("Error in GetFileVersionInfoSize: %1\n").arg(GetLastError());
            return result;
        }

        std::vector<BYTE> pbVersionInfo(dwSize);
        if (!GetFileVersionInfo(pszFilePath, 0, dwSize, pbVersionInfo.data()))
        {
            result.errorMsg = QStringLiteral("Error in GetFileVersionInfo: %1\n").arg(GetLastError());
            return result;
        }

        // Language independent version of VerQueryValue
        if (!VerQueryValue(pbVersionInfo.data(), TEXT("\\"), reinterpret_cast<LPVOID *>(&pFileInfo), &puLenFileInfo))
        {
            result.errorMsg = QStringLiteral("Error in VerQueryValue: %1\n").arg(GetLastError());
            return result;
        }

        // pFileInfo->dwFileVersionMS is usually zero.
        // However, you should check this if your version numbers seem to be wrong
        result.fileVersion = QStringLiteral("%1.%2.%3.%4")
                                 .arg((pFileInfo->dwFileVersionMS >> 16) & 0xffff)
                                 .arg((pFileInfo->dwFileVersionMS >> 0) & 0xffff)
                                 .arg((pFileInfo->dwFileVersionLS >> 16) & 0xffff)
                                 .arg((pFileInfo->dwFileVersionLS >> 0) & 0xffff);

        // pFileInfo->dwProductVersionMS is usually zero. However, you should check
        // this if your version numbers seem to be wrong
        result.productVersion = QStringLiteral("%1.%2.%3.%4")
                                    .arg((pFileInfo->dwProductVersionMS >> 16) & 0xffff)
                                    .arg((pFileInfo->dwProductVersionMS >> 0) & 0xffff)
                                    .arg((pFileInfo->dwProductVersionLS >> 16) & 0xffff)
                                    .arg((pFileInfo->dwProductVersionLS >> 0) & 0xffff);

        PrivateWindows::LanguageCodePage *lpTranslate;

        // Read the list of languages and code pages.
        VerQueryValue(pbVersionInfo.data(), TEXT("\\VarFileInfo\\Translation"), reinterpret_cast<LPVOID *>(&lpTranslate), &puLenFileInfo);

        // Read the file description for each language and code page.
        // All names: https://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx
        for (UINT i = 0; i < (puLenFileInfo / sizeof(struct PrivateWindows::LanguageCodePage)); i++)
        {
            // Retrieve file description for language and code page "i".
            const PrivateWindows::LanguageCodePage cp = lpTranslate[i];
            result.iso = PrivateWindows::languageToIsoCode(cp);
            result.fileDescription = PrivateWindows::queryStringFileInfo(pbVersionInfo.data(), cp, QStringLiteral("FileDescription"));
            result.productName = PrivateWindows::queryStringFileInfo(pbVersionInfo.data(), cp, QStringLiteral("ProductName"));
            result.productVersionName = PrivateWindows::queryStringFileInfo(pbVersionInfo.data(), cp, QStringLiteral("ProductVersion"));
            result.originalFilename = PrivateWindows::queryStringFileInfo(pbVersionInfo.data(), cp, QStringLiteral("OriginalFilename"));
            result.fullFilename = dllQFile.fileName();

            //! \fixme currently stopping at first language, maybe need to change that in future
            break;
        }

        // bye
        return result;
    }

    QList<CWinDllUtils::ProcessModule> CWinDllUtils::getModules(qint64 processId, const QString &nameFilter)
    {
        QList<CWinDllUtils::ProcessModule> results;

        const DWORD dwPID = static_cast<DWORD>(processId < 0 ? QCoreApplication::applicationPid() : processId);
        HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
        MODULEENTRY32 me32;

        // Take a snapshot of all modules in the specified process.
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
        if (hModuleSnap == INVALID_HANDLE_VALUE)
        {
            // printError(TEXT("CreateToolhelp32Snapshot (of modules)"));
            return results;
        }

        // Set the size of the structure before using it.
        me32.dwSize = sizeof(MODULEENTRY32);

        // Retrieve information about the first module,
        // and exit if unsuccessful
        if (!Module32First(hModuleSnap, &me32))
        {
            // printError(TEXT("Module32First")); // show cause of failure
            CloseHandle(hModuleSnap); // clean the snapshot object
            return results;
        }

        // Now walk the module list of the process,
        // and display information about each module
        do
        {
            ProcessModule pm;
            pm.moduleName = QString::fromWCharArray(static_cast<const wchar_t *>(&me32.szModule[0]));
            if (nameFilter.isEmpty() || pm.moduleName.contains(nameFilter, Qt::CaseInsensitive))
            {
                pm.executable = QString::fromWCharArray(static_cast<const wchar_t *>(&me32.szExePath[0]));
                pm.processId = static_cast<qint64>(me32.th32ProcessID);
                results.append(pm);
                if (!nameFilter.isEmpty()) { break; }
            }
        }
        while (Module32Next(hModuleSnap, &me32));

        CloseHandle(hModuleSnap);
        return results;
    }

#else
    // Dummy functions
    CWinDllUtils::DLLInfo CWinDllUtils::getDllInfo(const QString &dllFile)
    {
        Q_UNUSED(dllFile);
        DLLInfo result;
        result.errorMsg = QString("Only works on Windows");
        return result;
    }

    QList<CWinDllUtils::ProcessModule> CWinDllUtils::getModules(qint64 processId, const QString &nameFilter)
    {
        Q_UNUSED(processId);
        Q_UNUSED(nameFilter);
        static const QList<CWinDllUtils::ProcessModule> results;
        return results;
    }
#endif

} // ns
