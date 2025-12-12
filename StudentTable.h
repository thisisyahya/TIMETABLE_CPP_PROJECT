#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>

using namespace std;

struct Temp_Header
{
    vector<string> subjects;
    vector<string> profs;
    vector<string> rooms;

    bool hasSubjects = false;
    bool hasProfs = false;
    bool hasRooms = false;

    int day = 0;
    vector<int> meta;
};

struct Timetable
{
    vector<string> professors;
    vector<string> rooms;
    vector<string> subjects;
    vector<char> groups;
    vector<array<int, 5>> day[7];
};


class StudentTable
{
private:
    Timetable t;
    string file;
    //string session;
    
public:
    Temp_Header temp;

    StudentTable(const string &file);

    void deseriallizeTable(ifstream &in);

    bool handle_conflict(int day, pair<int, int> dur);

    bool insert(const string &professor, const string &subject, const pair<int, int> &dur, int day, string room);
};
