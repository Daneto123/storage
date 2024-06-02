/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *AddProduct;
    QDateEdit *ExpirationDate_2;
    QDateEdit *DateStorage;
    QLineEdit *ProductLine;
    QLineEdit *ManLine;
    QLineEdit *QuantLine;
    QLineEdit *UnitLine;
    QLineEdit *LocationLine;
    QTextEdit *CommentLine;
    QPlainTextEdit *PlainProduct;
    QLabel *Quantity;
    QLabel *Location;
    QLabel *ExpirationDate;
    QLabel *ProductName;
    QLabel *Manufacturer;
    QLabel *DateInStorage;
    QLabel *Unit;
    QLabel *Comment;
    QPushButton *LoadProducts;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        AddProduct = new QPushButton(centralwidget);
        AddProduct->setObjectName("AddProduct");
        AddProduct->setGeometry(QRect(230, 300, 75, 24));
        ExpirationDate_2 = new QDateEdit(centralwidget);
        ExpirationDate_2->setObjectName("ExpirationDate_2");
        ExpirationDate_2->setGeometry(QRect(140, 110, 110, 22));
        DateStorage = new QDateEdit(centralwidget);
        DateStorage->setObjectName("DateStorage");
        DateStorage->setGeometry(QRect(360, 110, 110, 22));
        DateStorage->setDate(QDate(2024, 5, 1));
        ProductLine = new QLineEdit(centralwidget);
        ProductLine->setObjectName("ProductLine");
        ProductLine->setGeometry(QRect(140, 70, 113, 22));
        ManLine = new QLineEdit(centralwidget);
        ManLine->setObjectName("ManLine");
        ManLine->setGeometry(QRect(360, 70, 113, 22));
        QuantLine = new QLineEdit(centralwidget);
        QuantLine->setObjectName("QuantLine");
        QuantLine->setGeometry(QRect(140, 150, 113, 22));
        UnitLine = new QLineEdit(centralwidget);
        UnitLine->setObjectName("UnitLine");
        UnitLine->setGeometry(QRect(360, 150, 113, 22));
        LocationLine = new QLineEdit(centralwidget);
        LocationLine->setObjectName("LocationLine");
        LocationLine->setGeometry(QRect(140, 200, 113, 22));
        CommentLine = new QTextEdit(centralwidget);
        CommentLine->setObjectName("CommentLine");
        CommentLine->setGeometry(QRect(360, 200, 104, 71));
        PlainProduct = new QPlainTextEdit(centralwidget);
        PlainProduct->setObjectName("PlainProduct");
        PlainProduct->setGeometry(QRect(60, 400, 421, 141));
        Quantity = new QLabel(centralwidget);
        Quantity->setObjectName("Quantity");
        Quantity->setGeometry(QRect(70, 160, 49, 16));
        Location = new QLabel(centralwidget);
        Location->setObjectName("Location");
        Location->setGeometry(QRect(70, 200, 49, 16));
        ExpirationDate = new QLabel(centralwidget);
        ExpirationDate->setObjectName("ExpirationDate");
        ExpirationDate->setGeometry(QRect(40, 110, 81, 20));
        ProductName = new QLabel(centralwidget);
        ProductName->setObjectName("ProductName");
        ProductName->setGeometry(QRect(40, 70, 81, 16));
        Manufacturer = new QLabel(centralwidget);
        Manufacturer->setObjectName("Manufacturer");
        Manufacturer->setGeometry(QRect(270, 70, 81, 20));
        DateInStorage = new QLabel(centralwidget);
        DateInStorage->setObjectName("DateInStorage");
        DateInStorage->setGeometry(QRect(270, 110, 81, 20));
        Unit = new QLabel(centralwidget);
        Unit->setObjectName("Unit");
        Unit->setGeometry(QRect(310, 150, 31, 16));
        Comment = new QLabel(centralwidget);
        Comment->setObjectName("Comment");
        Comment->setGeometry(QRect(290, 200, 61, 16));
        LoadProducts = new QPushButton(centralwidget);
        LoadProducts->setObjectName("LoadProducts");
        LoadProducts->setGeometry(QRect(210, 360, 101, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        AddProduct->setText(QCoreApplication::translate("MainWindow", "add product", nullptr));
        Quantity->setText(QCoreApplication::translate("MainWindow", "Quantity", nullptr));
        Location->setText(QCoreApplication::translate("MainWindow", "Location", nullptr));
        ExpirationDate->setText(QCoreApplication::translate("MainWindow", "Expiration date", nullptr));
        ProductName->setText(QCoreApplication::translate("MainWindow", "Product name", nullptr));
        Manufacturer->setText(QCoreApplication::translate("MainWindow", "Manufacturer", nullptr));
        DateInStorage->setText(QCoreApplication::translate("MainWindow", "Date in storage", nullptr));
        Unit->setText(QCoreApplication::translate("MainWindow", "Unit", nullptr));
        Comment->setText(QCoreApplication::translate("MainWindow", "Comment", nullptr));
        LoadProducts->setText(QCoreApplication::translate("MainWindow", "Load Products", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
