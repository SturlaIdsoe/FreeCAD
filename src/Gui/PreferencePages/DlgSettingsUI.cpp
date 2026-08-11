/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include <QComboBox>
#include <QCoreApplication>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QTimer>

#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/ParamHandler.h>

#include "DlgSettingsUI.h"
#include "ui_DlgSettingsUI.h"

#include "Dialogs/DlgThemeEditor.h"
#include "ModernUI/ModernUIController.h"

#include <Base/ServiceProvider.h>


using namespace Gui::Dialog;

namespace
{
void initializeModernUI()
{
    // Q_COREAPP_STARTUP_FUNCTION runs while QApplication is being constructed.
    // Defer until the event loop starts, by which point FreeCAD's MainWindow exists.
    QTimer::singleShot(0, qApp, []() {
        if (auto* mainWindow = Gui::MainWindow::getInstance()) {
            Gui::ModernUI::ModernUIController::instance().initialize(mainWindow);
        }
    });
}

Q_COREAPP_STARTUP_FUNCTION(initializeModernUI)
}  // namespace

/* TRANSLATOR Gui::Dialog::DlgSettingsUI */

/**
 *  Constructs a DlgSettingsUI which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
DlgSettingsUI::DlgSettingsUI(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsUI)
{
    ui->setupUi(this);

    auto* interfaceGroup = new QGroupBox(tr("Interface"), this);
    auto* interfaceLayout = new QFormLayout(interfaceGroup);

    interfaceModeCombo = new QComboBox(interfaceGroup);
    interfaceModeCombo->addItem(tr("Classic FreeCAD"), QStringLiteral("Classic"));
    interfaceModeCombo->addItem(tr("Modern"), QStringLiteral("Modern"));
    interfaceModeCombo->setToolTip(
        tr("Switch between the original FreeCAD interface and the modern interface. "
           "Each mode keeps its own main-window layout.")
    );
    interfaceLayout->addRow(tr("Interface mode"), interfaceModeCombo);

    modernThemeCombo = new QComboBox(interfaceGroup);
    modernThemeCombo->addItem(tr("Dark"), QStringLiteral("Dark"));
    modernThemeCombo->addItem(tr("Light"), QStringLiteral("Light"));
    modernThemeCombo->setToolTip(tr("Color foundation used by the modern interface."));
    interfaceLayout->addRow(tr("Modern theme"), modernThemeCombo);

    ui->verticalLayout->insertWidget(0, interfaceGroup);

    connect(ui->themeEditorButton, &QPushButton::clicked, [this]() { openThemeEditor(); });
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsUI::~DlgSettingsUI() = default;

void DlgSettingsUI::saveSettings()
{
    auto modernUi = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/ModernUI"
    );
    const auto mode = interfaceModeCombo->currentData().toString();
    const auto theme = modernThemeCombo->currentData().toString();
    modernUi->SetASCII("InterfaceMode", mode.toLatin1().constData());
    modernUi->SetASCII("ThemeVariant", theme.toLatin1().constData());

    auto& controller = Gui::ModernUI::ModernUIController::instance();
    controller.setThemeVariant(theme.toLatin1().constData());
    controller.setMode(
        mode == QStringLiteral("Modern") ? Gui::ModernUI::InterfaceMode::Modern
                                          : Gui::ModernUI::InterfaceMode::Classic
    );

    // Theme
    ui->ThemeAccentColor1->onSave();
    ui->ThemeAccentColor2->onSave();
    ui->ThemeAccentColor3->onSave();
    ui->StyleSheets->onSave();
    ui->OverlayStyleSheets->onSave();

    // Tree View
    ui->fontSizeSpinBox->onSave();
    ui->iconSizeSpinBox->onSave();
    ui->resizableColumnsCheckBox->onSave();
    ui->showVisibilityIconCheckBox->onSave();
    ui->hideDescriptionCheckBox->onSave();
    ui->hideInternalNamesCheckBox->onSave();
    ui->hideTreeViewScrollBarCheckBox->onSave();
    ui->hideHeaderCheckBox->onSave();

    // Overlay
    ui->hideTabBarCheckBox->onSave();
    ui->hintShowTabBarCheckBox->onSave();
    ui->hidePropertyViewScrollBarCheckBox->onSave();
    ui->overlayAutoHideCheckBox->onSave();
    ui->mouseClickPassThroughCheckBox->onSave();
    ui->mouseWheelPassThroughCheckBox->onSave();

    // TaskWatcher
    ui->showTaskWatcherCheckBox->onSave();
}

void DlgSettingsUI::loadSettings()
{
    auto modernUi = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/ModernUI"
    );
    const QString mode = QString::fromLatin1(modernUi->GetASCII("InterfaceMode", "Classic").c_str());
    const QString theme = QString::fromLatin1(modernUi->GetASCII("ThemeVariant", "Dark").c_str());

    int modeIndex = interfaceModeCombo->findData(mode);
    interfaceModeCombo->setCurrentIndex(modeIndex >= 0 ? modeIndex : 0);
    int themeIndex = modernThemeCombo->findData(theme);
    modernThemeCombo->setCurrentIndex(themeIndex >= 0 ? themeIndex : 0);

    // Theme
    ui->ThemeAccentColor1->onRestore();
    ui->ThemeAccentColor2->onRestore();
    ui->ThemeAccentColor3->onRestore();

    // Tree View
    ui->fontSizeSpinBox->onRestore();
    ui->iconSizeSpinBox->onRestore();
    ui->resizableColumnsCheckBox->onRestore();
    ui->showVisibilityIconCheckBox->onRestore();
    ui->hideDescriptionCheckBox->onRestore();
    ui->hideInternalNamesCheckBox->onRestore();
    ui->hideTreeViewScrollBarCheckBox->onRestore();
    ui->hideHeaderCheckBox->onRestore();

    // Overlay
    ui->hideTabBarCheckBox->onRestore();
    ui->hintShowTabBarCheckBox->onRestore();
    ui->hidePropertyViewScrollBarCheckBox->onRestore();
    ui->overlayAutoHideCheckBox->onRestore();
    ui->mouseClickPassThroughCheckBox->onRestore();
    ui->mouseWheelPassThroughCheckBox->onRestore();

    // TaskWatcher
    ui->showTaskWatcherCheckBox->onRestore();

    loadStyleSheet();
}

void DlgSettingsUI::loadStyleSheet()
{
    static std::string translatedString;  // Make sure the memory doesn't disappear on us
    translatedString = tr("No style sheet").toStdString();
    populateStylesheets("StyleSheet", "qss", ui->StyleSheets, translatedString.c_str());
    populateStylesheets("OverlayActiveStyleSheet", "overlay", ui->OverlayStyleSheets, "Auto");
}

void DlgSettingsUI::populateStylesheets(
    const char* key,
    const char* path,
    PrefComboBox* combo,
    const char* def,
    QStringList filter
)
{
    auto hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/MainWindow"
    );
    // List all .qss/.css files
    QMap<QString, QString> cssFiles;
    QDir dir;
    if (filter.isEmpty()) {
        filter << QStringLiteral("*.qss");
        filter << QStringLiteral("*.css");
    }
    QFileInfoList fileNames;

    // read from user, resource and built-in directory
    QStringList qssPaths = QDir::searchPaths(QString::fromUtf8(path));
    for (QStringList::iterator it = qssPaths.begin(); it != qssPaths.end(); ++it) {
        dir.setPath(*it);
        fileNames = dir.entryInfoList(filter, QDir::Files, QDir::Name);
        for (QFileInfoList::iterator jt = fileNames.begin(); jt != fileNames.end(); ++jt) {
            if (cssFiles.find(jt->baseName()) == cssFiles.end()) {
                cssFiles[jt->baseName()] = jt->fileName();
            }
        }
    }

    combo->clear();

    // now add all unique items
    combo->addItem(tr(def), QStringLiteral(""));
    for (QMap<QString, QString>::iterator it = cssFiles.begin(); it != cssFiles.end(); ++it) {
        combo->addItem(it.key(), it.value());
    }

    QString selectedStyleSheet = QString::fromUtf8(hGrp->GetASCII(key).c_str());
    int index = combo->findData(selectedStyleSheet);

    // might be an absolute path name
    if (index < 0 && !selectedStyleSheet.isEmpty()) {
        QFileInfo fi(selectedStyleSheet);
        if (fi.isAbsolute()) {
            QString path = fi.absolutePath();
            if (qssPaths.indexOf(path) >= 0) {
                selectedStyleSheet = fi.fileName();
            }
            else {
                selectedStyleSheet = fi.absoluteFilePath();
                combo->addItem(fi.baseName(), selectedStyleSheet);
            }
        }
    }

    combo->setCurrentIndex(index);
    combo->onRestore();
}

void DlgSettingsUI::openThemeEditor()
{
    Gui::DlgThemeEditor editor;
    editor.exec();
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsUI::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        loadStyleSheet();
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsUI.cpp"
