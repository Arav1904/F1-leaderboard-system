// race_sim.cpp
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
  #include <windows.h>
#endif

using namespace std;

struct Car {
    int id;
    string driver;
    double totalTime;
    bool wrecked;
    int lapsDone;
    Car(int i, string d) : id(i), driver(d), totalTime(0.0), wrecked(false), lapsDone(0) {}
};

// Cross-platform clear screen
void clearScreen() {
#ifdef _WIN32
    // Use cls on Windows
    std::system("cls");
#else
    // Use ANSI escape sequence on Unix-like systems
    std::cout << "\x1B[2J\x1B[H";
#endif
}

// Optional: enable ANSI on Windows 10+ (uncomment if you want to use ANSI on Windows terminals)
void enableAnsiOnWindows() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

// Generate a lap time for a car (seconds), with some randomness and environmental effects
double lapTimeWithModifiers(bool safetyCar, int lapNumber) {
    // base lap in seconds - can be tuned
    double base = 60.0 + (rand() % 16);       // 60..75s
    // chance of rain -> add penalty
    if (rand() % 10 < 2) {                    // ~20% chance
        base += 8.0 + (rand() % 7);           // +8..14s penalty
    }
    // tire wear effect (slightly slower as laps increase)
    base += lapNumber * 0.4;                  // +0.4s per lap
    // safety car effect (big slowdown)
    if (safetyCar) base += 25.0 + (rand() % 10);
    // add small random jitter
    base += ((rand() % 100) - 50) / 100.0;    // -0.5 .. +0.49
    if (base < 30.0) base = 30.0;
    return base;
}

void printLeaderboard(const vector<Car*>& cars, int lap, bool showHeaderEmoji = false) {
    clearScreen();

    // If you want emojis and your console supports UTF-8, set showHeaderEmoji=true and
    // run `chcp 65001` (Windows) and use a font that supports emoji.
    if (showHeaderEmoji) {
        cout << "🏁 F1 LIVE LEADERBOARD   After Lap " << lap << "\n";
    } else {
        cout << "=========== F1 LIVE LEADERBOARD ===========\n";
        cout << "            After Lap " << lap << "\n";
    }
    cout << "-------------------------------------------\n";
    cout << left << setw(5) << "Pos"
         << setw(8) << "Car"
         << setw(18) << "Driver"
         << setw(10) << "Time(s)"
         << setw(10) << "Status" << "\n";
    cout << "-------------------------------------------\n";

    int pos = 1;
    for (auto c : cars) {
        string status = c->wrecked ? "OUT" : "OK";
        // print only active cars in ranking
        if (!c->wrecked) {
            cout << setw(5)  << pos
                 << setw(8)  << ("#" + to_string(c->id))
                 << setw(18) << c->driver
                 << setw(10) << int(c->totalTime)
                 << setw(10) << status
                 << "\n";
            ++pos;
        }
    }
    cout << "-------------------------------------------\n";
    cout << "Active cars: " << pos - 1 << " / " << cars.size() << "\n";
    cout << "-------------------------------------------\n";
}

int main() {
    srand((unsigned) time(nullptr));

    // Uncomment to try enabling ANSI on Windows (may help with colors/escape sequences)
    // enableAnsiOnWindows();

    // Create cars
    vector<Car*> cars = {
        new Car(44, "Hamilton"),
        new Car(1,  "Verstappen"),
        new Car(16, "Leclerc"),
        new Car(11, "Perez"),
        new Car(55, "Sainz")
    };

    int totalLaps = 12;
    bool safetyCar = false;
    int safetyCarRemaining = 0;

    // Keep vector of active cars for sorting each lap
    for (int lap = 1; lap <= totalLaps; ++lap) {
        // chance of wreck before lap updates (some randomness)
        for (auto c : cars) {
            if (!c->wrecked && (rand() % 200) == 0) { // very small chance to keep demo realistic
                c->wrecked = true;
                safetyCar = true;
                safetyCarRemaining = 2; // next 2 laps safety car
                cout << ">>> CRASH: Car #" << c->id << " (" << c->driver << ") retired!\n";
                // small pause to show event before clearing screen
                std::this_thread::sleep_for(std::chrono::milliseconds(650));
            }
        }

        // If safety car active, decrement duration
        if (safetyCar) {
            cout << "!!! SAFETY CAR ACTIVE for " << safetyCarRemaining << " lap(s)\n";
            if (--safetyCarRemaining <= 0) {
                safetyCar = false;
                cout << "!!! Safety car period ended.\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
        }

        // Update lap times for each car
        for (auto c : cars) {
            if (c->wrecked) continue;
            double lapT = lapTimeWithModifiers(safetyCar, lap);
            c->totalTime += lapT;
            c->lapsDone = lap;
            // random pit chance - simple model (optional)
            if ((rand() % 100) < 5) { // 5% chance to have a pit slowdown this lap
                c->totalTime += 8.0 + (rand() % 6); // pit cost + random
            }
        }

        // Build sorted ranking vector (ascending totalTime)
        vector<Car*> ranking;
        for (auto c : cars) if (!c->wrecked) ranking.push_back(c);
        sort(ranking.begin(), ranking.end(), [](Car* a, Car* b){
            return a->totalTime < b->totalTime;
        });

        // Print leaderboard once per lap (real-time feel)
        printLeaderboard(ranking, lap, false);

        // small delay to simulate time between laps
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Final classification
    vector<Car*> finalStanding;
    for (auto c : cars) if (!c->wrecked) finalStanding.push_back(c);
    sort(finalStanding.begin(), finalStanding.end(), [](Car* a, Car* b){
        return a->totalTime < b->totalTime;
    });

    cout << "\n=========== FINAL RESULTS ===========\n";
    int pos = 1;
    for (auto c : finalStanding) {
        cout << pos++ << ". #" << c->id << " " << c->driver << " - " << int(c->totalTime) << "s\n";
    }
    cout << "=====================================\n";

    // cleanup
    for (auto c : cars) delete c;
    return 0;
}
