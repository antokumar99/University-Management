#include "FeeDao.h"
#include "../core/DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

QList<FeeStructure> FeeDao::allStructures() {
    QList<FeeStructure> out;
    QSqlQuery q("SELECT * FROM fee_structures ORDER BY id DESC", DatabaseManager::instance().db());
    while (q.next()) {
        FeeStructure f;
        f.id = q.value("id").toInt();
        f.department = q.value("department").toString();
        f.semester = q.value("semester").toString();
        f.amount = q.value("amount").toDouble();
        f.description = q.value("description").toString();
        out.append(f);
    }
    return out;
}

bool FeeDao::insertStructure(FeeStructure& f) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO fee_structures (department, semester, amount, description) VALUES (?,?,?,?)");
    q.addBindValue(f.department);
    q.addBindValue(f.semester);
    q.addBindValue(f.amount);
    q.addBindValue(f.description);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    f.id = q.lastInsertId().toInt();
    return true;
}

bool FeeDao::updateStructure(const FeeStructure& f) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("UPDATE fee_structures SET department=?, semester=?, amount=?, description=? WHERE id=?");
    q.addBindValue(f.department);
    q.addBindValue(f.semester);
    q.addBindValue(f.amount);
    q.addBindValue(f.description);
    q.addBindValue(f.id);
    return q.exec();
}

bool FeeDao::removeStructure(int id) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("DELETE FROM fee_structures WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}

QList<Payment> FeeDao::paymentsFor(int studentId) {
    QList<Payment> out;
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT * FROM payments WHERE student_id=? ORDER BY id DESC");
    q.addBindValue(studentId);
    q.exec();
    while (q.next()) {
        Payment p;
        p.id = q.value("id").toInt();
        p.studentId = q.value("student_id").toInt();
        p.feeStructureId = q.value("fee_structure_id").toInt();
        p.amountPaid = q.value("amount_paid").toDouble();
        p.paidAt = q.value("paid_at").toString();
        p.receiptNo = q.value("receipt_no").toString();
        out.append(p);
    }
    return out;
}

bool FeeDao::recordPayment(Payment& p) {
    if (p.receiptNo.isEmpty())
        p.receiptNo = "RCPT-" + QString::number(QDateTime::currentSecsSinceEpoch());
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("INSERT INTO payments (student_id, fee_structure_id, amount_paid, receipt_no) VALUES (?,?,?,?)");
    q.addBindValue(p.studentId);
    q.addBindValue(p.feeStructureId > 0 ? QVariant(p.feeStructureId) : QVariant(QMetaType(QMetaType::Int)));
    q.addBindValue(p.amountPaid);
    q.addBindValue(p.receiptNo);
    if (!q.exec()) { qWarning() << q.lastError().text(); return false; }
    p.id = q.lastInsertId().toInt();
    return true;
}

double FeeDao::totalDueFor(int studentId) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare(
        "SELECT COALESCE(SUM(fs.amount),0) FROM fee_structures fs "
        "JOIN students s ON s.department = fs.department "
        "WHERE s.id = ?");
    q.addBindValue(studentId);
    q.exec();
    double total = 0;
    if (q.next()) total = q.value(0).toDouble();
    return total;
}

double FeeDao::totalPaidFor(int studentId) {
    QSqlQuery q(DatabaseManager::instance().db());
    q.prepare("SELECT COALESCE(SUM(amount_paid),0) FROM payments WHERE student_id=?");
    q.addBindValue(studentId);
    q.exec();
    if (q.next()) return q.value(0).toDouble();
    return 0.0;
}
