#include "ReportService.h"
#include "../dao/StudentDao.h"
#include "../dao/GradeDao.h"
#include "../dao/CourseDao.h"
#include "../dao/AttendanceDao.h"
#include "../dao/FeeDao.h"
#include "../services/GradingService.h"
#include <QTextDocument>
#include <QPrinter>
#include <QPageLayout>
#include <QDateTime>
#include <QFile>

bool ReportService::renderHtmlToPdf(const QString& html, const QString& outputPath, QString& errorOut) {
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(outputPath);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    doc.print(&printer);
    if (!QFile::exists(outputPath)) {
        errorOut = "PDF file was not created.";
        return false;
    }
    return true;
}

bool ReportService::exportTranscript(int studentId, const QString& outputPath, QString& errorOut) {
    StudentDao studentDao;
    Student s = studentDao.byId(studentId);
    if (s.id < 0) { errorOut = "Student not found."; return false; }

    GradingService gs;
    QList<GradeRecord> records = gs.fullTranscript(studentId);

    QString html = QString(
        "<h2>Academic Transcript</h2>"
        "<p><b>%1</b> (%2)<br/>Department: %3</p>"
        "<table border='1' cellspacing='0' cellpadding='4' width='100%%'>"
        "<tr><th>Semester</th><th>Code</th><th>Course</th><th>Credit Hrs</th><th>Marks</th><th>Grade</th><th>Points</th></tr>"
    ).arg(s.fullName(), s.rollNo, s.department);

    for (const auto& g : records) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td></tr>")
            .arg(g.semester, g.courseCode, g.courseTitle)
            .arg(g.creditHours)
            .arg(g.marks, 0, 'f', 1)
            .arg(g.letter)
            .arg(g.point, 0, 'f', 2);
    }
    html += QString("</table><p><b>CGPA: %1</b></p>").arg(gs.cgpa(studentId), 0, 'f', 2);
    html += QString("<p style='color:#888;font-size:small'>Generated %1</p>")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));

    return renderHtmlToPdf(html, outputPath, errorOut);
}

bool ReportService::exportAttendanceSheet(int courseId, const QString& semester, const QString& classDate,
                                           const QString& outputPath, QString& errorOut) {
    CourseDao courseDao;
    AttendanceDao attDao;
    Course c = courseDao.byId(courseId);
    if (c.id < 0) { errorOut = "Course not found."; return false; }

    auto roster = attDao.rosterFor(courseId, semester, classDate);
    QString html = QString(
        "<h2>Attendance Sheet</h2>"
        "<p><b>%1 - %2</b><br/>Semester: %3 &nbsp; Date: %4</p>"
        "<table border='1' cellspacing='0' cellpadding='4' width='100%%'>"
        "<tr><th>Roll No</th><th>Student</th><th>Status</th></tr>"
    ).arg(c.code, c.title, semester, classDate);

    for (const auto& r : roster) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
            .arg(r.rollNo, r.studentName, r.present ? "Present" : "Absent");
    }
    html += "</table>";
    return renderHtmlToPdf(html, outputPath, errorOut);
}

bool ReportService::exportFeeStatement(int studentId, const QString& outputPath, QString& errorOut) {
    StudentDao studentDao;
    FeeDao feeDao;
    Student s = studentDao.byId(studentId);
    if (s.id < 0) { errorOut = "Student not found."; return false; }

    auto payments = feeDao.paymentsFor(studentId);
    double due = feeDao.totalDueFor(studentId);
    double paid = feeDao.totalPaidFor(studentId);

    QString html = QString(
        "<h2>Fee Statement</h2>"
        "<p><b>%1</b> (%2)<br/>Department: %3</p>"
        "<table border='1' cellspacing='0' cellpadding='4' width='100%%'>"
        "<tr><th>Receipt No</th><th>Date</th><th>Amount Paid</th></tr>"
    ).arg(s.fullName(), s.rollNo, s.department);

    for (const auto& p : payments) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
            .arg(p.receiptNo, p.paidAt).arg(p.amountPaid, 0, 'f', 2);
    }
    html += QString("</table><p>Total Due: %1<br/>Total Paid: %2<br/><b>Balance: %3</b></p>")
        .arg(due, 0, 'f', 2).arg(paid, 0, 'f', 2).arg(due - paid, 0, 'f', 2);

    return renderHtmlToPdf(html, outputPath, errorOut);
}
