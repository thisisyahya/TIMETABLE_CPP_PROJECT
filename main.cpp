#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <filesystem>
#include <memory> //for using unique_ptr

using namespace std;
namespace fs = filesystem;

const string professors_folder_path = "professors";
const string students_folder_path = "students";
const string rooms_folder_path = "rooms";

struct Teacher_Slot
{
    int subjectIndex;
    int roomIndex;
    int startTime;
    int endTime;
    vector<string> groupCodes;
};

struct Teacher_Timetable
{
    vector<string> subjects;
    vector<string> sessions;
    vector<string> rooms;
    vector<Teacher_Slot> days[7];
};

struct Student_Slot
{
    int subjectIndex;
    int professorIndex;
    int roomIndex;
    int startTime;
    int endTime;
    char group;
};

struct Student_Timetable
{
    vector<string> professors;
    vector<string> rooms;
    vector<string> subjects;
    vector<Student_Slot> days[7];
};

struct Room_Slot
{
    int startTime;
    int endTime;
    int profIndex;
    int subjectIndex;
    vector<string> groupCodes;
};

struct Rooms_Timetable
{
    vector<string> profs;
    vector<string> subjects;
    vector<string> sessions;
    vector<Room_Slot> days[7];
};

// Structure representing a timetable insertion
struct Insert
{
    string subject;
    // vector<pair<string, string>> session;
    string session;
    char group;
    string room;
    int day;
    pair<int, int> dur;
};

// Structure representing a timetable deletion
struct Delete
{
    int day;        // Day of the session to delete
    int start_time; // Start time of the session to delete
};

class Timetable
{ // base abstract class

public:
    virtual void printTimetable() = 0;
    virtual void printLowLevelTimetable() = 0;
   // virtual bool handle_conflict(int day, pair<int, int> dur, string session, const char *group = nullptr, string *teachername = nullptr) = 0;
    virtual void load_binary_file_into_memory(const std::string &fileLocation) = 0;
    virtual void write_timetable_into_binary_file(const std::string &studentFileLocation) = 0;
};

class StudentTable : public Timetable
{

    Student_Timetable s;

public:
    static vector<string> sessions_timeTable_name;
    StudentTable(const Teacher_Timetable &t, const std::string &profname);
    StudentTable();
    void printTimetable() override;
    void printLowLevelTimetable() override;
    void load_binary_file_into_memory(const std::string &studentFileLocation) override;
    void write_timetable_into_binary_file(const std::string &studentFileLocation) override;

    // bool handle_conflict(int day, pair<int, int> dur, const char *group, string *professorname) override;
    bool handle_conflict(int day, pair<int, int> dur, char group);
};

StudentTable::StudentTable()
{
    cout << "\n\n==============================object initializeed\n==============================";
}

StudentTable::StudentTable(const Teacher_Timetable &t, const std::string &profname){

    // clear existing student timetable (fixed 7 days)
    for (int d = 0; d < 7; d++)
    {
        s.days[d].clear();
    }

    for (const std::string &session : t.sessions)
    {
        ofstream st;

        if (fs::exists("students/" + session + ".bin"))
        {
            load_binary_file_into_memory("students/" + session + ".bin"); // fills 's'
            std::cout << "session file " << session << " opened  !!!\n";
        }
        else
        {
            st.open("students/" + session + ".bin", ios::binary);
            if (!st.is_open())
            {
                std::cerr << "error opening session file from professor!" << std::endl;
                return;
            }
            std::cout << "session file " << session << " created  !!!\n";
            s = {};
        }

        // ---------- utility lambdas ----------

        auto find_or_insert_professor = [&]() -> int
        {
            for (int i = 0; i < s.professors.size(); i++)
            {
                if (s.professors[i] == profname)
                    return i;
            }
            s.professors.push_back(profname);
            return s.professors.size() - 1;
        };

        auto find_or_insert_subject = [&](const std::string &subjectname) -> int
        {
            for (int i = 0; i < s.subjects.size(); i++)
            {
                if (s.subjects[i] == subjectname)
                    return i;
            }
            s.subjects.push_back(subjectname);
            return s.subjects.size() - 1;
        };

        auto find_or_insert_room = [&](const std::string &roomname) -> int
        {
            for (int i = 0; i < s.rooms.size(); i++)
            {
                if (s.rooms[i] == roomname)
                    return i;
            }
            s.rooms.push_back(roomname);
            return s.rooms.size() - 1;
        };

        // ---------- reconstruction logic ----------

        for (int day = 0; day < 7; day++)
        {

            for (const Teacher_Slot &slot : t.days[day])
            {

                for (const std::string &groupCode : slot.groupCodes)
                {

                    // safety checks
                    if (groupCode.empty())
                        continue;

                    int sessionIndex = groupCode[0] - '0';

                    if (sessionIndex < 0 || sessionIndex >= (int)t.sessions.size())
                        continue;

                    if (t.sessions[sessionIndex] == session)
                    {

                        bool conflictResolved = handle_conflict(day, {slot.startTime, slot.endTime}, groupCode[1]);
                        if (!conflictResolved)
                        {
                            cout << "\nslot already present in student timetable ... skipping this addition!" << endl;
                        }
                        else
                        {

                            s.days[day].push_back({find_or_insert_subject(t.subjects[slot.subjectIndex]),
                                                   find_or_insert_professor(),
                                                   find_or_insert_room(t.rooms[slot.roomIndex]),
                                                   slot.startTime,
                                                   slot.endTime,
                                                   groupCode[1]});
                        }
                    }
                }
            }
        }

        write_timetable_into_binary_file("students/" + session + ".bin");
        //    printLowLevelTimetable();
        // printTimetable();

        if (st.is_open())
            st.close();
    }
}

void StudentTable::printTimetable()
{
    cout << "\n===== STUDENT TIMETABLE =====\n";

    const string daysOfWeek[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    for (int day = 0; day < 7; ++day)
    {
        cout << "\n"
             << daysOfWeek[day] << ":\n";

        if (s.days[day].empty())
        {
            cout << "  No entries\n";
            continue;
        }

        // Group slots by subject
        map<int, vector<Student_Slot>> subjectSlots;
        for (const auto &slot : s.days[day])
        {
            subjectSlots[slot.subjectIndex].push_back(slot);
        }

        for (const auto &subjPair : subjectSlots)
        {
            int subjIndex = subjPair.first;
            string subjectName = (subjIndex >= 0 && subjIndex < s.subjects.size()) ? s.subjects[subjIndex] : "Unknown Subject";
            cout << "\nSubject: " << subjectName << "\n";

            for (const auto &slot : subjPair.second)
            {
                // Professor
                string professorName = (slot.professorIndex >= 0 && slot.professorIndex < s.professors.size()) ? s.professors[slot.professorIndex] : "Unknown Professor";
                cout << "  Professor: " << professorName << "\n";

                // Room
                string roomName = (slot.roomIndex >= 0 && slot.roomIndex < s.rooms.size()) ? s.rooms[slot.roomIndex] : "Unknown Room";
                cout << "  Room: " << roomName << "\n";

                // Time in HH:MM format
                int startHour = slot.startTime / 60;
                int startMin = slot.startTime % 60;
                int endHour = slot.endTime / 60;
                int endMin = slot.endTime % 60;
                printf("  Time: %02d:%02d - %02d:%02d\n", startHour, startMin, endHour, endMin);

                // Group
                string groupStr;
                if (slot.group == 'f' || slot.group == 'F')
                    groupStr = "Full Class";
                else
                    groupStr = "Group " + string(1, slot.group);

                cout << "  Group: " << groupStr << "\n";
            }
        }
    }

    cout << "\n===== END OF TIMETABLE =====\n";
}

void StudentTable::printLowLevelTimetable()
{

    cout << "===== STUDENT LOW LEVEL TIMETABLE =====\n\n";

    // ---- META TABLES ----
    cout << "Subjects:\n";
    for (size_t i = 0; i < s.subjects.size(); i++)
    {
        cout << "  [" << i << "] " << s.subjects[i] << "\n";
    }

    cout << "\nProfessors:\n";
    for (size_t i = 0; i < s.professors.size(); i++)
    {
        cout << "  [" << i << "] " << s.professors[i] << "\n";
    }

    cout << "\nRooms:\n";
    for (size_t i = 0; i < s.rooms.size(); i++)
    {
        cout << "  [" << i << "] " << s.rooms[i] << "\n";
    }

    // ---- DAYS ----
    cout << "\nSchedule by Days:\n";

    for (int day = 0; day < 7; day++)
    {

        cout << "\nDay " << day << ":\n";

        if (s.days[day].empty())
        {
            cout << "  No entries\n";
            continue;
        }

        for (size_t i = 0; i < s.days[day].size(); i++)
        {

            const Student_Slot &slot = s.days[day][i];

            cout << "  Entry " << i + 1 << ":\n";

            cout << "    Subject Index: " << slot.subjectIndex;
            if (slot.subjectIndex >= 0 && slot.subjectIndex < (int)s.subjects.size())
                cout << " (" << s.subjects[slot.subjectIndex] << ")";
            cout << "\n";

            cout << "    Professor Index: " << slot.professorIndex;
            if (slot.professorIndex >= 0 && slot.professorIndex < (int)s.professors.size())
                cout << " (" << s.professors[slot.professorIndex] << ")";
            cout << "\n";

            cout << "    Room Index: " << slot.roomIndex;
            if (slot.roomIndex >= 0 && slot.roomIndex < (int)s.rooms.size())
                cout << " (" << s.rooms[slot.roomIndex] << ")";
            cout << "\n";

            cout << "    Start Time: " << slot.startTime << "\n";
            cout << "    End Time: " << slot.endTime << "\n";

            cout << "    Group: " << slot.group << "\n";
        }
    }

    cout << "\n===== END =====\n";
}

void StudentTable::write_timetable_into_binary_file(const std::string &studentFileLocation)
{

    std::ofstream out;

    if (studentFileLocation.empty())
    {
        std::cerr << "Error: no file path provided.\n";
        return;
    }
    else
    {
        std::cout << "file path : " << studentFileLocation << std::endl;
        out.open(studentFileLocation, std::ios::binary);
    }

    if (!out.is_open())
    {
        std::cerr << "File could not be opened for writing.\n";
        return;
    }

    // ---- Write string vectors: professors, rooms, subjects ----
    std::vector<std::string> *vecPtrs[] = {&s.professors, &s.rooms, &s.subjects};

    for (int v = 0; v < 3; v++)
    {
        size_t vecSize = vecPtrs[v]->size();
        out.write(reinterpret_cast<const char *>(&vecSize), sizeof(vecSize));
        for (size_t i = 0; i < vecPtrs[v]->size(); i++)
        {
            size_t len = (*vecPtrs[v])[i].size();
            out.write(reinterpret_cast<const char *>(&len), sizeof(len));
            out.write((*vecPtrs[v])[i].data(), len);
        }
    }

    // ---- Write Student_Slot members in correct order ----
    for (int d = 0; d < 7; d++)
    {

        size_t numStudentSlots = s.days[d].size();
        out.write(reinterpret_cast<const char *>(&numStudentSlots), sizeof(numStudentSlots));

        for (size_t i = 0; i < numStudentSlots; i++)
        {

            Student_Slot &m = s.days[d][i];

            // Correct order: subject → professor → room → start → end → group
            out.write(reinterpret_cast<const char *>(&m.subjectIndex), sizeof(m.subjectIndex));
            out.write(reinterpret_cast<const char *>(&m.professorIndex), sizeof(m.professorIndex));
            out.write(reinterpret_cast<const char *>(&m.roomIndex), sizeof(m.roomIndex));
            out.write(reinterpret_cast<const char *>(&m.startTime), sizeof(m.startTime));
            out.write(reinterpret_cast<const char *>(&m.endTime), sizeof(m.endTime));
            out.write(reinterpret_cast<const char *>(&m.group), sizeof(m.group));
        }
    }

    out.close();

    // Optional: call printTimetable() to verify
    // this->printTimetable();
}

void StudentTable::load_binary_file_into_memory(const std::string &studentFileLocation)
{

    auto &readS = this->s;
    std::ifstream in(studentFileLocation, std::ios::binary);
    if (!in.is_open())
    {
        std::cerr << "File could not be opened for reading.\n";
        return;
    }

    // ---- Read string vectors: professors, rooms, subjects ----
    std::vector<std::string> *readVecPtrs[] = {&readS.professors, &readS.rooms, &readS.subjects};

    for (int v = 0; v < 3; v++)
    {
        size_t vecSize;
        in.read(reinterpret_cast<char *>(&vecSize), sizeof(vecSize));
        readVecPtrs[v]->resize(vecSize);
        for (size_t i = 0; i < vecSize; i++)
        {
            size_t len;
            in.read(reinterpret_cast<char *>(&len), sizeof(len));
            (*readVecPtrs[v])[i].resize(len);
            in.read((*readVecPtrs[v])[i].data(), len);
        }
    }

    // ---- Read days[7] and their Student_Slots ----
    for (int d = 0; d < 7; d++)
    {
        size_t numStudentSlots;
        in.read(reinterpret_cast<char *>(&numStudentSlots), sizeof(numStudentSlots));
        readS.days[d].resize(numStudentSlots);

        for (size_t i = 0; i < numStudentSlots; i++)
        {
            Student_Slot &m = readS.days[d][i];

            // Read primitive members
            in.read(reinterpret_cast<char *>(&m.subjectIndex), sizeof(m.subjectIndex));
            in.read(reinterpret_cast<char *>(&m.professorIndex), sizeof(m.professorIndex));
            in.read(reinterpret_cast<char *>(&m.roomIndex), sizeof(m.roomIndex));
            in.read(reinterpret_cast<char *>(&m.startTime), sizeof(m.startTime));
            in.read(reinterpret_cast<char *>(&m.endTime), sizeof(m.endTime));
            in.read(reinterpret_cast<char *>(&m.group), sizeof(m.group));
        }
    }

    // Populate the class member
    s = readS;

    // Optional: print to verify
    // this->printTimetable();

    in.close();
}

bool StudentTable::handle_conflict(int day, pair<int, int> dur, char group)
{
    const auto &daySlots = this->s.days[day];

    // No slots → no conflict
    if (daySlots.empty())
        return true;

    for (const auto &slot : daySlots)
    {
        bool overlap =
            dur.first <= slot.endTime &&
            dur.second >= slot.startTime;

        if (!overlap)
            continue;

        // Full-class blocks everything
        if (slot.group == 'f')
            return false;

        // Different groups cannot overlap
        if (slot.group == group)
            return false;
    }

    return true;
}

class TeacherTable : public Timetable
{
private:
    // Stores the complete timetable data for a professor
    Teacher_Timetable t;

public:
    // Static list to keep track of all professor timetable files
    // Shared across all instances of TeacherTable

    Teacher_Timetable get_professor_timetable()
    {
        return t;
    }

    static vector<string> professors_timeTable_name;

    // Name of the professor associated with this timetable
    string professorName;

    TeacherTable(const string professorFileName, bool firstCreation);

    void load_binary_file_into_memory(const std::string &studentFileLocation) override;

    void write_timetable_into_binary_file(const string &fileName) override;

    void printTimetable() override; // overriding the base class method

    void printLowLevelTimetable() override; // for developer only debugging

    void deleteEntry(Delete entry); // Delete entry = {day, start_time}

    bool handle_conflict(int day, pair<int, int> dur, string session, char group, string room); // handle conflict must runs before any insertion operation

    void insert(Insert entry); // Insert entry = {subject, session, room, day, start_time, end_time}
};

TeacherTable::TeacherTable(const string professorFileLocation, bool firstCreation)
{

    this->professorName = professorFileLocation.substr(professorFileLocation.find_last_of("/\\") + 1);

    if (!firstCreation)
    {

        cout << "timetable exists already for : " << this->professorName << endl;
    }

    else if (firstCreation)
    {

        this->write_timetable_into_binary_file(professorFileLocation); // creating professor timetbale file wiht proper binary configuraitons ( empty file)
    }
}

void TeacherTable::printTimetable()
{
    cout << "\n===== PROFESSOR TIMETABLE =====\n";

    const string daysOfWeek[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    for (int day = 0; day < 7; ++day)
    {
        cout << "\n"
             << daysOfWeek[day] << ":\n";

        if (t.days[day].empty())
        {
            cout << "  No entries\n";
            continue;
        }

        // Group slots by subject
        map<int, vector<Teacher_Slot>> subjectTeacher_Slots;
        for (const auto &slot : t.days[day])
        {
            subjectTeacher_Slots[slot.subjectIndex].push_back(slot);
        }

        for (const auto &subjPair : subjectTeacher_Slots)
        {
            int subjIndex = subjPair.first;
            string subjectName = (subjIndex >= 0 && subjIndex < t.subjects.size()) ? t.subjects[subjIndex] : "Unknown Subject";
            cout << "\nSubject: " << subjectName << "\n";

            // For each slot of this subject
            for (const auto &slot : subjPair.second)
            {
                // Room
                string roomName = (slot.roomIndex >= 0 && slot.roomIndex < t.rooms.size()) ? t.rooms[slot.roomIndex] : "Unknown Room";
                cout << "  Room: " << roomName << "\n";

                // Time in HH:MM format
                int startHour = slot.startTime / 60;
                int startMin = slot.startTime % 60;
                int endHour = slot.endTime / 60;
                int endMin = slot.endTime % 60;
                printf("  Time: %02d:%02d - %02d:%02d\n", startHour, startMin, endHour, endMin);

                // Sessions & Groups
                cout << "  Sessions & Groups:\n";
                for (const auto &code : slot.groupCodes)
                {
                    string sessionName = "Unknown";
                    string groupStr;

                    // Extract session index
                    if (!code.empty() && isdigit(code[0]))
                    {
                        int sessionIdx = code[0] - '0'; // assuming single-digit index
                        if (sessionIdx >= 0 && sessionIdx < t.sessions.size())
                            sessionName = t.sessions[sessionIdx];
                    }

                    // Group
                    if (code.find('f') != string::npos)
                        groupStr = "Full Class";
                    else
                        groupStr = "Group " + string(1, code.back());

                    cout << "       " << sessionName << " (" << groupStr << ")\n";
                }
            }
        }
    }

    cout << "\n===== END OF TIMETABLE =====\n";
}

void TeacherTable::printLowLevelTimetable()
{
    // --- PRINT TO TERMINAL ---
    cout << "===== TIMETABLE =====\n\n";

    auto &readT = this->t;
    cout << "Subjects:\n";
    for (auto &s : readT.subjects)
        cout << "  - " << s << "\n";

    cout << "\nSessions:\n";
    for (auto &s : readT.sessions)
        cout << "  - " << s << "\n";

    cout << "\nRooms:\n";
    for (auto &s : readT.rooms)
        cout << "  - " << s << "\n";

    cout << "\nSchedule by Days:\n";
    for (int day = 0; day < 7; day++)
    {
        cout << "\nDay " << day << ":\n";
        if (readT.days[day].empty())
        {
            cout << "  No entries\n";
            continue;
        }
        for (size_t i = 0; i < readT.days[day].size(); i++)
        {
            Teacher_Slot &m = readT.days[day][i];
            cout << "  Entry " << i + 1 << ":\n";

            cout << "    Subject Index: " << m.subjectIndex << "\n";
            cout << "    Room Index: " << m.roomIndex << "\n";
            cout << "    Start Time: " << m.startTime << "\n";
            cout << "    End Time: " << m.endTime << "\n";

            cout << "    Group Codes: ";
            for (size_t j = 0; j < m.groupCodes.size(); j++)
            {
                cout << m.groupCodes[j];
                if (j + 1 < m.groupCodes.size())
                    cout << ", ";
            }
            cout << "\n";
        }
    }

    cout << "\n===== END =====\n";
}

void TeacherTable ::write_timetable_into_binary_file(const string &profFileLocation)
{
    // --- WRITE TO BINARY FILE ---
    ofstream out;
    if (profFileLocation == "")
    {
        cout << "file path : " << profFileLocation << endl;
        out.open(professors_folder_path + "/" + this->professorName, ios::binary);
    }
    else
    {
        cout << "file path : " << profFileLocation << endl;
        out.open(profFileLocation, ios::binary);
    }

    if (!out.is_open())
    {
        cerr << "File could not be opened for writing.\n";
    }

    // Write string vectors
    vector<string> *vecPtrs[] = {&t.subjects, &t.sessions, &t.rooms};
    for (int v = 0; v < 3; v++)
    {
        size_t vecSize = vecPtrs[v]->size();
        out.write(reinterpret_cast<const char *>(&vecSize), sizeof(vecSize));
        for (size_t i = 0; i < vecPtrs[v]->size(); i++)
        {
            size_t len = (*vecPtrs[v])[i].size();
            out.write(reinterpret_cast<const char *>(&len), sizeof(len));
            out.write((*vecPtrs[v])[i].data(), len);
        }
    }

    // Write Teacher_Slot members in correct order
    for (int d = 0; d < 7; d++)
    {
        size_t numTeacher_Slots = t.days[d].size();
        out.write(reinterpret_cast<const char *>(&numTeacher_Slots), sizeof(numTeacher_Slots));
        for (size_t i = 0; i < t.days[d].size(); i++)
        {
            Teacher_Slot &m = t.days[d][i];

            // Correct order: subject → room → start → end
            out.write(reinterpret_cast<const char *>(&m.subjectIndex), sizeof(m.subjectIndex));
            out.write(reinterpret_cast<const char *>(&m.roomIndex), sizeof(m.roomIndex));
            out.write(reinterpret_cast<const char *>(&m.startTime), sizeof(m.startTime));
            out.write(reinterpret_cast<const char *>(&m.endTime), sizeof(m.endTime));

            // Write groupCodes vector
            size_t numCodes = m.groupCodes.size();
            out.write(reinterpret_cast<const char *>(&numCodes), sizeof(numCodes));
            for (size_t j = 0; j < m.groupCodes.size(); j++)
            {
                size_t len = m.groupCodes[j].size();
                out.write(reinterpret_cast<const char *>(&len), sizeof(len));
                out.write(m.groupCodes[j].data(), len);
            }
        }
    }

    out.close();

    // PrintTimetable(t);
}

void TeacherTable ::load_binary_file_into_memory(const std::string &profFileLocation)
{
    // --- READ FROM BINARY FILE ---

    auto &readT = this->t;

    ifstream in(profFileLocation, ios::binary);
    if (!in.is_open())
    {
        cerr << "File could not be opened for reading.\n";
    }

    // Read string vectors
    vector<string> *readVecPtrs[] = {&readT.subjects, &readT.sessions, &readT.rooms};
    for (int v = 0; v < 3; v++)
    {
        size_t vecSize;
        in.read(reinterpret_cast<char *>(&vecSize), sizeof(vecSize));
        readVecPtrs[v]->resize(vecSize);
        for (size_t i = 0; i < vecSize; i++)
        {
            size_t len;
            in.read(reinterpret_cast<char *>(&len), sizeof(len));
            (*readVecPtrs[v])[i].resize(len);
            in.read((*readVecPtrs[v])[i].data(), len);
        }
    }

    // Read days[7] and their Teacher_Slots
    for (int d = 0; d < 7; d++)
    {
        size_t numTeacher_Slots;
        in.read(reinterpret_cast<char *>(&numTeacher_Slots), sizeof(numTeacher_Slots));
        readT.days[d].resize(numTeacher_Slots);
        for (size_t i = 0; i < numTeacher_Slots; i++)
        {
            Teacher_Slot &m = readT.days[d][i];

            // Read individual Teacher_Slot members
            in.read(reinterpret_cast<char *>(&m.subjectIndex), sizeof(m.subjectIndex));
            in.read(reinterpret_cast<char *>(&m.roomIndex), sizeof(m.roomIndex));
            in.read(reinterpret_cast<char *>(&m.startTime), sizeof(m.startTime));
            in.read(reinterpret_cast<char *>(&m.endTime), sizeof(m.endTime));

            // Read groupCodes vector
            size_t numCodes;
            in.read(reinterpret_cast<char *>(&numCodes), sizeof(numCodes));
            m.groupCodes.resize(numCodes);
            for (size_t j = 0; j < numCodes; j++)
            {
                size_t len;
                in.read(reinterpret_cast<char *>(&len), sizeof(len));
                m.groupCodes[j].resize(len);
                in.read(m.groupCodes[j].data(), len);
            }
        }
    }

    t = readT;

    // this->printTimetable(); // this -> has timetable

    in.close();
}

bool TeacherTable::handle_conflict(int day, pair<int, int> dur, string session, char group, string room)
{
    cout << "\n\n-------------------entering conflict resolution mode-----------\n\n";
    bool slotFound = false;
    auto t = this->t;

    if (t.days[day].size() == 0)
    {
        slotFound = true;
        return slotFound;
    }

    // for (auto mapping : t.days[day])
    // {
    //     // cout << mapping.subjectIndex << " " << mapping.roomIndex << " " << mapping.startTime << " " << mapping.endTime << endl;
    // }

    for (auto slot : t.days[day])
    {
        // if (
        //     (dur.first < slot.startTime && dur.second > slot.startTime && dur.second < slot.endTime) ||
        //     (dur.first < slot.startTime && dur.second > slot.endTime) ||
        //     (dur.first > slot.startTime && dur.first < slot.endTime && dur.second > slot.endTime) ||
        //     (dur.first > slot.startTime && dur.second < slot.endTime))
        if(dur.first < slot.endTime && dur.second > slot.startTime)
        {
            slotFound = false;
            break;
        }
        else
        {
            slotFound = true;
        }
    }

     unique_ptr<StudentTable> sessionObj = make_unique<StudentTable>();
     sessionObj->load_binary_file_into_memory(students_folder_path + "/" + session);

     slotFound = sessionObj->handle_conflict(day, dur, group);

    return slotFound;
}




void TeacherTable::insert(Insert entry)
{
    try
    {
        cout << "\n\n------------------------\n\n";

        // Helper lambda to find index or add if not found
        auto findOrAdd = [](vector<string> &vec, const string &value) -> int {
            for (int i = 0; i < vec.size(); i++)
                if (vec[i] == value) return i;
            vec.push_back(value);
            return vec.size() - 1;
        };

        int indexOfSubject = findOrAdd(t.subjects, entry.subject);
        int indexOfRoom    = findOrAdd(t.rooms, entry.room);
        int indexOfSession = findOrAdd(t.sessions, entry.session);

        // Check for conflict
        if (!this->handle_conflict(entry.day, entry.dur, entry.session, entry.group, entry.room))
        {
            cout << "Teacher_Slot already Reserved !!!" << endl;
            return;
        }
       
        // Add entry to timetable
        t.days[entry.day].push_back({
            indexOfSubject,
            indexOfRoom,
            entry.dur.first,
            entry.dur.second,
            {entry.session + entry.group}
        });

        // Save and print
        this->write_timetable_into_binary_file(professors_folder_path + "/" + this->professorName);
        this->printTimetable();

         cout << "Slot has been reserved !!!" << endl;

    }
    catch (exception &e)
    {
        cerr << "An error occurred: " << e.what() << endl;
    }
}




void TeacherTable::deleteEntry(Delete entry)
{
    if (entry.day < 0 || entry.day >= 7)
        return;

    int n = 0;
    for (const auto &mapping : t.days[entry.day])
    {
        if (mapping.startTime == entry.start_time)
        {
            t.days[entry.day].erase(t.days[entry.day].begin() + n);
            this->write_timetable_into_binary_file(professors_folder_path + "/" + this->professorName);
            this->printTimetable();
            return;
        }
        n++;
    }
}

class RoomsTable : public Timetable
{
private:
    Rooms_Timetable r;

public:
    static vector<string> rooms_timeTable_name;
    RoomsTable(const Teacher_Timetable &t, const string &profName);

    RoomsTable();
    void printTimetable() override;
    void printLowLevelTimetable() override;
    bool handle_conflict(int day, pair<int, int> dur, const char *group = nullptr, string *teachername = nullptr);
    void load_binary_file_into_memory(const std::string &fileLocation) override;
    void write_timetable_into_binary_file(const std::string &fileLocation) override;
};

RoomsTable::RoomsTable()
{
    cout << "\n\n==============================object initializeed\n==============================";
}

RoomsTable::RoomsTable(const Teacher_Timetable &t, const string &profName)
{



        // clear existing student timetable (fixed 7 days)
    for (int d = 0; d < 7; d++)
    {
        r.days[d].clear();
    }

    string Room;
ofstream out;

    for (string room : t.rooms)
    {
            

        if (fs::exists("rooms/" + room + ".bin"))
        {
            load_binary_file_into_memory("rooms/" + room + ".bin"); // fills 's'
            std::cout << "room file " << room << " opened  !!!\n";
        }
        else
        {
            out.open("rooms/" + room + ".bin", ios::binary);
            if (!out.is_open())
            {
                std::cerr << "error opening session file from professor!" << std::endl;
                return;
            }
            std::cout << "room file " << room << " created  !!!\n";
            r = {};
        }

        Room = room;
        // ---------- utility lambdas ----------

        auto find_or_insert_professor = [&](const string &profName) -> int
        {
            for (int i = 0; i < r.profs.size(); i++)
            {
                if (r.profs[i] == profName)
                    return i;
            }
            r.profs.push_back(profName);
            return r.profs.size() - 1;
        };

        auto find_or_insert_subject = [&](const std::string &subjectname) -> int
        {
            for (int i = 0; i < r.subjects.size(); i++)
            {
                if (r.subjects[i] == subjectname)
                    return i;
            }
            r.subjects.push_back(subjectname);
            return r.subjects.size() - 1;
        };

        vector<string> tempGroupCodes;

        auto find_or_insert_session = [&](const std::string &sessionName, const char group) -> int
        {
            for (int i = 0; i < r.sessions.size(); i++)
            {
                if (r.sessions[i] == sessionName)
                {
                    tempGroupCodes.push_back(to_string(r.sessions.size() - 1) + group);
                    return i;
                }
            }
            r.sessions.push_back(sessionName);
            tempGroupCodes.push_back(to_string(r.sessions.size() - 1) + group);
            return r.sessions.size() - 1;
        };

        
    int day = 0;
        for (vector<Teacher_Slot> day_slots : t.days)
        {

            for (Teacher_Slot slot : day_slots)
            {
                if (room == t.rooms[slot.roomIndex])
                {

                    for (string groupCode : slot.groupCodes)
                    {

                        int sesisonIndex = groupCode[0] - '0';
                        find_or_insert_session(t.sessions[sesisonIndex], groupCode[1]);
                    }

                    r.days[day].push_back({slot.startTime, slot.endTime, find_or_insert_professor(profName), find_or_insert_subject(t.subjects[slot.subjectIndex]), {tempGroupCodes}});
                }
            }
        }
        day++;
    }

write_timetable_into_binary_file(rooms_folder_path + "/" + Room + ".bin");
    printTimetable();
}

void RoomsTable ::printTimetable()
{
    cout << "\n===== ROOMS TIMETABLE =====\n";

    const string daysOfWeek[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    for (int day = 0; day < 7; ++day)
    {
        cout << "\n"
             << daysOfWeek[day] << ":\n";

        if (r.days[day].empty())
        {
            cout << "  No entries\n";
            continue;
        }

        // Group slots by subject
        map<int, vector<Room_Slot>> subjectSlots;
        for (const auto &slot : r.days[day])
        {
            subjectSlots[slot.subjectIndex].push_back(slot);
        }

        for (const auto &subjPair : subjectSlots)
        {
            int subjIndex = subjPair.first;
            string subjectName = (subjIndex >= 0 && subjIndex < r.subjects.size()) ? r.subjects[subjIndex] : "Unknown Subject";
            cout << "\nSubject: " << subjectName << "\n";

            for (const auto &slot : subjPair.second)
            {
                // Professor
                string professorName = (slot.profIndex >= 0 && slot.profIndex < r.profs.size()) ? r.profs[slot.profIndex] : "Unknown Professor";
                cout << "  Professor: " << professorName << "\n";

                // Sessions (if any)
                string sessionName = (slot.subjectIndex >= 0 && slot.subjectIndex < r.sessions.size()) ? r.sessions[slot.subjectIndex] : "Unknown Session";
                cout << "  Session: " << sessionName << "\n";

                // Time in HH:MM format
                int startHour = slot.startTime / 60;
                int startMin = slot.startTime % 60;
                int endHour = slot.endTime / 60;
                int endMin = slot.endTime % 60;
                printf("  Time: %02d:%02d - %02d:%02d\n", startHour, startMin, endHour, endMin);

                // Groups
                if (!slot.groupCodes.empty())
                {
                    cout << "  Groups: ";
                    for (size_t i = 0; i < slot.groupCodes.size(); ++i)
                    {
                        cout << slot.groupCodes[i];
                        if (i != slot.groupCodes.size() - 1)
                            cout << ", ";
                    }
                    cout << "\n";
                }
            }
        }
    }

    cout << "\n===== END OF ROOMS TIMETABLE =====\n";
}



bool RoomsTable::handle_conflict(int day, std::pair<int, int> dur, const char *group, std::string *teachername)
{
    // Implementation
    return false; // must return a bool
}




// Helper to write a string
void write_string(std::ofstream &out, const std::string &s) {
    size_t len = s.size();
    out.write(reinterpret_cast<const char *>(&len), sizeof(len));
    out.write(s.data(), len);
}

// Helper to read a string
void read_string(std::ifstream &in, std::string &s) {
    size_t len;
    in.read(reinterpret_cast<char *>(&len), sizeof(len));
    s.resize(len);
    in.read(s.data(), len);
}

// Helper to write vector of strings
void write_string_vector(std::ofstream &out, const std::vector<std::string> &vec) {
    size_t vecSize = vec.size();
    out.write(reinterpret_cast<const char *>(&vecSize), sizeof(vecSize));
    for (const auto &str : vec) write_string(out, str);
}

// Helper to read vector of strings
void read_string_vector(std::ifstream &in, std::vector<std::string> &vec) {
    size_t vecSize;
    in.read(reinterpret_cast<char *>(&vecSize), sizeof(vecSize));
    vec.resize(vecSize);
    for (auto &str : vec) read_string(in, str);
}

// ---- Simplified write function ----
void RoomsTable::write_timetable_into_binary_file(const std::string &roomFileLocation) {
    if (roomFileLocation.empty()) { std::cerr << "Error: no file path.\n"; return; }
    std::ofstream out(roomFileLocation, std::ios::binary);
    if (!out.is_open()) { std::cerr << "Cannot open file for writing.\n"; return; }

    // Write professors, subjects, sessions
    write_string_vector(out, r.profs);
    write_string_vector(out, r.subjects);
    write_string_vector(out, r.sessions);

    // Write Room_Slots day-wise
    for (auto &day : r.days) {
        size_t numSlots = day.size();
        out.write(reinterpret_cast<const char *>(&numSlots), sizeof(numSlots));
        for (auto &slot : day) {
            out.write(reinterpret_cast<const char *>(&slot.subjectIndex), sizeof(slot.subjectIndex));
            out.write(reinterpret_cast<const char *>(&slot.profIndex), sizeof(slot.profIndex));
            out.write(reinterpret_cast<const char *>(&slot.startTime), sizeof(slot.startTime));
            out.write(reinterpret_cast<const char *>(&slot.endTime), sizeof(slot.endTime));
            write_string_vector(out, slot.groupCodes);
        }
    }
}

// ---- Simplified read function ----
void RoomsTable::load_binary_file_into_memory(const std::string &roomFileLocation) {
    std::ifstream in(roomFileLocation, std::ios::binary);
    if (!in.is_open()) { std::cerr << "Cannot open file for reading.\n"; return; }

    // Read professors, subjects, sessions
    read_string_vector(in, r.profs);
    read_string_vector(in, r.subjects);
    read_string_vector(in, r.sessions);

    // Read Room_Slots day-wise
    for (auto &day : r.days) {
        size_t numSlots;
        in.read(reinterpret_cast<char *>(&numSlots), sizeof(numSlots));
        day.resize(numSlots);
        for (auto &slot : day) {
            in.read(reinterpret_cast<char *>(&slot.subjectIndex), sizeof(slot.subjectIndex));
            in.read(reinterpret_cast<char *>(&slot.profIndex), sizeof(slot.profIndex));
            in.read(reinterpret_cast<char *>(&slot.startTime), sizeof(slot.startTime));
            in.read(reinterpret_cast<char *>(&slot.endTime), sizeof(slot.endTime));
            read_string_vector(in, slot.groupCodes);
        }
    }
}


void RoomsTable::printLowLevelTimetable() {
    // Provide actual implementation, even if empty
    cout << "RoomsTable low-level timetable" << endl;
}


vector<string> TeacherTable ::professors_timeTable_name; // static variable
vector<string> StudentTable ::sessions_timeTable_name;   // static variable
vector<string> RoomsTable ::rooms_timeTable_name;        // static variable

int main()
{

    if (!fs::exists(professors_folder_path) || !fs::is_directory(professors_folder_path))
    {
        throw runtime_error("Error: Folder \"professors\" does not exist!");
    }

    for (const auto &entry : fs::directory_iterator(professors_folder_path))
    {

        if (entry.is_regular_file())
        {

            TeacherTable::professors_timeTable_name.push_back(entry.path().filename().string()); // track professors record in class's static variable
            TeacherTable *professor = new TeacherTable(professors_folder_path + "/" + entry.path().filename().string(), false);
            professor->load_binary_file_into_memory(professors_folder_path + "/" + entry.path().filename().string());

            unique_ptr<StudentTable> s = make_unique<StudentTable>(professor->get_professor_timetable(), entry.path().filename().string());
          unique_ptr<RoomsTable> r = make_unique<RoomsTable>(professor->get_professor_timetable(), entry.path().filename().string());
            // s.reoconstruct_multiple_students_timetables_form_this_professor_into_memory(professor->get_professor_timetable(), entry.path().filename().string());
        }
    }


    for (const auto &entry : fs::directory_iterator(students_folder_path))
    {

        if (entry.is_regular_file())
        {
            StudentTable ::sessions_timeTable_name.push_back(entry.path().filename().string()); // track ztudents record in class's static variable
        }
    }

    for (const auto &entry : fs::directory_iterator("rooms"))
    {

        if (entry.is_regular_file())
        {
            RoomsTable ::rooms_timeTable_name.push_back(entry.path().filename().string()); // track rooms record in class's static variable
        }
    }

    while (true)
    {

        cout << "\n\n-----PROFESSOR SELECTION MENU-----\n"
             << "1. ACCESSS EXISTING PROFESSOR\n"
             << "2. CREATE A NEW PROFESSOR\n"
             << "3. VIEW STUDENTS TIMETABLE\n"
             << "4. VIEW ROOMS TIMETABLE\n"
             << endl;

        int menu_choice;
        cin >> menu_choice;

        if (menu_choice == 3)
        {

            cout << "\nList of Sessions Available: \n"
                 << endl;
            int n = 1;
            for (const string name : StudentTable::sessions_timeTable_name)
            {
                cout << n << ". " << name << endl;
                n++;
            }

            cout << "\nSelect Session Id : " << endl;

            int id;
            cin >> id;

            unique_ptr<StudentTable> session = make_unique<StudentTable>();
            session->load_binary_file_into_memory(students_folder_path + "/" + StudentTable::sessions_timeTable_name[id - 1]);

            session->printTimetable();
        }

        // if (menu_choice == 4)
        // {
        //     cout << "\nList of Rooms Available: \n"
        //          << endl;

        //     int n = 1;
        //     for (const string &name : RoomsTable::rooms_timeTable_name)
        //     {
        //         cout << n << ". " << name << endl;
        //         n++;
        //     }

        //     cout << "\nSelect Room Id: ";
        //     int id;
        //     cin >> id;

        //     // Create a unique pointer to RoomsTable object
        //     unique_ptr<RoomsTable> roomSession = make_unique<RoomsTable>();

        //     // Load its binary file into memory
        //     roomSession->load_binary_file_into_memory(
        //         rooms_folder_path + "/" + RoomsTable::rooms_timeTable_name[id - 1]);

        //     // Print the timetable
        //     // roomSession->printTimetable();
        // }

        if (menu_choice == 2)
        {
            string prof_name;
            cout << "Enter Teacher Name (without spaces): ";
            cin >> prof_name;

            TeacherTable(professors_folder_path + "/" + prof_name + ".bin", true);
            cout << "Teacher " << prof_name << " created successfully!" << endl;

            bool professorFound = false;

            for (const string &prof : TeacherTable::professors_timeTable_name)
            {
                if (prof == prof_name + ".bin")
                {
                    professorFound = true;
                    break;
                }
            }

            if (!professorFound)
            {
                TeacherTable::professors_timeTable_name.push_back(prof_name + ".bin");
            }

            continue;
        }

        if (menu_choice == 1)
        {
            cout << "\nList of Teachers Available: \n"
                 << endl;
            int n = 1;
            for (const string name : TeacherTable::professors_timeTable_name)
            {
                cout << n << ". " << name << endl;
                n++;
            }

            cout << "\nSelect Teacher Id : " << endl;

            int id;
            cin >> id;

            unique_ptr<TeacherTable> professor = make_unique<TeacherTable>(professors_folder_path + "/" + TeacherTable::professors_timeTable_name[id - 1], false);
            professor->load_binary_file_into_memory(professors_folder_path + "/" + TeacherTable::professors_timeTable_name[id - 1]);

            cout << "Teacher Selected: " << professor->professorName << endl;

            cout << "\n\n------------------MENU------------------\n1. Insert Entry\n2. Delete Entry\n3. Print Timetable \n4. Skip (exit the professor)\n---------------------------------------\n";
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
            {

            

                auto convert_into_minutes = [](const std::string &time) -> int
                {
                    int hours = stoi(time.substr(0, 2));
                    int mins = stoi(time.substr(3, 2)); // start after the colon
                    return hours * 60 + mins;
                };

                Insert i;

                cout << "In which day you want to reserve slot : ";
                cin >> i.day;
                cout << endl;

                cout << "Enter Subject Name : ";
                cin >> i.subject;
                cout << endl;

                string sessionName;
                char group;

                label :
                cout<<"Must enter valid group (a / b)";

                cout << "Enter Session(s) Name | group : ";
                cin >> i.session >> i.group;

                 if((i.group != 'a') || i.group!='b') goto label;
                // while (true)
                // {
                //     cout << "Enter Session(s) Name | group (0 to proceed): ";
                //     cin >> sessionName >> group;
                //     cout << endl;
                //     if (sessionName == "0" || group == "0")
                //         break;

                //     i.session.push_back({sessionName, group});
                //     cout << endl;
                // }

                cout << "Enter Room Name : ";
                cin >> i.room;
                cout << endl;

                string st, nd;
                cout << "Enter Start_Time and End_Time (in 24-hr format ): ";

                cin >> st;
                cin >> nd;

                i.dur.first = convert_into_minutes(st);
                i.dur.second = convert_into_minutes(nd);

                cout << endl;

                professor->insert(i);

                break;
            }
            case 2:
            {
                Delete d;

                auto convert_into_minutes = [](const std::string &time) -> int
                {
                    int hours = stoi(time.substr(0, 2));
                    int mins = stoi(time.substr(3, 2)); // start after the colon
                    return hours * 60 + mins;
                };

                string starting_time;

                cout << "In which day you want to delete entry : ";
                cin >> d.day;
                cout << endl;

                cout << "Enter Start Time of the session to delete (in 24-hr format without colon, e.g., 1300 for 1 PM): ";
                cin >> starting_time;

                d.start_time = convert_into_minutes(starting_time);

                cout << endl;

                professor->deleteEntry(d);

                break;
            }

            case 3:
            {

                professor->printTimetable();

                break;
            }
            case 4:
            {
                cout << "skipping ... " << endl;
                // delete professor;  //deleting professor from dynamic memory
                break;
            }

            case 0:
            {
                professor->printLowLevelTimetable(); // low level debugging
                break;
            }
            default:
            {
                cout << "Invalid choice!!!";
            }
            }
        }
    }

    return 0;
}




