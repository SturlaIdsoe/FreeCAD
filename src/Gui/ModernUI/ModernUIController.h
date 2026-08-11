#pragma once

#include <QApplication>
#include <QByteArray>
#include <QMainWindow>
#include <QPointer>
#include <QTimer>

#include <string>

#include <App/Application.h>
#include <Base/Parameter.h>

namespace Gui::ModernUI
{

enum class InterfaceMode
{
    Classic,
    Modern,
};

class ModernUIController
{
public:
    static ModernUIController& instance()
    {
        static ModernUIController controller;
        return controller;
    }

    void initialize(QMainWindow* mainWindow)
    {
        if (!mainWindow || _initialized) {
            return;
        }

        _mainWindow = mainWindow;
        _mode = storedMode();
        _initialized = true;
        updateModeProperties();

        // Upstream FreeCAD restores its legacy main-window state asynchronously.
        // Seed and restore our named states after that initial restore has settled.
        QTimer::singleShot(250, mainWindow, [this]() {
            if (!_mainWindow) {
                return;
            }
            seedLayoutIfMissing(InterfaceMode::Classic);
            seedLayoutIfMissing(InterfaceMode::Modern);
            restoreActiveLayout();
        });

        QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, [this]() { saveActiveLayout(); });
    }

    InterfaceMode mode() const
    {
        return _mode;
    }

    void setMode(InterfaceMode mode)
    {
        if (!_initialized) {
            _mode = mode;
            persistMode(mode);
            return;
        }

        if (_mode != mode) {
            saveLayout(_mode);
            _mode = mode;
        }

        persistMode(mode);
        updateModeProperties();
        seedLayoutIfMissing(mode);
        restoreLayout(mode);
    }

    void setThemeVariant(const char* variant)
    {
        preferences()->SetASCII(
            "ThemeVariant",
            variant && std::string(variant) == "Light" ? "Light" : "Dark"
        );
        updateModeProperties();
    }

    void saveActiveLayout()
    {
        if (_initialized && _mainWindow) {
            saveLayout(_mode);
        }
    }

    void restoreActiveLayout()
    {
        if (_initialized && _mainWindow) {
            restoreLayout(_mode);
            updateModeProperties();
        }
    }

private:
    ModernUIController() = default;

    static ParameterGrp::handle preferences()
    {
        return App::GetApplication().GetParameterGroupByPath(
            "User parameter:BaseApp/Preferences/ModernUI"
        );
    }

    InterfaceMode storedMode() const
    {
        return preferences()->GetASCII("InterfaceMode", "Classic") == "Modern"
            ? InterfaceMode::Modern
            : InterfaceMode::Classic;
    }

    static const char* modeName(InterfaceMode mode)
    {
        return mode == InterfaceMode::Modern ? "Modern" : "Classic";
    }

    static const char* stateKey(InterfaceMode mode)
    {
        return mode == InterfaceMode::Modern ? "ModernMainWindowState" : "ClassicMainWindowState";
    }

    void persistMode(InterfaceMode mode)
    {
        preferences()->SetASCII("InterfaceMode", modeName(mode));
    }

    void saveLayout(InterfaceMode mode)
    {
        if (!_mainWindow) {
            return;
        }
        const QByteArray state = _mainWindow->saveState();
        preferences()->SetASCII(stateKey(mode), state.toBase64().constData());
    }

    bool restoreLayout(InterfaceMode mode)
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

    void seedLayoutIfMissing(InterfaceMode mode)
    {
        if (_mainWindow && preferences()->GetASCII(stateKey(mode), "").empty()) {
            saveLayout(mode);
        }
    }

    void updateModeProperties()
    {
        if (!_mainWindow) {
            return;
        }
        _mainWindow->setProperty("freecadModernUI", _mode == InterfaceMode::Modern);
        const auto theme = preferences()->GetASCII("ThemeVariant", "Dark");
        _mainWindow->setProperty(
            "freecadModernTheme",
            QString::fromLatin1(theme == "Light" ? "Light" : "Dark")
        );
        _mainWindow->style()->unpolish(_mainWindow);
        _mainWindow->style()->polish(_mainWindow);
        _mainWindow->update();
    }

    QPointer<QMainWindow> _mainWindow;
    InterfaceMode _mode = InterfaceMode::Classic;
    bool _initialized = false;
};

}  // namespace Gui::ModernUI
