#pragma once
#include <QString>

// Renders HTML into PDF via QTextDocument + QPrinter (no extra deps needed
// beyond Qt PrintSupport). Each method builds an HTML string and prints it.
class ReportService {
public:
    bool exportTranscript(int studentId, const QString& outputPath, QString& errorOut);
    bool exportAttendanceSheet(int courseId, const QString& semester, const QString& classDate,
                                const QString& outputPath, QString& errorOut);
    bool exportFeeStatement(int studentId, const QString& outputPath, QString& errorOut);

private:
    bool renderHtmlToPdf(const QString& html, const QString& outputPath, QString& errorOut);
};
