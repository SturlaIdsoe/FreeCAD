#include "ModernUIController.h"

#include <QApplication>
#include <QByteArray>
#include <QMainWindow>
#include <QTimer>

#include <App/Application.h>
#include <Base/Parameter.h>

namespace Gui::ModernUI
{

namespace
{
constexpr auto preferencesPath = "User parameter:BaseApp/Preferences/ModernUI";
constexpr auto interfaceModeKey = "InterfaceMode";
constexpr auto themeVariantKey = "ThemeVariant";

ParameterGrp::handle preferences()
{
    return App::GetApplication().GetParameterGroupByPath(preferencesPath);
}
}  // namespace

ModernUIController& ModernUIController::instance()
{
    static ModernUIController controller;
    return controller;
}

ModernUIController::ModernUIController() = default;

void ModernUIController::initialize(QMainWindow* mainWindow)
{
    if (!mainWindow || _initialized) {
        return;
    }

    _mainWindow = mainWindow;
    _mode = storedMode();
    _initialized = true;
    updateModeProperties();

    // FreeCAD restores its legacy main-window state shortly after constructing the
    // main window. Seed/restore the named Modern UI states after that work has had
    // a chance to finish. Classic remains the default and therefore preserves the
    // current upstream behaviour for existing users.
    QTimer::singleShot(250, mainWindow, [this]() {
        if (!_mainWindow) {
            return;
        }

        seedLayoutIfMissing(InterfaceMode::Classic);
        seedLayoutIfMissing(InterfaceMode::Modern);
        restoreActiveLayout();
    });

    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() { saveActiveLayout(); });
}

InterfaceMode ModernUIController::mode() const
{
    return _mode;
}

void ModernUIController::setMode(InterfaceMode mode)
{
    if (!_initialized) {
        _mode = mode;
        persistMode(mode);
        return;
    }

    if (_mode == mode) {
        persistMode(mode);
        updateModeProperties();
        return;
    }

    saveLayout(_mode);
    applyMode(mode, true);
}

void ModernUIController::saveActiveLayout()
{
    if (_initialized && _mainWindow) {
        saveLayout(_mode);
    }
}

void ModernUIController::restoreActiveLayout()
{
    if (_initialized && _mainWindow) {
        restoreLayout(_mode);
        updateModeProperties();
    }
}

InterfaceMode ModernUIController::storedMode() const
{
    const auto value = preferences()->GetASCII(interfaceModeKey, "Classic");
    return value == "Modern" ? InterfaceMode::Modern : InterfaceMode::Classic;
}

void ModernUIController::persistMode(InterfaceMode mode)
{
    preferences()->SetASCII(interfaceModeKey, modeName(mode));
}

void ModernUIController::applyMode(InterfaceMode mode, bool restoreLayout)
{
    _mode = mode;
    persistMode(mode);
    updateModeProperties();

    if (restoreLayout) {
        seedLayoutIfMissing(mode);
        restoreLayout(mode);
    }
}

void ModernUIController::saveLayout(InterfaceMode mode)
{
    if (!_mainWindow) {
        return;
    }

    const QByteArray state = _mainWindow->saveState();
    preferences()->SetASCII(stateKey(mode), state.toBase64().constData());
}

bool ModernUIController::restoreLayout(InterfaceMode mode)
{
    if (!_mainWindow) {
        return false;
    }

    const auto encoded = preferences()->GetASCII(stateKey(mode), "");
    if (encoded.empty()) {
        return false;
    }

    return _mainWindow->restoreState(QByteArray::fromBase64(QByteArray::fromStdString(encoded)));
}

void ModernUIController::seedLayoutIfMissing(InterfaceMode mode)
{
    if (!_mainWindow) {
        return;
    }

    if (!preferences()->GetASCII(stateKey(mode), "").empty()) {
        return;
    }

    saveLayout(mode);
}

void ModernUIController::updateModeProperties()
{
    if (!_mainWindow) {
        return;
    }

    const bool modern = _mode == InterfaceMode::Modern;
    _mainWindow->setProperty("freecadModernUI", modern);

    const auto theme = preferences()->GetASCII(themeVariantKey, "Dark");
    _mainWindow->setProperty(
        "freecadModernTheme",
        QString::fromLatin1(theme == "Light" ? "Light" : "Dark")
    );

    // Refresh style-dependent selectors without imposing any new stylesheet yet.
    _mainWindow->style()->unpolish(_mainWindow);
    _mainWindow->style()->polish(_mainWindow);
    _mainWindow->update();
}

const char* ModernUIController::modeName(InterfaceMode mode)
{
    return mode == InterfaceMode::Modern ? "Modern" : "Classic";
}

const char* ModernUIController::stateKey(InterfaceMode mode)
{
    return mode == InterfaceMode::Modern ? "ModernMainWindowState" : "ClassicMainWindowState";
}

}  // namespace Gui::ModernUI

#include "moc_ModernUIController.cpp"
