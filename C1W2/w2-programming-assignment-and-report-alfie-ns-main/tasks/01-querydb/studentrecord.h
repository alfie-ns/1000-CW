#ifndef STUDENTRECORD_H  // To make sure you don't declare the function more than once by including the header multiple times.
#define STUDENTRECORD_H // This is called the header guard.
#include <iostream>
#include <vector>

//Basic data structure for a record
struct Record {
    int SID;        //Student ID
    std::string name;    //Student Name
    std::vector<std::string> enrollments;
    std::vector<float> grades;
    std::string phone;
};

void printRecord(Record& r);

#endif // STUDENTRECORD_H
