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

template <unsigned long N>
static inline bitset<N> getRandomBitset()
{
    bitset<N> ret;
    for (unsigned i = 0; i < N; ++i)
        ret[i] = rand() & 1;
    return ret;
}

template <unsigned long N>
static inline cv::Mat bitset2Mat(const bitset<N>& src)
{
    static_assert(N % 8 == 0, "num of bits must be multiple of 8");
    int size = (N >> 3);
    cv::Mat ret(1, size, CV_8UC1);
    memcpy(ret.data, reinterpret_cast<const void*>(&src), size);
    return ret;
}

template <typename T>
static inline bool floatEq(T v1, T v2)
{
    constexpr T eps = numeric_limits<T>::epsilon();
    if (std::abs(v1) < eps)
        return std::abs(v2) < eps;
    return std::abs(v1 - v2) / v1 < 1e-5;
}

// map<uint, double>
template <typename T1, typename T2>
static bool compareBowVector(const T1& bv1, T2& bv2)
{
    if (bv1.size() != bv2.size())
        return false;
    for (const auto& bow : bv1) {
        auto q = bv2.find(bow.first);
        if (bv2.end() == q || !floatEq(q->second, bow.second))
            return false;
    }
    return true;
}

template <typename Int1, typename Int2>
static bool compareIntVector(const vector<Int1>& vec1, const vector<Int2>& vec2)
{
    if (vec1.size() != vec2.size())
        return false;
    for (size_t i = 0; i < vec1.size(); ++i)
        if (vec1[i] != vec2[i])
            return false;
    return true;
}

// map<uint, vector<uint>>
template <typename T1, typename T2>
static bool compareFeatureVector(const T1& fv1, const T2& fv2)
{
    if (fv1.size() != fv2.size())
        return false;
    for (const auto& fe : fv1) {
        auto q = fv2.find(fe.first);
        if (fv2.end() == q || !compareIntVector(q->second, fe.second))
            return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        cout << "input: bin fsbin" << endl;
        return 0;
    }
    auto time_start = high_resolution_clock::now();
    auto timerStart = [&time_start] {
        time_start = high_resolution_clock::now();
    };
    auto timerEnd = [&time_start](const string& msg) {
        auto time_end = high_resolution_clock::now();
        double time_cost = duration_cast<nanoseconds>(time_end - time_start).count() * 1e-6;
        cout << msg << ": " << time_cost << " ms" << endl;
    };

    cout << "time of loading volcabulary -----------" << endl;
    ORBVocabularyOrigin voc_origin;
    {
        timerStart();
        voc_origin.loadFromBinaryFile(argv[1]);
        timerEnd("origin");
    }
    ORBVocabulary voc_bin;
    {
        timerStart();
        voc_bin.loadFromBinaryFile(argv[1]);
        timerEnd("  bin");
    }
    ORBVocabulary voc_fsbin;
    {
        timerStart();
        voc_fsbin.loadFromFsBinFile(argv[2]);
        timerEnd("fsbin");
    }

    cout << "transform test ------------------------" << endl;
    // generate descriptors
    constexpr int num_descs = 10000;
    cout << "num of descriptors: " << num_descs << endl;
    vector<ORBVocabulary::TDescriptor> descs;
    vector<cv::Mat> descs_org;
    descs.reserve(num_descs);
    descs_org.reserve(num_descs);
    for (int i = 0; i < num_descs; ++i) {
        descs.push_back(getRandomBitset<256>());
        descs_org.emplace_back(bitset2Mat(descs.back()));
    }
    auto transformTest = [&](int levelsup) {
        cout << "\33[33mlevelsup = " << levelsup << "\33[0m" << endl;
        // origin
        origin::BowVector bv_origin;
        origin::FeatureVector fv_origin;
        timerStart();
        voc_origin.transform(descs_org, bv_origin, fv_origin, levelsup);
        timerEnd("origin");
        // bin
        DBoW2::BowVector bv_bin;
        DBoW2::FeatureVector fv_bin;
        timerStart();
        voc_bin.transform(descs, bv_bin, fv_bin, levelsup);
        timerEnd("  bin");
        // fsbin
        DBoW2::BowVector bv_fsbin;
        DBoW2::FeatureVector fv_fsbin;
        timerStart();
        voc_fsbin.transform(descs, bv_fsbin, fv_fsbin, levelsup);
        timerEnd("fsbin");

        // compare
        const string str_matched = "\33[32mmatched\33[0m";
        const string str_unmatched = "\33[31mUNMATCHED\33[0m";
        cout << "BowVector of bin: " << (compareBowVector(bv_origin, bv_bin) ? str_matched : str_unmatched) << endl;
        cout << "FeatureVector of bin: " << (compareFeatureVector(fv_origin, fv_bin) ? str_matched : str_unmatched) << endl;
        cout << "BowVector of fsbin: " << (compareBowVector(bv_origin, bv_fsbin) ? str_matched : str_unmatched) << endl;
        cout << "FeatureVector of fsbin: " << (compareFeatureVector(fv_origin, fv_fsbin) ? str_matched : str_unmatched) << endl;
    };
    transformTest(4);
    transformTest(0);

    return 0;
}
