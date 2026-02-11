#include <BlFa3/VariousUtilityFunctions.hpp>

namespace BlFa3 {

const bool getbin(int Value,const int Get) {
    
    // Function used for extracting specific bits from integers (or other similar formats) so that larger settings may be used.
    // Probably a really bad way of doing it but it's far better than my other way lmao
    
    int Compare = 1;
    int Layers = 0;
    bool Result = 0;
    while (Compare <= Value) {
        Compare *= 2;
        Layers++;
    };
    while(Layers >= Get) {
        Compare /= 2;
        if(Value >= Compare) {
            Value -= Compare;
            Result = 1;
        } else {
            Result = 0;
        };
        Layers-=1;
    }
    return Result;
}

} // Namespace BlFa3