#include "DBoW2/FORB.h"
#include "DBoW2/TemplatedVocabulary.h"
#include "origin/FORB.h"
#include "origin/TemplatedVocabulary.h"
#include <chrono>
#include <iostream>

using namespace std;
using namespace chrono;

using ORBVocabulary = DBoW2::TemplatedVocabulary<DBoW2::FORB, 10>;
using ORBVocabularyOrigin = origin::TemplatedVocabulary<origin::FORB::TDescriptor, origin::FORB>;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        cout << "input: bin fsbin" << endl;
        return 0;
    }

    cout << "time of loading volcabulary -----------"<<endl;
    ORBVocabularyOrigin voc_origin;
    {
        auto time_start = high_resolution_clock::now();
        voc_origin.loadFromBinaryFile(argv[1]);
        auto time_end = high_resolution_clock::now();
        double time_cost = duration_cast<nanoseconds>(time_end - time_start).count() * 1e-6;
        cout << "origin: " << time_cost << " ms" << endl;
    }

    ORBVocabulary voc_bin;
    {
        auto time_start = high_resolution_clock::now();
        voc_bin.loadFromBinaryFile(argv[1]);
        auto time_end = high_resolution_clock::now();
        double time_cost = duration_cast<nanoseconds>(time_end - time_start).count() * 1e-6;
        cout << "bin  : " << time_cost << " ms" << endl;
    }

    ORBVocabulary voc_fsbin;
    {
        auto time_start = high_resolution_clock::now();
        voc_fsbin.loadFromFsBinFile(argv[2]);
        auto time_end = high_resolution_clock::now();
        double time_cost = duration_cast<nanoseconds>(time_end - time_start).count() * 1e-6;
        cout << "fsbin: " << time_cost << " ms" << endl;
    }

    return 0;
}
