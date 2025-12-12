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
    vector<int> meta;   //stores session, subject, start, end, room
};

struct p_subjMeta{
    int details[4];  //subject, start, end, room
    vector<string> sessionsCode;   // how many sessions
};

struct P_timetable
{
    vector<string> sessions;
    vector<string> rooms;
    vector<string> subjects;
    vector<p_subjMeta> day[7];
};



class ProfessorTable
{
private:
    P_timetable t;
    const string folderPath = "professors";
    static vector<string> professors_timeTable_name;

public:
    P_Temp_Header temp;

    ProfessorTable();

    void deseriallize(const string& fileName);

    bool handle_conflict(int day, pair<int, int> dur);

    bool insert(const string &professor, const string &subject, const pair<int, int> &dur, int day, string room);
};


