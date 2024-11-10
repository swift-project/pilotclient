// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/guiutility.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/views/viewbaseproxystyle.h"
#include "blackgui/views/viewbaseitemdelegate.h"
#include "blackgui/models/allmodels.h"
#include "blackgui/components/texteditdialog.h"
#include "misc/worker.h"
#include "config/buildconfig.h"

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QTextEdit>
#include <QStringBuilder>

using namespace swift::misc;
using namespace BlackGui;
using namespace BlackGui::Menus;
using namespace BlackGui::Models;
using namespace BlackGui::Filters;
using namespace BlackGui::Settings;
using namespace BlackGui::Components;

namespace BlackGui::Views
{
    template <class T>
    CViewBase<T>::CViewBase(QWidget *parent, ModelClass *model) : CViewBaseNonTemplate(parent), m_model(model)
    {
        this->setSortingEnabled(true);
        if (model)
        {
            this->standardInit(model);
        }
    }

    template <class T>
    int CViewBase<T>::updateContainer(const ContainerType &container, bool sort, bool resize)
    {
        Q_ASSERT_X(m_model, Q_FUNC_INFO, "Missing model");
        if (container.isEmpty())
        {
            // shortcut
            this->clear();
            return 0;
        }

        // we have data
        this->showLoadIndicator(container.size());
        const bool reallyResize = resize && isResizeConditionMet(container.size()); // do we really perform resizing
        bool presize = (m_resizeMode == PresizeSubset) &&
                       this->isEmpty() && // only when no data yet
                       !reallyResize; // not when we resize later
        presize = presize || (this->isEmpty() && resize && !reallyResize); // we presize if we wanted to resize but actually do not because of condition
        const bool presizeThresholdReached = presize && container.size() > ResizeSubsetThreshold; // only when size making sense

        // when we will not resize, we might presize
        if (presizeThresholdReached)
        {
            const int presizeRandomElements = this->getPresizeRandomElementsSize(container.size());
            if (presizeRandomElements > 0)
            {
                m_model->update(container.sampleElements(presizeRandomElements), false);
                this->fullResizeToContents();
            }
        }

        const int c = m_model->update(container, sort);

        // resize after real update according to mode
        if (presizeThresholdReached) // cppcheck-suppress knownConditionTrueFalse
        {
            // currently no furhter actions
        }
        else if (reallyResize)
        {
            this->resizeToContents(); // mode based resize
        }
        else if (presize && !presizeThresholdReached) // cppcheck-suppress knownConditionTrueFalse
        {
            // small amount of data not covered before
            this->fullResizeToContents();
        }
        this->updateSortIndicator(); // make sure sort indicator represents sort order
        this->hideLoadIndicator();
        return c;
    }

    template <class T>
    CWorker *CViewBase<T>::updateContainerAsync(const ContainerType &container, bool sort, bool resize)
    {
        // avoid unnecessary effort when empty
        if (container.isEmpty())
        {
            this->clear();
            return nullptr;
        }

        Q_UNUSED(sort)
        ModelClass *model = this->derivedModel();
        const auto sortColumn = model->getSortColumn();
        const auto sortOrder = model->getSortOrder();
        this->showLoadIndicator(container.size());
        CWorker *worker = CWorker::fromTask(this, "ViewSort", [model, container, sortColumn, sortOrder]() {
            return model->sortContainerByColumn(container, sortColumn, sortOrder);
        });
        worker->thenWithResult<ContainerType>(this, [this, resize](const ContainerType &sortedContainer) {
            this->updateContainer(sortedContainer, false, resize);
        });
        worker->then(this, &CViewBase::asyncUpdateFinished);
        return worker;
    }

    template <class T>
    void CViewBase<T>::updateContainerMaybeAsync(const ContainerType &container, bool sort, bool resize)
    {
        if (container.isEmpty())
        {
            this->clear();
        }
        else if (container.size() > ASyncRowsCountThreshold && sort)
        {
            // larger container with sorting
            this->updateContainerAsync(container, sort, resize);
        }
        else
        {
            this->updateContainer(container, sort, resize);
        }
    }

    template <class T>
    void CViewBase<T>::insert(const ObjectType &value, bool resize)
    {
        Q_ASSERT(m_model);
        if (this->rowCount() < 1)
        {
            // this allows presizing
            this->updateContainerMaybeAsync(ContainerType({ value }), true, resize);
        }
        else
        {
            m_model->insert(value);
            if (resize) { this->performModeBasedResizeToContent(); }
        }
    }

    template <class T>
    void CViewBase<T>::insert(const ContainerType &container, bool resize)
    {
        Q_ASSERT(m_model);
        if (this->rowCount() < 1)
        {
            // this allows presizing
            this->updateContainerMaybeAsync(container, true, resize);
        }
        else
        {
            m_model->insert(container);
            if (resize) { this->performModeBasedResizeToContent(); }
        }
    }

    template <class T>
    void CViewBase<T>::push_back(const ObjectType &value, bool resize)
    {
        Q_ASSERT(m_model);
        if (this->rowCount() < 1)
        {
            // this allows presizing
            this->updateContainerMaybeAsync(ContainerType({ value }), true, resize);
        }
        else
        {
            m_model->push_back(value);
            if (resize) { this->performModeBasedResizeToContent(); }
        }
    }

    template <class T>
    void CViewBase<T>::push_back(const ContainerType &container, bool resize)
    {
        Q_ASSERT(m_model);
        if (this->rowCount() < 1)
        {
            // this allows presizing
            this->updateContainerMaybeAsync(container, true, resize);
        }
        else
        {
            m_model->push_back(container);
            if (resize) { this->performModeBasedResizeToContent(); }
        }
    }

    template <class T>
    const typename CViewBase<T>::ObjectType &CViewBase<T>::at(const QModelIndex &index) const
    {
        Q_ASSERT(m_model);
        return m_model->at(index);
    }

    template <class T>
    const typename CViewBase<T>::ContainerType &CViewBase<T>::container() const
    {
        Q_ASSERT(m_model);
        return m_model->container();
    }

    template <class T>
    QList<int> CViewBase<T>::rowsOf(const ContainerType &container) const
    {
        QList<int> rows;
        for (const ObjectType &o : container)
        {
            const int i = this->rowOf(o);
            if (i >= 0) { rows.push_back(i); }
        }
        return rows;
    }

    template <class T>
    int CViewBase<T>::rowOf(const ObjectType &obj) const
    {
        //! \fixme KB 4-19 any smarter solution?
        const ContainerType objects = m_model->containerOrFilteredContainer();
        int i = 0;
        for (const ObjectType &o : objects)
        {
            if (o == obj) { return i; }
            ++i;
        }
        return -1;
    }

    template <class T>
    const typename CViewBase<T>::ContainerType &CViewBase<T>::containerOrFilteredContainer(bool *filtered) const
    {
        Q_ASSERT(m_model);
        return m_model->containerOrFilteredContainer(filtered);
    }

    template <class T>
    typename CViewBase<T>::ContainerType CViewBase<T>::selectedObjects() const
    {
        if (!this->hasSelection()) { return ContainerType(); }
        ContainerType c;
        const QModelIndexList indexes = this->selectedRows();
        for (const QModelIndex &i : indexes)
        {
            c.push_back(this->at(i));
        }
        return c;
    }

    template <class T>
    typename CViewBase<T>::ContainerType CViewBase<T>::unselectedObjects() const
    {
        if (!this->hasSelection()) { return this->containerOrFilteredContainer(); }
        ContainerType c;
        const QModelIndexList indexes = this->unselectedRows();
        for (const QModelIndex &i : indexes)
        {
            c.push_back(this->at(i));
        }
        return c;
    }

    template <class T>
    typename CViewBase<T>::ObjectType CViewBase<T>::firstSelectedOrDefaultObject() const
    {
        if (this->hasSelection())
        {
            return this->selectedObjects().front();
        }
        if (this->rowCount() < 2)
        {
            return this->containerOrFilteredContainer().frontOrDefault();
        }

        // too many, not selected
        return ObjectType();
    }

    template <class T>
    int CViewBase<T>::updateSelected(const CPropertyIndexVariantMap &vm)
    {
        if (vm.isEmpty()) { return 0; }
        if (!hasSelection()) { return 0; }
        int c = 0;

        int lastUpdatedRow = -1;
        int firstUpdatedRow = -1;
        const CPropertyIndexList propertyIndexes(vm.indexes());
        const QModelIndexList indexes = this->selectedRows();

        for (const QModelIndex &i : indexes)
        {
            if (i.row() == lastUpdatedRow) { continue; }
            lastUpdatedRow = i.row();
            if (firstUpdatedRow < 0 || lastUpdatedRow < firstUpdatedRow) { firstUpdatedRow = lastUpdatedRow; }
            ObjectType obj(this->at(i));

            // update all properties in map
            for (const CPropertyIndex &pi : propertyIndexes)
            {
                obj.setPropertyByIndex(pi, vm.value(pi));
            }

            // and update container
            if (this->derivedModel()->setInContainer(i, obj))
            {
                c++;
            }
        }

        if (c > 0)
        {
            this->derivedModel()->emitDataChanged(firstUpdatedRow, lastUpdatedRow);
        }
        return c;
    }

    template <class T>
    int CViewBase<T>::updateSelected(const CVariant &variant, const CPropertyIndex &index)
    {
        const CPropertyIndexVariantMap vm(index, variant);
        return this->updateSelected(vm);
    }

    template <class T>
    typename CViewBase<T>::ObjectType CViewBase<T>::selectedObject() const
    {
        const ContainerType c = this->selectedObjects();
        return c.frontOrDefault();
    }

    template <class T>
    int CViewBase<T>::removeSelectedRows()
    {
        if (!this->hasSelection()) { return 0; }
        if (this->isEmpty()) { return 0; }

        const int currentRows = this->rowCount();
        const ContainerType selected(this->selectedObjects());
        const CVariant deletedObjsVariant = CVariant::from(selected);
        int delta = 0;

        if (!this->hasFilter() && currentRows == this->selectedRowCount())
        {
            // shortcut if all are selected
            this->clear(); // clear all
            delta = currentRows;
        }
        else
        {
            ContainerType unselectedObjects(this->container());
            unselectedObjects.removeIfInSubset(selected);
            this->updateContainerMaybeAsync(unselectedObjects);
            delta = currentRows - unselectedObjects.size();
        }
        emit this->objectsDeleted(deletedObjsVariant);
        return delta;
    }

    template <class T>
    void CViewBase<T>::presizeOrFullResizeToContents()
    {
        const int rc = this->rowCount();
        if (rc > ResizeSubsetThreshold)
        {
            const int presizeRandomElements = this->getPresizeRandomElementsSize(rc);
            if (presizeRandomElements > 0)
            {
                const ContainerType containerBackup(this->container());
                m_model->update(containerBackup.sampleElements(presizeRandomElements), false);
                this->fullResizeToContents();
                m_model->update(containerBackup, false);
            }
        }
        else
        {
            this->fullResizeToContents();
        }
    }

    template <class T>
    void CViewBase<T>::clearHighlighting()
    {
        Q_ASSERT(m_model);
        return m_model->clearHighlighting();
    }

    template <class T>
    void CViewBase<T>::materializeFilter()
    {
        Q_ASSERT(m_model);
        if (!m_model->hasFilter()) { return; }
        if (this->isEmpty()) { return; }
        ContainerType filtered(m_model->containerFiltered());
        this->removeFilter();
        this->updateContainerMaybeAsync(filtered);
    }

    template <class T>
    void CViewBase<T>::clear()
    {
        Q_ASSERT(m_model);
        m_model->clear();
        this->hideLoadIndicator();
    }

    template <class T>
    int CViewBase<T>::rowCount() const
    {
        Q_ASSERT(m_model);
        return m_model->rowCount();
    }

    template <class T>
    int CViewBase<T>::columnCount() const
    {
        Q_ASSERT(m_model);
        return m_model->columnCount(QModelIndex());
    }

    template <class T>
    bool CViewBase<T>::isEmpty() const
    {
        Q_ASSERT(m_model);
        return m_model->rowCount() < 1;
    }

    template <class T>
    bool CViewBase<T>::isOrderable() const
    {
        Q_ASSERT(m_model);
        return m_model->isOrderable();
    }

    template <class T>
    void CViewBase<T>::allowDragDrop(bool allowDrag, bool allowDrop, bool allowDropJsonFile)
    {
        Q_ASSERT(m_model);

        // see model for implementing logic of drag
        this->viewport()->setAcceptDrops(allowDrop);
        this->setDragEnabled(allowDrag);
        this->setDropIndicatorShown(allowDrag || allowDrop);
        m_model->allowDrop(allowDrop);
        m_model->allowFileDrop(allowDropJsonFile);
    }

    template <class T>
    bool CViewBase<T>::isDropAllowed() const
    {
        Q_ASSERT(m_model);
        return m_model->isDropAllowed();
    }

    template <class T>
    void CViewBase<T>::dropEvent(QDropEvent *event)
    {
        if (m_model && m_model->isJsonFileDropAllowed() && CGuiUtility::isMimeRepresentingReadableJsonFile(event->mimeData()))
        {
            const QFileInfo fi = CGuiUtility::representedMimeFile(event->mimeData());
            const CStatusMessage msgs = this->loadJsonFile(fi.absoluteFilePath());
            Q_UNUSED(msgs)
            return;
        }
        CViewBaseNonTemplate::dropEvent(event);
    }

    template <class T>
    bool CViewBase<T>::acceptDrop(const QMimeData *mimeData) const
    {
        Q_ASSERT(m_model);
        const bool a = m_model->acceptDrop(mimeData);
        return a;
    }

    template <class T>
    bool CViewBase<T>::setSorting(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
    {
        Q_ASSERT(m_model);
        return m_model->setSorting(propertyIndex, order);
    }

    template <class T>
    void CViewBase<T>::sortByPropertyIndex(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
    {
        m_model->sortByPropertyIndex(propertyIndex, order);
    }

    template <class T>
    void CViewBase<T>::sort()
    {
        m_model->sort();
    }

    template <class T>
    void CViewBase<T>::resort()
    {
        m_model->resort();
    }

    template <class T>
    QJsonObject CViewBase<T>::toJson(bool selectedOnly) const
    {
        Q_ASSERT(m_model);
        return m_model->toJson(selectedOnly);
    }

    template <class T>
    QString CViewBase<T>::toJsonString(QJsonDocument::JsonFormat format, bool selectedOnly) const
    {
        Q_ASSERT(m_model);
        return m_model->toJsonString(format, selectedOnly);
    }

    template <class T>
    void CViewBase<T>::setObjectName(const QString &name)
    {
        // then name here is mainly set for debugging purposes so each model can be identified
        Q_ASSERT(m_model);
        QTableView::setObjectName(name);
        m_model->setObjectName(name);
    }

    template <class T>
    void CViewBase<T>::takeFilterOwnership(std::unique_ptr<BlackGui::Models::IModelFilter<ContainerType>> &filter)
    {
        this->derivedModel()->takeFilterOwnership(filter);
    }

    template <class T>
    void CViewBase<T>::removeFilter()
    {
        this->derivedModel()->removeFilter();
    }

    template <class T>
    bool CViewBase<T>::hasFilter() const
    {
        return derivedModel()->hasFilter();
    }

    template <class T>
    void CViewBase<T>::addContainerTypesAsDropTypes(bool objectType, bool containerType)
    {
        if (objectType) { m_model->addAcceptedMetaTypeId(qMetaTypeId<ObjectType>()); }
        if (containerType) { m_model->addAcceptedMetaTypeId(qMetaTypeId<ContainerType>()); }
    }

    template <class T>
    void CViewBase<T>::initAsOrderable()
    {
        Q_ASSERT_X(isOrderable(), Q_FUNC_INFO, "Model not orderable");
        this->allowDragDrop(true, true);
        this->setDragDropMode(InternalMove);
        this->setDropActions(Qt::MoveAction);
        this->addContainerTypesAsDropTypes(true, true);
    }

    template <class T>
    void CViewBase<T>::setTabWidgetViewText(QTabWidget *tw, int index)
    {
        if (!tw) { return; }
        QString o = tw->tabText(index);
        const QString f = this->hasFilter() ? "F" : "";
        o = CGuiUtility::replaceTabCountValue(o, this->rowCount()) + f;
        tw->setTabText(index, o);
    }

    template <class T>
    void CViewBase<T>::setPercentageColumnWidths()
    {
        const int width = this->width() - 25; // avoid scrollbars etc, reserve a little space
        QList<int> widths = this->getColumns().calculateWidths(width);
        if (widths.isEmpty()) { return; }
        for (int c = 0; c < this->getColumns().size(); c++)
        {
            const int w = widths.at(c);
            this->setColumnWidth(c, w);
        }
    }

    template <class T>
    void CViewBase<T>::setSortIndicator()
    {
        if (m_model->hasValidSortColumn())
        {
            Q_ASSERT(this->horizontalHeader());
            this->horizontalHeader()->setSortIndicator(
                m_model->getSortColumn(),
                m_model->getSortOrder());
        }
    }

    template <class T>
    void CViewBase<T>::standardInit(ModelClass *model)
    {
        Q_ASSERT_X(model || m_model, Q_FUNC_INFO, "Missing model");
        if (model)
        {
            if (model == m_model) { return; }
            if (m_model)
            {
                m_model->disconnect();
            }

            m_model = model;
            m_model->setSelectionModel(this); // set myself as selection model
            bool c = connect(m_model, &ModelClass::modelDataChanged, this, &CViewBase::modelDataChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
            c = connect(m_model, &ModelClass::modelDataChangedDigest, this, &CViewBase::modelDataChangedDigest);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
            c = connect(m_model, &ModelClass::objectChanged, this, &CViewBase::objectChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
            c = connect(m_model, &ModelClass::changed, this, &CViewBase::modelChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
            c = connect(m_model, &ModelClass::changed, this, &CViewBase::onModelChanged);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
            Q_UNUSED(c)
        }

        this->setModel(m_model); // via QTableView
        CViewBaseNonTemplate::init();
        this->setSortIndicator();
    }

    template <class T>
    bool CViewBase<T>::reachedResizeThreshold(int containerSize) const
    {
        if (containerSize < 0) { return this->rowCount() > m_skipResizeThreshold; }
        return containerSize > m_skipResizeThreshold;
    }

    template <class T>
    void CViewBase<T>::performModeBasedResizeToContent()
    {
        // small set or large set? This only performs real resizing, no presizing
        // remark, see also presizeOrFullResizeToContents
        if (this->isResizeConditionMet())
        {
            this->fullResizeToContents();
        }
        else
        {
            m_resizeCount++; // skipped resize
        }
    }

    template <class T>
    int CViewBase<T>::performUpdateContainer(const swift::misc::CVariant &variant, bool sort, bool resize)
    {
        ContainerType c(variant.to<ContainerType>());
        return this->updateContainer(c, sort, resize);
    }

    template <class T>
    void CViewBase<T>::updateSortIndicator()
    {
        if (this->derivedModel()->hasValidSortColumn())
        {
            const int index = this->derivedModel()->getSortColumn();
            Qt::SortOrder order = this->derivedModel()->getSortOrder();
            this->horizontalHeader()->setSortIndicator(index, order);
        }
    }

    template <class T>
    void CViewBase<T>::mouseOverCallback(const QModelIndex &index, bool mouseOver)
    {
        // void
        Q_UNUSED(index)
        Q_UNUSED(mouseOver)
    }

    template <class T>
    void CViewBase<T>::drawDropIndicator(bool indicator)
    {
        m_dropIndicator = indicator;
    }

    template <class T>
    void CViewBase<T>::selectObjects(const ContainerType &selectedObjects)
    {
        Q_UNUSED(selectedObjects)
    }

    template <class T>
    CStatusMessage CViewBase<T>::modifyLoadedJsonData(ContainerType &data) const
    {
        Q_UNUSED(data)
        static const CStatusMessage e(this, CStatusMessage::SeverityInfo, u"no modification", true);
        return e;
    }

    template <class T>
    CStatusMessage CViewBase<T>::validateLoadedJsonData(const ContainerType &data) const
    {
        Q_UNUSED(data)
        static const CStatusMessage e(this, CStatusMessage::SeverityInfo, u"validation passed", true);
        return e;
    }

    template <class T>
    void CViewBase<T>::jsonLoadedAndModelUpdated(const ContainerType &data)
    {
        Q_UNUSED(data)
    }

    template <class T>
    void CViewBase<T>::customMenu(CMenuActions &menuActions)
    {
        CViewBaseNonTemplate::customMenu(menuActions);

        // Clear highlighting
        if (this->derivedModel()->hasHighlightedRows())
        {
            menuActions.addAction(CIcons::refresh16(), "Clear highlighting", CMenuAction::pathViewClearHighlighting(), nullptr, { this, &CViewBaseNonTemplate::clearHighlighting });
        }
    }

    template <class T>
    CStatusMessage CViewBase<T>::loadJsonFile(const QString &fileName)
    {
        CStatusMessage m;
        do
        {
            if (fileName.isEmpty())
            {
                m = CStatusMessage(this).error(u"Load canceled, no file name");
                break;
            }

            const QString json(CFileUtils::readFileToString(fileName));
            if (json.isEmpty())
            {
                m = CStatusMessage(this).warning(u"Reading '%1' yields no data") << fileName;
                break;
            }
            if (!json::looksLikeSwiftJson(json))
            {
                m = CStatusMessage(this).warning(u"No swift JSON '%1'") << fileName;
                break;
            }

            try
            {
                const bool allowCacheFormat = this->allowCacheFileFormatJson();
                const QJsonObject jsonObject = json::jsonObjectFromString(json, allowCacheFormat);
                if (jsonObject.isEmpty())
                {
                    m = CStatusMessage(this).warning(u"No valid swift JSON '%1'") << fileName;
                    break;
                }

                ContainerType container;

                if (jsonObject.contains("type") && jsonObject.contains("value"))
                {
                    // read from variant format
                    CVariant containerVariant;
                    containerVariant.convertFromJson(jsonObject);
                    if (!containerVariant.canConvert<ContainerType>())
                    {
                        m = CStatusMessage(this).warning(u"No valid swift JSON '%1'") << fileName;
                        break;
                    }
                    container = containerVariant.value<ContainerType>();
                }
                else
                {
                    // container format directly
                    container.convertFromJson(jsonObject);
                }

                const int countBefore = container.size();
                m = this->modifyLoadedJsonData(container);
                if (m.isFailure()) { break; } // modification error
                if (countBefore > 0 && container.isEmpty()) { break; }
                m = this->validateLoadedJsonData(container);
                if (m.isFailure()) { break; } // validaton error
                this->updateContainerMaybeAsync(container);
                m = CStatusMessage(this, CStatusMessage::SeverityInfo, "Reading " + fileName + " completed", true);
                this->jsonLoadedAndModelUpdated(container);
                this->rememberLastJsonDirectory(fileName);
            }
            catch (const CJsonException &ex)
            {
                m = CStatusMessage::fromJsonException(ex, this, QString("Reading JSON from '%1'").arg(fileName));
                break;
            }
        }
        while (false);

        emit this->jsonLoadCompleted(m);
        return m;
    }

    template <class T>
    void CViewBase<T>::displayContainerAsJsonPopup(bool selectedOnly)
    {
        const ContainerType container = selectedOnly ? this->selectedObjects() : this->container();
        const QString json = container.toJsonString();
        QTextEdit *te = this->textEditDialog()->textEdit();
        te->setReadOnly(true);
        te->setText(json);
        this->textEditDialog()->show();
    }

    template <class T>
    CStatusMessage CViewBase<T>::loadJson(const QString &directory)
    {
        const QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                              tr("Load data file"),
                                                              directory.isEmpty() ? this->getFileDialogFileName(true) : directory,
                                                              tr("swift (*.json *.txt)"));
        return this->loadJsonFile(fileName);
    }

    template <class T>
    CStatusMessage CViewBase<T>::saveJson(bool selectedOnly, const QString &directory)
    {
        const QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                              tr("Save data file"),
                                                              directory.isEmpty() ? this->getFileDialogFileName(false) : directory,
                                                              tr("swift (*.json *.txt)"));
        if (fileName.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityDebug, u"Save canceled", true); }
        const QString json(this->toJsonString(QJsonDocument::Indented, selectedOnly)); // save as CVariant JSON

        // save file
        CWorker::fromTask(qApp, Q_FUNC_INFO, [=] { CFileUtils::writeStringToFile(json, fileName); });
        this->rememberLastJsonDirectory(fileName);
        return CStatusMessage(this, CStatusMessage::SeverityInfo, u"Writing " % fileName % u" in progress", true);
    }

    template <class T>
    void CViewBase<T>::copy()
    {
        QClipboard *clipboard = QApplication::clipboard();
        if (!clipboard) { return; }
        if (!this->hasSelection()) { return; }
        const ContainerType selection = this->selectedObjects();
        if (selection.isEmpty()) { return; }
        const CVariant copyJson = CVariant::from(selection);
        const QString json = copyJson.toJsonString();
        clipboard->setText(json);
    }

    template <class T>
    void CViewBase<T>::cut()
    {
        if (!QApplication::clipboard()) { return; }
        this->copy();
        this->removeSelectedRows();
    }

    template <class T>
    void CViewBase<T>::paste()
    {
        const QClipboard *clipboard = QApplication::clipboard();
        if (!clipboard) { return; }
        const QString json = clipboard->text();
        if (json.isEmpty()) { return; }
        if (!json::looksLikeSwiftJson(json)) { return; } // no JSON
        try
        {
            ContainerType objects;
            objects.convertFromJson(json);
            if (!objects.isEmpty())
            {
                this->insert(objects);
            }
        }
        catch (const CJsonException &ex)
        {
            Q_UNUSED(ex)
        }
    }

    template <class T>
    bool CViewBase<T>::filterDialogFinished(int status)
    {
        const QDialog::DialogCode statusCode = static_cast<QDialog::DialogCode>(status);
        return filterWidgetChangedFilter(statusCode == QDialog::Accepted);
    }

    template <class T>
    bool CViewBase<T>::filterWidgetChangedFilter(bool enabled)
    {
        if (enabled)
        {
            if (!m_filterWidget)
            {
                this->removeFilter();
            }
            else
            {
                // takes the filter and triggers the filtering
                IModelFilterProvider<ContainerType> *provider = dynamic_cast<IModelFilterProvider<ContainerType> *>(m_filterWidget);
                Q_ASSERT_X(provider, Q_FUNC_INFO, "Filter widget does not provide interface");
                if (!provider) { return false; }
                std::unique_ptr<IModelFilter<ContainerType>> f(provider->createModelFilter());
                if (f->isValid())
                {
                    this->takeFilterOwnership(f);
                }
                else
                {
                    this->removeFilter();
                }
            }
        }
        else
        {
            // no filter
            this->removeFilter();
        }
        return true; // handled
    }

    template <class T>
    void CViewBase<T>::onClicked(const QModelIndex &index)
    {
        if (!m_acceptClickSelection) { return; }
        if (!index.isValid()) { return; }
        emit this->objectClicked(CVariant::fromValue(at(index)));
    }

    template <class T>
    void CViewBase<T>::onDoubleClicked(const QModelIndex &index)
    {
        if (!m_acceptDoubleClickSelection) { return; }
        if (!index.isValid()) { return; }
        emit this->objectDoubleClicked(CVariant::fromValue(at(index)));
    }

    template <class T>
    void CViewBase<T>::onRowSelected(const QModelIndex &index)
    {
        if (!m_acceptRowSelection) { return; }
        if (!index.isValid()) { return; }
        emit this->objectSelected(CVariant::fromValue(at(index)));
    }

} // namespace
