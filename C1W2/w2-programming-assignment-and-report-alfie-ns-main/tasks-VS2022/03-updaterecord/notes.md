

 # Update an EXISTING user in an existing database file
 - [ ] user can pass the following parameters to this application: updaterecord -db <database file> -sid <id> [-name <name>] [-phone <phone-number>] [-modulecode <code> -grade <grade>]
 - [ ] updaterecord with no parameters creates a database
 - [ ] -db and -name are required to update a record
 - [ ] if file doesn'nt exist, create it as an empty database
 - [ ] if -sid given, must be integer <id>, must match a record in the database file
 - [ ] Open files should always be closed before the program exits
 - [ ] must pass -sid to use any of these opitonal parameters
 
 - [ ] optional: -name tag <name> composed as 2<= words, separated by spaces, replaces existing name
 - [ ] optional: number can be added OR updated by specifying -phone tag followed by new phone number
 - [ ] optional: individual student grade can be added OR updated using -modulecode and -grade parameters together

 - note that the format of all data items should be consistent with those specified in the previous tasks, same error checking should also apply

  **********************
  *** REQUIRED FUNCTIONS ***
  **********************

  get all of these working
 
  - [x] Create an empty database file computing.txt
   updaterecord -db computing.txt
 
  - [x] update the phone number of a record with a student ID 12345
   updaterecord -db computing.txt -sid 12345 -phone 00-12-34567
 
  - [x] update the name and phone number of a record with a student ID 12345
   updaterecord -db computing.txt -sid 12345 -phone 00-12-34567 -name Jo Kingly Blunte
 
  - [ ] update (or add) the COMP1001 grade for student with id 12345
   updaterecord -db computing.txt -sid 12345 -modulecode COMP1001 -grade 78.4
 
  - [ ] Enrol the student in COMP1001 for student with id 12345
   updaterecord -db computing.txt -sid 12345 -modulecode COMP1001
 
  **********************
  *** ERROR CHECK ***
  **********************
 
   Errors should be communicated with the user
 
  - [ ] Missing the database tag
    updaterecord -sid 12345
   
  - [ ] No tag should be repeated (e.g. you cannot specify -sid twice) 
 
  - [ ] ID does not exist (assuming there is no record with ID 99999)
    updaterecord -db computing.txt -sid 99999 -modulecode COMP1001 -grade 78.4
 
  - [ ] Missing module code
    updaterecord -db computing.txt -sid 12345 -grade 78.4
 
  - [ ] Spaces in the phone number
    updaterecord -db computing.txt -sid 12345 -phone 00 12 34567 -name Jo Kingly Blunte
 
  - [ ] Name is missing
    updaterecord -db computing.txt -sid 12345 -phone 00-12-34567 -name
 
  - [ ] Name must have at least 2 words
    updaterecord -db computing.txt -sid 12345 -phone 00-12-34567 -name Sam
 
 * *************
 * *** NOTES ***
 * *************


 * o changing an existing object. The object in this context is a student record.
 * o You cannot update a student record that does not exist
 * o You cannot add a student record with update command
 * o You can update OR add data within an existing record
 * o For example, if you update a student phone number and no number is currently held on record,
     then in effect you are adding a phone number.

 * o Code should be consistently indented and commented
 * o Marks are awarded for reuse of code from Tasks A and/or B (where possible)
 * o Marks are awarded for using Object Orientated Programming to make your code more modular and reusable
*/


OLD 03-updaterecord CODE:

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
int findArg(int argc, char* argv[], string pattern);



int main(int argc, char* argv[])
{
    // argv is an array of strings, where argv[0] is the path to the program, argv[1] is the first parameter, ...
    // argc is the number of strings in the array argv
    // These are passed to the application as command line arguments
    // Return value should be EXIT_FAILURE if the application exited with any form of error, or EXIT_SUCCESS otherwise

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
        dataBaseName = argv[p + 1];
        cout << "Data base: " << dataBaseName << "\n";
    }
    else {
        cout << "Please provide a database with -db <filename>\n";
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


    //*********
    //Main loop
    //*********
    try
    {

        if (p) {

            while (getline(ip, nextLine)) // AN: read every line in the file
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
            ip.close();

        }
         //end try


        // IF WE MADE IT THIS FAR, THE DATABASE FILE WAS SUCCESSFULLY READ!


        //**************************************************************
        //Find the record with the given student ID
        //**************************************************************
        p = findArg(argc, argv, "-sid");
        if (p) {
            //Are there more parameters to follow?
            if (p == (argc - 1)) {
                cerr << "Please provide a student ID after -sid " << endl;
                return EXIT_FAILURE;
            }

            //Did they provide a SID?
            string strID = argv[p + 1];

            //Try to convert to a number
            int sid;
            try {
                //Extract student ID as integer - can throw an exception if not an integer
                sid = stoi(strID);

                //Search for record with this ID
                bool found = false;
                for (Record& r : db) {
                    if (r.SID == sid) {
                        found = true;
                        //found the record so can update.

                        //Check if phone update needed
                        int phoneUpdate = findArg(argc, argv, "-phone"); // fetch position of -phone in argv
                        if (phoneUpdate > 0) { // if phoneUpdate is more than 0, then -phone was provided in command line

                            if (argv[phoneUpdate + 1] == 0) { // if there is no phone number after -phone
                                cout << "Please provide a phone number after -phone " << endl;
                                return EXIT_FAILURE;
                            }

                            string newPhoneNumber = argv[phoneUpdate + 1]; // new phone number is the next argument after -phone
                            r.phone = newPhoneNumber; // update records phone number to new phone number
                        }

                        //Check if name update needed
                        int nameUpdate = findArg(argc, argv, "-name"); // fetch position of -name in argv
                        if (nameUpdate > 0) {

                            if (argv[nameUpdate + 1] == 0) { // if there is no name after -name
                                cout << "Please provide a name after -name " << endl;
                                return EXIT_FAILURE;
                            }

                            string newName = argv[nameUpdate + 1]; // new name is the next argument after -name
                            r.name = newName; // update records name to new name

                        }

                    }
                }

                //Start overwriting file
                ofstream overwriteFile(dataBaseName); // open file in write mode
                if (overwriteFile.is_open()) { // if file is open
                    for (Record& r2 : db) { // for every record in db, create a reference to each interation of the for loop

                        overwriteFile << endl;
                        overwriteFile << "#RECORD" << endl;
                        overwriteFile << " #SID" << endl;
                        overwriteFile << "     " << r2.SID << endl;
                        overwriteFile << " #NAME" << endl; // TODO: check 2 words
                        overwriteFile << "     " << r2.name << endl;
                        overwriteFile << " #ENROLLMENTS" << endl;
                        overwriteFile << "     ";
                        for (const auto& course : r2.enrollments) {
                            overwriteFile << course << " ";
                        }
                        overwriteFile << endl;
                        overwriteFile << " #GRADES" << endl;
                        overwriteFile << "     ";
                        for (const auto& grade : r2.grades) {
                            overwriteFile << grade << " ";
                        }
                        overwriteFile << endl;

                        cout << "File has been updated with new record, use -showAll to see!" << endl;

                        // Check for file write errors
                        if (overwriteFile.fail()) {
                            cerr << "Failed to write the record to the file." << endl;
                        }

                        overwriteFile.close(); // Always close the file when done

                    }

                }

            }
            catch (exception e) {
                cout << "Please provide a student ID as an integer" << endl;
                return EXIT_FAILURE;
            } //End try

            if (findArg(argc, argv, "-showAll")) {
                for (Record& r : db) {
                    printRecord(r);
                    cout << endl;
                }
            }

        }

    //In the event a match was not found, tell the user

    }
    catch (exception e) {
		cout << "Error reading data" << endl;
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	} //end try

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








//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <sstream>
//#include <regex>
//#include <map>
//#include <string>
//#include <limits>
//#include "testdb.h"
//#include "studentrecord.h"
//using namespace std;
//
//
//int findArg(int argc, char* argv[], string pattern)
//{
//    for (int n = 1; n < argc; n++) {
//        string s1(argv[n]);
//        if (s1 == pattern) {
//            return n;
//        }
//    }
//    return 0;
//}
//
//// Check if the SID is unique
//bool isUniqueSID(const string& sid, const string& dataBaseName) {
//    fstream myFile(dataBaseName, ios::in);
//    string line;
//
//    if (myFile.is_open()) {
//        while (getline(myFile, line)) {
//            // Assuming the SID line format is " #SID\n     <SID>"
//            if (line.find(" #SID") != string::npos) {
//                getline(myFile, line); // Get the actual SID line
//                if (line.find(sid) != string::npos) {
//                    myFile.close();
//                    return false; // SID is not unique
//                }
//            }
//        }
//        myFile.close();
//    }
//    else {
//        cerr << "Failed to open the file." << endl;
//    }
//    return true; // SID is unique
//}
//
//
//
////Build data structure with all data contained within it
//vector<Record> db;
//
////Locals used for navigating the database file
//string nextLine;
//string nextStr;
//int recordNumber = -1;
//Record nextRecord;
//stringstream moduleCodes;
//stringstream moduleGrades;
//string moduleCode = "";
//string moduleGrade = "";
//
////Locals used for the "state machine"
//
////This is just an integer type, where START=0, NEXTTAG=1, RECORD=2 etc.....
//enum state_t { START, NEXTTAG, RECORD, SID, NAME, ENROLLMENTS, GRADES, PHONE };
//state_t state = START;
//
////"Look up table" - maps a string to an integer. e.g. nextState["#SID"] returns 1
//map<string, state_t> nextState = {
//    {"#RECORD", RECORD},
//    {"#SID", SID},
//    {"#NAME", NAME},
//    {"#ENROLLMENTS", ENROLLMENTS},
//    {"#GRADES", GRADES},
//    {"#PHONE", PHONE}
//};
//
//
//// main program
//
//int main(int argc, char* argv[])
//{
//
//    //Scan command line for -db switch
//    string dataBaseName; // pre-initialize dataBaseName
//    fstream ip(dataBaseName);// fstream to write to file
//    int p = findArg(argc, argv, "-db");
//
//
//
//
//    if (p) { // if -db is found,
//        dataBaseName = argv[p + 1]; // set dataBaseName to the next argument after -db
//        cout << "Database: " << dataBaseName << "\n";
//        cout << "-------------------------" << endl;
//    }
//    else { // if -db is not found, cout error message
//        cout << "Please provide a database with -db <filename>\n";
//        return EXIT_FAILURE;
//    }
//
//    if (argc == 1) { // if no arguments are passed, create new database file called computing.txt
//        cout << "querydb (c)2023" << endl;
//        createTestDB("computing.txt"); // create test database
//        return EXIT_SUCCESS;
//    }
//
//    ip.open(dataBaseName); // open file
//
//    if (!ip.is_open()) { // if cannot open file, cout error message
//        cout << "Cannot open file " << dataBaseName << "\n";
//        return EXIT_FAILURE; // return EXIT_FAILURE specifying error
//    }
//
//    if (ip.is_open()) { // if file is opened
//
//        //*********
//        //Main loop
//        //*********
//        try
//        {
//            //Read the next line (loop exits on end of file)
//            while (getline(ip, nextLine))
//            {
//                // Remove leading spaces
//                // Replace "start of line (^) followed by any number of trailing spaces (' +')" with with nothing ""
//                string nextStr = regex_replace(nextLine, regex("^ +"), "");
//
//                //Skip blank lines
//                if (nextStr.empty()) continue;
//
//                //Enter "state machine" - study this carefully - it's a really useful "pattern"
//                switch (state)
//                {
//                case START:
//                    //We begin here - the first non-blank line MUST start with "#RECORD"
//                    if (nextStr != "#RECORD") {
//                        //The first list MUST simply read #RECORD
//                        throw("Expected #RECORD as first tag");
//                    }
//                    //Next time around the loop, use the RECORD state
//                    state = RECORD;
//                    recordNumber = 0;
//                    break;
//                case RECORD: //Everytime a #RECORD is found, we enter this state on the next line
//                    //Except for the first occasion, save the record we have just finished reading
//                    if (recordNumber > 0) {
//                        //For each new #RECORD tag, store the previous
//                        db.push_back(nextRecord);
//                        //Reset the nextRecord to defaults
//                        nextRecord = { 0 };
//                    }
//                    //Increment the record number
//                    recordNumber++;
//                    //Fall through into SEEK (note the break is missing) - #RECORD is always followed by a tag
//                case NEXTTAG:
//                    //nextString should contain a tag at this point - will throw an exception if not
//                    state = nextState[nextStr];
//                    break;
//                case SID:
//                    //nextStr should contain a string representation of an integer. If not, an exception will be thrown
//                    nextRecord.SID = stoi(nextStr);
//                    //Now look for the next tag
//                    state = NEXTTAG;
//                    break;
//                case NAME:
//                    //nextStr should contain a name
//                    nextRecord.name = nextStr;
//                    state = NEXTTAG;
//                    break;
//                case ENROLLMENTS:
//                    //nextString should be a list of module codes separated by spaces
//                    moduleCodes = stringstream(nextStr);
//                    //Extract each separate module code
//                    while (moduleCodes.eof() == false) {
//                        moduleCodes >> moduleCode;
//                        //If the prevous line did not succeed, we've probably read a space on the end
//                        if (moduleCodes.fail()) {
//                            break;
//                        }
//                        //Add the module string to the `enrollments` vector
//                        nextRecord.enrollments.push_back(moduleCode);
//                    }
//                    state = NEXTTAG;
//                    break;
//                case GRADES:
//                    //nextString should contain a list of grades, separated by spaces
//                    moduleGrades = stringstream(nextStr);
//                    //Extract each separate module grade as a float
//                    while (moduleGrades.eof() == false) {
//                        moduleGrades >> moduleGrade;
//                        if (moduleGrades.fail()) {
//                            break;
//                        }
//                        //Convert to float, and save in the grades vector
//                        nextRecord.grades.push_back(stof(moduleGrade)); //This can throw an exception
//                    }
//                    state = NEXTTAG;
//                    break;
//
//                case PHONE:
//                    //Next String should be a phone number (no spaces)
//                    nextRecord.phone = nextStr;
//                    state = NEXTTAG;
//                    break;
//                } //End Switch
//
//            } //End while
//
//            //The loop above may exit before pushing the last record into db
//            if (nextRecord.SID > 0) {
//                db.push_back(nextRecord);
//            }
//
//            //Close the file - we are done reading it. Everything is now in the db vector
//            ip.close();
//
//        }
//        catch (exception e) {
//            //Many things could go wrong, so we catch them here, tell the user and close the file (tidy up)
//            ip.close();
//            cout << "Error reading data" << endl;
//            cerr << e.what() << endl;
//            return EXIT_FAILURE;
//        } //end try
//
//
//        // IF WE MADE IT THIS FAR, THE DATABASE FILE WAS SUCCESSFULLY READ!
//
//        if (findArg(argc, argv, "-showAll")) {
//            for (Record& r : db) {
//                printRecord(r);
//                cout << endl;
//            }
//        }
//        //**************************************************************
//
//        if (findArg(argc, argv, "-updaterecord")) {
//            if (findArg(argc, argv, "-sid")) {
//                p = findArg(argc, argv, "-sid");
//                if (p == (argc - 1)) {
//                    cerr << "Please provide a student ID after -sid" << endl;
//                    return EXIT_FAILURE;
//                }
//
//                string strID = argv[p + 1];
//
//                int sid;
//                try {
//                    sid = stoi(strID);
//
//                    if (to_string(sid) != strID) { // stoi removes non int characters, so if the string is not the same as the int, then non int characters were removed
//                        cout << "Please provide student ID as an integer" << endl;
//                    }
//
//                    bool found = false; // init found as false first
//                    for (Record& r : db) {
//                        if (r.SID == sid)
//                        {
//                            found = true;
//
//                            //Check if phone update needed
//                            int phoneUpdate = findArg(argc, argv, "-phone"); // fetch position of -phone in argv
//                            if (phoneUpdate > 0) { // if phoneUpdate is more than 0, then -phone was provided in command line
//
//                                if (argv[phoneUpdate + 1] == 0) { // if there is no phone number after -phone
//                                    cout << "Please provide a phone number after -phone " << endl;
//                                    return EXIT_FAILURE;
//                                }
//
//                                fstream ip(dataBaseName);// fstream to write to file
//                                ip.open(dataBaseName);
//                                if (ip.is_open()) {
//                                    r.phone = argv[phoneUpdate + 1];
//                                    cout << "Phone number updated" << endl;
//                                }
//                                ip.close();
//
//
//
//    
//                            }
//
//                            //Check if name update needed
//                            int nameUpdate = findArg(argc, argv, "-name");
//                            if (nameUpdate > 0) {
//
//                                if (argv[nameUpdate + 1] == 0) {
//                                    cout << "Please provide a name after -name " << endl;
//                                    return EXIT_FAILURE;
//                                }
//
//                                r.name = argv[nameUpdate + 1];
//
//                            }
//                        }
//                    }
//
//
//                }
//                catch (exception e) {
//                    cout << "Please provide a student ID as an integer" << endl;
//                    return EXIT_FAILURE;
//                } //End try
//
//            }
//        }
//        return EXIT_SUCCESS;
//    }
//}
//
