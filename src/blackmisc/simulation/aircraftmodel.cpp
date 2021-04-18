/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "matchingutils.h"
#include "aircraftmodel.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/verify.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QString>
#include <QtGlobal>
#include <QStringBuilder>
#include <QFileInfo>

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Db;

namespace BlackMisc
{
    namespace Simulation
    {
        void CAircraftModel::registerMetadata()
        {
            CValueObject<CAircraftModel>::registerMetadata();
            qRegisterMetaType<ModelType>();
        }

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type) :
            m_modelString(model.trimmed().toUpper()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const CAircraftIcaoCode &icao, const CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_modelString(model.trimmed().toUpper()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const QString &description, const CAircraftIcaoCode &icao, const Aviation::CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_modelString(model.trimmed().toUpper()), m_description(description.trimmed()), m_modelType(type)
        {}

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const CSimulatorInfo &simulator, const QString &name, const QString &description) :
            m_simulator(simulator), m_modelString(model.trimmed().toUpper()), m_name(name.trimmed()), m_description(description.trimmed()), m_modelType(type)
        { }

        CAircraftModel::CAircraftModel(const QString &model, CAircraftModel::ModelType type, const CSimulatorInfo &simulator, const QString &name, const QString &description, const CAircraftIcaoCode &icao, const CLivery &livery) :
            m_aircraftIcao(icao), m_livery(livery), m_simulator(simulator), m_modelString(model.trimmed().toUpper()), m_name(name.trimmed()), m_description(description.trimmed()), m_modelType(type)
        { }

        QString CAircraftModel::convertToQString(bool i18n) const
        {
            const QString s =
                (this->hasAnyModelString() ? inApostrophes(this->getAllModelStringsAliasesAndDbKey(), true) % QStringLiteral(" ") : QString()) %
                u" type: '" % this->getModelTypeAsString() %
                u"' ICAO: {" % this->getAircraftIcaoCode().toQString(i18n) %
                u"} CG: " % this->getCG().valueRoundedWithUnit(1) %
                u"' livery: {" % m_livery.toQString(i18n) %
                u"} file: '" % m_fileName % u'\'';
            return s;
        }

        QJsonObject CAircraftModel::toDatabaseJson() const
        {
            QJsonObject obj;

            // filename not in DB
            obj.insert("id", this->getDbKeyAsJsonValue());
            obj.insert("name", this->getName());
            obj.insert("modelstring", QJsonValue(m_modelString));
            obj.insert("description", QJsonValue(m_description));
            obj.insert("parts", QJsonValue(m_supportedParts));
            obj.insert("version", CBuildConfig::getVersionString());
            obj.insert("mode", QJsonValue(getModelModeAsString().left(1).toUpper())); // clazy:exclude=qstring-left
            if (m_cg.isNull())
            {
                obj.insert("cgft", QJsonValue()); // null value
            }
            else
            {
                obj.insert("cgft", QJsonValue(m_cg.value(CLengthUnit::ft())));
            }

            // sims
            const CSimulatorInfo sim(getSimulator());
            QString flag = CDatastoreUtility::boolToDbYN(sim.isFSX());
            obj.insert("simfsx", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.isP3D());
            obj.insert("simp3d", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.isFS9());
            obj.insert("simfs9", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.isXPlane());
            obj.insert("simxplane", QJsonValue(flag));
            flag = CDatastoreUtility::boolToDbYN(sim.isFG());
            obj.insert("simfg", QJsonValue(flag));

            // foreign keys
            obj.insert("iddistributor", this->getDistributor().getDbKeyAsJsonValue());
            obj.insert("idaircrafticao", this->getAircraftIcaoCode().getDbKeyAsJsonValue());
            obj.insert("idlivery", this->getLivery().getDbKeyAsJsonValue());
            obj.insert("idairlineicao", this->getLivery().getAirlineIcaoCode().getDbKeyAsJsonValue()); // not really needed if livery is complete

            return obj;
        }

        QString CAircraftModel::toDatabaseJsonString(QJsonDocument::JsonFormat format) const
        {
            return QJsonDocument(this->toDatabaseJson()).toJson(format);
        }

        QJsonObject CAircraftModel::toMemoizedJson(MemoHelper::CMemoizer &helper) const
        {
            QJsonObject json;
            introspect<CAircraftModel>().forEachMember([ &, this ](auto member)
            {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForJson>()))
                {
                    auto &&maybeMemo = helper.maybeMemoize(member.in(*this));
                    json << std::make_pair(CExplicitLatin1String(member.latin1Name()), std::cref(maybeMemo));
                }
            });
            return json;
        }

        void CAircraftModel::convertFromMemoizedJson(const QJsonObject &json, const MemoHelper::CUnmemoizer &helper)
        {
            introspect<CAircraftModel>().forEachMember([ &, this ](auto member)
            {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForJson>()))
                {
                    auto it = json.find(CExplicitLatin1String(member.latin1Name()));
                    if (it != json.end()) { it.value() >> helper.maybeUnmemoize(member.in(*this)).get(); }
                }
            });
        }

        QString CAircraftModel::asHtmlSummary(const QString &separator) const
        {
            return QStringLiteral("Model: %1 changed: %2%3Simulator: %4 Mode: %5 Distributor: %6%7Aircraft ICAO: %8%9Livery: %10")
                   .arg(this->getModelStringAndDbKey(), this->getFormattedUtcTimestampYmdhms(), separator,
                        this->getSimulator().toQString(true), this->getModelModeAsString(), this->getDistributor().getIdAndDescription(), separator,
                        this->getAircraftIcaoCode().asHtmlSummary(), separator)
                   .arg(this->getLivery().asHtmlSummary("&nbsp;")).replace(" ", "&nbsp;");
        }

        bool CAircraftModel::matchesFileName(const QString &fileName) const
        {
            return stringCompare(fileName, m_fileName, CFileUtils::osFileNameCaseSensitivity());
        }

        CStatusMessageList CAircraftModel::verifyModelData() const
        {
            CStatusMessageList msgs;
            const ModelType t = this->getModelType();
            if (t == TypeOwnSimulatorModel || t == TypeManuallySet || t == TypeDatabaseEntry || t == TypeModelMatching || t == TypeModelMatchingDefaultModel)
            {
                if (!this->hasExistingCorrespondingFile())
                {
                    const CStatusMessage m = CStatusMessage(this).validationWarning(u"File '%1' not readable") << this->getFileName();
                    msgs.push_back(m);
                }
                else
                {
                    const CStatusMessage m = CStatusMessage(this).validationInfo(u"File '%1' existing") << this->getFileName();
                    msgs.push_back(m);
                }
            }
            else
            {
                const CStatusMessage m = CStatusMessage(this).validationError(u"Invalid model type to check: '%1'") << this->getModelTypeAsString();
                msgs.push_back(m);
            }
            return msgs;
        }

        bool CAircraftModel::canInitializeFromFsd() const
        {
            const bool nw = this->getModelType() == CAircraftModel::TypeQueriedFromNetwork ||
                            this->getModelType() == CAircraftModel::TypeFSInnData ||
                            this->getModelType() == CAircraftModel::TypeUnknown;
            return nw;
        }

        void CAircraftModel::setCallsign(const CCallsign &callsign)
        {
            m_callsign = callsign;
            m_callsign.setTypeHint(CCallsign::Aircraft);
        }

        QString CAircraftModel::getModelStringAndDbKey() const
        {
            if (this->hasValidDbKey())
            {
                return this->hasModelString() ?
                       this->getModelString() % this->getDbKeyAsStringInParentheses(" ") :
                       this->getDbKeyAsString();
            }
            else
            {
                return this->getModelString();
            }
        }

        QString CAircraftModel::getAllModelStringsAndAliases() const
        {
            if (!this->hasModelStringAlias()) { return m_modelString; }
            if (!this->hasModelString()) { return m_modelStringAlias; }
            return m_modelString % u", " % m_modelStringAlias;
        }

        QString CAircraftModel::getAllModelStringsAliasesAndDbKey() const
        {
            if (!this->hasModelStringAlias() || m_modelString.isEmpty()) { return this->getModelStringAndDbKey(); }
            if (!this->isLoadedFromDb()) { return this->getAllModelStringsAndAliases(); }
            return this->getAllModelStringsAndAliases() % " " % this->getDbKeyAsStringInParentheses();
        }

        bool CAircraftModel::isVtol() const
        {
            return this->getAircraftIcaoCode().isVtol();
        }

        QVariant CAircraftModel::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            if (IOrderable::canHandleIndex(index)) { return IOrderable::propertyByIndex(index);}

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:           return QVariant(m_modelString);
            case IndexModelStringAlias:      return QVariant(m_modelStringAlias);
            case IndexAllModelStrings:       return this->getAllModelStringsAndAliases();
            case IndexHasQueriedModelString: return QVariant::fromValue(this->hasQueriedModelString());
            case IndexModelType:             return QVariant::fromValue(m_modelType);
            case IndexModelTypeAsString:     return QVariant(this->getModelTypeAsString());
            case IndexModelMode:             return QVariant::fromValue(m_modelMode);
            case IndexModelModeAsString:     return QVariant::fromValue(this->getModelModeAsString());
            case IndexModelModeAsIcon:       return QVariant::fromValue(this->getModelModeAsIcon());
            case IndexDistributor:           return m_distributor.propertyByIndex(index.copyFrontRemoved());
            case IndexSimulatorInfo:         return m_simulator.propertyByIndex(index.copyFrontRemoved());
            case IndexSimulatorInfoAsString: return QVariant(m_simulator.toQString());
            case IndexDescription:           return QVariant(m_description);
            case IndexName:                  return QVariant(m_name);
            case IndexFileName:              return QVariant(m_fileName);
            case IndexCG:                    return m_cg.propertyByIndex(index.copyFrontRemoved());
            case IndexSupportedParts:        return QVariant(m_supportedParts);
            case IndexFileTimestamp:         return QVariant::fromValue(this->getFileTimestamp());
            case IndexFileTimestampFormattedYmdhms: return QVariant::fromValue(this->getFormattedFileTimestampYmdhms());
            case IndexIconPath:              return QVariant(m_iconFile);
            case IndexAircraftIcaoCode:      return m_aircraftIcao.propertyByIndex(index.copyFrontRemoved());
            case IndexLivery:                return m_livery.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign:              return m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexMembersDbStatus:       return this->getMembersDbStatus();
            default:                         return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftModel::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CAircraftModel>(); return; }
            if (IOrderable::canHandleIndex(index)) { IOrderable::setPropertyByIndex(index, variant); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:      m_modelString = variant.toString(); break;
            case IndexModelStringAlias: m_modelStringAlias = variant.toString(); break;
            case IndexAircraftIcaoCode: m_aircraftIcao.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexLivery:           m_livery.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexDistributor:      m_distributor.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexDescription:      m_description = variant.toString(); break;
            case IndexSimulatorInfo:    m_simulator.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexName:             m_name = variant.toString(); break;
            case IndexIconPath:         m_iconFile = variant.toString(); break;
            case IndexCG:               m_cg.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexSupportedParts:   this->setSupportedParts(variant.toString()); break;
            case IndexModelType:        m_modelType = variant.value<ModelType>(); break;
            case IndexFileName:         m_fileName = variant.toString(); break;
            case IndexCallsign:
                m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant);
                m_callsign.setTypeHint(CCallsign::Aircraft);
                break;
            case IndexFileTimestamp:
                if (variant.canConvert<QDateTime>())
                {
                    this->setFileTimestamp(variant.value<QDateTime>());
                }
                else if (variant.canConvert<qint64>())
                {
                    m_fileTimestamp = variant.value<qint64>();
                }
                break;
            case IndexModelMode:
                if (static_cast<QMetaType::Type>(variant.type()) == QMetaType::QString)
                {
                    this->setModelModeAsString(variant.toString());
                }
                else
                {
                    m_modelMode = variant.value<ModelMode>();
                }
                break;
            // no setter indexes ignored
            case IndexHasQueriedModelString: break;
            case IndexModelTypeAsString:     break;
            case IndexModelModeAsString:     break;
            case IndexModelModeAsIcon:       break;
            case IndexFileTimestampFormattedYmdhms: break;
            case IndexSimulatorInfoAsString: break;
            case IndexMembersDbStatus:       break;

            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAircraftModel::comparePropertyByIndex(CPropertyIndexRef index, const CAircraftModel &compareValue) const
        {
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(index, compareValue);}
            if (IOrderable::canHandleIndex(index)) { return IOrderable::comparePropertyByIndex(index, compareValue);}
            if (index.isMyself()) { return m_modelString.compare(compareValue.getModelString(), Qt::CaseInsensitive); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:      return m_modelString.compare(compareValue.getModelString(), Qt::CaseInsensitive);
            case IndexModelStringAlias: return m_modelStringAlias.compare(compareValue.getModelStringAlias(), Qt::CaseInsensitive);
            case IndexAllModelStrings:  return this->getAllModelStringsAndAliases().compare(compareValue.getAllModelStringsAndAliases(), Qt::CaseInsensitive);
            case IndexHasQueriedModelString: return Compare::compare(this->hasQueriedModelString(), compareValue.hasQueriedModelString());
            case IndexAircraftIcaoCode: return m_aircraftIcao.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAircraftIcaoCode());
            case IndexLivery:           return m_livery.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getLivery());
            case IndexDistributor:      return m_distributor.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getDistributor());
            case IndexDescription:      return m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
            case IndexName:             return m_name.compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexCallsign:         return m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexFileName:         return m_fileName.compare(compareValue.getFileName(), Qt::CaseInsensitive);
            case IndexIconPath:         return m_iconFile.compare(compareValue.getIconFile(), Qt::CaseInsensitive);
            case IndexCG:               return m_cg.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCG());
            case IndexSupportedParts:   return m_supportedParts.compare(compareValue.getSupportedParts());
            case IndexModelTypeAsString:
            case IndexModelType: return Compare::compare(m_modelType, compareValue.getModelType());
            case IndexSimulatorInfoAsString:
            case IndexSimulatorInfo: return m_simulator.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getSimulator());
            case IndexFileTimestamp:
            case IndexFileTimestampFormattedYmdhms: return Compare::compare(m_fileTimestamp, compareValue.m_fileTimestamp);
            case IndexModelMode:
            case IndexModelModeAsString:
            case IndexModelModeAsIcon: return Compare::compare(m_modelMode, compareValue.getModelMode());
            case IndexMembersDbStatus: return getMembersDbStatus().compare(compareValue.getMembersDbStatus());
            default: break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
            return 0;
        }

        bool CAircraftModel::setAircraftIcaoCode(const CAircraftIcaoCode &aircraftIcaoCode)
        {
            if (m_aircraftIcao == aircraftIcaoCode) { return false; }
            m_aircraftIcao = aircraftIcaoCode;
            return true;
        }

        void CAircraftModel::setAircraftIcaoDesignator(const QString &designator)
        {
            m_aircraftIcao.setDesignator(designator);
        }

        void CAircraftModel::setAircraftIcaoCodes(const CAircraftIcaoCode &aircraftIcaoCode, const CAirlineIcaoCode &airlineIcaoCode)
        {
            m_aircraftIcao = aircraftIcaoCode;
            m_livery.setAirlineIcaoCode(airlineIcaoCode);
        }

        bool CAircraftModel::hasValidAircraftAndAirlineDesignator() const
        {
            return this->hasKnownAircraftDesignator() && m_livery.hasValidAirlineDesignator();
        }

        bool CAircraftModel::hasAircraftDesignator() const
        {
            return m_aircraftIcao.hasDesignator();
        }

        bool CAircraftModel::hasKnownAircraftDesignator() const
        {
            return m_aircraftIcao.hasKnownDesignator();
        }

        bool CAircraftModel::hasCategory() const
        {
            return m_aircraftIcao.hasCategory();
        }

        bool CAircraftModel::hasAirlineDesignator() const
        {
            return m_livery.hasValidAirlineDesignator();
        }

        bool CAircraftModel::hasAircraftAndAirlineDesignator() const
        {
            return this->hasAircraftDesignator() && this->hasAirlineDesignator();
        }

        bool CAircraftModel::isMilitary() const
        {
            return this->getAircraftIcaoCode().isMilitary() ||
                   this->getLivery().isMilitary();
        }

        bool CAircraftModel::isCivilian() const
        {
            return !this->isMilitary();
        }

        bool CAircraftModel::setDistributorOrder(int order)
        {
            if (order < 0) { return false; }
            if (!m_distributor.isLoadedFromDb()) { return false; }
            m_distributor.setOrder(order);
            return true;
        }

        bool CAircraftModel::setDistributorOrder(const CDistributorList &distributors)
        {
            if (distributors.isEmpty()) { return false; }
            bool found = false;
            const int noDistributorOrder = distributors.size();
            if (this->hasDbDistributor())
            {
                const CDistributor d = distributors.findByKeyOrAlias(m_distributor.getDbKey());
                if (d.hasValidDbKey())
                {
                    m_distributor.setOrder(d.getOrder());
                    found = true;
                }
                else
                {
                    m_distributor.setOrder(noDistributorOrder);
                }
            }
            else
            {
                m_distributor.setOrder(noDistributorOrder);
            }
            return found;
        }

        bool CAircraftModel::hasDbDistributor() const
        {
            return m_distributor.isLoadedFromDb();
        }

        bool CAircraftModel::hasDistributor() const
        {
            return m_distributor.hasValidDbKey(); // key is valid, but not guaranteed from DB
        }

        bool CAircraftModel::matchesDbDistributor(const CDistributor &distributor) const
        {
            if (!distributor.isLoadedFromDb()) { return false; }
            if (!this->hasDbDistributor()) { return false; }
            return m_distributor.getDbKey() == distributor.getDbKey();
        }

        bool CAircraftModel::matchesAnyDbDistributor(const CDistributorList &distributors) const
        {
            if (distributors.isEmpty()) { return false; }
            if (!this->hasDbDistributor()) { return false; }
            return distributors.matchesAnyKeyOrAlias(m_distributor.getDbKey());
        }

        void CAircraftModel::setSupportedParts(const QString &supportedParts)
        {
            m_supportedParts = CAircraftModel::cleanUpPartsString(supportedParts);
        }

        bool CAircraftModel::matchesMode(ModelModeFilter mode) const
        {
            if (mode == All) { return true; }
            return (mode & m_modelMode) > 0;
        }

        const CIcon &CAircraftModel::getModelModeAsIcon() const
        {
            switch (this->getModelMode())
            {
            case Include:   return CIcon::iconByIndex(CIcons::ModelInclude);
            case Exclude:   return CIcon::iconByIndex(CIcons::ModelExclude);
            case Undefined: return CIcon::iconByIndex(CIcons::StandardIconUnknown16);
            default:
                Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
                break;
            }
            return CIcon::iconByIndex(CIcons::ModelInclude);
        }

        void CAircraftModel::setModelModeAsString(const QString &mode)
        {
            this->setModelMode(CAircraftModel::modelModeFromString(mode));
        }

        bool CAircraftModel::matchesSimulator(const CSimulatorInfo &simulator) const
        {
            return (static_cast<int>(simulator.getSimulator()) & static_cast<int>(this->getSimulator().getSimulator())) > 0;
        }

        bool CAircraftModel::matchesSimulatorFlag(CSimulatorInfo::Simulator simulator) const
        {
            return (static_cast<int>(simulator) & static_cast<int>(this->getSimulator().getSimulator())) > 0;
        }

        QDateTime CAircraftModel::getFileTimestamp() const
        {
            return this->hasValidFileTimestamp() ? QDateTime::fromMSecsSinceEpoch(m_fileTimestamp, Qt::UTC) : QDateTime();
        }

        QString CAircraftModel::getFormattedFileTimestampYmdhms() const
        {
            return this->hasValidFileTimestamp() ?
                   this->getFileTimestamp().toString("yyyy-MM-dd HH:mm:ss") :
                   "";
        }

        bool CAircraftModel::hasValidFileTimestamp() const
        {
            return m_fileTimestamp >= 0;
        }

        void CAircraftModel::setFileTimestamp(const QDateTime &timestampUtc)
        {
            m_fileTimestamp = timestampUtc.isValid() ? timestampUtc.toMSecsSinceEpoch() : -1;
        }

        void CAircraftModel::setFileTimestamp(qint64 timestamp)
        {
            m_fileTimestamp = (timestamp < 0) ? -1 : timestamp;
        }

        void CAircraftModel::setFileDetailsAndTimestamp(const QFileInfo &fileInfo)
        {
            this->setFileName(fileInfo.absoluteFilePath());
            const QDateTime modified = fileInfo.lastModified();
            if (modified.isValid())
            {
                this->setFileTimestamp(modified);
                this->setUtcTimestamp(modified);
            }
            else
            {
                const QDateTime created = fileInfo.lastModified();
                this->setFileTimestamp(created);
                this->setUtcTimestamp(created);
            }
        }

        CPixmap CAircraftModel::loadIcon(CStatusMessage &success) const
        {
            static const CStatusMessage noIcon(this, CStatusMessage::SeverityInfo, u"no icon");
            static const CStatusMessage loaded(this, CStatusMessage::SeverityInfo, u"icon loaded");
            if (m_iconFile.isEmpty()) { success = noIcon; return CPixmap(); }
            if (!QFile(m_iconFile).exists()) { success = noIcon; return CPixmap(); }

            QPixmap pm;
            if (!pm.load(m_iconFile)) { success = noIcon; return CPixmap(); }
            success = loaded;
            return pm;
        }

        const QString &CAircraftModel::liveryStringPrefix()
        {
            static const QString p("swift_");
            return p;
        }

        bool CAircraftModel::isSwiftLiveryString(const QString &liveryString)
        {
            return (liveryString.length() > liveryStringPrefix().length() && liveryString.startsWith(liveryStringPrefix(), Qt::CaseInsensitive));
        }

        QString CAircraftModel::getSwiftLiveryString(bool aircraftIcao, bool livery, bool model) const
        {
            if (!aircraftIcao && !livery && !model) { return QString(); }
            const QString l =
                (livery       && this->getLivery().hasValidDbKey() ? u'l' % this->getLivery().getDbKeyAsString() : QString()) %
                (aircraftIcao && this->getAircraftIcaoCode().hasValidDbKey() ? QStringLiteral("a") % this->getAircraftIcaoCode().getDbKeyAsString() : QString()) %
                (model        && this->hasValidDbKey() ? u'm' % this->getDbKeyAsString() : QString());

            return l.isEmpty() ? QString() : liveryStringPrefix() % l;
        }

        QString CAircraftModel::getSwiftLiveryString(const CSimulatorInfo &sim) const
        {
            return (sim.isFG()) ?
                   this->getSwiftLiveryString(true, false, false) :
                   this->getSwiftLiveryString();
        }

        DBTripleIds CAircraftModel::parseNetworkLiveryString(const QString &liveryString)
        {
            // "swift_m22l33a11"
            if (!CAircraftModel::isSwiftLiveryString(liveryString)) { return DBTripleIds(); }

            DBTripleIds ids;
            const QString ls = liveryString.mid(liveryStringPrefix().length()).toLower();
            for (int c = 0; c < ls.length(); c++)
            {
                const QChar m = ls[c];
                if ((m == 'm' || m == 'a' || m == 'l') && (c + 1) < ls.length())
                {
                    const int cs = c + 1;
                    int cc = cs;
                    while (cc < ls.length() && ls[cc].isDigit()) { cc++; } // find end of id
                    if (cc > cs)
                    {
                        const QString idString = ls.mid(cs, cc - cs);
                        const int id = idString.toInt();
                        c = cc - 1; // +1 again in for

                        if (m == 'm')      { ids.model    = id; }
                        else if (m == 'a') { ids.aircraft = id; }
                        else if (m == 'l') { ids.livery   = id; }
                    }
                }
            }
            return ids;
        }

        void CAircraftModel::updateMissingParts(const CAircraftModel &otherModel, bool dbModelPriority)
        {
            if (dbModelPriority && !this->hasValidDbKey() && otherModel.hasValidDbKey())
            {
                // we have no DB data, but the other one has
                // so we change roles. We take the DB object as base, and update our parts
                CAircraftModel copy(otherModel);
                copy.updateMissingParts(*this);
                *this = copy;
                return;
            }

            // local file names and file timestamp
            this->updateLocalFileNames(otherModel);
            if (m_fileTimestamp < 0 || otherModel.m_fileTimestamp > m_fileTimestamp) { this->setFileTimestamp(otherModel.getFileTimestamp()); }

            // both are DB data, treat as being the same except for filename maybe
            if (this->hasValidDbKey() && otherModel.hasValidDbKey()) { return; }

            // update attributes where applicable
            if (m_callsign.isEmpty())       { this->setCallsign(otherModel.getCallsign()); }
            if (m_modelString.isEmpty())    { this->setModelString(otherModel.getModelString()); }
            if (m_name.isEmpty())           { this->setName(otherModel.getName()); }
            if (m_modelType == TypeUnknown) { m_modelType = otherModel.getModelType(); }
            if (m_modelMode == Undefined)   { m_modelType = otherModel.getModelType(); }
            if (m_description.isEmpty() || m_description.startsWith(CAircraftModel::autoGenerated(), Qt::CaseInsensitive)) { this->setDescription(otherModel.getDescription()); }
            if (this->getSimulator().isUnspecified())
            {
                // simulator can only be overridden as simulators can also be removed
                this->setSimulator(otherModel.getSimulator());
            }

            ITimestampBased::updateMissingParts(otherModel);
            m_livery.updateMissingParts(otherModel.getLivery());
            m_aircraftIcao.updateMissingParts(otherModel.getAircraftIcaoCode());
            m_distributor.updateMissingParts(otherModel.getDistributor());
        }

        void CAircraftModel::updateByExistingDirectories(const CAircraftModel &otherModel)
        {
            if (otherModel.hasExistingCorrespondingFile()) { this->setFileName(otherModel.getFileName()); }
            if (otherModel.hasExistingIconFile()) { this->setIconFile(otherModel.getIconFile()); }
        }

        bool CAircraftModel::hasQueriedModelString() const
        {
            return m_modelType == TypeQueriedFromNetwork && this->hasModelString();
        }

        bool CAircraftModel::hasManuallySetString() const
        {
            return m_modelType == TypeManuallySet && this->hasModelString();
        }

        bool CAircraftModel::hasDescription(bool ignoreAutoGenerated) const
        {
            if (m_description.isEmpty()) { return false; }
            if (!ignoreAutoGenerated) { return true; }
            return (!this->getDescription().startsWith(autoGenerated(), Qt::CaseInsensitive));
        }

        bool CAircraftModel::hasValidSimulator() const
        {
            return m_simulator.isAnySimulator();
        }

        QString CAircraftModel::getMembersDbStatus() const
        {
            return
                (this->isLoadedFromDb() ? QStringLiteral("M") : QStringLiteral("m")) %
                (this->getDistributor().isLoadedFromDb() ? QStringLiteral("D") : QStringLiteral("d")) %
                (this->getAircraftIcaoCode().isLoadedFromDb() ? QStringLiteral("A") : QStringLiteral("a")) %
                (this->getLivery().isLoadedFromDb() && getLivery().isColorLivery() ?
                 QStringLiteral("C-") :
                 (this->getLivery().isLoadedFromDb() ? QStringLiteral("L") : QStringLiteral("l")) %
                 (this->getLivery().getAirlineIcaoCode().isLoadedFromDb() ? QStringLiteral("A") : QStringLiteral("a")));
        }

        void CAircraftModel::normalizeFileNameForDb()
        {
            m_fileName = CAircraftModel::normalizeFileNameForDb(m_fileName);
        }

        void CAircraftModel::updateLocalFileNames(const CAircraftModel &model)
        {
            if (this->getModelType() == CAircraftModel::TypeOwnSimulatorModel)
            {
                // this is already a local model, ignore
                return;
            }

            if (model.getModelType() == CAircraftModel::TypeOwnSimulatorModel)
            {
                // other local, priority
                this->setFileName(model.getFileName());
                this->setIconFile(model.getIconFile());
                return;
            }

            // both not local, override empty values
            if (m_fileName.isEmpty()) { this->setFileName(model.getFileName()); }
            if (m_iconFile.isEmpty()) { this->setIconFile(model.getIconFile()); }
        }

        bool CAircraftModel::adjustLocalFileNames(const QString &newModelDir, const QString &stripModelDirIndicator)
        {
            if (!this->hasFileName()) { return false; }
            const QString md = CFileUtils::normalizeFilePathToQtStandard(newModelDir);
            int i = -1;
            if (stripModelDirIndicator.isEmpty())
            {
                QString strip = md.mid(md.lastIndexOf('/'));
                i = m_fileName.lastIndexOf(strip);
            }
            else
            {
                i = m_fileName.lastIndexOf(stripModelDirIndicator);
            }
            if (i < 0) { return false; }
            m_fileName = CFileUtils::appendFilePaths(newModelDir, m_fileName.mid(i));
            return true;
        }

        bool CAircraftModel::hasExistingCorrespondingFile() const
        {
            if (!this->hasFileName()) { return false; }
            const QFileInfo fi(CFileUtils::fixWindowsUncPath(this->getFileName()));
            if (!fi.exists()) { return false; }
            const bool r = fi.isReadable();
            return r;
        }

        QDir CAircraftModel::getFileDirectory() const
        {
            if (!this->hasFileName()) { return QDir(); }
            const QFileInfo fi(CFileUtils::fixWindowsUncPath(this->getFileName()));
            return fi.absoluteDir();
        }

        QString CAircraftModel::getFileDirectoryPath() const
        {
            if (!this->hasFileName()) { return QStringLiteral(""); }
            return this->getFileDirectory().absolutePath();
        }

        bool CAircraftModel::isInPath(const QString &path, Qt::CaseSensitivity cs) const
        {
            const QString p(this->getFileDirectoryPath());
            if (path.isEmpty() || p.isEmpty()) { return false; }
            if (path.startsWith('/'))
            {
                if (path.endsWith('/')) { return p.contains(path.mid(1, path.length() - 2), cs); }
                return p.contains(path.mid(1));
            }
            if (path.endsWith('/')) { return p.contains(path.left(path.length() - 1), cs); }
            return (p.contains(path, cs));
        }

        bool CAircraftModel::hasExistingIconFile() const
        {
            if (m_iconFile.isEmpty()) { return false; }
            const QFileInfo fi(m_iconFile);
            return fi.exists();
        }

        bool CAircraftModel::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return stringCompare(modelString, m_modelString, sensitivity);
        }

        bool CAircraftModel::matchesModelStringOrAlias(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            if (this->matchesModelString(modelString, sensitivity)) { return true; }
            return stringCompare(modelString, m_modelStringAlias, sensitivity);
        }

        int CAircraftModel::calculateScore(const CAircraftModel &compareModel, bool preferColorLiveries, CStatusMessageList *log) const
        {
            const int icaoScore = this->getAircraftIcaoCode().calculateScore(compareModel.getAircraftIcaoCode(), log);
            const int liveryScore = this->getLivery().calculateScore(compareModel.getLivery(), preferColorLiveries, log);
            CCallsign::addLogDetailsToList(log, this->getCallsign(), QStringLiteral("ICAO score: %1 | livery score: %2").arg(icaoScore).arg(liveryScore));
            return qRound(0.5 * (icaoScore + liveryScore));
        }

        CStatusMessageList CAircraftModel::validate(bool withNestedObjects) const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
            CStatusMessageList msgs;
            if (!hasModelString()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Model: missing model string (aka key)")); }
            if (!hasValidSimulator()) {msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Model: no simulator set")); }
            // as of T34 made description optional, lines can be removed after 6/2017
            // if (!hasDescription()) {msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, u"Model: no description")); }
            if (withNestedObjects)
            {
                msgs.push_back(m_aircraftIcao.validate());
                msgs.push_back(m_livery.validate());
                msgs.push_back(m_distributor.validate());
            }
            return msgs;
        }

        //! Create an appropriate status message for an attribute comparison
        CStatusMessage equalMessage(bool same, const CAircraftModel &model, const QString &description, const QString &oldValue, const QString &newValue)
        {
            if (same)
            {
                static const CStatusMessage msgSame({ CLogCategories::validation() }, CStatusMessage::SeverityWarning, u"Model '%1' same %2 '%3'");
                return CStatusMessage(msgSame) << model.getModelStringAndDbKey() << description << newValue;
            }
            else
            {
                static const CStatusMessage msgDiff({ CLogCategories::validation() }, CStatusMessage::SeverityInfo, u"Model '%1' changed %2 '%3'->'%4'");
                return CStatusMessage(msgDiff) << model.getModelStringAndDbKey() << description << oldValue << newValue;
            }
        }

        bool CAircraftModel::isEqualForPublishing(const CAircraftModel &dbModel, CStatusMessageList *details) const
        {
            if (!dbModel.isLoadedFromDb())
            {
                static const CStatusMessage msgNoDbModel({ CLogCategories::validation() }, CStatusMessage::SeverityInfo, u"No DB model yet");
                if (details) { details->push_back(msgNoDbModel); }
                return false;
            }

            CStatusMessageList validationMsgs;
            bool changed = false;
            bool equal = dbModel.getLivery().isLoadedFromDb() && dbModel.getLivery().isDbEqual(this->getLivery());
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("livery"), dbModel.getLivery().getCombinedCodePlusInfoAndId(), this->getLivery().getCombinedCodePlusInfoAndId())); }
            changed |= !equal;

            equal = dbModel.getAircraftIcaoCode().isLoadedFromDb() && dbModel.getAircraftIcaoCode().isDbEqual(this->getAircraftIcaoCode());
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("aircraft ICAO"), dbModel.getAircraftIcaoCode().getDesignatorDbKey(), this->getAircraftIcaoCode().getDesignatorDbKey())); }
            changed |= !equal;

            equal = dbModel.getDistributor().isLoadedFromDb() && dbModel.getDistributor().isDbEqual(this->getDistributor());
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("distributor"), dbModel.getDistributor().getDescription(), this->getDistributor().getDescription())); }
            changed |= !equal;

            equal = dbModel.getSimulator() == this->getSimulator();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("simulator"), dbModel.getSimulator().toQString(), this->getSimulator().toQString())); }
            changed |= !equal;

            equal = dbModel.getDescription() == this->getDescription();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("description"), dbModel.getDescription(), this->getDescription())); }
            changed |= !equal;

            equal = dbModel.getName() == this->getName();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("name"), dbModel.getName(), this->getName())); }
            changed |= !equal;

            equal = dbModel.getModelMode() == this->getModelMode();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("mode"), dbModel.getModelModeAsString(), this->getModelModeAsString())); }
            changed |= !equal;

            equal = dbModel.getCG() == this->getCG();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("CG"), dbModel.getCG().toQString(true), this->getCG().toQString(true))); }
            changed |= !equal;

            equal = dbModel.getSupportedParts() == this->getSupportedParts();
            if (details) { validationMsgs.push_back(equalMessage(equal, *this, QStringLiteral("Supported parts"), dbModel.getSupportedParts(), this->getSupportedParts())); }
            changed |= !equal;

            // clean messages
            if (changed && details)
            {
                // we have a changed entity, remove the warnings as they are just noise
                validationMsgs.removeSeverity(CStatusMessage::SeverityWarning);
            }

            if (details) { details->push_back(validationMsgs); }
            return !changed;
        }

        const QString &CAircraftModel::modelTypeToString(CAircraftModel::ModelType type)
        {
            static const QString queried("queried");
            static const QString matching("matching");
            static const QString db("database");
            static const QString def("map.default");
            static const QString ownSim("own simulator");
            static const QString set("manually set");
            static const QString fsinn("FSInn");
            static const QString probe("probe");
            static const QString reverse("reverse lookup");
            static const QString unknown("unknown");

            switch (type)
            {
            case TypeQueriedFromNetwork: return queried;
            case TypeModelMatching:      return matching;
            case TypeDatabaseEntry:      return db;
            case TypeManuallySet:        return set;
            case TypeFSInnData:          return fsinn;
            case TypeTerrainProbe:       return probe;
            case TypeReverseLookup:      return reverse;
            case TypeOwnSimulatorModel:  return ownSim;
            case TypeModelMatchingDefaultModel: return def;
            case TypeUnknown:
            default: return unknown;
            }
        }

        QString CAircraftModel::normalizeFileNameForDb(const QString &filePath)
        {
            const QString n = CFileUtils::normalizeFilePathToQtStandard(filePath).toUpper();
            if (n.count('/') < 2) { return n; }
            return n.section('/', -2, -1);
        }

        CAircraftModel::ModelMode CAircraftModel::modelModeFromString(const QString &mode)
        {
            if (mode.isEmpty() || mode.startsWith('I', Qt::CaseInsensitive)) { return Include;}
            if (mode.startsWith('E', Qt::CaseInsensitive)) { return Exclude; }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, "wrong mode");
            return Include; // default
        }

        const QString &CAircraftModel::modelModeToString(CAircraftModel::ModelMode mode)
        {
            static const QString i("Include");
            static const QString e("Exclude");

            switch (mode)
            {
            case Include: return i;
            case Exclude: return e;
            default: Q_ASSERT_X(false, Q_FUNC_INFO, "wrong mode");
            }
            return i; // default
        }

        CAircraftModel CAircraftModel::fromDatabaseJsonBaseImpl(const QJsonObject &json, const QString &prefix, const CAircraftIcaoCode &aircraftIcao, const CLivery &livery, const CDistributor &distributor)
        {
            const QString modelString(json.value(prefix % u"modelstring").toString());
            const QString modelStringAlias(json.value(prefix % u"modelstringalias").toString());
            const QString modelDescription(json.value(prefix % u"description").toString());
            const QString modelName(json.value(prefix % u"name").toString());
            const QString modelMode(json.value(prefix % u"mode").toString());
            const QString parts(json.value(prefix % u"parts").toString());

            // check for undefined to rule out 0ft values
            const QJsonValue cgjv = json.value(prefix % u"cgft");
            const CLength cg = (cgjv.isNull() || cgjv.isUndefined()) ? CLength::null() : CLength(cgjv.toDouble(), CLengthUnit::ft());

            const CSimulatorInfo simInfo = CSimulatorInfo::fromDatabaseJson(json, prefix);
            CAircraftModel model(modelString, CAircraftModel::TypeDatabaseEntry, simInfo, modelName, modelDescription);
            model.setModelStringAlias(modelStringAlias);
            model.setModelModeAsString(modelMode);
            model.setSupportedParts(parts);
            model.setCG(cg);
            model.setKeyVersionTimestampFromDatabaseJson(json, prefix);
            model.setDistributor(distributor);
            model.setAircraftIcaoCode(aircraftIcao);
            model.setLivery(livery);
            return model;
        }

        CAircraftModel CAircraftModel::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            static const QString prefixAircraftIcao("ac_");
            static const QString prefixLivery("liv_");
            static const QString prefixDistributor("dist_");
            const QString idDistributor = json.value(prefixDistributor % u"id").toString();
            const int idAircraftIcao = json.value(prefixAircraftIcao % u"id").toInt(-1);
            const int idLivery = json.value(prefixLivery % u"id").toInt(-1);

            CDistributor distributor(CDistributor::fromDatabaseJson(json, prefixDistributor));
            CAircraftIcaoCode aircraftIcao(CAircraftIcaoCode::fromDatabaseJson(json, prefixAircraftIcao));
            CLivery livery(CLivery::fromDatabaseJson(json, prefixLivery));

            if (!aircraftIcao.isLoadedFromDb() && idAircraftIcao >= 0) { aircraftIcao.setDbKey(idAircraftIcao); }
            if (!livery.isLoadedFromDb() && idLivery >= 0) { livery.setDbKey(idLivery); }
            if (!distributor.isLoadedFromDb() && !idDistributor.isEmpty()) { distributor.setDbKey(idDistributor); }

            // full model
            return CAircraftModel::fromDatabaseJsonBaseImpl(json, prefix, aircraftIcao, livery, distributor);
        }

        CAircraftModel CAircraftModel::fromDatabaseJsonCaching(
            const QJsonObject &json,
            AircraftIcaoIdMap &aircraftIcaos, const AircraftCategoryIdMap &categories, LiveryIdMap &liveries, DistributorIdMap &distributors,
            const QString &prefix)
        {
            static const QString prefixAircraftIcao("ac_");
            static const QString prefixLivery("liv_");
            static const QString prefixDistributor("dist_");
            const QString idDistributor = json.value(prefixDistributor % u"id").toString();
            const int idAircraftIcao = json.value(prefixAircraftIcao % u"id").toInt(-1);
            const int idLivery = json.value(prefixLivery % u"id").toInt(-1);

            const bool cachedAircraftIcao = (idAircraftIcao >= 0) && aircraftIcaos.contains(idAircraftIcao);
            const bool cachedLivery = (idLivery >= 0) && liveries.contains(idLivery);
            const bool cachedDistributor = !idDistributor.isEmpty() && distributors.contains(idDistributor);

            CAircraftIcaoCode aircraftIcao(cachedAircraftIcao ?
                                           aircraftIcaos[idAircraftIcao] :
                                           CAircraftIcaoCode::fromDatabaseJson(json, prefixAircraftIcao));

            CLivery livery(cachedLivery ?
                           liveries[idLivery] :
                           CLivery::fromDatabaseJson(json, prefixLivery));

            CDistributor distributor(cachedDistributor ?
                                     distributors[idDistributor] :
                                     CDistributor::fromDatabaseJson(json, prefixDistributor));

            if (!aircraftIcao.isLoadedFromDb() && idAircraftIcao >= 0) { aircraftIcao.setDbKey(idAircraftIcao); }
            if (!livery.isLoadedFromDb() && idLivery >= 0) { livery.setDbKey(idLivery); }
            if (!distributor.isLoadedFromDb() && !idDistributor.isEmpty()) { distributor.setDbKey(idDistributor); }

            // update category
            if (!cachedAircraftIcao)
            {
                const int catId = aircraftIcao.getCategory().getDbKey();
                if (catId >= 0 && categories.contains(catId))
                {
                    aircraftIcao.setCategory(categories[catId]);
                }
            }

            // store in temp.cache
            if (!cachedAircraftIcao && aircraftIcao.isLoadedFromDb()) { aircraftIcaos[aircraftIcao.getDbKey()] = aircraftIcao; }
            if (!cachedLivery && livery.isLoadedFromDb()) { liveries[livery.getDbKey()] = livery; }
            if (!cachedDistributor && distributor.isLoadedFromDb()) { distributors[distributor.getDbKey()] = distributor; }

            // full model
            return CAircraftModel::fromDatabaseJsonBaseImpl(json, prefix, aircraftIcao, livery, distributor);
        }

        const QString &CAircraftModel::autoGenerated()
        {
            static const QString ag("swift auto generated");
            return ag;
        }

        QString CAircraftModel::cleanUpPartsString(const QString &p)
        {
            if (p.isEmpty()) { return QString(); }
            QString pc = removeChars(p.toUpper(), [](QChar c) { return !supportedParts().contains(c); });
            std::sort(pc.begin(), pc.end());
            return pc;
        }

        const QString &CAircraftModel::supportedParts()
        {
            static const QString p("EFGLS");
            return p;
        }
    } // namespace
} // namespace
