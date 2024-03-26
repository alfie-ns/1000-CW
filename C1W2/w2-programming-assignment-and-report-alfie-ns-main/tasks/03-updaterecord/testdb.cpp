#include "testdb.h"
using namespace std;

void createTestDB(string fileName)
{
    ifstream ip(fileName); // input file stream
    if (ip.is_open()) {
        cout << fileName << " exists" << endl;
        ip.close();
        return;
    }

    //Create some test data
    cout << "Creating starter database for testing" << endl;
    ofstream op(fileName); // output file stream
    op << TESTSTR;
    op.close();
}
