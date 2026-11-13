#include "productdialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

using storage::Product;

namespace {

QSpinBox* placeSpin(const QString& suffix) {
    auto* s = new QSpinBox;
    s->setRange(0, 999);
    s->setPrefix(suffix);
    return s;
}

}  // namespace

ProductDialog::ProductDialog(Mode mode, const storage::Inventory& inventory,
                             std::function<QString(const Product&)> apply, QWidget* parent)
    : QDialog(parent), mode_(mode), inventory_(inventory), apply_(std::move(apply)) {
    setWindowTitle(mode == Mode::Receive ? tr("Заприхождаване на стока") : tr("Редактиране"));
    setMinimumWidth(440);

    name_ = new QLineEdit;
    name_->setPlaceholderText(tr("напр. Банани"));
    QStringList names;
    for (const auto& kind : inventory.productKinds()) {
        if (!names.contains(kind.first)) names << kind.first;
    }
    auto* completer = new QCompleter(names, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    name_->setCompleter(completer);

    manufacturer_ = new QLineEdit;

    quantity_ = new QDoubleSpinBox;
    quantity_->setRange(0, 1e9);
    quantity_->setDecimals(3);
    quantity_->setValue(1);
    quantity_->setGroupSeparatorShown(true);

    unit_ = new QComboBox;
    unit_->setEditable(true);
    unit_->addItems(storage::standardUnits());

    auto* qtyRow = new QHBoxLayout;
    qtyRow->addWidget(quantity_, 1);
    qtyRow->addWidget(unit_);

    hasExpiration_ = new QCheckBox(tr("има срок"));
    hasExpiration_->setChecked(true);
    expiration_ = new QDateEdit(QDate::currentDate().addMonths(1));
    expiration_->setCalendarPopup(true);
    expiration_->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));
    connect(hasExpiration_, &QCheckBox::toggled, expiration_, &QWidget::setEnabled);
    auto* expRow = new QHBoxLayout;
    expRow->addWidget(expiration_, 1);
    expRow->addWidget(hasExpiration_);

    received_ = new QDateEdit(QDate::currentDate());
    received_->setCalendarPopup(true);
    received_->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));

    section_ = placeSpin(tr("секция "));
    shelf_ = placeSpin(tr("рафт "));
    slot_ = placeSpin(tr("място "));
    auto* placeRow = new QHBoxLayout;
    placeRow->addWidget(section_);
    placeRow->addWidget(shelf_);
    placeRow->addWidget(slot_);

    comment_ = new QPlainTextEdit;
    comment_->setFixedHeight(64);

    error_ = new QLabel;
    error_->setWordWrap(true);
    error_->setStyleSheet(QStringLiteral("color: #b91c1c;"));
    error_->hide();

    auto* form = new QFormLayout;
    form->addRow(tr("Продукт:"), name_);
    form->addRow(tr("Производител:"), manufacturer_);
    form->addRow(tr("Количество:"), qtyRow);
    form->addRow(tr("Годен до:"), expRow);
    form->addRow(tr("Постъпил на:"), received_);
    form->addRow(tr("Място:"), placeRow);
    form->addRow(tr("Бележка:"), comment_);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(mode == Mode::Receive ? tr("Заприходи") : tr("Запази"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Отказ"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(error_);
    layout->addWidget(buttons);

    if (mode == Mode::Receive) {
        connect(name_, &QLineEdit::editingFinished, this, &ProductDialog::fillFromKnownProduct);
        connect(completer, qOverload<const QString&>(&QCompleter::activated), this,
                &ProductDialog::fillFromKnownProduct);
    }
}

void ProductDialog::fillFromKnownProduct() {
    // Typing a product that is already in stock fills in the usual details,
    // so a new delivery only needs quantity and dates.
    const QString name = name_->text().trimmed();
    if (name.isEmpty() || !manufacturer_->text().isEmpty()) return;
    for (const auto& p : inventory_.products()) {
        if (p.name.compare(name, Qt::CaseInsensitive) == 0) {
            name_->setText(p.name);
            manufacturer_->setText(p.manufacturer);
            unit_->setCurrentText(p.unit);
            section_->setValue(p.location.section);
            shelf_->setValue(p.location.shelf);
            slot_->setValue(p.location.slot);
            return;
        }
    }
}

void ProductDialog::setProduct(const Product& p) {
    id_ = p.id;
    name_->setText(p.name);
    manufacturer_->setText(p.manufacturer);
    quantity_->setValue(p.quantity);
    unit_->setCurrentText(p.unit);
    hasExpiration_->setChecked(p.expiration.isValid());
    if (p.expiration.isValid()) expiration_->setDate(p.expiration);
    if (p.received.isValid()) received_->setDate(p.received);
    section_->setValue(p.location.section);
    shelf_->setValue(p.location.shelf);
    slot_->setValue(p.location.slot);
    comment_->setPlainText(p.comment);
}

Product ProductDialog::product() const {
    Product p;
    p.id = id_;
    p.name = name_->text();
    p.manufacturer = manufacturer_->text();
    p.quantity = quantity_->value();
    p.unit = unit_->currentText();
    p.expiration = hasExpiration_->isChecked() ? expiration_->date() : QDate();
    p.received = received_->date();
    p.location = {section_->value(), shelf_->value(), slot_->value()};
    p.comment = comment_->toPlainText();
    return p;
}

void ProductDialog::accept() {
    const QString error = apply_(product());
    if (!error.isEmpty()) {
        error_->setText(error);
        error_->show();
        return;
    }
    QDialog::accept();
}
