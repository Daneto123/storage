#pragma once

#include <QDialog>

#include "inventory.h"

class QDateEdit;
class QLabel;
class QLineEdit;
class QTableWidget;

// History of every receipt, withdrawal and change between two dates.
class LogDialog : public QDialog {
    Q_OBJECT

public:
    explicit LogDialog(const storage::Inventory& inventory, QWidget* parent = nullptr);

private:
    void reload();

    const storage::Inventory& inventory_;
    QDateEdit* from_;
    QDateEdit* to_;
    QLineEdit* search_;
    QTableWidget* table_;
    QLabel* summary_;
};
