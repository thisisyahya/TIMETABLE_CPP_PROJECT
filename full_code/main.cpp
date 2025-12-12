#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <map>

using namespace std;

/* ===========================
   HEADER CONTENT (INLINED)
=========================== */

struct Temp_Header
{

    vector<string> subjects;
    vector<string> profs;
    vector<string> rooms;
    bool hasSubjects = false;
    bool hasProfs = false;
    bool hasRooms = false;
    // int linesCount = 0;
    // int metaCount = 0;
    // int day = 0;
    // int subMeta[4];
    vector<int> meta;
    int day=0;
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

    bool insert(const string &professor, const string &subject,
                const pair<int, int> &dur, int day, string room);
};


/* ===========================
   IMPLEMENTATION CONTENT
=========================== */

StudentTable::StudentTable(const string &file)
{
    this->file = file;

    ifstream in(file);

    if(!in.is_open()){
        throw runtime_error("file not found");
    }

    deseriallizeTable(in);
}

void StudentTable::deseriallizeTable(ifstream &in)
{
    string line;
    while (getline(in, line))
    {
        // temp.linesCount++;

        if (line == "")
            continue;

        if (!temp.hasSubjects)
        {
            if (line == "\\0-0\\")
            {
                temp.hasSubjects = true;
                continue;
            }
            t.subjects.push_back(line);
            continue;
        }

        if (!temp.hasProfs)
        {
            if (line == "\\0-0\\")
            {
                temp.hasProfs = true;
                continue;
            }
            t.professors.push_back(line);
            continue;
        }

        if(!temp.hasRooms){
            if (line == "\\0-0\\")
            {
                temp.hasRooms = true;
                continue;
            }
            t.rooms.push_back(line);
            continue;
        }

       

       
        if(temp.day<7 && line=="\\0-0\\"){

           for(int i = 0; i<temp.meta.size()/5; i++){
          //  cout<<"slots in day is "<<temp.meta.size()/5<<endl;
              t.day[temp.day].push_back({
    temp.meta[i*5],
    temp.meta[i*5 + 1],
    temp.meta[i*5 + 2],
    temp.meta[i*5 + 3],
    temp.meta[i*5 + 4]
});

           }
       temp.meta.clear();
       temp.day++;
        }else{
            int num = stoi(line);
             temp.meta.push_back(num);
            // cout<<"pushing : "<<line<<" in mapping"<<endl;
        }


    }

    cout<<"\n---------------Subjects------------\n";
    for(const string& sub : t.subjects){
        cout<<sub<<endl;
    }

    cout<<"\n\n---------------Professors------------\n";
    for(const string& prof : t.professors){
        cout<<prof<<endl;
    }

    cout<<"\n\n---------------rooms------------\n";
    for(const string& room : t.rooms){
        cout<<room<<endl;
    }

    cout<<"\n\n--------------------------\n";
}

bool StudentTable::handle_conflict(int day, pair<int, int> dur)
{
    cout << "\n\n-------------------entering conflict resolution mode-----------\n\n";
    bool slotFound = false;

    if(t.day[day].size() == 0){
        slotFound = true;
        return slotFound;
    }

    for (auto subjectMeta : t.day[day])
    {
        cout << subjectMeta[0] << " " << subjectMeta[1] << " " << subjectMeta[2] << " " << subjectMeta[3] << endl;
    }

    for (auto subjectMeta : t.day[day])
    {
        if (
            (dur.first <= subjectMeta[2] && dur.second >= subjectMeta[2] && dur.second <= subjectMeta[3]) ||
            (dur.first <= subjectMeta[2] && dur.second >= subjectMeta[3]) ||
            (dur.first >= subjectMeta[2] && dur.first <= subjectMeta[3] && dur.second >= subjectMeta[3]) ||
            (dur.first >= subjectMeta[2] && dur.second <= subjectMeta[3])
        )
        {
            slotFound = false;
            break;
        }
        else
        {
            slotFound = true;
        }
    }

    return slotFound;
}

bool StudentTable::insert(const string &professor, const string &subject,
                          const pair<int, int> &dur, int day, string room)
{
    try
    {
        cout << "\n\n------------------------\n\n";
        int indexOfSubject;
        int indexOfSir;
        int indexOfRoom;

        bool subjectFound = false;
        bool profFound = false;
        bool roomFound = false;

        for (int i = 0; i < t.subjects.size(); i++)
        {
            if (t.subjects[i] == subject)
            {
                subjectFound = true;
                indexOfSubject = i;
                break;
            }
        }

        for (int i = 0; i < t.professors.size(); i++)
        {
            if (t.professors[i] == professor)
            {
                profFound = true;
                indexOfSir = i;
                break;
            }
        }

        for (int i = 0; i < t.rooms.size(); i++)
        {
            if (t.rooms[i] == room)
            {
                roomFound = true;
                indexOfRoom = i;
                break;
            }
        }

        if (!roomFound)
        {
            t.rooms.push_back(room);
            indexOfRoom= t.rooms.size()-1;
        }

        if (!subjectFound)
        {
            t.subjects.push_back(subject);
            indexOfSubject = t.subjects.size()-1;
        }

        if (!profFound)
        {
            t.professors.push_back(professor);
            indexOfSir = t.professors.size()-1;
        }

        bool conflictResolved = handle_conflict(day, dur);

        if (!conflictResolved)
        {
            cout << "Slot already Reserved !!!" << endl;
            return false;
        }
        else
        {
            cout << "slot is free !!!" << endl;
        }

        t.day[day].push_back({indexOfSubject, indexOfSir, dur.first, dur.second, indexOfRoom});

        ofstream out(file, ios::trunc);

        if (!out.is_open())
        {
            cerr << "File not exist" << endl;
            return false;
        }

        for (string s : t.subjects) out << s << endl;
        out << "\\0-0\\" << endl;
        for (string p : t.professors) out << p << endl;
        out << "\\0-0\\" << endl;
        for (string r : t.rooms) out << r << endl;
        out << "\\0-0\\" << endl;
        for (const auto &dayVec : t.day)
        {
            for (const auto &subjectInfo : dayVec)
            {
                for (int val : subjectInfo)
                    out << val << endl;
            }
            out << "\\0-0\\" << endl;
        }

        return true;
    }
    catch (exception &e)
    {
        cerr << "An error occurred: " << e.what() << endl;
        return false;
    }
}


/* ===========================
   MAIN (UNCHANGED)
=========================== */

int main()
{
    // StudentTable mct_A_24("students/24-mct-A.txt");

    map<string, StudentTable> s_tt;

    s_tt.emplace("24-mct-A", StudentTable("../students/24-mct-A.txt"));

    auto it = s_tt.find("24-mct-A");

    if(it != s_tt.end()){
        cout<<"Timetable found !!!\n";

      //  it->second.insert("Prof. A", "Math", {1100, 1200}, 0, "2nd-lecture-hall");
    }
    else{
        cout<<"Timetable not found";
    }

   // mct_A_24.insert("Prof. A", "Math", {1100, 1200}, 0, "2nd-lecture-hall");

    return 0;
}
