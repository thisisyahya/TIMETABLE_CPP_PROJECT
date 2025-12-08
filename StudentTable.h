#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>

using namespace std;

struct Temp_Header
{
    int headerValues[9];
    vector<string> subjects;
    vector<string> profs;
    bool hasSubjects = false;
    bool hasProfs = false;
    bool hasRooms = false;
    int linesCount = 0;
    int metaCount = 0;
    int day = 0;
    int subMeta[4];
};

struct Timetable
{
    vector<string> professors;
    vector<string> rooms;
    vector<string> subjects;
    vector<array<int, 5>> day[7];
};



class StudentTable
{
private:
    Timetable t;
    string file;
    string session;
    
public:
    Temp_Header temp;

    StudentTable(const string &file);

    void deseriallizeTable(ifstream &in);

    bool handle_conflict(int day, pair<int, int> dur);

    bool insert(const string &professor, const string &subject, const pair<int, int> &dur, int day, string room);
};
