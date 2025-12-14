#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>

using namespace std;

struct P_Temp_Header
{

    vector<string> subjects;
    vector<string> rooms;
    vector<string> sessions;

    bool hasSessions = false;
    bool hasSubjects = false;
    bool hasRooms = false;
    
    int day = 0;


    array<int, 4> details;
    vector<string> sessionCodes;

    int counter = 0;
};

struct Mapping{
    array<int, 4> details;  //subject, start, end, room
    vector<string> sessionsCode;  // how many sessions
};

struct P_timetable
{
    vector<string> sessions;
    vector<string> rooms;
    vector<string> subjects;
    vector<Mapping> day[7];
};



class ProfessorTable
{
private:
    P_timetable t;
    const string folderPath = "professors";
    string professorName;
    static vector<string> professors_timeTable_name;

public:
    P_Temp_Header temp;

    ProfessorTable(const string professorFileName);

    void deseriallize(const string& fileName);

    bool handle_conflict(int day, pair<int, int> dur);

    bool insert(const string &professor, const string &subject, const pair<int, int> &dur, int day, string room);
};


