#pragma once

#include <QColor>

namespace Gui::ModernUI
{

enum class ThemeVariant
{
    Dark,
    Light,
};

struct ThemeTokens
{
    QColor backgroundPrimary;
    QColor backgroundSecondary;
    QColor backgroundFloating;
    QColor textPrimary;
    QColor textSecondary;
    QColor textDisabled;
    QColor borderNormal;
    QColor borderStrong;
    QColor accent;
    QColor accentHover;
    QColor accentPressed;
    QColor selection;
    QColor warning;
    QColor error;
    QColor success;
};

class ModernTheme
{
public:
    static ThemeTokens tokens(ThemeVariant variant);
};

}  // namespace Gui::ModernUI
