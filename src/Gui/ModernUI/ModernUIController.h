#pragma once

#include <QObject>
#include <QPointer>

class QMainWindow;

namespace Gui::ModernUI
{

enum class InterfaceMode
{
    Classic,
    Modern,
};

class ModernUIController: public QObject
{
    Q_OBJECT

public:
    static ModernUIController& instance();

    void initialize(QMainWindow* mainWindow);

    InterfaceMode mode() const;
    void setMode(InterfaceMode mode);

    void saveActiveLayout();
    void restoreActiveLayout();

private:
    ModernUIController();

    InterfaceMode storedMode() const;
    void persistMode(InterfaceMode mode);
    void applyMode(InterfaceMode mode, bool restoreLayout);
    void saveLayout(InterfaceMode mode);
    bool restoreLayout(InterfaceMode mode);
    void seedLayoutIfMissing(InterfaceMode mode);
    void updateModeProperties();

    static const char* modeName(InterfaceMode mode);
    static const char* stateKey(InterfaceMode mode);

    QPointer<QMainWindow> _mainWindow;
    InterfaceMode _mode = InterfaceMode::Classic;
    bool _initialized = false;
};

}  // namespace Gui::ModernUI
