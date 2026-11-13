#pragma once

#include <QDialog>

#include <functional>

#include "inventory.h"

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;

// Taking stock out: pick the product, the amount and an optional note.
// The oldest-expiring batches are used first.
class WithdrawDialog : public QDialog {
    Q_OBJECT

public:
    using Apply = std::function<QString(const QString& name, const QString& unit, double quantity,
                                        const QString& note)>;

    WithdrawDialog(const storage::Inventory& inventory, const QDate& today, Apply apply,
                   QWidget* parent = nullptr);

    // Preselects a product (e.g. the row selected in the table).
    void select(const QString& name, const QString& unit);

    void accept() override;

private:
    void updateAvailable();

    const storage::Inventory& inventory_;
    QDate today_;
    Apply apply_;

    QComboBox* product_;
    QDoubleSpinBox* quantity_;
    QLabel* available_;
    QLineEdit* note_;
    QLabel* error_;
};
