#include "logdialog.h"

#include <QBrush>
#include <QColor>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include <algorithm>

LogDialog::LogDialog(const storage::Inventory& inventory, QWidget* parent)
    : QDialog(parent), inventory_(inventory) {
    setWindowTitle(tr("Дневник на движенията"));
    resize(900, 520);

    from_ = new QDateEdit(QDate::currentDate().addDays(-30));
    to_ = new QDateEdit(QDate::currentDate());
    for (auto* d : {from_, to_}) {
        d->setCalendarPopup(true);
        d->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));
        connect(d, &QDateEdit::dateChanged, this, &LogDialog::reload);
    }
    search_ = new QLineEdit;
    search_->setPlaceholderText(tr("Търсене по продукт или бележка…"));
    search_->setClearButtonEnabled(true);
    connect(search_, &QLineEdit::textChanged, this, &LogDialog::reload);

    auto* all = new QPushButton(tr("Цялата история"));
    connect(all, &QPushButton::clicked, this, [this] {
        QDate first = QDate::currentDate();
        for (const auto& m : inventory_.movements()) first = std::min(first, m.time.date());
        from_->setDate(first);
        to_->setDate(QDate::currentDate());
    });

    auto* filters = new QHBoxLayout;
    filters->addWidget(new QLabel(tr("От:")));
    filters->addWidget(from_);
    filters->addWidget(new QLabel(tr("До:")));
    filters->addWidget(to_);
    filters->addWidget(all);
    filters->addSpacing(12);
    filters->addWidget(search_, 1);

    table_ = new QTableWidget(0, 7);
    table_->setHorizontalHeaderLabels({tr("Дата и час"), tr("Действие"), tr("Продукт"), tr("Количество"),
                                       tr("Мярка"), tr("Място"), tr("Бележка")});
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setAlternatingRowColors(true);
    table_->verticalHeader()->hide();
    table_->horizontalHeader()->setStretchLastSection(true);

    summary_ = new QLabel;

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    buttons->button(QDialogButtonBox::Close)->setText(tr("Затвори"));
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(filters);
    layout->addWidget(table_, 1);
    layout->addWidget(summary_);
    layout->addWidget(buttons);

    reload();
}

void LogDialog::reload() {
    auto list = inventory_.movementsBetween(from_->date(), to_->date());
    const QString search = search_->text().trimmed();
    if (!search.isEmpty()) {
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [&](const storage::Movement& m) {
                                      return !m.productName.contains(search, Qt::CaseInsensitive) &&
                                             !m.note.contains(search, Qt::CaseInsensitive);
                                  }),
                   list.end());
    }
    std::stable_sort(list.begin(), list.end(),  // newest first
                     [](const storage::Movement& a, const storage::Movement& b) { return a.time > b.time; });

    table_->setRowCount(static_cast<int>(list.size()));
    int received = 0, withdrawn = 0;
    for (int r = 0; r < list.size(); ++r) {
        const auto& m = list[r];
        const QString qty = (m.quantity > 0 ? QStringLiteral("+") : QString()) + storage::formatQuantity(m.quantity);
        const QStringList cells = {m.time.toString(QStringLiteral("dd.MM.yyyy HH:mm")),
                                   storage::movementLabel(m.type),
                                   m.productName,
                                   qty,
                                   m.unit,
                                   m.location.toString(),
                                   m.note};
        for (int c = 0; c < cells.size(); ++c) {
            auto* item = new QTableWidgetItem(cells[c]);
            if (c == 3) {
                item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                if (m.quantity > 0) item->setForeground(QColor(0x15, 0x80, 0x3d));
                if (m.quantity < 0) item->setForeground(QColor(0xb9, 0x1c, 0x1c));
            }
            table_->setItem(r, c, item);
        }
        if (m.type == storage::MovementType::Received) ++received;
        if (m.type == storage::MovementType::Withdrawn) ++withdrawn;
    }
    table_->resizeColumnsToContents();
    summary_->setText(tr("%1 записа · %2 заприхождавания · %3 изписвания")
                          .arg(list.size())
                          .arg(received)
                          .arg(withdrawn));
}
