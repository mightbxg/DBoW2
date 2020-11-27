#include "DBoW2/FORB.h"
#include "DBoW2/TemplatedVocabulary.h"
#include <chrono>
#include <iostream>

using ORBVocabulary = DBoW2::TemplatedVocabulary<DBoW2::FORB, 10>;
using namespace std;
using namespace chrono;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        cout << "input: bin fsbin" << endl;
        return 0;
    }

    {
        ORBVocabulary voc;
        auto time_start = high_resolution_clock::now();
        voc.loadFromBinaryFile(argv[1]);
        auto time_end = high_resolution_clock::now();
        double time_cost = duration_cast<nanoseconds>(time_end - time_start).count() * 1e-6;
        cout << "bin  : " << time_cost << " ms" << endl;
    }

    {
        ORBVocabulary voc;
        auto time_start = high_resolution_clock::now();
        voc.loadFromFsBinFile(argv[2]);
        auto time_end = high_resolution_clock::now();
        double time_cost = duration_cast<nanoseconds>(time_end - time_start).count() * 1e-6;
        cout << "fsbin: " << time_cost << " ms" << endl;
    }

    return 0;
}
