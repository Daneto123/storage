#pragma once

#include <QAbstractTableModel>
#include <QDate>
#include <QSortFilterProxyModel>

#include "inventory.h"

// Shows the batches of an Inventory as a table.
class ProductModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column { Name, Manufacturer, Quantity, Unit, Expiration, Received, Place, Comment, ColumnCount };
    static constexpr int IdRole = Qt::UserRole + 1;
    static constexpr int SortRole = Qt::UserRole + 2;
    static constexpr int StatusRole = Qt::UserRole + 3;  // 0 ok, 1 expiring soon, 2 expired

    static constexpr int kSoonDays = 7;

    explicit ProductModel(const storage::Inventory* inventory, QObject* parent = nullptr);

    void refresh();  // call after the inventory changed
    void setToday(const QDate& today);
    QDate today() const { return today_; }

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    const storage::Inventory* inventory_;
    QDate today_;
};

// Text search over all columns plus an optional "only date problems" filter.
class ProductFilter : public QSortFilterProxyModel {
    Q_OBJECT

public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    void setSearchText(const QString& text);
    void setOnlyProblems(bool on);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QString search_;
    bool onlyProblems_ = false;
};
