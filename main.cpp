#include <iostream>
#include <ctime>
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
    int startSwimTime = 0;        // start swim time, millisecond
    int finalSwimTime = 0;        // final swim time, millisecond
    double resultSwimTime = 0.0;  // result swim time, second
    thread* track = nullptr;      // thread object
} swimmer[N];  // swimmer data array
mutex swimmer_access;

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
    int startTime = (swimmer[ID].startSwimTime = clock());  // timing the start of the thread
    string name = swimmer[ID].name;
    double speed = swimmer[ID].speed;
    swimmer_access.unlock();
    int t;
    bool fFinish;

    do{
        // pause
        this_thread::sleep_for(chrono::milliseconds(64));

        // calculating the distance
        int clock_time = clock();
        double distance = speed * (clock_time-startTime)/1000.0;
        fFinish = distance >= distanceMax;

        // information output
        if( fFinish || (clock_time >= t) ) {
            //
            t = clock_time + 1000;

            fOut_access.lock();
            cout << "Swimmer " << name << " swam " << distance << " meters. "
                 << (fFinish ? "(FINISH!!!)" : "") << endl;
            fOut_access.unlock();
        }
    }while( !fFinish );

    // timing the completion of a thread
    swimmer_access.lock();
    swimmer[ID].finalSwimTime = clock();
    swimmer_access.unlock();
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

    // calculation of the final swim time
    for( auto &s: swimmer )
        s.resultSwimTime = (s.finalSwimTime - s.startSwimTime) / 1000.0;

    // sorting the result
    for(int i=0; i<(N-1); i++)
        for(int j=(i+1); j<N; j++)
            if( swimmer[i].resultSwimTime > swimmer[j].resultSwimTime )
                swap(swimmer[i], swimmer[j]);

    // conclusion of the final result
    cout << "\nFinal result, seconds:\n";
    for(const auto &s: swimmer)
        cout << s.name << " - " << s.resultSwimTime << endl;
    cout
        << "-------------------------------------------------------\n"
        << "End of table.\n";

    return 0;
}
