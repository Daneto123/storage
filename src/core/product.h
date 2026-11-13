#pragma once

#include <QDate>
#include <QDateTime>
#include <QString>
#include <QStringList>

#include <optional>

namespace storage {

// Place in the warehouse: section / shelf / slot, shown as "01/02/05".
struct Location {
    int section = 0;
    int shelf = 0;
    int slot = 0;

    QString toString() const;
    static std::optional<Location> parse(const QString& text);

    friend bool operator==(const Location& a, const Location& b) {
        return a.section == b.section && a.shelf == b.shelf && a.slot == b.slot;
    }
    friend bool operator!=(const Location& a, const Location& b) { return !(a == b); }
    friend bool operator<(const Location& a, const Location& b) {
        if (a.section != b.section) return a.section < b.section;
        if (a.shelf != b.shelf) return a.shelf < b.shelf;
        return a.slot < b.slot;
    }
};

// One batch of a product: same name, expiration date and place.
struct Product {
    QString id;  // unique, stable between saves
    QString name;
    QString manufacturer;
    QString unit;
    double quantity = 0.0;
    QDate expiration;  // invalid date = does not expire
    QDate received;
    Location location;
    QString comment;

    bool isExpired(const QDate& today) const { return expiration.isValid() && expiration < today; }
    bool expiresWithin(int days, const QDate& today) const {
        return expiration.isValid() && !isExpired(today) && today.daysTo(expiration) <= days;
    }
};

enum class MovementType { Received, Withdrawn, Edited, Removed, Expired };

// One line of the stock history ("дневник").
struct Movement {
    QDateTime time;
    MovementType type = MovementType::Received;
    QString productName;
    double quantity = 0.0;  // positive for received, negative for taken out
    QString unit;
    Location location;
    QString note;
};

QString movementKey(MovementType type);                         // used in files
std::optional<MovementType> movementFromKey(const QString& key);
QString movementLabel(MovementType type);                       // shown to the user

// Units offered in the UI; any other text is accepted too.
QStringList standardUnits();

// "12.5" -> "12.5", "3.0" -> "3": no needless decimals in the UI and files.
QString formatQuantity(double quantity);

}  // namespace storage
