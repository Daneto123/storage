#include "product.h"

#include <QRegularExpression>

#include <cmath>

namespace storage {

QString Location::toString() const {
    return QStringLiteral("%1/%2/%3")
        .arg(section, 2, 10, QLatin1Char('0'))
        .arg(shelf, 2, 10, QLatin1Char('0'))
        .arg(slot, 2, 10, QLatin1Char('0'));
}

std::optional<Location> Location::parse(const QString& text) {
    static const QRegularExpression re(QStringLiteral(R"(^\s*(\d{1,4})\s*[/.\-]\s*(\d{1,4})\s*[/.\-]\s*(\d{1,4})\s*$)"));
    const auto m = re.match(text);
    if (!m.hasMatch()) return std::nullopt;
    return Location{m.captured(1).toInt(), m.captured(2).toInt(), m.captured(3).toInt()};
}

QString movementKey(MovementType type) {
    switch (type) {
        case MovementType::Received: return QStringLiteral("received");
        case MovementType::Withdrawn: return QStringLiteral("withdrawn");
        case MovementType::Edited: return QStringLiteral("edited");
        case MovementType::Removed: return QStringLiteral("removed");
        case MovementType::Expired: return QStringLiteral("expired");
    }
    return {};
}

std::optional<MovementType> movementFromKey(const QString& key) {
    for (auto t : {MovementType::Received, MovementType::Withdrawn, MovementType::Edited,
                   MovementType::Removed, MovementType::Expired}) {
        if (movementKey(t) == key) return t;
    }
    return std::nullopt;
}

QString movementLabel(MovementType type) {
    switch (type) {
        case MovementType::Received: return QStringLiteral("Заприходено");
        case MovementType::Withdrawn: return QStringLiteral("Изписано");
        case MovementType::Edited: return QStringLiteral("Редактирано");
        case MovementType::Removed: return QStringLiteral("Изтрито");
        case MovementType::Expired: return QStringLiteral("Бракувано (изтекъл срок)");
    }
    return {};
}

QStringList standardUnits() {
    return {QStringLiteral("kg"), QStringLiteral("g"), QStringLiteral("l"), QStringLiteral("ml"),
            QStringLiteral("pcs"), QStringLiteral("lbs")};
}

QString formatQuantity(double quantity) {
    // Round away floating point noise such as 0.30000000000000004.
    const double rounded = std::round(quantity * 1000.0) / 1000.0;
    return QString::number(rounded, 'g', 15);
}

}  // namespace storage
