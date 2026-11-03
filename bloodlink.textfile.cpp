// BloodLink - Blood Bank Management System
// Compile: g++ -std=c++17 bloodlink.cpp -o bloodlink
// Run:     ./bloodlink      (Windows: bloodlink.exe)

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <ctime>
#include <algorithm>
using namespace std;

const vector<string> GROUPS = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};

struct Donor {
    int id;
    string name;
    int age;
    string bg;
    string phone;
};

struct Donation {            // one donation = one blood bag (1 unit)
    int id;
    int donorId;
    string bg;
    string date;
    string status;           // Available / Issued
    int requestId;           // 0 if not issued
};

struct Request {
    int id;
    string patient;
    string hospital;
    string bg;
    int units;
    string urgency;          // Normal / Urgent / Critical
    string status;           // Pending / Fulfilled
    vector<int> donationIds; // bags issued
};

vector<Donor> donors;
vector<Donation> donations;
vector<Request> requests;

// ---------- helpers ----------
string today() {
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
    return buf;
}

string clean(string s) {
    replace(s.begin(), s.end(), '|', ' ');
    return s;
}

string readLine(const string &prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return clean(s);
}

int readInt(const string &prompt) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        cout << "  Invalid number, try again.\n";
    }
}

string readBloodGroup() {
    while (true) {
        string s = readLine("Blood group (A+, A-, B+, B-, AB+, AB-, O+, O-): ");
        transform(s.begin(), s.end(), s.begin(), ::toupper);
        if (find(GROUPS.begin(), GROUPS.end(), s) != GROUPS.end()) return s;
        cout << "  Invalid blood group.\n";
    }
}

// Groups that can be given to a patient of group bg (exact match first)
vector<string> compatibleDonors(const string &bg) {
    map<string, vector<string>> m = {
        {"A+",  {"A+", "A-", "O+", "O-"}},
        {"A-",  {"A-", "O-"}},
        {"B+",  {"B+", "B-", "O+", "O-"}},
        {"B-",  {"B-", "O-"}},
        {"AB+", {"AB+", "AB-", "A+", "A-", "B+", "B-", "O+", "O-"}},
        {"AB-", {"AB-", "A-", "B-", "O-"}},
        {"O+",  {"O+", "O-"}},
        {"O-",  {"O-"}}};
    return m[bg];
}

int nextId(int size) { return size + 1; }

Donor *findDonor(int id) {
    for (auto &d : donors) if (d.id == id) return &d;
    return nullptr;
}
Donation *findDonation(int id) {
    for (auto &d : donations) if (d.id == id) return &d;
    return nullptr;
}
Request *findRequest(int id) {
    for (auto &r : requests) if (r.id == id) return &r;
    return nullptr;
}

int availableCount(const string &bg) {
    int c = 0;
    for (auto &d : donations)
        if (d.bg == bg && d.status == "Available") c++;
    return c;
}

// ---------- file storage ----------
vector<string> split(const string &s, char delim) {
    vector<string> out;
    string item;
    stringstream ss(s);
    while (getline(ss, item, delim)) out.push_back(item);
    return out;
}

void saveAll() {
    ofstream f1("donors.txt");
    for (auto &d : donors)
        f1 << d.id << '|' << d.name << '|' << d.age << '|' << d.bg << '|' << d.phone << '\n';

    ofstream f2("donations.txt");
    for (auto &d : donations)
        f2 << d.id << '|' << d.donorId << '|' << d.bg << '|' << d.date << '|' << d.status << '|' << d.requestId << '\n';

    ofstream f3("requests.txt");
    for (auto &r : requests) {
        string ids;
        for (size_t i = 0; i < r.donationIds.size(); i++) {
            if (i) ids += ",";
            ids += to_string(r.donationIds[i]);
        }
        f3 << r.id << '|' << r.patient << '|' << r.hospital << '|' << r.bg << '|' << r.units << '|'
           << r.urgency << '|' << r.status << '|' << ids << '\n';
    }
}

void loadAll() {
    string line;
    ifstream f1("donors.txt");
    while (getline(f1, line)) {
        auto p = split(line, '|');
        if (p.size() < 5) continue;
        donors.push_back({stoi(p[0]), p[1], stoi(p[2]), p[3], p[4]});
    }
    ifstream f2("donations.txt");
    while (getline(f2, line)) {
        auto p = split(line, '|');
        if (p.size() < 6) continue;
        donations.push_back({stoi(p[0]), stoi(p[1]), p[2], p[3], p[4], stoi(p[5])});
    }
    ifstream f3("requests.txt");
    while (getline(f3, line)) {
        auto p = split(line, '|');
        if (p.size() < 7) continue;
        Request r{stoi(p[0]), p[1], p[2], p[3], stoi(p[4]), p[5], p[6], {}};
        if (p.size() > 7 && !p[7].empty())
            for (auto &x : split(p[7], ',')) r.donationIds.push_back(stoi(x));
        requests.push_back(r);
    }
}

// ---------- features ----------
void addDonor() {
    cout << "\n--- Register Donor ---\n";
    Donor d;
    d.id = nextId(donors.size());
    d.name = readLine("Name: ");
    d.age = readInt("Age: ");
    if (d.age < 18 || d.age > 65) {
        cout << "Donor must be between 18 and 65 years old.\n";
        return;
    }
    d.bg = readBloodGroup();
    d.phone = readLine("Phone: ");
    donors.push_back(d);
    saveAll();
    cout << "Donor registered with ID " << d.id << "\n";
}

void viewDonors() {
    cout << "\n--- Donors ---\n";
    if (donors.empty()) { cout << "No donors yet.\n"; return; }
    cout << left << setw(5) << "ID" << setw(22) << "Name" << setw(6) << "Age"
         << setw(8) << "Group" << "Phone\n";
    cout << string(55, '-') << "\n";
    for (auto &d : donors)
        cout << left << setw(5) << d.id << setw(22) << d.name << setw(6) << d.age
             << setw(8) << d.bg << d.phone << "\n";
}

void recordDonation() {
    cout << "\n--- Record Donation ---\n";
    int did = readInt("Donor ID: ");
    Donor *d = findDonor(did);
    if (!d) { cout << "Donor not found.\n"; return; }
    Donation n{nextId(donations.size()), d->id, d->bg, today(), "Available", 0};
    donations.push_back(n);
    saveAll();
    cout << "Bag #" << n.id << " (" << n.bg << ") added to inventory from " << d->name << ".\n";
}

void viewInventory() {
    cout << "\n--- Blood Inventory (units available) ---\n";
    cout << left << setw(8) << "Group" << "Units\n";
    cout << string(20, '-') << "\n";
    for (auto &g : GROUPS) {
        int c = availableCount(g);
        cout << left << setw(8) << g << c << (c <= 2 ? "   <-- LOW STOCK" : "") << "\n";
    }
}

// Try to issue blood for a request. Returns true if fulfilled.
bool tryFulfill(Request &r) {
    vector<string> order = compatibleDonors(r.bg);
    int total = 0;
    for (auto &g : order) total += availableCount(g);
    if (total < r.units) return false;

    int need = r.units;
    for (auto &g : order) {
        for (auto &d : donations) {
            if (need == 0) break;
            if (d.bg == g && d.status == "Available") {
                d.status = "Issued";
                d.requestId = r.id;
                r.donationIds.push_back(d.id);
                need--;
            }
        }
        if (need == 0) break;
    }
    r.status = "Fulfilled";
    return true;
}

void newRequest() {
    cout << "\n--- Emergency / Blood Request ---\n";
    Request r;
    r.id = nextId(requests.size());
    r.patient = readLine("Patient name: ");
    r.hospital = readLine("Hospital: ");
    r.bg = readBloodGroup();
    r.units = readInt("Units needed: ");
    if (r.units <= 0) { cout << "Units must be positive.\n"; return; }
    int u;
    do {
        u = readInt("Urgency (1=Normal, 2=Urgent, 3=Critical): ");
    } while (u < 1 || u > 3);
    r.urgency = (u == 1) ? "Normal" : (u == 2) ? "Urgent" : "Critical";
    r.status = "Pending";

    if (tryFulfill(r)) {
        cout << "Request #" << r.id << " FULFILLED. Bags issued: ";
        for (int id : r.donationIds) {
            Donation *d = findDonation(id);
            cout << "#" << id << "(" << d->bg << ") ";
        }
        cout << "\n";
    } else {
        cout << "Not enough compatible stock. Request #" << r.id << " saved as PENDING.\n";
        cout << "Tip: ask donors of compatible groups to donate.\n";
    }
    requests.push_back(r);
    saveAll();
}

void viewRequests() {
    cout << "\n--- Requests ---\n";
    if (requests.empty()) { cout << "No requests yet.\n"; return; }
    cout << left << setw(5) << "ID" << setw(18) << "Patient" << setw(16) << "Hospital"
         << setw(7) << "Group" << setw(7) << "Units" << setw(10) << "Urgency" << "Status\n";
    cout << string(75, '-') << "\n";
    for (auto &r : requests)
        cout << left << setw(5) << r.id << setw(18) << r.patient << setw(16) << r.hospital
             << setw(7) << r.bg << setw(7) << r.units << setw(10) << r.urgency << r.status << "\n";
}

void retryPending() {
    cout << "\n--- Retry Pending Requests ---\n";
    // serve Critical first, then Urgent, then Normal
    vector<string> pri = {"Critical", "Urgent", "Normal"};
    int done = 0;
    for (auto &p : pri)
        for (auto &r : requests)
            if (r.status == "Pending" && r.urgency == p && tryFulfill(r)) {
                cout << "Request #" << r.id << " (" << r.patient << ") fulfilled.\n";
                done++;
            }
    if (!done) cout << "No pending request could be fulfilled.\n";
    saveAll();
}

void traceBag() {
    cout << "\n--- Traceability ---\n";
    int id = readInt("Blood bag ID: ");
    Donation *d = findDonation(id);
    if (!d) { cout << "Bag not found.\n"; return; }
    Donor *dn = findDonor(d->donorId);
    cout << "Bag #" << d->id << " | Group: " << d->bg << " | Collected: " << d->date << "\n";
    cout << "Donor: " << (dn ? dn->name : "Unknown") << " (ID " << d->donorId << ", "
         << (dn ? dn->phone : "-") << ")\n";
    cout << "Status: " << d->status << "\n";
    if (d->status == "Issued") {
        Request *r = findRequest(d->requestId);
        if (r)
            cout << "Issued to: " << r->patient << " at " << r->hospital
                 << " (Request #" << r->id << ")\n";
    }
}

void findDonorsByGroup() {
    cout << "\n--- Find Donors by Blood Group ---\n";
    string bg = readBloodGroup();
    bool found = false;
    for (auto &d : donors)
        if (d.bg == bg) {
            cout << "ID " << d.id << " | " << d.name << " | " << d.phone << "\n";
            found = true;
        }
    if (!found) cout << "No donors with " << bg << " registered.\n";
}

void menu() {
    cout << "\n==============================\n";
    cout << "   BLOODLINK - BLOOD BANK\n";
    cout << "==============================\n";
    cout << "1. Register donor\n";
    cout << "2. View donors\n";
    cout << "3. Record donation (add bag)\n";
    cout << "4. View inventory\n";
    cout << "5. New blood request\n";
    cout << "6. View requests\n";
    cout << "7. Retry pending requests\n";
    cout << "8. Trace blood bag\n";
    cout << "9. Find donors by blood group\n";
    cout << "0. Exit\n";
}

int main() {
    loadAll();
    while (true) {
        menu();
        int ch = readInt("Choose: ");
        switch (ch) {
            case 1: addDonor(); break;
            case 2: viewDonors(); break;
            case 3: recordDonation(); break;
            case 4: viewInventory(); break;
            case 5: newRequest(); break;
            case 6: viewRequests(); break;
            case 7: retryPending(); break;
            case 8: traceBag(); break;
            case 9: findDonorsByGroup(); break;
            case 0:
                saveAll();
                cout << "Goodbye!\n";
                return 0;
            default: cout << "Invalid choice.\n";
        }
    }
}
