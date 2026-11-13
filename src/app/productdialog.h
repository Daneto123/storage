#pragma once

#include <QDialog>

#include <functional>

#include "inventory.h"

class QCheckBox;
class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QSpinBox;

// Form for a new delivery or for editing a batch.
class ProductDialog : public QDialog {
    Q_OBJECT

public:
    enum class Mode { Receive, Edit };

    // `apply` performs the operation and returns an error message, or an
    // empty string on success; on error the dialog stays open.
    ProductDialog(Mode mode, const storage::Inventory& inventory,
                  std::function<QString(const storage::Product&)> apply, QWidget* parent = nullptr);

    void setProduct(const storage::Product& product);
    storage::Product product() const;

    void accept() override;

private:
    void fillFromKnownProduct();

    Mode mode_;
    const storage::Inventory& inventory_;
    std::function<QString(const storage::Product&)> apply_;
    QString id_;

    QLineEdit* name_;
    QLineEdit* manufacturer_;
    QDoubleSpinBox* quantity_;
    QComboBox* unit_;
    QCheckBox* hasExpiration_;
    QDateEdit* expiration_;
    QDateEdit* received_;
    QSpinBox* section_;
    QSpinBox* shelf_;
    QSpinBox* slot_;
    QPlainTextEdit* comment_;
    QLabel* error_;
};
