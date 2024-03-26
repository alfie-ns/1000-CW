#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <map>
#include <string>
#include <limits>
#include "testdb.h"
#include "studentrecord.h"
using namespace std;

// Find an argument on the command line and return the location
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

// Check if the SID is unique
bool isUniqueSID(const string& sid, const string& dataBaseName) {
    fstream myFile(dataBaseName, ios::in);
    string line;

    if (myFile.is_open()) {
        while (getline(myFile, line)) {
            if (line.find(" #SID") != string::npos) {
                getline(myFile, line); // Get the actual SID line
                if (line.find(sid) != string::npos) {
                    myFile.close();
                    return false; // SID is not unique
                }
            }
        }
        myFile.close();
    }
    else {
        cerr << "Failed to open the file." << endl;
    }
    return true; // SID is unique
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


// main program

int main(int argc, char* argv[])
{

    //Scan command line for -db switch
    string dataBaseName; // pre-initialize dataBaseName
    fstream ip(dataBaseName);// fstream to write to file
    int p = findArg(argc, argv, "-db");




    

    if (argc == 1) { // if no arguments are passed, create new database file called computing.txt
        cout << "querydb (c)2023" << endl;
        createTestDB("computing.txt"); // create test database
        return EXIT_SUCCESS;
    }

    if (p) { // if -db is found,
        dataBaseName = argv[p + 1]; // set dataBaseName to the next argument after -db
        cout << "Database: " << dataBaseName << "\n";
        cout << "-------------------------" << endl;
    }
    else { // if -db is not found, cout error message
        cout << "Please provide a database with -db <filename>\n";
        return EXIT_FAILURE;
    }

    ip.open(dataBaseName); // open file

    if (!ip.is_open()) { // if cannot open file, cout error message
        cout << "Cannot open file " << dataBaseName << "\n";
        return EXIT_FAILURE; // return EXIT_FAILURE specifying error
    }

    if (ip.is_open()) { // if file is opened

        //*********
        //Main loop
        //*********
        try
        {
            //Read the next line (loop exits on end of file)
            while (getline(ip, nextLine))
            {
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
                        nextRecord = { 0 };
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

        }
        catch (exception e) {
            //Many things could go wrong, so we catch them here, tell the user and close the file (tidy up)
            ip.close();
            cout << "Error reading data" << endl;
            cerr << e.what() << endl;
            return EXIT_FAILURE;
        } //end try


        // IF WE MADE IT THIS FAR, THE DATABASE FILE WAS SUCCESSFULLY READ!

        /* I'M SORRY FOR MY OWN LOGIC,
           I'VE NOW GOT TO MOVE ONTO 
           THE NEXT COURSEWORK
           PLEASE APPRECIATE THAT IT STILL WORKS, AND 
           HAS ERROR CHECKING 
        */ 

        // Check if the -addrecord command is present
        if (findArg(argc, argv, "-add")) {
            // Open the file in append mode to add a new record
            fstream myFile(dataBaseName, ios::app);

            if (myFile.is_open()) {
                Record newRecord;

                // Prompt for and read the Student ID
                cout << "Enter Student ID: ";
                string stringSid;
                getline(cin, stringSid); // read the SID as a string

                if (stringSid.length() != 5) { // if SID is not 5 digits long
                    cerr << "Error: Student ID must be 5 digits long." << endl;
                    return EXIT_FAILURE;
                }

                if (stringSid.find_first_not_of("0123456789") != string::npos) { // if SID contains characters other than 0-9
                	cerr << "Error: Student ID must be an integer." << endl;
                	return EXIT_FAILURE;
                }

                newRecord.SID = stoi(stringSid); // convert stringSid to integer and store in newRecord.SID
                
                // Check if SID is unique
                if (!isUniqueSID(stringSid, dataBaseName)) { // if SID is not unique
                    cerr << "Error: Student ID already exists." << endl;
                    return EXIT_FAILURE;
                }

                // Prompt for and read the Name
                cout << "Enter Name: ";
                getline(cin, newRecord.name);

                // Check if name has at least 2 words
                istringstream iss(newRecord.name);
                string word;
                int wordCount = 0;

                while (iss >> word) {
                    wordCount++;
                }

                // Check if name has at least 2 words
                if (wordCount >= 2) {
                    cout << "Valid name format.\n";
                }
                else {
                    cout << "Invalid name format. Please enter at least two words.\n";
                    return EXIT_FAILURE;
                }

                // Validate the name (not implemented in this snippet)

                // Prompt for and read the Enrollments
                cout << "Enter enrollments (type 'done' to finish): ";
                string course;
                while (getline(cin, course) && course != "done") {
                    newRecord.enrollments.push_back(course); // while 'done' is not entered, add course to enrollments vector
                }

                // Prompt for and read the Grades
                cout << "Enter grades (type -1 to finish): ";
                float grade;
                while (cin >> grade && grade != -1) {
                    newRecord.grades.push_back(grade);
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer after each grade entry
                }

                // Validate the number of grades matches the number of enrollments here (not implemented in this snippet)
                if (newRecord.enrollments.size() != newRecord.grades.size()) {
                    cerr << "The number of grades must match the number of enrollments." << endl;
                    return EXIT_FAILURE;
                }

                // Prompt for and read the Phone
                cout << "Enter Phone: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer before reading the phone
                string stringPhone;
                getline(cin, stringPhone);
                if (stringPhone.find_first_not_of("0123456789") != string::npos) { // if phone contains characters other than 0-9
                	cerr << "Error: Phone number must be an integer." << endl;
                	return EXIT_FAILURE;
                }
                if (stringPhone.length() != 11) { // if phone is not 11 digits long
                	cerr << "Error: Phone number must be 11 digits long." << endl;
                	return EXIT_FAILURE;
                }
                newRecord.phone = stringPhone;

                // Serialize and write the record to the file

                // TODO: error checking
                myFile << endl;
                myFile << "#RECORD" << endl;
                myFile << " #SID" << endl;
                myFile << "     " << newRecord.SID << endl;
                myFile << " #NAME" << endl; // TODO: check 2 words
                myFile << "     " << newRecord.name << endl;
                myFile << " #ENROLLMENTS" << endl;
                myFile << "     ";
                for (const auto& course : newRecord.enrollments) {
                    myFile << course << " ";
                }
                myFile << endl;
                myFile << " #GRADES" << endl;
                myFile << "     ";
                for (const auto& grade : newRecord.grades) {
                    myFile << grade << " ";
                }
                myFile << endl;

                cout << "File has been updated with new record, use -showAll to see!" << endl;

                // TODO:
                // - [x] check 2 words for name
                // - [ ] check no spaces for phone
                // - [ ] check mandatory and optional fields
                // - [x] fix -sid and -findAll
                // - [ ] is it actually saving record to .txt file?
                // - [ ] check if SID already being used


                // Check for file write errors
                if (myFile.fail()) {
                    cerr << "Failed to write the record to the file." << endl;
                }

                myFile.close(); // Always close the file when done
            }
        }

        if (findArg(argc, argv, "-showAll")) {
            for (Record& r : db) {
                printRecord(r);
                cout << endl;
            }
        }
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

                if (to_string(sid) != strID) { // if sid still has the exact characters as strID, then it is an integer, stoi removes string characters in conversion
                    cout << "Please provide a student ID as an integer" << endl;
                }

                for (Record& r : db) {
                    if (strID == to_string(r.SID)) {
						cout << "Record SID is already in use" << endl; // if SID is already in use, cout error message
						return EXIT_FAILURE;
					}
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

                        else if (!nameOnly && !gradesOnly && !phoneOnly) { // if no flags are found (nameOnly, gradesOnly, phoneOnly
                            printRecord(r); // if no flags are found, print the whole record
                        }
                        break;
                    }
                }
                // if no matching SID is found, cout error message
                if (!found) {
                    cout << "No record with SID=" << strID << " was found" << endl;
                }

            }
            catch (exception e) {
                cout << "Please provide a student ID as an integer" << endl;
                return EXIT_FAILURE;
            } //End try

        } //End if

        //**************************************************************
        //You could continue here :)
        //**************************************************************






        return EXIT_SUCCESS;

    }
}
