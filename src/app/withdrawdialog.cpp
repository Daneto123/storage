#include "withdrawdialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

WithdrawDialog::WithdrawDialog(const storage::Inventory& inventory, const QDate& today, Apply apply,
                               QWidget* parent)
    : QDialog(parent), inventory_(inventory), today_(today), apply_(std::move(apply)) {
    setWindowTitle(tr("Изписване на стока"));
    setMinimumWidth(400);

    product_ = new QComboBox;
    for (const auto& kind : inventory.productKinds()) {
        product_->addItem(QStringLiteral("%1 (%2)").arg(kind.first, kind.second),
                          QStringList{kind.first, kind.second});
    }

    quantity_ = new QDoubleSpinBox;
    quantity_->setRange(0, 1e9);
    quantity_->setDecimals(3);
    quantity_->setValue(1);

    available_ = new QLabel;
    available_->setStyleSheet(QStringLiteral("color: palette(mid);"));

    note_ = new QLineEdit;
    note_->setPlaceholderText(tr("напр. за магазин №2"));

    error_ = new QLabel;
    error_->setWordWrap(true);
    error_->setStyleSheet(QStringLiteral("color: #b91c1c;"));
    error_->hide();

    auto* form = new QFormLayout;
    form->addRow(tr("Продукт:"), product_);
    form->addRow(tr("Количество:"), quantity_);
    form->addRow(QString(), available_);
    form->addRow(tr("Бележка:"), note_);

    auto* hint = new QLabel(tr("Първо се изписват партидите с най-близък срок на годност. "
                               "Изтеклите не се изписват."));
    hint->setWordWrap(true);
    hint->setStyleSheet(QStringLiteral("color: palette(mid);"));

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Изпиши"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Отказ"));
    buttons->button(QDialogButtonBox::Ok)->setEnabled(product_->count() > 0);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(hint);
    layout->addWidget(error_);
    layout->addWidget(buttons);

    connect(product_, qOverload<int>(&QComboBox::currentIndexChanged), this, &WithdrawDialog::updateAvailable);
    updateAvailable();
}

void WithdrawDialog::select(const QString& name, const QString& unit) {
    for (int i = 0; i < product_->count(); ++i) {
        const auto pair = product_->itemData(i).toStringList();
        if (pair.value(0).compare(name, Qt::CaseInsensitive) == 0 &&
            pair.value(1).compare(unit, Qt::CaseInsensitive) == 0) {
            product_->setCurrentIndex(i);
            return;
        }
    }
}

void WithdrawDialog::updateAvailable() {
    const auto pair = product_->currentData().toStringList();
    if (pair.size() != 2) {
        available_->setText(tr("Складът е празен."));
        return;
    }
    const double available = inventory_.availableQuantity(pair[0], pair[1], today_);
    available_->setText(tr("Налични: %1 %2").arg(storage::formatQuantity(available), pair[1]));
    quantity_->setMaximum(available);
}

void WithdrawDialog::accept() {
    const auto pair = product_->currentData().toStringList();
    if (pair.size() != 2) return;
    const QString error = apply_(pair[0], pair[1], quantity_->value(), note_->text().trimmed());
    if (!error.isEmpty()) {
        error_->setText(error);
        error_->show();
        return;
    }
    QDialog::accept();
}
