#include "task3_functions.h"
#include <fstream>
#include <sstream>
#include <string>
#include <utility>  // For std::pair

using namespace std;

string findCorners(string filePath) {
    string result;
    ifstream file(filePath);
    if (!file.is_open()) {
        return "{\"Error\": \"File could not be opened\"}";
    }

    int row = 0, col = 0;
    bool topLeftFound = false;
    pair<int, int> topLeft(-1, -1), bottomRight(-1, -1);

    string line;
    while (getline(file, line)) {
        col = 0;
        istringstream iss(line);
        char ch;
        while (iss >> ch) {
            if (ch == '*' && !topLeftFound) {
                topLeft = { row, col };
                topLeftFound = true;
            }
            else if (ch != '*' && topLeftFound) {
                if (bottomRight.first < row) bottomRight.first = row;
                if (bottomRight.second < col - 1) bottomRight.second = col - 1;
                // Reset topLeftFound only if you move to a new row or the line ends
            }
            col++;
        }
        // Check for the last '*' of the rectangle on this line
        if (topLeftFound && bottomRight.second == -1) {
            bottomRight = { row, col - 1 };
        }
        row++;
    }
    file.close();

    if (topLeft.first != -1 && bottomRight.first != -1) {
        result = "{\n  \"topLeft\": [" + to_string(topLeft.first) + ", " + to_string(topLeft.second) + "],\n";
        result += "  \"bottomRight\": [" + to_string(bottomRight.first) + ", " + to_string(bottomRight.second) + "]\n}";
    }
    else {
        result = "{\"Error\": \"No rectangle found\"}";  // retult = error message
    }

    return result;
}























//string findCorners(string filePath) {
//	string result;
//	ifstream file(filePath);
//	int row = 0, col = 0;
//	bool topLeftFound = false;
//	pair<int, int> topLeft(-1, -1), bottomRight(-1, -1);
//
//	string line;
//	while (getline(file, line)) {
//		col = 0;
//		istringstream iss(line);
//		char ch;
//		while (iss >> ch) {
//			if (ch == '*' && !topLeftFound) {
//				topLeft = { row, col };
//				topLeftFound = true;
//			}
//			else if (ch != '*' && topLeftFound) {
//				if (bottomRight.first < row) bottomRight.first = row;
//				if (bottomRight.second < col - 1) bottomRight.second = col - 1;
//				topLeftFound = false;  // Reset for next rectangle, if any
//			}
//			col++;
//		}
//		if (topLeftFound && bottomRight.first < row) {
//			bottomRight = { row, col - 1 };
//			topLeftFound = false;
//		}
//		row++;
//	}
//
//	if (topLeft.first != -1 && bottomRight.first != -1) {
//		result = "{\n  \"topLeft\": [" + to_string(topLeft.first) + ", " + to_string(topLeft.second) + "],\n";
//		result += "  \"bottomRight\": [" + to_string(bottomRight.first) + ", " + to_string(bottomRight.second) + "]\n}";
//	}
//	else {
//		result = "Failed to locate rectangle";  // Or any error message
//	}
//
//	return result;
//}


//string findCorners(std::string filePath) {
//    string result;
//    ifstream file(filePath);
//    int row = 0, col = 0;
//    bool topLeftFound = false;
//    pair<int, int> topLeft(-1, -1), bottomRight(-1, -1);
//
//    string line;
//    while (getline(file, line)) {
//        col = 0;
//        istringstream iss(line);
//        char ch;
//        while (iss >> ch) {
//            if (ch == '*' && !topLeftFound) {
//                topLeft = { row, col };
//                topLeftFound = true;
//            }
//            else if (ch != '*' && topLeftFound) {
//                if (bottomRight.first < row) bottomRight.first = row;
//                if (bottomRight.second < col - 1) bottomRight.second = col - 1;
//                topLeftFound = false;  // Reset for next rectangle, if any
//            }
//            col++;
//        }
//        if (topLeftFound && bottomRight.first < row) {
//            bottomRight = { row, col - 1 };
//            topLeftFound = false;
//        }
//        row++;
//    }
//
//    if (topLeft.first != -1 && bottomRight.first != -1) {
//        result = "{\n  \"topLeft\": [" + to_string(topLeft.first) + ", " + to_string(topLeft.second) + "],\n";
//        result += "  \"bottomRight\": [" + to_string(bottomRight.first) + ", " + to_string(bottomRight.second) + "]\n}";
//    }
//    else {
//        result = "Failed to locate rectangle";  // Or any error message
//    }
//
//    return result;
//}
