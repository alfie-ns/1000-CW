#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <map>
#include <string>
#include "testdb.h"
#include "studentrecord.h"
using namespace std;

int findArg(int argc, char* argv[], string pattern); // see bottem of main

// Main program here

int main(int argc, char* argv[])
{
    if (argc == 1) {
        //Welcome message
        cout << "updaterecord (c)2023" << endl;

        //Create some test data
        createTestDB("computing.txt");

        //Done
        return EXIT_SUCCESS;
    }

    Record s;
    int dbArg = findArg(argc, argv, "-db");
    string dbName = argv[dbArg + 1];

    // find -db argument first


    if (dbArg) {
        cout << "Database: " << dbName << endl;
    }
    else {
        cout << "Please specify a database file with -db <filename>" << endl;
    }

    // OPEN DATABASE FILE
    ifstream dbFile("computing.txt"); // open file as read only
    if (!dbFile.is_open()) {
        cout << "Cannot not open file " << dbName << endl;
        return EXIT_FAILURE;
    }


    //Build data structure with all data contained within it
    vector<Record> db;

    //Locals used for navigating the database file
    string nextLine;
    string nextStr;
    int recordNumber = -1;
    Record nextRecord;
    stringstream moduleCodes;
    stringstream moduleGrades;
    string moduleCode = "";
    string moduleGrade = "";

    //This is just an integer type, where START=0, NEXTTAG=1, RECORD=2 etc.....
    enum state_t { START, NEXTTAG, RECORD, SID, NAME, ENROLLMENTS, GRADES, PHONE };
    state_t state = START;

    //"Look up table" - maps a string to an integer. e.g. nextState["#SID"] returns 1
    map<string, state_t> nextState = {
        {"#RECORD", RECORD},
        {"#SID", SID},
        {"#NAME", NAME},
        {"#ENROLLMENTS", ENROLLMENTS},
        {"#GRADES", GRADES},
        {"#PHONE", PHONE}
    };

    // READ DATABASE TO MEMORY
    try
    {

        if (dbFile) {

            while (getline(dbFile, nextLine)) // AN: read every line in the file
            {
                // Remove leading spaces
                // Replace "start of line (^) followed by any number of trailing spaces (' +')" with with nothing ""
                string nextStr = regex_replace(nextLine, regex("^ +"), ""); // AN: trims off the starting spaces


                if (nextStr.empty()) continue; // AN: if nextStr is empty, continue to next line


                switch (state) // AN: switch statement to switch between states
                {

                case START: // AN: if state is START
                    //We begin here - the first non-blank line MUST start with "#RECORD"
                    if (nextStr != "#RECORD") { // AN: if nextStr is not equal to "#RECORD"
                        // throw error cus first line MUST simply read #RECORD
                        throw("Expected #RECORD as first tag");
                    }
                    //Next time around the loop, use the RECORD state
                    state = RECORD; // AN: set state to RECORD
                    recordNumber = 0; // AN: set recordNumber to 0 because we are starting from the first record
                    break;

                case RECORD: //Everytime a #RECORD is found, we enter this state on the next line
                    //Except for the first occasion, save the record we have just finished reading
                    if (recordNumber > 0) { // AN: if recordNumber is more than 0, then we have already read a record
                        //For each new #RECORD tag, store the previous
                        db.push_back(nextRecord); // AN: push the nextRecord into the db vector. Push_back() adds a new element to the end of the vector
                        //Reset the nextRecord to defaults
                        nextRecord = { 0 }; // AN: reset record struct to default/starting values
                    }
                    //Increment the record number
                    recordNumber++; // AN: serves a counter for each record
                    // AN: don't break because we want to fall through to NEXTTAG

                //Fall through into SEEK (note the break is missing) - #RECORD is always followed by a tag
                case NEXTTAG:
                    //nextString should contain a tag at this point - will throw an exception if not
                    state = nextState[nextStr]; // AN: set state to the next string in the nextState map
                    break;

                case SID: // check for SID state
                    //nextStr should contain a string representation of an integer. If not, an exception will be thrown
                    nextRecord.SID = stoi(nextStr); // AN: convert nxtStr to an int and store in the nextRecord struct
                    //Now look for the next tag
                    state = NEXTTAG; // AN: set state to NEXTTAG to move to the next tag
                    break;

                case NAME:
                    //nextStr should contain a name
                    nextRecord.name = nextStr; // AN: records.name = the nextStr
                    state = NEXTTAG; // AN: set state to NEXTTAG to move to the next tag
                    break;

                case ENROLLMENTS:
                    //nextString should be a list of module codes separated by spaces
                    moduleCodes = stringstream(nextStr); // AN: convert nextStr to a stringstream so that so that individual moduleCodes can be easily extracted
                    //Extract each separate module code
                    while (moduleCodes.eof() == false) { // AN: while the end of the stringstream is not reached
                        moduleCodes >> moduleCode; // AN:  for each iteration, extract the next moduleCode
                        //If the prevous line did not succeed, we've probably read a space on the end
                        if (moduleCodes.fail()) { // AN: if the extraction fails(if previous line didn't succeed), break out of the while loop
                            break;
                        }
                        //Add the module string to the `enrollments` vector
                        nextRecord.enrollments.push_back(moduleCode);
                    }
                    state = NEXTTAG;
                    break;

                case GRADES:
                    //nextString should contain a list of grades, separated by spaces
                    moduleGrades = stringstream(nextStr); // AN: convert nextStr to a stringstream so that so that individual moduleGrades can be easily extracted
                    //Extract each separate module grade as a float
                    while (moduleGrades.eof() == false) { // AN: while the end of the stringstream is not reached
                        moduleGrades >> moduleGrade; // AN:  for each iteration, extract the next moduleGrade
                        if (moduleGrades.fail()) { // AN: if the extraction fails(if previous line didn't succeed), break out of the while loop
                            break;
                        }
                        //Convert to float, and save in the grades vector
                        nextRecord.grades.push_back(stof(moduleGrade)); //This can throw an exception
                    }
                    state = NEXTTAG;
                    break;

                case PHONE:
                    //Next String should be a phone number (no spaces)
                    nextRecord.phone = nextStr; // AN: records.phone = the nextStr
                    state = NEXTTAG;
                    break;
                } //End Switch

            } //End while

            //The loop above may exit before pushing the last record into db
            if (nextRecord.SID > 0) {
                db.push_back(nextRecord);
            }

            //Close the file - we are done reading it. Everything is now in the db vector
            dbFile.close();

        }
        //end try

        // THE FILE HAS BEEN READ INTO MEMORY

        // first init showall argument to print record
        if (findArg(argc, argv, "-showAll")) {
            for (Record& r : db) {
                printRecord(r);
                cout << endl;
            }
        }

        // find sid argument
        int sidArg = findArg(argc, argv, "-sid"); // position of sid argument
        string strID = argv[sidArg + 1]; // get the sid argument
        if (sidArg) {
            if (strID.empty() || strID[0] == '-' || strID.length() != 5 || strID.find_first_not_of("0123456789") != string::npos) { // if sid is empty or starts with a - or is not 5 digits or contains non digits
				cout << "Please enter a int 5 dig SID. Example: 12345" << endl;
				return EXIT_FAILURE;
			}
            int sid = stoi(strID); // safely convert sid argument to int knowing it's a number
            /*
            TODO:
                - ** [YES] is it deleting a random record? ** fixed=true
                - [x] implement error checking
                - [x] make sure if updating with 2 seperate strings, that it adds both of them. for example the name must be 2 strings
                - [x] if phone is empty, add a new phone number
                - [x] show updated record
                - [x] if only sid is given, print out the record
                - [ ] test if its all working
             */#
            bool found = false;
            for (Record& r : db) { // for each record in db vector

                if (r.SID == sid) { // if the record's SID is equal to the sid argument
                    found = true;

                    if (argc == 5) { // if no other arguments after sid value, print record
                        printRecord(r);
                        return EXIT_SUCCESS;
                    }

                    // TODO: [x] fix error checking when 1 argyment after sid is given

                    int findName = findArg(argc, argv, "-n");
                    if (findName) {

                        if (argc <= findName + 2) { // if arg count is less than or equal to the position of the -n argument + 1, then there is no name is given
                            cout << "Please enter both first and second names." << endl;
                            return EXIT_FAILURE;
                        }

                        string firstName = argv[findName + 1];
                        string secondName = argv[findName + 2];

                        if (firstName.empty() || firstName[0] == '-') {
                            cout << "Please enter a name" << endl;
                            return EXIT_FAILURE;
                        }
                        if (secondName.empty() || (secondName[0] == '-')) {
                            cout << "Please enter a second name" << endl;
                            return EXIT_FAILURE;
                        }
                        
                        r.name = firstName + " " + secondName;
                        

                    }
                    int findPhone = findArg(argc, argv, "-p");
                    if (findPhone) {

                        if (findPhone + 1 >= argc) {  // Checks if there is no additional argument after "-p"
                            cout << "Please enter a phone number after the -p switch" << endl;
                            return EXIT_FAILURE;
                        }

                        string newPhone = argv[findPhone + 1];
                        if (newPhone.empty() || newPhone[0] == '-' || newPhone.length() != 11 || newPhone.find_first_not_of("0123456789") != string::npos) {
                            cout << "Please enter a 11-dig phone number" << endl;
                            return EXIT_FAILURE;
                        }
                        r.phone = newPhone;
                        // printRecord(r);

                    }
                    printRecord(r);
                }
            }
            // if no matching SID is found, cout error message
            if (!found) {
                cout << "No record with SID=" << strID << " was found" << endl;
            }

            // write to file
            ofstream dbFile("computing.txt");
            if (!dbFile.is_open()) { // if file cannot be opened
                cout << "Could not open database file for write" << endl;
            }
            else {
                for (Record& r : db) { // for each record in db vector, write everything back to file
                    dbFile << "#RECORD" << endl;
                    dbFile << "#SID" << endl;
                    dbFile << r.SID << endl;
                    dbFile << "#NAME" << endl;
                    dbFile << r.name << endl;
                    dbFile << "#ENROLLMENTS" << endl;
                    for (string& e : r.enrollments) {
                        dbFile << e << " ";
                    }
                    dbFile << endl;
                    dbFile << "#GRADES" << endl;
                    for (float& g : r.grades) {
                        dbFile << g << " ";
                    }
                    dbFile << endl;
                    if (!(r.phone.empty())) { // if phone number is NOT empty write it to file
                        dbFile << "#PHONE" << endl;
                        dbFile << r.phone << endl;
                    }
                    dbFile << endl;
                }
                // close db
                dbFile.close();
            }
        }
    }
    catch (const char* msg)
    {
        cerr << msg << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int findArg(int argc, char* argv[], string pattern)
{
    for (int n = 1; n < argc; n++) {
        string s1(argv[n]);
        if (s1 == pattern) {
            return n;
        }
    }
    return 0;
}
