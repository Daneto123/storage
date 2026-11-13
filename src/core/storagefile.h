#pragma once

#include <QString>
#include <QStringList>

#include "inventory.h"

namespace storage {

// Files on disk. A warehouse is two CSV files that open in Excel too:
//   name.csv      the products (one batch per row)
//   name.log.csv  the history of every change
// Both are UTF-8 with ';' as separator; when reading, ',' also works.

struct LoadResult {
    bool ok = false;
    QString error;
    QList<Product> products;
    QList<Movement> movements;
};

QString logPathFor(const QString& productsPath);

LoadResult loadStorage(const QString& productsPath);

// Writes both files atomically (a crash never leaves half a file).
// Returns an empty string on success, otherwise the error.
QString saveStorage(const QString& productsPath, const Inventory& inventory);

// Reads the text format written by the original 2020 console version
// ("name of the product 0 is: banan" ...). Dates there are numbers like
// 3052020 = 03.05.2020.
struct LegacyImport {
    bool ok = false;
    QString error;
    QList<Product> products;
};
LegacyImport importLegacyFile(const QString& path);
LegacyImport parseLegacyText(const QString& text);

// CSV helpers, exposed for the tests.
QString csvLine(const QStringList& fields, QChar separator = QLatin1Char(';'));
QList<QStringList> parseCsv(const QString& text, QChar separator);

}  // namespace storage
