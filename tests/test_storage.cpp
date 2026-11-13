#include <QTemporaryDir>
#include <QTest>

#include "inventory.h"
#include "storagefile.h"

using namespace storage;

namespace {

const QDateTime kNow(QDate(2026, 3, 10), QTime(9, 30));

Product make(const QString& name, double qty, const QDate& expiration, Location loc = {1, 1, 1},
             const QString& unit = QStringLiteral("kg")) {
    Product p;
    p.name = name;
    p.manufacturer = QStringLiteral("Иван");
    p.unit = unit;
    p.quantity = qty;
    p.expiration = expiration;
    p.received = QDate(2026, 3, 1);
    p.location = loc;
    return p;
}

}  // namespace

class StorageTests : public QObject {
    Q_OBJECT

private slots:
    void locationFormatAndParse() {
        QCOMPARE((Location{1, 2, 5}).toString(), QStringLiteral("01/02/05"));
        const auto parsed = Location::parse(QStringLiteral(" 3/14/7 "));
        QVERIFY(parsed.has_value());
        QCOMPARE(*parsed, (Location{3, 14, 7}));
        QVERIFY(!Location::parse(QStringLiteral("abc")).has_value());
    }

    void receiveCreatesAndMerges() {
        Inventory inv;
        const auto a = inv.receive(make(QStringLiteral("Банан"), 20, QDate(2026, 5, 3)), kNow);
        QVERIFY(a.ok);
        QVERIFY(!a.merged);
        QCOMPARE(a.productId, QStringLiteral("P0001"));

        // Same batch -> quantity grows.
        const auto b = inv.receive(make(QStringLiteral("банан "), 5, QDate(2026, 5, 3)), kNow);
        QVERIFY(b.ok);
        QVERIFY(b.merged);
        QCOMPARE(inv.products().size(), 1);
        QCOMPARE(inv.products().first().quantity, 25.0);

        // Different expiration -> separate batch, may share the place.
        const auto c = inv.receive(make(QStringLiteral("Банан"), 7, QDate(2026, 6, 1)), kNow);
        QVERIFY(c.ok);
        QCOMPARE(inv.products().size(), 2);
        QCOMPARE(inv.movements().size(), 3);
    }

    void placeIsTakenByAnotherProduct() {
        Inventory inv;
        QVERIFY(inv.receive(make(QStringLiteral("Мед"), 3, QDate(2026, 8, 1), {3, 3, 7}), kNow).ok);
        const auto r = inv.receive(make(QStringLiteral("Захар"), 3, QDate(2026, 8, 1), {3, 3, 7}), kNow);
        QVERIFY(!r.ok);
        QVERIFY(r.error.contains(QStringLiteral("03/03/07")));
        QVERIFY(r.error.contains(QStringLiteral("Мед")));
    }

    void validationRejectsBadInput() {
        Inventory inv;
        QVERIFY(!inv.receive(make(QString(), 1, QDate(2026, 5, 1)), kNow).ok);
        QVERIFY(!inv.receive(make(QStringLiteral("Мляко"), 0, QDate(2026, 5, 1)), kNow).ok);
        QVERIFY(!inv.receive(make(QStringLiteral("Мляко"), 1, QDate(2026, 2, 1)), kNow).ok);  // expires before received
        QVERIFY(inv.receive(make(QStringLiteral("Сол"), 1, QDate()), kNow).ok);              // no expiration is fine
    }

    void withdrawTakesFirstExpiringFirst() {
        Inventory inv;
        inv.receive(make(QStringLiteral("Мляко"), 10, QDate(2026, 4, 20), {1, 1, 1}, QStringLiteral("l")), kNow);
        inv.receive(make(QStringLiteral("Мляко"), 10, QDate(2026, 3, 20), {1, 1, 2}, QStringLiteral("l")), kNow);
        inv.receive(make(QStringLiteral("Мляко"), 10, QDate(2026, 3, 1), {1, 1, 3}, QStringLiteral("l")), kNow);  // expired

        QCOMPARE(inv.availableQuantity(QStringLiteral("мляко"), QStringLiteral("l"), kNow.date()), 20.0);

        const auto r = inv.withdraw(QStringLiteral("Мляко"), QStringLiteral("l"), 12, kNow);
        QVERIFY2(r.ok, qPrintable(r.error));
        // March batch emptied and removed, April batch has 8 left, expired untouched.
        QCOMPARE(inv.products().size(), 2);
        double april = -1, expired = -1;
        for (const auto& p : inv.products()) {
            if (p.expiration == QDate(2026, 4, 20)) april = p.quantity;
            if (p.expiration == QDate(2026, 3, 1)) expired = p.quantity;
        }
        QCOMPARE(april, 8.0);
        QCOMPARE(expired, 10.0);
    }

    void withdrawRefusesWhenNotEnough() {
        Inventory inv;
        inv.receive(make(QStringLiteral("Захар"), 5, QDate(2026, 9, 1)), kNow);
        const auto r = inv.withdraw(QStringLiteral("Захар"), QStringLiteral("kg"), 6, kNow);
        QVERIFY(!r.ok);
        QVERIFY(r.error.contains(QStringLiteral("5")));
        QCOMPARE(inv.products().first().quantity, 5.0);  // nothing changed
        QVERIFY(!inv.withdraw(QStringLiteral("Захар"), QStringLiteral("g"), 1, kNow).ok);  // other unit
    }

    void removeExpiredAndHistory() {
        Inventory inv;
        inv.receive(make(QStringLiteral("Кисело мляко"), 4, QDate(2026, 3, 5), {1, 1, 1}), kNow);
        inv.receive(make(QStringLiteral("Ориз"), 4, QDate(2027, 1, 1), {1, 1, 2}), kNow);
        QCOMPARE(inv.removeExpired(kNow.date(), kNow), 1);
        QCOMPARE(inv.products().size(), 1);
        QCOMPARE(inv.movements().last().type, MovementType::Expired);
        QCOMPARE(inv.movements().last().quantity, -4.0);
        QCOMPARE(inv.movementsBetween(QDate(2026, 3, 10), QDate(2026, 3, 10)).size(), 3);
        QCOMPARE(inv.movementsBetween(QDate(2026, 3, 11), QDate()).size(), 0);
    }

    void updateAndRemove() {
        Inventory inv;
        const auto id = inv.receive(make(QStringLiteral("Чай"), 2, QDate(2026, 12, 1)), kNow).productId;
        Product p = *inv.find(id);
        p.quantity = 5;
        p.location = {2, 2, 2};
        QVERIFY(inv.update(p, kNow).ok);
        QCOMPARE(inv.find(id)->quantity, 5.0);
        QCOMPARE(inv.movements().last().quantity, 3.0);
        QVERIFY(inv.movements().last().note.contains(QStringLiteral("02/02/02")));
        QVERIFY(inv.remove(id, kNow).ok);
        QVERIFY(inv.products().isEmpty());
        QVERIFY(!inv.remove(id, kNow).ok);
    }

    void csvQuotingRoundTrip() {
        const QStringList fields = {QStringLiteral("обикновен"), QStringLiteral("с ; точка и запетая"),
                                    QStringLiteral("с \"кавички\""), QStringLiteral("два\nреда"),
                                    QString(), QStringLiteral(" интервал ")};
        const auto rows = parseCsv(csvLine(fields) + QLatin1Char('\n'), QLatin1Char(';'));
        QCOMPARE(rows.size(), 1);
        QCOMPARE(rows.first(), fields);
    }

    void saveAndLoadRoundTrip() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("склад.csv"));

        Inventory inv;
        auto p = make(QStringLiteral("Олио \"Бисер\""), 12.5, QDate(2026, 10, 1), {4, 2, 9}, QStringLiteral("l"));
        p.comment = QStringLiteral("рафт до вратата; внимавай");
        inv.receive(p, kNow);
        inv.receive(make(QStringLiteral("Сол"), 3, QDate(), {4, 2, 10}), kNow);
        inv.withdraw(QStringLiteral("Сол"), QStringLiteral("kg"), 1, kNow, QStringLiteral("за кухнята"));

        QCOMPARE(saveStorage(path, inv), QString());
        QVERIFY(QFile::exists(logPathFor(path)));

        const auto loaded = loadStorage(path);
        QVERIFY2(loaded.ok, qPrintable(loaded.error));
        QCOMPARE(loaded.products.size(), 2);
        const auto& o = loaded.products.first();
        QCOMPARE(o.name, p.name);
        QCOMPARE(o.quantity, 12.5);
        QCOMPARE(o.expiration, p.expiration);
        QCOMPARE(o.received, p.received);
        QCOMPARE(o.location, p.location);
        QCOMPARE(o.comment, p.comment);
        QVERIFY(!loaded.products.last().expiration.isValid());
        QCOMPARE(loaded.products.last().quantity, 2.0);

        QCOMPARE(loaded.movements.size(), inv.movements().size());
        QCOMPARE(loaded.movements.last().type, MovementType::Withdrawn);
        QCOMPARE(loaded.movements.last().note, QStringLiteral("за кухнята"));
        QCOMPARE(loaded.movements.last().time, kNow);
    }

    void loadAcceptsCommaSeparatedFiles() {
        QTemporaryDir dir;
        const QString path = dir.filePath(QStringLiteral("hand.csv"));
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("name,quantity,unit,location\nBread,\"2,5\",kg,1/2/3\nMilk,4,l,\n");
        f.close();
        const auto loaded = loadStorage(path);
        QVERIFY2(loaded.ok, qPrintable(loaded.error));
        QCOMPARE(loaded.products.size(), 2);
        QCOMPARE(loaded.products[0].quantity, 2.5);
        QCOMPARE(loaded.products[0].location, (Location{1, 2, 3}));
        QVERIFY(loaded.products[0].id != loaded.products[1].id);
    }

    void loadReportsBrokenFiles() {
        QVERIFY(!loadStorage(QStringLiteral("/no/such/file.csv")).ok);
        QTemporaryDir dir;
        const QString path = dir.filePath(QStringLiteral("bad.csv"));
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("name;quantity\nBread;много\n");
        f.close();
        const auto loaded = loadStorage(path);
        QVERIFY(!loaded.ok);
        QVERIFY(loaded.error.contains(QStringLiteral("Ред 2")));
    }

    void importsTheOriginal2020File() {
        const auto imported = importLegacyFile(QStringLiteral(STORAGE_DATA_DIR "/legacy-products.txt"));
        QVERIFY2(imported.ok, qPrintable(imported.error));
        QCOMPARE(imported.products.size(), 8);
        const auto& banana = imported.products.first();
        QCOMPARE(banana.name, QStringLiteral("banan"));
        QCOMPARE(banana.expiration, QDate(2020, 5, 3));
        QCOMPARE(banana.received, QDate(2020, 1, 3));
        QCOMPARE(banana.quantity, 20.0);
        QCOMPARE(banana.location, (Location{1, 1, 5}));
        QCOMPARE(banana.comment, QStringLiteral("tova e testov product"));

        // The old program did not check dates: "desert" expired before it
        // arrived. The new one refuses that batch and keeps the rest.
        Inventory inv;
        QStringList rejected;
        for (const auto& p : imported.products) {
            if (!inv.receive(p, kNow).ok) rejected << p.name;
        }
        QCOMPARE(inv.products().size(), 7);
        QCOMPARE(rejected, QStringList{QStringLiteral("desert")});
    }
};

QTEST_GUILESS_MAIN(StorageTests)
#include "test_storage.moc"
