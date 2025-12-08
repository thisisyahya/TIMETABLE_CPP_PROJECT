#include "StudentTable.h"
#include<stdexcept>
StudentTable::StudentTable(const string &file)
{
    this->file = file;

    ifstream in(file);

    if(!in.is_open()){
        throw runtime_error("file not found");
       //cout<<"creating new file"<<endl;
    }

     deseriallizeTable(in);
    // else{
    //    ofstream out(file);

    //    try{
    //    if(out.is_open()){
    //     cout<<"File created successfully";
    //    }
    //   cout << "constructor says : reconstructing timetable in memory ..." << endl;
    // deseriallizeTable(in);
    // }
    // catch(const exception& e){
    //     cout<<"Error occoured in deseriilized function --> : "<<e.what()<<endl;
    // }
    
    // }

}

void StudentTable::deseriallizeTable(ifstream &in)
{

    string line;
    while (getline(in, line))
    {
        temp.linesCount++;
     //   cout << "Line read: \"" << line << "\"" << endl;

        if (line == "")
            continue;

        if (!temp.hasSubjects)
        {
          //  cout << "making subjects array!!!" << endl;
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
          //  cout << "making professors array!!!" << endl;
            if (line == "\\0-0\\")
            {
                temp.hasProfs = true;
                continue;
            }
            t.professors.push_back(line);
            continue;
        }

        if(!temp.hasRooms){
           // cout << "making Rooms Array!!!" << endl;
            if (line == "\\0-0\\")
            {
                temp.hasRooms = true;
                continue;
            }
            t.rooms.push_back(line);
            continue;
        }

        if (temp.metaCount < 5)
        {
            if (line == "\\0-0\\")
            {
                temp.day++;
                continue;
            }
            temp.subMeta[temp.metaCount] = stoi(line);
          cout << "making subjects mappings!!! : " << temp.metaCount << " : " << temp.subMeta[temp.metaCount] << endl << endl;
            temp.metaCount++;
            continue;
        }
        else if (temp.metaCount == 5)
        {
            t.day[temp.day].push_back({temp.subMeta[0], temp.subMeta[1], temp.subMeta[2], temp.subMeta[3], temp.subMeta[4]});
            temp.metaCount = 0;

            if (line == "\\0-0\\")
            {
                temp.day++;
                continue;
            }
            temp.subMeta[temp.metaCount] = stoi(line);
          //  cout << "making subjects mappings!!! : " << temp.metaCount << " : " << temp.subMeta[temp.metaCount] << endl << endl;
            temp.metaCount++;
            continue;
        }
        else
        {
           // cout << "ends here!!!" << endl;
            continue;
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
