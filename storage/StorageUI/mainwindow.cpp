#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_AddProduct_clicked()
{
    QString name = ui->nameLineEdit->text();
    int expiration_date = ui->expirationDateLineEdit->text().toInt();
    int date_in_storage = ui->dateInStorageLineEdit->text().toInt();
    QString manufacturer = ui->manufacturerLineEdit->text();
    QString unit = ui->unitLineEdit->text();
    int quantity = ui->quantityLineEdit->text().toInt();
    QString comment = ui->commentLineEdit->text();
    int location = ui->locationLineEdit->text().toInt();

    Product product(name.toStdString().c_str(), expiration_date, date_in_storage, 
                    manufacturer.toStdString().c_str(), unit.toStdString().c_str(), 
                    quantity, comment.toStdString().c_str());
    storage.add_product(product, location);

    QString filename = QFileDialog::getSaveFileName(this, "Save Product", "", "Text Files (*.txt)");
    if (!filename.isEmpty())
    {
        storage.in_to_file(filename.toStdString());
    }
}

void MainWindow::on_loadButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Product", "", "Text Files (*.txt)");
    if (!filename.isEmpty())
    {
        // Assuming storage has a method to load from file
        storage.load_from_file(filename.toStdString());

        // Populate UI fields with loaded data (assuming only one product for simplicity)
        // You might need to adjust this based on your actual file structure and data handling.
        const Product& product = storage.get_product(0); // Get the first product as an example
        ui->nameLineEdit->setText(product.get_name());
        ui->expirationDateLineEdit->setText(QString::number(product.get_expiration_date()));
        ui->dateInStorageLineEdit->setText(QString::number(product.get_date_in_storage()));
        ui->manufacturerLineEdit->setText(product.get_name_of_manufacturer());
        ui->unitLineEdit->setText(product.get_unit());
        ui->quantityLineEdit->setText(QString::number(product.get_quantity()));
        ui->commentLineEdit->setText(product.get_comment());
        ui->locationLineEdit->setText(QString::number(product.get_location()));
    }
}
