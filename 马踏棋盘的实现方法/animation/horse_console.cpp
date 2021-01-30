#include "horsejump.h"

int main() {
    {
    HorseJump<5, 6> horse;
    horse.StepAllGreedy(0, XY{0, 0});
    }
    // coroutine
/*
    {
        HorseJump<5, 6> horse;
        auto g = horse.CoroStep(0, XY{4, 0});
        for(auto&& [depth, x, y] : g) {
            cout<<"coro step: "<<x<<", "<<y<<"\n";
        }
    }
*/
}
