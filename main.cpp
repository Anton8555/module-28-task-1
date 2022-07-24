#include <iostream>
#include <thread>
#include <mutex>
using namespace std;


// data

// swimming distance in meters
const double distanceMax = 100.0;

// swimmer data
const int N = 6;  // number of swimmers
struct Swimmer {
    string name;                 // name
    double speed = 0.0;          // speed
    double distance = 0.0;       // distance traveled
    double finalSwimTime = 0.0;  // final swim time
    thread* track = nullptr;     // thread object
} swimmer[N];  // swimmer data array
mutex swimmer_access;


/*!
 * @brief thread subroutine.
 * @param ID - element number in the data array.
 */
void track(int ID) {
    // data
    double time = 0.0;      // time counter
    double distance = 0.0;  // distance traveled

    while( distance < 100.0 ) {
        // pause
        this_thread::sleep_for(chrono::seconds(1));
        time += 1;

        // calculating the distance and storing the result in an array
        swimmer_access.lock();
        swimmer[ID].distance = (distance = swimmer[ID].speed * time);
        swimmer_access.unlock();
    }

    // calculation of the final time of the swim
    swimmer_access.lock();
    swimmer[ID].finalSwimTime = distanceMax / swimmer[ID].speed;
    swimmer_access.unlock();
}



int main() {
    // data input
    for(auto &s: swimmer) {
        cout << "Enter name:";
        cin >> s.name;
        cout << "Enter speed:";
        cin >> s.speed;

        s.distance = 0.0;
    }

    // start
    cout << "\nStart all!\n";
    for(int i=0; i<N; i++) {
        swimmer[i].track = new thread(track, i);
    }

    // output of the current distance covered by swimmers
    bool finishAll = false;  // no one finished as they had just started.
    int time = 0;
    while( !finishAll ) {
        // pause
        this_thread::sleep_for(chrono::seconds(1));
        cout << "\nTime: " << ++time << endl;

        // output loop
        finishAll = true;  // let's say everyone finished
        swimmer_access.lock();
        for(const auto &s: swimmer) {
            // data
            bool finish;
            // distance analysis
            finishAll &= (finish = (s.distance >= distanceMax));
            // information output
            cout << "Swimmer " << s.name << " swam " << s.distance << " meters. " <<(finish?"(FINISH!!!)":"") << endl;
        }
        swimmer_access.unlock();
    }

    // finish
    for( auto &s: swimmer )
        if( s.track->joinable() )
            s.track->join();

    // program termination output
    cout << "\nFinish all!\n";

    swimmer_access.lock();

    // sorting the result
    for(int i=0; i<(N-1); i++)
        for(int j=(i+1); j<N; j++)
            if( swimmer[i].finalSwimTime > swimmer[j].finalSwimTime )
                swap(swimmer[i], swimmer[j]);

    // conclusion of the final result
    cout << "\nFinal result:\n";
    for(const auto &s: swimmer)
        cout << s.name << " - " << s.finalSwimTime << endl;
    cout
        << "-------------------------------------------------------\n"
        << "End of table.\n";

    swimmer_access.unlock();
    return 0;
}
