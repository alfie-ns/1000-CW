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

//See bottom of main
int findArg(int argc, char *argv[], string pattern);


/*
 *
 * The user can pass the following parameters to this application:
 *
 * -db <database file>          Specifies the path to the database file (required)
 * -showAll                     Writes all records to the terminal
 * -sid <student ID> [-n|-g|-p] Writes the record with a specific student ID (integer).
 *                              By default, this displays the complete record
 *                              Add one of the following to reduce the information
 *                                 -n       Just display the name
 *                                 -g       Just display the mode codes and grades
 *                                 -p       Just display the phone number
 *
 * ****************
 * *** EXAMPLES ***
 * ****************
 * querydb                                  Creates an example database computing.txt (done for you)
 * querydb -db computing.txt -showAll       Displays all records in the database computing.txt (done for you)
 * querydb -db computing.txt -sid 12345     Displays the complete record for student with ID 12345 (done for you)
 *
 * ********* The above three examples are already implemented, but not fully tested *********
 *
 * For the -sid option, you can further narrow down the information displayed (not yet implemented)
 *
 * ****************
 * *** EXAMPLES ***
 * ****************
 * 
 * querydb -db computing.txt -sid 12345 -n      Displays only the name for student with ID 12345, or an error if not found
 * querydb -db computing.txt -sid 12345 -g      Displays only the module codes and grades for student with ID 12345, or an error if not found
 * querydb -db computing.txt -sid 12345 -p      Displays only the phone number for student with ID 12345, or an error if not found
 * querydb -db computing.txt -sid 12345 -n -p   Display both the name and the phone number for student with ID 12345
 * 
 * *************
 * *** NOTES ***
 * *************
 * 
 * o You should test the existing application first and verify which of the above is already complete.
 * o The -n, -g and -p switches have not yet been implemented (this is your task).
 * o You need to handle error conditions, and inform the user appropriately.
 * o Errors should be communicated with the user.
 * o No tag should be repeated (e.g. you cannot specify -sid twice).
 * o Open files should always be closed before the program exits.
 * o You do not have to use Object Orientated Programming in this task, but it is recommended.
 * o Try to write your code such that is can be reused in other tasks.
 * o Code should be consistently indented and commented
*/


int main(int argc, char *argv[])
{
// argv = array of strings (char*[] or char**), representing command line arguments
// argc = integer (int), counting the number of command line arguments, including the program name
// These are passed as command line arguments to the main function of a C program
// The return value of main should be EXIT_SUCCESS for a successful execution, or EXIT_FAILURE for an error


    if (argc == 1) {
        //Welcome message
        cout << "querydb (c)2023" << endl;

        //Create some test data
        createTestDB("computing.txt");

        //Done
        return EXIT_SUCCESS;
    }

    //Scan command line for -db switch
    string dataBaseName;
    int p = findArg(argc, argv, "-db");
    if (p) {
        dataBaseName = argv[p+1];
        cout << "Data base: " << dataBaseName << "\n";
    } else {
        cout << "Please proviude a database with -db <filename>\n";
        return EXIT_FAILURE;
    }

    //Open database file
    ifstream ip(dataBaseName);
    if (!ip.is_open()) {
        cout << "Cannot open file " << dataBaseName << "\n";
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

    //Locals used for the "state machine"

    //This is just an integer type, where START=0, NEXTTAG=1, RECORD=2 etc.....
    enum state_t {START, NEXTTAG, RECORD, SID, NAME, ENROLLMENTS, GRADES, PHONE};
    state_t state = START;

    //"Look up table" - maps a string to an integer. e.g. nextState["#SID"] returns 1
    map<string,state_t> nextState = {
        {"#RECORD", RECORD},
        {"#SID", SID},
        {"#NAME", NAME},
        {"#ENROLLMENTS", ENROLLMENTS},
        {"#GRADES", GRADES},
        {"#PHONE", PHONE}
    };


    //*********
    //Main loop
    //*********
    try {
        //Read the next line (loop exits on end of file)
        while (getline(ip, nextLine)) {
            // Remove leading spaces
            // Replace "start of line (^) followed by any number of trailing spaces (' +')" with with nothing ""
            string nextStr = regex_replace(nextLine, regex("^ +"), "");

            //Skip blank lines
            if (nextStr.empty()) continue;

            //Enter "state machine" - study this carefully - it's a really useful "pattern"
            switch (state)
            {
            case START:
                //We begin here - the first non-blank line MUST start with "#RECORD"
                if (nextStr != "#RECORD") {
                    //The first list MUST simply read #RECORD
                    throw("Expected #RECORD as first tag");
                }
                //Next time around the loop, use the RECORD state
                state = RECORD;
                recordNumber = 0;
                break;
            case RECORD: //Everytime a #RECORD is found, we enter this state on the next line
                //Except for the first occasion, save the record we have just finished reading
                if (recordNumber > 0) {
                    //For each new #RECORD tag, store the previous
                    db.push_back(nextRecord);
                    //Reset the nextRecord to defaults
                    nextRecord = {0};
                }
                //Increment the record number
                recordNumber++;
                //Fall through into SEEK (note the break is missing) - #RECORD is always followed by a tag
            case NEXTTAG:
                //nextString should contain a tag at this point - will throw an exception if not
                state = nextState[nextStr];
                break;
            case SID:
                //nextStr should contain a string representation of an integer. If not, an exception will be thrown
                nextRecord.SID = stoi(nextStr);
                //Now look for the next tag
                state = NEXTTAG;
                break;
            case NAME:
                //nextStr should contain a name
                nextRecord.name = nextStr;
                state = NEXTTAG;
                break;
            case ENROLLMENTS:
                //nextString should be a list of module codes separated by spaces
                moduleCodes = stringstream(nextStr);
                //Extract each separate module code
                while (moduleCodes.eof() == false) {
                    moduleCodes >> moduleCode;
                    //If the prevous line did not succeed, we've probably read a space on the end
                    if (moduleCodes.fail()) {
                        break;
                    }
                    //Add the module string to the `enrollments` vector
                    nextRecord.enrollments.push_back(moduleCode);
                }
                state = NEXTTAG;
                break;
            case GRADES:
                //nextString should contain a list of grades, separated by spaces
                moduleGrades = stringstream(nextStr);
                //Extract each separate module grade as a float
                while (moduleGrades.eof() == false) {
                    moduleGrades >> moduleGrade;
                    if (moduleGrades.fail()) {
                        break;
                    }
                    //Convert to float, and save in the grades vector
                    nextRecord.grades.push_back(stof(moduleGrade)); //This can throw an exception
                }
                state = NEXTTAG;
                break;

            case PHONE:
                //Next String should be a phone number (no spaces)
                nextRecord.phone = nextStr;
                state = NEXTTAG;
                break;
            } //End Switch

        } //End while

        //The loop above may exit before pushing the last record into db
        if (nextRecord.SID > 0) {
            db.push_back(nextRecord);
        }

        //Close the file - we are done reading it. Everything is now in the db vector
        ip.close();

    }  catch (exception e) {
        //Many things could go wrong, so we catch them here, tell the user and close the file (tidy up)
        ip.close();
        cout << "Error reading data" << endl;
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    } //end try


    // IF WE MADE IT THIS FAR, THE DATABASE FILE WAS SUCCESSFULLY READ!

    //*******************************
    //Option to display data ALL DATA
    //*******************************
    if (findArg(argc, argv, "-showAll")) {
        for (Record& r : db) {
            printRecord(r);
            cout << endl;
        }
    }

    //**************************************************************
    //Option to display data from one record with a given student ID
    //**************************************************************
    p = findArg(argc, argv, "-sid");
    if (p) {
        //Are there more parameters to follow?
        if (p == (argc-1)) {
            cerr << "Please provide a student ID after -sid " << endl;
            return EXIT_FAILURE;
        }

        //Did they provide a SID?
        string strID = argv[p+1];

        //Try to convert to a number
        int sid;
        try {
            //Extract student ID as integer - can throw an exception if not an integer
            sid = stoi(strID);

            if (to_string(sid) != strID) { // if sid still has the exact characters as strID, then it is an integer, stoi removes string characters in conversion
				cout << "Please provide a student ID as an integer" << endl;
			} 

            //Search for record with this ID
            bool found = false;
            for (Record& r : db) {
                if (r.SID == sid) {

                    found = true;
                    bool nameOnly = findArg(argc, argv, "-n") > 0;
                    bool gradesOnly = findArg(argc, argv, "-g") > 0;
                    bool phoneOnly = findArg(argc, argv, "-p") > 0;


                    if (nameOnly) {
                        cout << r.name << endl; // if -n is found, print name only
                    }
                    // if nameOnly = false, move past

                    if (gradesOnly) {
                        for (int i = 0; i < r.enrollments.size(); i++) {
                            cout << r.enrollments[i] << " " << r.grades[i] << endl; // if -g is found, print module codes and grades
                        } // cout enrollments with respective grades
                    }
                    // if gradesOnly = false, move past

                    if (phoneOnly) {
                        if (r.phone == "") {
                            cout << "this student doesn't have a phone number" << endl; // if no phone number is found
                        }
                        else {
                            cout << r.phone << endl; // if -p is found, print phone number only
                        }
					}

                    else if(!nameOnly && !gradesOnly && !phoneOnly) { // if no flags are found (nameOnly, gradesOnly, phoneOnly
						printRecord(r); // if no flags are found, print the whole record
					}
                    break;
                }
            }
            // if no matching SID is found, cout error message
            if (!found) {
                cout << "No record with SID=" << strID << " was found" << endl;
            }

        } catch (exception e) {
            cout << "Please provide a student ID as an integer" << endl;
            return EXIT_FAILURE;
        } //End try

    } //End if

    //**************************************************************
    //You could continue here :)
    //**************************************************************






    return EXIT_SUCCESS;
}

// Find an argument on the command line and return the location
int findArg(int argc, char *argv[], string pattern)
{
    for (int n=1; n<argc; n++) {
        string s1(argv[n]);
        if (s1 == pattern) {
            return n;
        }
    }
    return 0;
}
