#include <iostream>
#include <stdlib.h>
#include <vector>
using namespace std;

#define MAX 10  // maximum number of drivers

// Driver class
class Driver {
public:
    string name;
    string team;
    double lapTime;
    double totalTime;
    bool wrecked;

    Driver() {
        lapTime = 0;
        totalTime = 0;
        wrecked = false;
    }
};

// Pit stop structure (for queue)
struct PitStop {
    string driver;
    double duration;
};

// Simple queue for pit stops
struct PitQueue {
    PitStop arr[50];
    int front;
    int rear;
    PitQueue()
    {
        front = 0;
        rear = -1;
    }
    void enqueue(PitStop p)
    {
        arr[++rear] = p;
    }
    PitStop dequeue()
    {
        return arr[front++];
    }
    bool isEmpty()
    {
        return front > rear;
    }
};

// Swap two drivers
void swapDrivers(Driver &a, Driver &b) {
    Driver temp = a;
    a = b;
    b = temp;
}

// Display leaderboard
void displayLeaderboard(Driver drivers[], int n, int lap, int totalLaps) {
    system("cls"); // or clrscr() in Turbo C++
    cout << "================ F1 RACE LEADERBOARD ================\n";
    cout << "Lap " << lap << "/" << totalLaps << "\n";
    cout << "Pos\tDriver\t\tTeam\t\tLapTime\tTotalTime\tStatus\n";
    cout << "--------------------------------------------------------\n";
    for (int i = 0; i < n; i++) {
        cout << i+1 << "\t" << drivers[i].name << "\t\t"
             << drivers[i].team << "\t\t"
             << drivers[i].lapTime << "\t"
             << drivers[i].totalTime << "\t\t"
             << (drivers[i].wrecked ? "💥 OUT" : "🏎 Racing") << endl;
    }
    cout << "======================================================\n";
}

// Sort leaderboard and print overtakes
void sortLeaderboard(Driver drivers[], int n, bool pitStop=false) {
    for (int i = 0; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
            if (drivers[i].wrecked && !drivers[j].wrecked) {
                // wreck handled separately, no overtakes here
                swapDrivers(drivers[i], drivers[j]);
            }
            else if (!drivers[i].wrecked && !drivers[j].wrecked && drivers[i].totalTime > drivers[j].totalTime) {
                if (pitStop) {
                    cout << drivers[j].name << " overtakes " << drivers[i].name << " due to 🛠 Pit Stop! and moves to P" << (i+1) << "!\n";
                } else {
                    cout << drivers[j].name << " is just behind " << drivers[i].name << "... he makes his move and slingshot pass " << drivers[i].name
                         << " on track 📈 and moves to P" << (i+1) << "!\n";
                }
                swapDrivers(drivers[i], drivers[j]);
            }
        }
    }
}

// Next lap
void nextLap(Driver drivers[], int n, bool safetyCar=false) {
    for (int i = 0; i < n; i++) {
        if (!drivers[i].wrecked) {
            cout << "Enter lap time for " << drivers[i].name << ": ";
            cin >> drivers[i].lapTime;
            if (safetyCar) drivers[i].lapTime += 5;
            drivers[i].totalTime += drivers[i].lapTime;
        }
    }
    sortLeaderboard(drivers, n);
}

// Handle pit stops using queue
void handlePitStops(Driver drivers[], int n, PitQueue &pq) {
    while (!pq.isEmpty()) {
        PitStop p = pq.dequeue();
        vector<string> overtakers;
        int pitIndex = -1;

        for (int i = 0; i < n; i++) {
            if (drivers[i].name == p.driver && !drivers[i].wrecked) {
                pitIndex = i;
                cout << drivers[i].name << " enters pit stop for " << p.duration << "s 🛠";
                drivers[i].totalTime += p.duration;
            }
        }

        if (pitIndex != -1) {
            for (int j = pitIndex+1; j < n; j++) {
                if (!drivers[j].wrecked) {
                    overtakers.push_back(drivers[j].name);
                }
            }
            if (!overtakers.empty()) {
                cout << " and meanwhile ";
                for (int i = 0; i < overtakers.size(); i++) {
                    cout << overtakers[i];
                    if (i < overtakers.size()-1) cout << ", ";
                }
                cout << " overtake and gain track position!\n";
            } else {
                cout << " but holds position, no one passes.\n";
            }
        }
    }
    sortLeaderboard(drivers, n, true);
}

// Wreck a car
void wreckCar(Driver drivers[], int n, string driverName) {
    for (int i = 0; i < n; i++) {
        if (drivers[i].name == driverName && !drivers[i].wrecked) {
            drivers[i].wrecked = true;
            drivers[i].totalTime += 1000;
            cout << "🎙 Oh no! " << drivers[i].name << " has crashed out 💥!\n";
            cout << "🚨 Safety Car deployed! The incident slows down the pack.\n";
            cout << "All drivers behind " << drivers[i].name
                 << " move up a place on the leaderboard!\n";
        }
    }
    sortLeaderboard(drivers, n);
}

int main() {
    Driver drivers[MAX];
    int n, totalLaps;
    PitQueue pitQueue;

    cout << "Enter number of drivers (max " << MAX << "): ";
    cin >> n;
    if(n > MAX) n = MAX;

    for (int i = 0; i < n; i++) {
        cout << "Enter name of driver " << i+1 << ": ";
        cin >> drivers[i].name;
        cout << "Enter team of " << drivers[i].name << ": ";
        cin >> drivers[i].team;
    }

    cout << "Enter total number of laps: ";
    cin >> totalLaps;

    cout << "\n🎙 David Croft: Hello and welcome everyone to today's Formula 1 Grand Prix!\n";
    cout << "🎙 David Croft: Joining me in the commentary box, the legend himself, Murray Walker! 👏\n";
    cout << "🎙 Murray Walker: Thank you Crofty! We are here on this beautiful circuit with "
         << totalLaps << " laps ahead of us!\n";
    cout << "🎙 Croft: And here are our gladiators today:\n";
    for (int i = 0; i < n; i++) {
        cout << " - " << drivers[i].name << " for " << drivers[i].team << " 🏎\n";
    }
    cout << "\n🎙 Croft: It's Lights Out and Away We Go! 🚦🔥\n";

    int currentLap = 0;
    displayLeaderboard(drivers, n, currentLap, totalLaps);

    int choice;
    while (true) {
        cout << "\n1. Next Lap\n2. Add Pit Stop\n3. Car Wrecked\n4. Safety Car Lap\n5. Exit\nChoice: ";
        cin >> choice;

        switch(choice) {
            case 1: {
                if (currentLap < totalLaps) {
                    currentLap++;
                    nextLap(drivers, n);
                    displayLeaderboard(drivers, n, currentLap, totalLaps);
                    if (currentLap == totalLaps) {
                        cout << "\n🏁 We've got a winner! We've got a winner!! 🏁\n";
                        cout << drivers[0].name << " has done it for " << drivers[0].team << "! 🏆\n";
                        cout << "They are the new Formula 1 World Champion thanks to "
                             << drivers[0].name << "'s miraculous drive on this circuit! ✨\n";
                        cout << "The crowd is on their feet, the team is overjoyed 🎉🥳, "
                             << "and the garage erupts in celebration 🙌👏.\n";
                        cout << "These are unforgettable moments in Formula 1 history! 🏎🔥\n";
                        return 0;
                    }
                } else {
                    cout << "Race already finished!\n";
                }
                break;
            }
            case 2: {
                string driver;
                double duration;
                cout << "Enter driver for pit stop: "; cin >> driver;
                cout << "Enter pit stop duration (s): "; cin >> duration;
                PitStop p = {driver, duration};
                pitQueue.enqueue(p);
                handlePitStops(drivers, n, pitQueue);
                displayLeaderboard(drivers, n, currentLap, totalLaps);
                break;
            }
            case 3: {
                string driver;
                cout << "Enter driver who is wrecked: "; cin >> driver;
                wreckCar(drivers, n, driver);
                displayLeaderboard(drivers, n, currentLap, totalLaps);
                break;
            }
            case 4: {
                cout << "🚨 Safety Car deployed! All cars slowed.\n";
                if (currentLap < totalLaps) {
                    currentLap++;
                    nextLap(drivers, n, true);
                    displayLeaderboard(drivers, n, currentLap, totalLaps);
                    if (currentLap == totalLaps) {
                        cout << "\n🏁 We've got a winner! We've got a winner!! 🏁\n";
                        cout << drivers[0].name << " has done it for " << drivers[0].team << "! 🏆\n";
                        cout << "They are the new Formula 1 World Champion thanks to "
                             << drivers[0].name << "'s miraculous drive on this circuit! ✨\n";
                        cout << "The crowd is on their feet, the team is overjoyed 🎉🥳, "
                             << "and the garage erupts in celebration 🙌👏.\n";
                        cout << "These are unforgettable moments in Formula 1 history! 🏎🔥\n";
                        return 0;
                    }
                }
                break;
            }
            case 5: return 0;
            default: cout << "Invalid choice!\n";
        }
    }
}
