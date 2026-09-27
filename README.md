# University Management System

A desktop university management application built with **Qt6 Widgets** and **C++17**,
using a local **SQLite** database (via Qt SQL) so it runs with zero external setup.

## Features implemented

- **Auth & Roles** — Admin, Faculty, Student, Registrar. Login screen, session
  singleton, and a role→permission matrix (`core/Session.h`) that drives which
  tabs/actions each role sees in `MainWindow`.
- **Student Management** — admission, profile fields, search, edit, remove
  (`ui/student`).
- **Faculty Management** — profiles, department/designation, workload display
  (sum of assigned course credit hours vs. max load) (`ui/faculty`).
- **Course & Curriculum** — courses with credit hours, department, semester,
  seat limits, faculty assignment, and a checkable prerequisite picker
  (`ui/course`).
- **Enrollment/Registration** — register/drop with live seat-limit checks,
  duplicate-enrollment prevention, and prerequisite verification, all enforced
  in `services/EnrollmentService` (not just the UI).
- **Attendance** — per-course, per-date roster with checkable present/absent,
  persisted per enrollment (`ui/attendance`, `dao/AttendanceDao`).
- **Examination & Grading** — marks entry per course offering; letter grade
  and grade point are derived automatically (`services/GradingService`), with
  semester GPA and CGPA calculation.
- **Fees/Finance** — fee structures by department/semester, payment recording
  with auto-generated receipt numbers, running balance per student
  (`ui/fees`).
- **Reporting** — PDF export for transcripts, attendance sheets, and fee
  statements via `QTextDocument` + `QPrinter` (`services/ReportService`).

## Architecture

```
src/
├── main.cpp              # opens DB, shows LoginWindow, then MainWindow
├── core/                 # DatabaseManager (SQLite singleton), Session (RBAC)
├── models/                # plain structs: Student, Faculty, Course, Enrollment, ...
├── dao/                  # one *Dao class per table — raw SQL only, no business rules
├── services/              # AuthService, EnrollmentService, GradingService, ReportService
└── ui/
    ├── LoginWindow, MainWindow
    ├── student/  faculty/  course/  enrollment/
    ├── attendance/  exam/  fees/  reports/
```

The layering is deliberate: **DAOs** only know SQL, **services** hold business
rules (seat limits, prerequisites, GPA math, PDF rendering), and **UI widgets**
are thin — they call a service or DAO and render the result. This keeps rules
like "can't register if the course is full" testable and reusable outside the
GUI.

### Note on scope vs. the originally sketched tree

This build consolidates a few pieces from the original folder sketch for
practicality in a single-pass build:
- `StudentTableModel`/`CourseTableModel` (`QAbstractTableModel` subclasses)
  were simplified to `QTableWidget` population in each widget. Swapping in
  real table models is a straightforward follow-up if you need sorting/
  filtering at scale.
- `.ui` Designer files were skipped in favor of code-built layouts, so there's
  no dependency on Qt Designer / `.ui` XML — everything in `ui/` is plain C++.
- The DB uses SQLite (`database/DatabaseManager` equivalent is
  `core/DatabaseManager`) rather than a MySQL/Postgres server, so the app runs
  without any DB server install. The schema is embedded in
  `DatabaseManager::initSchema()` rather than a separate `schema.sql`/
  migrations folder — copy those `CREATE TABLE` statements out if you want a
  standalone `docs/schema.sql`.
- `tests/` unit tests were not included in this pass.

## Building

Requirements: CMake 3.16+, Qt6 (Widgets, Sql, PrintSupport modules), a C++17
compiler.

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64   # adjust to your Qt install
cmake --build .
./UniversityManagementSystem
```

On first run the app creates `university.db` in the working directory and
seeds two accounts:

| Username    | Password       | Role       |
|-------------|----------------|------------|
| `admin`     | `admin123`     | Admin      |
| `registrar` | `registrar123` | Registrar  |

Admin/Registrar can add students and faculty; to log in *as* a student or
faculty member you'll need to create a `users` row linking to their
`students`/`faculty` id (the schema and `AuthService::registerUser` support
this — a "create login for this person" button in the Student/Faculty forms
is a natural next addition).

## Known follow-ups

- No `AuthService::registerUser` UI hookup yet — new student/faculty logins
  must be created via `UserDao`/SQL directly.
- Attendance percentage (`AttendanceDao::attendancePercent`) is computed but
  not yet surfaced in a dedicated attendance-report view.
- Fee structures apply by department only.