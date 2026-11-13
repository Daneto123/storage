#include "productmodel.h"

#include <QBrush>
#include <QColor>

using storage::Product;

namespace {

const QString kDate = QStringLiteral("dd.MM.yyyy");

int status(const Product& p, const QDate& today) {
    if (p.isExpired(today)) return 2;
    if (p.expiresWithin(ProductModel::kSoonDays, today)) return 1;
    return 0;
}

}  // namespace

ProductModel::ProductModel(const storage::Inventory* inventory, QObject* parent)
    : QAbstractTableModel(parent), inventory_(inventory), today_(QDate::currentDate()) {}

void ProductModel::refresh() {
    beginResetModel();
    endResetModel();
}

void ProductModel::setToday(const QDate& today) {
    today_ = today;
    refresh();
}

int ProductModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(inventory_->products().size());
}

int ProductModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant ProductModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= inventory_->products().size()) return {};
    const Product& p = inventory_->products().at(index.row());
    const int st = status(p, today_);

    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case Name: return p.name;
                case Manufacturer: return p.manufacturer;
                case Quantity: return storage::formatQuantity(p.quantity);
                case Unit: return p.unit;
                case Expiration: return p.expiration.isValid() ? p.expiration.toString(kDate) : tr("без срок");
                case Received: return p.received.toString(kDate);
                case Place: return p.location.toString();
                case Comment: return p.comment.simplified();
            }
            break;
        case SortRole:
            switch (index.column()) {
                case Quantity: return p.quantity;
                case Expiration: return p.expiration.isValid() ? p.expiration : QDate(9999, 12, 31);
                case Received: return p.received;
                case Place: return p.location.toString();
                default: return data(index, Qt::DisplayRole).toString().toLower();
            }
        case Qt::TextAlignmentRole:
            if (index.column() == Quantity) return int(Qt::AlignRight | Qt::AlignVCenter);
            if (index.column() == Expiration || index.column() == Received || index.column() == Place) {
                return int(Qt::AlignCenter);
            }
            break;
        case Qt::BackgroundRole:
            if (st == 2) return QBrush(QColor(0xfd, 0xe2, 0xe2));
            if (st == 1) return QBrush(QColor(0xfe, 0xf3, 0xc7));
            break;
        case Qt::ForegroundRole:
            if (st == 2) return QBrush(QColor(0x99, 0x1b, 0x1b));
            if (st == 1) return QBrush(QColor(0x92, 0x40, 0x0e));
            break;
        case Qt::ToolTipRole:
            if (st == 2) return tr("Срокът на годност е изтекъл");
            if (st == 1) {
                const auto days = today_.daysTo(p.expiration);
                if (days == 0) return tr("Изтича днес");
                if (days == 1) return tr("Изтича утре");
                return tr("Изтича след %1 дни").arg(days);
            }
            if (index.column() == Comment && !p.comment.isEmpty()) return p.comment;
            break;
        case IdRole: return p.id;
        case StatusRole: return st;
    }
    return {};
}

QVariant ProductModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
        case Name: return tr("Продукт");
        case Manufacturer: return tr("Производител");
        case Quantity: return tr("Количество");
        case Unit: return tr("Мярка");
        case Expiration: return tr("Годен до");
        case Received: return tr("Постъпил");
        case Place: return tr("Място");
        case Comment: return tr("Бележка");
    }
    return {};
}

void ProductFilter::setSearchText(const QString& text) {
    search_ = text.trimmed();
    invalidateFilter();
}

void ProductFilter::setOnlyProblems(bool on) {
    onlyProblems_ = on;
    invalidateFilter();
}

bool ProductFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    const auto* model = sourceModel();
    if (onlyProblems_) {
        const int st = model->index(sourceRow, 0, sourceParent).data(ProductModel::StatusRole).toInt();
        if (st == 0) return false;
    }
    if (search_.isEmpty()) return true;
    for (int c = 0; c < model->columnCount(sourceParent); ++c) {
        if (model->index(sourceRow, c, sourceParent).data().toString().contains(search_, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}
