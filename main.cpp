#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
using namespace std;


// data

// swimming distance in meters
const double distanceMax = 100.0;

// swimmer data
const int N = 6;  // number of swimmers
struct Swimmer {
    string name;                  // name
    double speed = 0.0;           // speed, meters/second
    thread* track = nullptr;      // thread object
} swimmer[N];  // swimmer data array
mutex swimmer_access;

// list of arrivals at the finish line
vector<string> nameFinish;
mutex nameFinish_access;

// condition variable: message output queue to the console
bool fOut;
mutex fOut_access;




/*!
 * @brief thread subroutine.
 * @param ID - element number in the data array.
 */
void track(int ID) {
    // data
    swimmer_access.lock();
    string name = swimmer[ID].name;
    double speed = swimmer[ID].speed;
    swimmer_access.unlock();
    int t = 0;
    bool fFinish;

    do{
        // pause
        this_thread::sleep_for(chrono::seconds(1));

        // calculating the distance
        double distance = speed * ++t;
        fFinish = distance >= distanceMax;

        // information output
        fOut_access.lock();
        cout << "Swimmer " << name << " swam " << distance << " meters. "
             << (fFinish ? "(FINISH!!!)" : "") << endl;
        fOut_access.unlock();
    }while( !fFinish );

    // adding to the list of finished
    nameFinish_access.lock();
    nameFinish.push_back(name);
    nameFinish_access.unlock();
}



int main() {
    // data input
    for(auto &s: swimmer) {
        cout << "Enter name:";
        cin >> s.name;
        cout << "Enter speed:";
        cin >> s.speed;
    }

    // start
    cout << "\nStart all!\n";
    for(int ID=0; ID<N; ID++)
        swimmer[ID].track = new thread(track, ID);

    // waiting for everyone to finish
    for( auto &s: swimmer )
        s.track->join();

    // conclusion about the termination of all threads
    cout << "\nFinish all!\n";

    // conclusion of the final result
    cout << "\nFinal result, seconds:\n";
    for(const auto &nf: nameFinish)
        for(const auto &s: swimmer)
            if( nf == s.name ) {
                cout << s.name << " - " << (distanceMax / s.speed) << endl;
                break;
            }
    cout
        << "-------------------------------------------------------\n"
        << "End of table.\n";

    return 0;
}
