#include "storagefile.h"

#include <QFile>
#include <QHash>
#include <QMap>
#include <QSet>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>

namespace storage {

namespace {

const QChar kSeparator = QLatin1Char(';');
const QString kDateFormat = QStringLiteral("yyyy-MM-dd");
const QString kTimeFormat = QStringLiteral("yyyy-MM-dd HH:mm:ss");

const QStringList kProductHeader = {
    QStringLiteral("id"),         QStringLiteral("name"),     QStringLiteral("manufacturer"),
    QStringLiteral("unit"),       QStringLiteral("quantity"), QStringLiteral("expiration"),
    QStringLiteral("received"),   QStringLiteral("location"), QStringLiteral("comment")};

const QStringList kLogHeader = {QStringLiteral("time"),     QStringLiteral("type"),
                                QStringLiteral("name"),     QStringLiteral("quantity"),
                                QStringLiteral("unit"),     QStringLiteral("location"),
                                QStringLiteral("note")};

bool readText(const QString& path, QString& text, QString& error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        error = QStringLiteral("Файлът „%1“ не може да се отвори: %2").arg(path, file.errorString());
        return false;
    }
    text = QString::fromUtf8(file.readAll());
    if (text.startsWith(QChar(0xFEFF))) text.remove(0, 1);  // BOM
    return true;
}

QChar detectSeparator(const QString& text) {
    const QString firstLine = text.left(text.indexOf(QLatin1Char('\n')));
    return firstLine.count(QLatin1Char(';')) >= firstLine.count(QLatin1Char(',')) ? QLatin1Char(';')
                                                                                   : QLatin1Char(',');
}

// Maps header names to column indexes so columns may be reordered by hand.
QHash<QString, int> columns(const QStringList& header) {
    QHash<QString, int> map;
    for (int i = 0; i < header.size(); ++i) map.insert(header[i].trimmed().toLower(), i);
    return map;
}

QString field(const QStringList& row, const QHash<QString, int>& cols, const QString& name) {
    const int i = cols.value(name, -1);
    return i >= 0 && i < row.size() ? row[i] : QString();
}

bool parseNumber(const QString& text, double& value) {
    QString t = text.trimmed();
    t.replace(QLatin1Char(','), QLatin1Char('.'));  // tolerate Bulgarian decimal comma
    bool ok = false;
    value = t.toDouble(&ok);
    return ok;
}

bool writeFile(const QString& path, const QString& text, QString& error) {
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        error = QStringLiteral("Файлът „%1“ не може да се запише: %2").arg(path, file.errorString());
        return false;
    }
    // BOM so that Excel shows Cyrillic correctly.
    file.write("\xEF\xBB\xBF");
    file.write(text.toUtf8());
    if (!file.commit()) {
        error = QStringLiteral("Файлът „%1“ не може да се запише: %2").arg(path, file.errorString());
        return false;
    }
    return true;
}

}  // namespace

QString csvLine(const QStringList& fields, QChar separator) {
    QStringList out;
    out.reserve(fields.size());
    for (const QString& f : fields) {
        const bool quote = f.contains(separator) || f.contains(QLatin1Char('"')) ||
                           f.contains(QLatin1Char('\n')) || f.contains(QLatin1Char('\r')) ||
                           f != f.trimmed();
        if (quote) {
            QString escaped = f;
            escaped.replace(QLatin1String("\""), QLatin1String("\"\""));
            out << QLatin1Char('"') + escaped + QLatin1Char('"');
        } else {
            out << f;
        }
    }
    return out.join(separator);
}

QList<QStringList> parseCsv(const QString& text, QChar separator) {
    QList<QStringList> rows;
    QStringList row;
    QString cell;
    bool inQuotes = false;
    bool rowHasContent = false;

    for (int i = 0; i < text.size(); ++i) {
        const QChar c = text[i];
        if (inQuotes) {
            if (c == QLatin1Char('"')) {
                if (i + 1 < text.size() && text[i + 1] == QLatin1Char('"')) {
                    cell += QLatin1Char('"');
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                cell += c;
            }
            continue;
        }
        if (c == QLatin1Char('"')) {
            inQuotes = true;
            rowHasContent = true;
        } else if (c == separator) {
            row << cell;
            cell.clear();
            rowHasContent = true;
        } else if (c == QLatin1Char('\n') || c == QLatin1Char('\r')) {
            if (c == QLatin1Char('\r') && i + 1 < text.size() && text[i + 1] == QLatin1Char('\n')) ++i;
            if (rowHasContent || !cell.isEmpty()) {
                row << cell;
                rows << row;
            }
            row.clear();
            cell.clear();
            rowHasContent = false;
        } else {
            cell += c;
            rowHasContent = true;
        }
    }
    if (rowHasContent || !cell.isEmpty()) {
        row << cell;
        rows << row;
    }
    return rows;
}

QString logPathFor(const QString& productsPath) {
    QString base = productsPath;
    if (base.endsWith(QLatin1String(".csv"), Qt::CaseInsensitive)) base.chop(4);
    return base + QStringLiteral(".log.csv");
}

LoadResult loadStorage(const QString& productsPath) {
    LoadResult result;
    QString text;
    if (!readText(productsPath, text, result.error)) return result;

    const auto rows = parseCsv(text, detectSeparator(text));
    if (rows.isEmpty()) {
        result.ok = true;  // empty file = empty warehouse
        return result;
    }
    const auto cols = columns(rows.first());
    if (!cols.contains(QStringLiteral("name")) || !cols.contains(QStringLiteral("quantity"))) {
        result.error = QStringLiteral("„%1“ не е файл на склада (липсват колони name/quantity).").arg(productsPath);
        return result;
    }

    QSet<QString> ids;
    for (int r = 1; r < rows.size(); ++r) {
        const auto& row = rows[r];
        Product p;
        p.id = field(row, cols, QStringLiteral("id")).trimmed();
        p.name = field(row, cols, QStringLiteral("name")).trimmed();
        p.manufacturer = field(row, cols, QStringLiteral("manufacturer")).trimmed();
        p.unit = field(row, cols, QStringLiteral("unit")).trimmed();
        p.comment = field(row, cols, QStringLiteral("comment"));
        if (!parseNumber(field(row, cols, QStringLiteral("quantity")), p.quantity)) {
            result.error = QStringLiteral("Ред %1: невалидно количество.").arg(r + 1);
            return result;
        }
        p.expiration = QDate::fromString(field(row, cols, QStringLiteral("expiration")).trimmed(), kDateFormat);
        p.received = QDate::fromString(field(row, cols, QStringLiteral("received")).trimmed(), kDateFormat);
        const QString loc = field(row, cols, QStringLiteral("location"));
        if (!loc.trimmed().isEmpty()) {
            const auto parsed = Location::parse(loc);
            if (!parsed) {
                result.error = QStringLiteral("Ред %1: невалидно място „%2“.").arg(r + 1).arg(loc);
                return result;
            }
            p.location = *parsed;
        }
        if (p.id.isEmpty() || ids.contains(p.id)) {
            p.id = QStringLiteral("P%1").arg(10000 + r);  // repair hand-edited files
        }
        ids.insert(p.id);
        result.products << p;
    }

    // The history is optional: a missing log just means no history yet.
    const QString logPath = logPathFor(productsPath);
    if (QFileInfo::exists(logPath)) {
        QString logText;
        if (!readText(logPath, logText, result.error)) return result;
        const auto logRows = parseCsv(logText, detectSeparator(logText));
        if (!logRows.isEmpty()) {
            const auto lc = columns(logRows.first());
            for (int r = 1; r < logRows.size(); ++r) {
                const auto& row = logRows[r];
                Movement m;
                m.time = QDateTime::fromString(field(row, lc, QStringLiteral("time")).trimmed(), kTimeFormat);
                const auto type = movementFromKey(field(row, lc, QStringLiteral("type")).trimmed());
                if (!m.time.isValid() || !type) continue;  // skip broken lines, keep the rest
                m.type = *type;
                m.productName = field(row, lc, QStringLiteral("name"));
                parseNumber(field(row, lc, QStringLiteral("quantity")), m.quantity);
                m.unit = field(row, lc, QStringLiteral("unit"));
                if (const auto loc = Location::parse(field(row, lc, QStringLiteral("location")))) m.location = *loc;
                m.note = field(row, lc, QStringLiteral("note"));
                result.movements << m;
            }
        }
    }

    result.ok = true;
    return result;
}

QString saveStorage(const QString& productsPath, const Inventory& inventory) {
    QString products = csvLine(kProductHeader, kSeparator) + QLatin1Char('\n');
    for (const auto& p : inventory.products()) {
        products += csvLine({p.id, p.name, p.manufacturer, p.unit, formatQuantity(p.quantity),
                             p.expiration.toString(kDateFormat), p.received.toString(kDateFormat),
                             p.location.toString(), p.comment},
                            kSeparator) +
                    QLatin1Char('\n');
    }

    QString log = csvLine(kLogHeader, kSeparator) + QLatin1Char('\n');
    for (const auto& m : inventory.movements()) {
        log += csvLine({m.time.toString(kTimeFormat), movementKey(m.type), m.productName,
                        formatQuantity(m.quantity), m.unit, m.location.toString(), m.note},
                       kSeparator) +
               QLatin1Char('\n');
    }

    QString error;
    if (!writeFile(productsPath, products, error)) return error;
    if (!writeFile(logPathFor(productsPath), log, error)) return error;
    return {};
}

LegacyImport parseLegacyText(const QString& text) {
    LegacyImport result;

    // "<key> <index> is: <value>", e.g. "unit of the product 3 is: kg"
    static const QRegularExpression line(QStringLiteral(R"(^(.*?)\s+(\d+)\s+is:\s?(.*)$)"));
    auto legacyDate = [](const QString& v) {
        // 3052020 -> 03.05.2020, 15122021 -> 15.12.2021
        bool ok = false;
        const qint64 n = v.trimmed().toLongLong(&ok);
        if (!ok || n <= 0) return QDate();
        return QDate(static_cast<int>(n % 10000), static_cast<int>((n / 10000) % 100),
                     static_cast<int>(n / 1000000));
    };

    QMap<int, Product> byIndex;
    const auto lines = text.split(QLatin1Char('\n'));
    for (QString l : lines) {
        l = l.trimmed();
        const auto m = line.match(l);
        if (!m.hasMatch()) continue;
        const QString key = m.captured(1).toLower();
        const int index = m.captured(2).toInt();
        const QString value = m.captured(3).trimmed();
        Product& p = byIndex[index];

        if (key.startsWith(QLatin1String("name of the product"))) p.name = value;
        else if (key.startsWith(QLatin1String("the expiration date"))) p.expiration = legacyDate(value);
        else if (key.startsWith(QLatin1String("date in storage"))) p.received = legacyDate(value);
        else if (key.startsWith(QLatin1String("name of manufactor"))) p.manufacturer = value;
        else if (key.startsWith(QLatin1String("unit of the product"))) p.unit = value;
        else if (key.startsWith(QLatin1String("available quantity"))) parseNumber(value, p.quantity);
        else if (key.startsWith(QLatin1String("location of the product"))) {
            if (const auto loc = Location::parse(value)) p.location = *loc;
        } else if (key.startsWith(QLatin1String("comment for the product"))) p.comment = value;
    }

    for (auto it = byIndex.cbegin(); it != byIndex.cend(); ++it) {
        if (!it->name.isEmpty()) result.products << *it;
    }
    if (result.products.isEmpty()) {
        result.error = QStringLiteral("Във файла няма продукти в стария формат.");
        return result;
    }
    result.ok = true;
    return result;
}

LegacyImport importLegacyFile(const QString& path) {
    QString text;
    LegacyImport result;
    if (!readText(path, text, result.error)) return result;
    return parseLegacyText(text);
}

}  // namespace storage
