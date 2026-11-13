#include "inventory.h"

#include <QSet>

#include <algorithm>

namespace storage {

namespace {

constexpr double kEpsilon = 1e-9;

bool sameText(const QString& a, const QString& b) {
    return a.trimmed().compare(b.trimmed(), Qt::CaseInsensitive) == 0;
}

Product normalized(Product p) {
    p.name = p.name.trimmed();
    p.manufacturer = p.manufacturer.trimmed();
    p.unit = p.unit.trimmed();
    p.comment = p.comment.trimmed();
    return p;
}

}  // namespace

void Inventory::reset(QList<Product> products, QList<Movement> movements) {
    products_ = std::move(products);
    movements_ = std::move(movements);
}

const Product* Inventory::find(const QString& id) const {
    for (const auto& p : products_) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

QString Inventory::validate(const Product& raw, const QString& ignoreId) const {
    const Product p = normalized(raw);
    if (p.name.isEmpty()) return QStringLiteral("Въведете име на продукта.");
    if (p.unit.isEmpty()) return QStringLiteral("Въведете мерна единица.");
    if (!(p.quantity > 0)) return QStringLiteral("Количеството трябва да е по-голямо от 0.");
    if (!p.received.isValid()) return QStringLiteral("Въведете дата на постъпване.");
    if (p.expiration.isValid() && p.expiration < p.received) {
        return QStringLiteral("Срокът на годност е преди датата на постъпване.");
    }
    if (p.location.section < 0 || p.location.shelf < 0 || p.location.slot < 0) {
        return QStringLiteral("Мястото в склада не може да е отрицателно.");
    }
    // One place holds only one kind of product (several batches are fine).
    for (const auto& other : products_) {
        if (other.id == ignoreId) continue;
        if (other.location == p.location && !sameText(other.name, p.name)) {
            return QStringLiteral("Място %1 е заето от „%2“.").arg(p.location.toString(), other.name);
        }
    }
    return {};
}

OperationResult Inventory::receive(Product product, const QDateTime& when) {
    product = normalized(product);
    if (const QString error = validate(product); !error.isEmpty()) {
        return OperationResult::failure(error);
    }

    for (auto& existing : products_) {
        if (sameText(existing.name, product.name) && sameText(existing.manufacturer, product.manufacturer) &&
            sameText(existing.unit, product.unit) && existing.expiration == product.expiration &&
            existing.location == product.location) {
            existing.quantity += product.quantity;
            log(MovementType::Received, existing, product.quantity, when, product.comment);
            return {true, {}, existing.id, true};
        }
    }

    if (product.id.isEmpty() || find(product.id)) product.id = newId();
    products_.append(product);
    log(MovementType::Received, product, product.quantity, when, product.comment);
    return {true, {}, product.id, false};
}

OperationResult Inventory::update(const Product& raw, const QDateTime& when) {
    const Product product = normalized(raw);
    auto it = std::find_if(products_.begin(), products_.end(),
                           [&](const Product& p) { return p.id == product.id; });
    if (it == products_.end()) return OperationResult::failure(QStringLiteral("Продуктът не е намерен."));
    if (const QString error = validate(product, product.id); !error.isEmpty()) {
        return OperationResult::failure(error);
    }

    const double difference = product.quantity - it->quantity;
    QStringList changes;
    if (it->name != product.name) changes << QStringLiteral("име: %1 → %2").arg(it->name, product.name);
    if (it->location != product.location) {
        changes << QStringLiteral("място: %1 → %2").arg(it->location.toString(), product.location.toString());
    }
    if (it->expiration != product.expiration) {
        changes << QStringLiteral("срок: %1 → %2")
                       .arg(it->expiration.toString(QStringLiteral("dd.MM.yyyy")),
                            product.expiration.toString(QStringLiteral("dd.MM.yyyy")));
    }
    *it = product;
    log(MovementType::Edited, product, difference, when, changes.join(QStringLiteral("; ")));
    return {true, {}, product.id, false};
}

OperationResult Inventory::remove(const QString& idRef, const QDateTime& when) {
    const QString id = idRef;  // the caller may pass a reference into products_
    auto it = std::find_if(products_.begin(), products_.end(), [&](const Product& p) { return p.id == id; });
    if (it == products_.end()) return OperationResult::failure(QStringLiteral("Продуктът не е намерен."));
    log(MovementType::Removed, *it, -it->quantity, when);
    products_.erase(it);
    return {true, {}, id, false};
}

OperationResult Inventory::withdraw(const QString& name, const QString& unit, double quantity,
                                    const QDateTime& when, const QString& note) {
    if (!(quantity > 0)) return OperationResult::failure(QStringLiteral("Количеството трябва да е по-голямо от 0."));

    const QDate today = when.date();
    const double available = availableQuantity(name, unit, today);
    if (available + kEpsilon < quantity) {
        return OperationResult::failure(QStringLiteral("Няма достатъчно „%1“: налични са %2 %3 (без изтеклите).")
                                            .arg(name.trimmed(), formatQuantity(available), unit.trimmed()));
    }

    // First expiring first out; batches without expiration go last.
    QList<int> order;
    for (int i = 0; i < products_.size(); ++i) {
        const auto& p = products_[i];
        if (sameText(p.name, name) && sameText(p.unit, unit) && !p.isExpired(today)) order.append(i);
    }
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        const auto& pa = products_[a];
        const auto& pb = products_[b];
        if (pa.expiration.isValid() != pb.expiration.isValid()) return pa.expiration.isValid();
        if (pa.expiration != pb.expiration) return pa.expiration < pb.expiration;
        return pa.received < pb.received;
    });

    double left = quantity;
    QSet<QString> emptied;
    for (int index : order) {
        if (left <= kEpsilon) break;
        auto& p = products_[index];
        const double take = std::min(left, p.quantity);
        p.quantity -= take;
        left -= take;
        log(MovementType::Withdrawn, p, -take, when, note);
        if (p.quantity <= kEpsilon) emptied.insert(p.id);
    }
    products_.erase(std::remove_if(products_.begin(), products_.end(),
                                   [&](const Product& p) { return emptied.contains(p.id); }),
                    products_.end());
    return {true, {}, {}, false};
}

int Inventory::removeExpired(const QDate& date, const QDateTime& when) {
    int removed = 0;
    for (auto it = products_.begin(); it != products_.end();) {
        if (it->isExpired(date)) {
            log(MovementType::Expired, *it, -it->quantity, when);
            it = products_.erase(it);
            ++removed;
        } else {
            ++it;
        }
    }
    return removed;
}

double Inventory::availableQuantity(const QString& name, const QString& unit, const QDate& today) const {
    double total = 0;
    for (const auto& p : products_) {
        if (sameText(p.name, name) && sameText(p.unit, unit) && !p.isExpired(today)) total += p.quantity;
    }
    return total;
}

QList<QPair<QString, QString>> Inventory::productKinds() const {
    QList<QPair<QString, QString>> kinds;
    for (const auto& p : products_) {
        const bool known = std::any_of(kinds.begin(), kinds.end(), [&](const auto& k) {
            return sameText(k.first, p.name) && sameText(k.second, p.unit);
        });
        if (!known) kinds.append({p.name, p.unit});
    }
    std::sort(kinds.begin(), kinds.end(), [](const auto& a, const auto& b) {
        const int c = a.first.localeAwareCompare(b.first);
        return c != 0 ? c < 0 : a.second < b.second;
    });
    return kinds;
}

QList<Movement> Inventory::movementsBetween(const QDate& from, const QDate& to) const {
    QList<Movement> result;
    for (const auto& m : movements_) {
        const QDate d = m.time.date();
        if ((!from.isValid() || d >= from) && (!to.isValid() || d <= to)) result.append(m);
    }
    return result;
}

QString Inventory::newId() const {
    // Short, readable ids: P0001, P0002, ...
    int highest = 0;
    for (const auto& p : products_) {
        if (p.id.startsWith(QLatin1Char('P'))) {
            bool ok = false;
            const int n = p.id.mid(1).toInt(&ok);
            if (ok) highest = std::max(highest, n);
        }
    }
    return QStringLiteral("P%1").arg(highest + 1, 4, 10, QLatin1Char('0'));
}

void Inventory::log(MovementType type, const Product& p, double quantity, const QDateTime& when,
                    const QString& note) {
    movements_.append({when, type, p.name, quantity, p.unit, p.location, note});
}

}  // namespace storage
