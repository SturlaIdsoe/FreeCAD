#include "ModernTheme.h"

namespace Gui::ModernUI
{

ThemeTokens ModernTheme::tokens(ThemeVariant variant)
{
    if (variant == ThemeVariant::Light) {
        return {
            QColor(QStringLiteral("#F3F5F7")),
            QColor(QStringLiteral("#E9EDF1")),
            QColor(QStringLiteral("#FFFFFF")),
            QColor(QStringLiteral("#1A1D21")),
            QColor(QStringLiteral("#58616B")),
            QColor(QStringLiteral("#939BA4")),
            QColor(QStringLiteral("#D1D7DE")),
            QColor(QStringLiteral("#B2BBC5")),
            QColor(QStringLiteral("#2F6FEB")),
            QColor(QStringLiteral("#255FCC")),
            QColor(QStringLiteral("#1F4FA8")),
            QColor(QStringLiteral("#DCE8FF")),
            QColor(QStringLiteral("#B26A00")),
            QColor(QStringLiteral("#C93535")),
            QColor(QStringLiteral("#2D8A54")),
        };
    }

    return {
        QColor(QStringLiteral("#14191F")),
        QColor(QStringLiteral("#1B2128")),
        QColor(QStringLiteral("#202730")),
        QColor(QStringLiteral("#F0F3F6")),
        QColor(QStringLiteral("#AAB3BD")),
        QColor(QStringLiteral("#69737E")),
        QColor(QStringLiteral("#303944")),
        QColor(QStringLiteral("#46515E")),
        QColor(QStringLiteral("#4C8DFF")),
        QColor(QStringLiteral("#6AA0FF")),
        QColor(QStringLiteral("#3776DE")),
        QColor(QStringLiteral("#263E64")),
        QColor(QStringLiteral("#D99A36")),
        QColor(QStringLiteral("#E35D6A")),
        QColor(QStringLiteral("#4FB77A")),
    };
}

}  // namespace Gui::ModernUI
