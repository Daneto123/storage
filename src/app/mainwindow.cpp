#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSet>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStyle>
#include <QTableView>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

#include "logdialog.h"
#include "productdialog.h"
#include "productmodel.h"
#include "storagefile.h"
#include "withdrawdialog.h"

using storage::Product;

namespace {
const QString kFileFilter = QObject::tr("Склад (*.csv);;Всички файлове (*)");
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    model_ = new ProductModel(&inventory_, this);
    filter_ = new ProductFilter(this);
    filter_->setSourceModel(model_);
    filter_->setSortRole(ProductModel::SortRole);
    filter_->setSortLocaleAware(true);

    buildActions();
    buildUi();

    QSettings settings;
    if (!table_->horizontalHeader()->restoreState(settings.value(QStringLiteral("window/columns")).toByteArray())) {
        table_->sortByColumn(ProductModel::Expiration, Qt::AscendingOrder);
    }
    if (!restoreGeometry(settings.value(QStringLiteral("window/geometry")).toByteArray())) resize(1100, 640);

    updateTitle();
    updateSummary();
    updateActions();
}

QString MainWindow::defaultFilePath() {
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(dir).filePath(QStringLiteral("sklad.csv"));
}

void MainWindow::buildActions() {
    auto* style = this->style();

    receiveAction_ = new QAction(style->standardIcon(QStyle::SP_FileDialogNewFolder), tr("Заприходи"), this);
    receiveAction_->setShortcut(QKeySequence::New);
    receiveAction_->setToolTip(tr("Нова доставка (Ctrl+N)"));
    connect(receiveAction_, &QAction::triggered, this, &MainWindow::receive);

    withdrawAction_ = new QAction(style->standardIcon(QStyle::SP_ArrowRight), tr("Изпиши"), this);
    withdrawAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    withdrawAction_->setToolTip(tr("Изписване на стока (Ctrl+I)"));
    connect(withdrawAction_, &QAction::triggered, this, &MainWindow::withdraw);

    editAction_ = new QAction(style->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Редактирай"), this);
    editAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    connect(editAction_, &QAction::triggered, this, &MainWindow::edit);

    removeAction_ = new QAction(style->standardIcon(QStyle::SP_TrashIcon), tr("Изтрий"), this);
    removeAction_->setShortcut(QKeySequence::Delete);
    connect(removeAction_, &QAction::triggered, this, &MainWindow::removeSelected);

    cleanAction_ = new QAction(style->standardIcon(QStyle::SP_DialogDiscardButton), tr("Бракувай изтеклите"), this);
    cleanAction_->setToolTip(tr("Премахва всички партиди с изтекъл срок на годност"));
    connect(cleanAction_, &QAction::triggered, this, &MainWindow::cleanExpired);

    logAction_ = new QAction(style->standardIcon(QStyle::SP_FileDialogContentsView), tr("Дневник"), this);
    logAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    logAction_->setToolTip(tr("История на движенията (Ctrl+L)"));
    connect(logAction_, &QAction::triggered, this, &MainWindow::showLog);

    // Works the same on every Qt 6 version (the addAction overloads differ).
    auto add = [this](QMenu* menu, const QString& text, const QKeySequence& keys, auto slot) {
        QAction* action = menu->addAction(text);
        action->setShortcut(keys);
        connect(action, &QAction::triggered, this, slot);
    };

    auto* file = menuBar()->addMenu(tr("&Файл"));
    add(file, tr("Нов склад…"), QKeySequence(), [this] { newStorage(); });
    add(file, tr("Отвори…"), QKeySequence::Open, [this] { openStorage(); });
    add(file, tr("Запази като…"), QKeySequence::SaveAs, [this] { saveAs(); });
    file->addSeparator();
    add(file, tr("Внеси от стария формат (2020)…"), QKeySequence(), [this] { importLegacy(); });
    add(file, tr("Покажи папката с файловете"), QKeySequence(), [this] {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path_).absolutePath()));
    });
    file->addSeparator();
    add(file, tr("Изход"), QKeySequence::Quit, [this] { close(); });

    auto* stock = menuBar()->addMenu(tr("&Склад"));
    stock->addAction(receiveAction_);
    stock->addAction(withdrawAction_);
    stock->addSeparator();
    stock->addAction(editAction_);
    stock->addAction(removeAction_);
    stock->addSeparator();
    stock->addAction(cleanAction_);
    stock->addAction(logAction_);

    auto* help = menuBar()->addMenu(tr("&Помощ"));
    add(help, tr("За програмата"), QKeySequence(), [this] { showAbout(); });
}

void MainWindow::buildUi() {
    auto* toolbar = addToolBar(tr("Действия"));
    toolbar->setObjectName(QStringLiteral("actions"));
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->addAction(receiveAction_);
    toolbar->addAction(withdrawAction_);
    toolbar->addSeparator();
    toolbar->addAction(editAction_);
    toolbar->addAction(removeAction_);
    toolbar->addSeparator();
    toolbar->addAction(cleanAction_);
    toolbar->addAction(logAction_);

    search_ = new QLineEdit;
    search_->setPlaceholderText(tr("Търсене по продукт, производител, място…"));
    search_->setClearButtonEnabled(true);
    connect(search_, &QLineEdit::textChanged, filter_, &ProductFilter::setSearchText);
    auto* find = new QAction(this);
    find->setShortcut(QKeySequence::Find);
    connect(find, &QAction::triggered, search_, [this] {
        search_->setFocus();
        search_->selectAll();
    });
    addAction(find);

    onlyProblems_ = new QCheckBox(tr("Само изтичащи и изтекли"));
    connect(onlyProblems_, &QCheckBox::toggled, filter_, &ProductFilter::setOnlyProblems);

    auto* filters = new QHBoxLayout;
    filters->addWidget(search_, 1);
    filters->addWidget(onlyProblems_);

    table_ = new QTableView;
    table_->setModel(filter_);
    table_->setSortingEnabled(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(false);
    table_->setWordWrap(false);
    table_->verticalHeader()->hide();
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table_->setColumnWidth(ProductModel::Name, 220);
    table_->setColumnWidth(ProductModel::Manufacturer, 160);
    table_->setColumnWidth(ProductModel::Quantity, 100);
    table_->setColumnWidth(ProductModel::Unit, 70);
    table_->setColumnWidth(ProductModel::Expiration, 110);
    table_->setColumnWidth(ProductModel::Received, 110);
    table_->setColumnWidth(ProductModel::Place, 90);
    connect(table_, &QTableView::doubleClicked, this, &MainWindow::edit);
    connect(table_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateActions);
    connect(filter_, &QAbstractItemModel::modelReset, this, &MainWindow::updateActions);

    auto* central = new QWidget;
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(8, 8, 8, 4);
    layout->addLayout(filters);
    layout->addWidget(table_, 1);
    setCentralWidget(central);

    summary_ = new QLabel;
    summary_->setTextFormat(Qt::RichText);
    statusBar()->addPermanentWidget(summary_);
}

bool MainWindow::openFile(const QString& path) {
    if (!QFileInfo::exists(path)) {
        // A new warehouse: the file appears with the first change.
        inventory_.clear();
        path_ = path;
    } else {
        const auto loaded = storage::loadStorage(path);
        if (!loaded.ok) {
            QMessageBox::warning(this, tr("Грешка при отваряне"), loaded.error);
            return false;
        }
        inventory_.reset(loaded.products, loaded.movements);
        path_ = path;
    }
    QSettings().setValue(QStringLiteral("lastFile"), path_);
    model_->setToday(QDate::currentDate());
    updateTitle();
    updateSummary();
    updateActions();
    return true;
}

void MainWindow::newStorage() {
    const QString path = QFileDialog::getSaveFileName(this, tr("Нов склад"), QFileInfo(path_).absolutePath(), kFileFilter);
    if (path.isEmpty()) return;
    QFile::remove(path);
    QFile::remove(storage::logPathFor(path));
    openFile(path);
    save();
}

void MainWindow::openStorage() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Отвори склад"), QFileInfo(path_).absolutePath(), kFileFilter);
    if (!path.isEmpty()) openFile(path);
}

void MainWindow::saveAs() {
    const QString path = QFileDialog::getSaveFileName(this, tr("Запази като"), path_, kFileFilter);
    if (path.isEmpty()) return;
    const QString old = path_;
    path_ = path;
    if (save()) {
        QSettings().setValue(QStringLiteral("lastFile"), path_);
        updateTitle();
    } else {
        path_ = old;
    }
}

void MainWindow::importLegacy() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Внеси от стария формат"), QFileInfo(path_).absolutePath(),
                                                      tr("Текстови файлове (*.txt);;Всички файлове (*)"));
    if (path.isEmpty()) return;
    const auto imported = storage::importLegacyFile(path);
    if (!imported.ok) {
        QMessageBox::warning(this, tr("Внасяне"), imported.error);
        return;
    }
    const QDateTime now = QDateTime::currentDateTime();
    int added = 0;
    QStringList problems;
    for (auto p : imported.products) {
        p.comment = p.comment.isEmpty() ? tr("внесено от %1").arg(QFileInfo(path).fileName()) : p.comment;
        const auto r = inventory_.receive(p, now);
        if (r.ok) ++added;
        else problems << QStringLiteral("• %1: %2").arg(p.name, r.error);
    }
    changed();
    QString message = tr("Внесени са %1 партиди.").arg(added);
    if (!problems.isEmpty()) {
        message += QStringLiteral("\n\n") + tr("Пропуснати:") + QLatin1Char('\n') + problems.join(QLatin1Char('\n'));
    }
    QMessageBox::information(this, tr("Внасяне"), message);
}

void MainWindow::showAbout() {
    QMessageBox::about(this, tr("За програмата"),
                       tr("<h3>Склад %1</h3>"
                          "<p>Складова програма: заприхождаване и изписване на стока, срокове на годност, "
                          "места в склада и дневник на движенията.</p>"
                          "<p>Данните се пазят в два CSV файла, които се отварят и с Excel:<br>"
                          "<code>%2</code></p>"
                          "<p>Първата версия е писана по Обектно-ориентирано програмиране, "
                          "летен семестър 2019/2020.</p>")
                           .arg(QApplication::applicationVersion(), path_.toHtmlEscaped()));
}

void MainWindow::receive() {
    const QDateTime now = QDateTime::currentDateTime();
    QString merged;
    ProductDialog dialog(ProductDialog::Mode::Receive, inventory_,
                         [&](const Product& p) {
                             const auto r = inventory_.receive(p, now);
                             if (r.ok && r.merged) merged = p.name.trimmed();
                             return r.ok ? QString() : r.error;
                         },
                         this);
    if (dialog.exec() != QDialog::Accepted) return;
    changed();
    statusBar()->showMessage(merged.isEmpty() ? tr("Стоката е заприходена.")
                                              : tr("Количеството на „%1“ е увеличено в съществуващата партида.").arg(merged),
                             5000);
}

void MainWindow::edit() {
    const QString id = selectedId();
    const Product* current = inventory_.find(id);
    if (!current) return;
    ProductDialog dialog(ProductDialog::Mode::Edit, inventory_,
                         [&](const Product& p) {
                             const auto r = inventory_.update(p, QDateTime::currentDateTime());
                             return r.ok ? QString() : r.error;
                         },
                         this);
    dialog.setProduct(*current);
    if (dialog.exec() == QDialog::Accepted) changed();
}

void MainWindow::removeSelected() {
    const Product* p = inventory_.find(selectedId());
    if (!p) return;
    const auto answer = QMessageBox::question(
        this, tr("Изтриване"),
        tr("Да се изтрие ли „%1“ (%2 %3, място %4)?\nЗа нормално изваждане от склада използвайте „Изпиши“.")
            .arg(p->name, storage::formatQuantity(p->quantity), p->unit, p->location.toString()));
    if (answer != QMessageBox::Yes) return;
    const QString id = p->id;  // `p` points into the list that is about to change
    inventory_.remove(id, QDateTime::currentDateTime());
    changed();
}

void MainWindow::withdraw() {
    const QDateTime now = QDateTime::currentDateTime();
    WithdrawDialog dialog(inventory_, now.date(),
                          [&](const QString& name, const QString& unit, double qty, const QString& note) {
                              const auto r = inventory_.withdraw(name, unit, qty, now, note);
                              return r.ok ? QString() : r.error;
                          },
                          this);
    if (const Product* p = inventory_.find(selectedId())) dialog.select(p->name, p->unit);
    if (dialog.exec() != QDialog::Accepted) return;
    changed();
    statusBar()->showMessage(tr("Стоката е изписана."), 5000);
}

void MainWindow::cleanExpired() {
    const QDate today = QDate::currentDate();
    int count = 0;
    for (const auto& p : inventory_.products()) {
        if (p.isExpired(today)) ++count;
    }
    if (count == 0) {
        QMessageBox::information(this, tr("Бракуване"), tr("Няма партиди с изтекъл срок."));
        return;
    }
    const QString question = count == 1 ? tr("1 партида е с изтекъл срок. Да се премахне ли от склада?")
                                        : tr("%1 партиди са с изтекъл срок. Да се премахнат ли от склада?").arg(count);
    const auto answer = QMessageBox::question(this, tr("Бракуване"), question);
    if (answer != QMessageBox::Yes) return;
    inventory_.removeExpired(today, QDateTime::currentDateTime());
    changed();
}

void MainWindow::showLog() {
    LogDialog dialog(inventory_, this);
    dialog.exec();
}

void MainWindow::changed() {
    model_->setToday(QDate::currentDate());
    save();
    updateSummary();
    updateActions();
}

bool MainWindow::save() {
    QDir().mkpath(QFileInfo(path_).absolutePath());
    const QString error = storage::saveStorage(path_, inventory_);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Грешка при запис"),
                              error + QStringLiteral("\n\n") + tr("Промените са само в паметта. Опитайте „Запази като…“."));
        return false;
    }
    statusBar()->showMessage(tr("Запазено в %1").arg(QDir::toNativeSeparators(path_)), 3000);
    return true;
}

void MainWindow::updateTitle() {
    setWindowTitle(tr("Склад — %1").arg(QFileInfo(path_).fileName()));
    setWindowFilePath(path_);
}

void MainWindow::updateSummary() {
    const QDate today = model_->today();
    int expired = 0, soon = 0;
    QSet<QString> names;
    for (const auto& p : inventory_.products()) {
        if (p.isExpired(today)) ++expired;
        else if (p.expiresWithin(ProductModel::kSoonDays, today)) ++soon;
        names.insert(p.name.toLower());
    }
    QString text = tr("%1 продукта · %2 партиди").arg(names.size()).arg(inventory_.products().size());
    if (soon > 0) text += tr(" · <span style='color:#92400e'>%1 изтичат до 7 дни</span>").arg(soon);
    if (expired > 0) text += tr(" · <span style='color:#991b1b'><b>%1 с изтекъл срок</b></span>").arg(expired);
    summary_->setText(text);
}

void MainWindow::updateActions() {
    const bool hasSelection = !selectedId().isEmpty();
    editAction_->setEnabled(hasSelection);
    removeAction_->setEnabled(hasSelection);
    withdrawAction_->setEnabled(!inventory_.products().isEmpty());
    cleanAction_->setEnabled(!inventory_.products().isEmpty());
}

QString MainWindow::selectedId() const {
    const auto rows = table_->selectionModel() ? table_->selectionModel()->selectedRows() : QModelIndexList();
    if (rows.isEmpty()) return {};
    return rows.first().data(ProductModel::IdRole).toString();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QSettings settings;
    settings.setValue(QStringLiteral("window/geometry"), saveGeometry());
    settings.setValue(QStringLiteral("window/columns"), table_->horizontalHeader()->saveState());
    event->accept();
}
