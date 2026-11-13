#pragma once

#include <QMainWindow>

#include "inventory.h"

class QAction;
class QCheckBox;
class QLabel;
class QLineEdit;
class QTableView;
class ProductFilter;
class ProductModel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    // Opens a warehouse file; returns false (and tells the user) on error.
    bool openFile(const QString& path);

    // Path used when the user has never chosen a file.
    static QString defaultFilePath();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void buildActions();
    void buildUi();

    void newStorage();
    void openStorage();
    void saveAs();
    void importLegacy();
    void showAbout();

    void receive();
    void edit();
    void removeSelected();
    void withdraw();
    void cleanExpired();
    void showLog();

    // Saves after every change (like the original program did) and
    // refreshes the table and the summary.
    void changed();
    bool save();
    void updateTitle();
    void updateSummary();
    void updateActions();
    QString selectedId() const;

    storage::Inventory inventory_;
    QString path_;

    ProductModel* model_;
    ProductFilter* filter_;
    QTableView* table_;
    QLineEdit* search_;
    QCheckBox* onlyProblems_;
    QLabel* summary_;

    QAction* receiveAction_;
    QAction* editAction_;
    QAction* removeAction_;
    QAction* withdrawAction_;
    QAction* cleanAction_;
    QAction* logAction_;
};
