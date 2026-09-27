#pragma once
#include "../models/Models.h"
#include <QList>

class FeeDao {
public:
    QList<FeeStructure> allStructures();
    bool insertStructure(FeeStructure& f);
    bool updateStructure(const FeeStructure& f);
    bool removeStructure(int id);

    QList<Payment> paymentsFor(int studentId);
    bool recordPayment(Payment& p);

    double totalDueFor(int studentId); // sum of applicable fee structures (by dept/semester) minus paid
    double totalPaidFor(int studentId);
};
