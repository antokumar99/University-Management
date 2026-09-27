#include "CourseFormDialog.h"
#include "../../dao/FacultyDao.h"
#include "../../dao/CourseDao.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QLabel>

CourseFormDialog::CourseFormDialog(QWidget* parent, const Course* existing) : QDialog(parent) {
    setWindowTitle(existing ? "Edit Course" : "Add Course");
    setMinimumWidth(420);

    m_code = new QLineEdit;
    m_title = new QLineEdit;
    m_dept = new QLineEdit;
    m_semester = new QLineEdit;
    m_semester->setPlaceholderText("e.g. Fall2026");
    m_creditHours = new QSpinBox;
    m_creditHours->setRange(1, 6);
    m_creditHours->setValue(3);
    m_seatLimit = new QSpinBox;
    m_seatLimit->setRange(1, 500);
    m_seatLimit->setValue(40);

    m_faculty = new QComboBox;
    m_faculty->addItem("(Unassigned)", -1);
    FacultyDao facultyDao;
    for (const auto& f : facultyDao.all()) m_faculty->addItem(f.fullName() + " — " + f.department, f.id);

    m_prereqList = new QListWidget;
    m_prereqList->setSelectionMode(QAbstractItemView::NoSelection);
    CourseDao courseDao;
    QList<int> existingPrereqs;
    if (existing) existingPrereqs = courseDao.prerequisiteIds(existing->id);
    for (const auto& c : courseDao.all()) {
        if (existing && c.id == existing->id) continue; // a course can't be its own prerequisite
        auto* item = new QListWidgetItem(c.code + " — " + c.title, m_prereqList);
        item->setData(Qt::UserRole, c.id);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(existingPrereqs.contains(c.id) ? Qt::Checked : Qt::Unchecked);
    }

    if (existing) {
        m_id = existing->id;
        m_code->setText(existing->code);
        m_title->setText(existing->title);
        m_dept->setText(existing->department);
        m_semester->setText(existing->semester);
        m_creditHours->setValue(existing->creditHours);
        m_seatLimit->setValue(existing->seatLimit);
        int idx = m_faculty->findData(existing->facultyId);
        if (idx >= 0) m_faculty->setCurrentIndex(idx);
    }

    auto* form = new QFormLayout;
    form->addRow("Course Code", m_code);
    form->addRow("Title", m_title);
    form->addRow("Department", m_dept);
    form->addRow("Semester", m_semester);
    form->addRow("Credit Hours", m_creditHours);
    form->addRow("Seat Limit", m_seatLimit);
    form->addRow("Assigned Faculty", m_faculty);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(new QLabel("Prerequisites:"));
    layout->addWidget(m_prereqList);
    layout->addWidget(buttons);
}

Course CourseFormDialog::result() const {
    Course c;
    c.id = m_id;
    c.code = m_code->text().trimmed();
    c.title = m_title->text().trimmed();
    c.department = m_dept->text().trimmed();
    c.semester = m_semester->text().trimmed();
    c.creditHours = m_creditHours->value();
    c.seatLimit = m_seatLimit->value();
    c.facultyId = m_faculty->currentData().toInt();
    return c;
}

QList<int> CourseFormDialog::selectedPrerequisiteIds() const {
    QList<int> out;
    for (int i = 0; i < m_prereqList->count(); ++i) {
        auto* item = m_prereqList->item(i);
        if (item->checkState() == Qt::Checked) out.append(item->data(Qt::UserRole).toInt());
    }
    return out;
}
