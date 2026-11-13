#pragma once

#include <QList>
#include <QString>

#include "product.h"

namespace storage {

struct OperationResult {
    bool ok = false;
    QString error;      // user-facing message when !ok
    QString productId;  // the batch that was created / changed
    bool merged = false;

    static OperationResult failure(const QString& message) { return {false, message, {}, false}; }
};

// The whole warehouse: product batches plus the history of every change.
// Pure logic, no files and no UI, so it can be tested on its own.
class Inventory {
public:
    const QList<Product>& products() const { return products_; }
    const QList<Movement>& movements() const { return movements_; }

    void reset(QList<Product> products, QList<Movement> movements);
    void clear() { reset({}, {}); }

    const Product* find(const QString& id) const;

    // Empty string when the product is acceptable, otherwise the reason.
    // `ignoreId` lets an edited product keep its own place.
    QString validate(const Product& product, const QString& ignoreId = {}) const;

    // Adds a new batch. If a batch with the same name, manufacturer, unit,
    // expiration date and place exists, its quantity grows instead.
    OperationResult receive(Product product, const QDateTime& when);

    // Replaces a batch (matched by id) with new data.
    OperationResult update(const Product& product, const QDateTime& when);

    // Deletes a batch completely.
    OperationResult remove(const QString& id, const QDateTime& when);

    // Takes `quantity` of a product out of stock. The batches that expire
    // first go out first (FEFO); expired batches are never handed out.
    // Batches that reach zero are removed.
    OperationResult withdraw(const QString& name, const QString& unit, double quantity,
                             const QDateTime& when, const QString& note = {});

    // Removes every batch that expired before `date`. Returns how many.
    int removeExpired(const QDate& date, const QDateTime& when);

    // Stock that can still be handed out (not expired) on `today`.
    double availableQuantity(const QString& name, const QString& unit, const QDate& today) const;

    // Distinct "name|unit" pairs, sorted, for pickers.
    QList<QPair<QString, QString>> productKinds() const;

    QList<Movement> movementsBetween(const QDate& from, const QDate& to) const;

private:
    QString newId() const;
    void log(MovementType type, const Product& p, double quantity, const QDateTime& when,
             const QString& note = {});

    QList<Product> products_;
    QList<Movement> movements_;
};

}  // namespace storage
