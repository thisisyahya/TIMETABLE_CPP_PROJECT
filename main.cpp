#include "StudentTable.h"
#include "ProfessorTable.h"
#include<map>

int main()
{
    //StudentTable mct_A_24("students/24-mct-A.txt");
    map< string, StudentTable> student_tables;

   ProfessorTable p_all = ProfessorTable(); // Also works

    student_tables.emplace("24-mct-A", StudentTable("24-mct-A.txt"));

    auto it = student_tables.find("24-mct-A");

   //it->second.insert("Prof. A", "Math", {1100, 1200}, 0, "2nd-lecture-hall");
    
   /// mct_A_24.insert("Prof. A", "Math", {22100, 12200}, 0, "2nd-lecture-hall");

    return 0;
}


// #include<iostream>
// #include<map>
// #include<vector>
// #include<string>
// #include<fstream>

// using namespace std;


// struct Temp_Header
// {
//     int headerValues[9];
//     vector<string> subjects;
//     vector<string> profs;
//     bool hasSubjects = false;
//     bool hasProfs = false;
//     bool hasRooms = false;
//     int linesCount = 0;
//     int metaCount = 0;
//     int day = 0;
//     int subMeta[4];
// };


// struct Timetable_p{

//     vector<string> subjects;
//     vector<string> sessions;
//     vector<string> rooms;
//     vector<array<int, 5>> day[7];   /* 1. start_time
//                                        2. end_time
//                                        3. subject
//                                        4. session
//                                        5. room
//                                        */
// };


// struct Timetable_s{
//     vector<string*> subjects; 
//     vector<string*> profs;
//     vector<array<int, 5>> day[7];
// };

// struct ProfInfo{
//     string name;
//     int age;
//     map<string, int> exp;
// };

// class ProfessorTable
// {
// private:
//     Timetable_p t;
//     string file;
//     ProfInfo p;
    
// public:
//     Temp_Header temp;

//     ProfessorTable(const string &file, string name);

//     void deseriallizeTable(ifstream &in);

//     bool handle_conflict(int day, pair<int, int> dur);

//     bool insert(const string &professor, const string &subject, const pair<int, int> &dur, int day, string room);
// };



// int main(){



//     return 0;
// }