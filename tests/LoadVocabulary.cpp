#include "DBoW2/FORB.h"
#include "DBoW2/TemplatedVocabulary.h"
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

using ORBVocabulary = DBoW2::TemplatedVocabulary<DBoW2::FORB, 10>;
using namespace std;
using namespace chrono;
using namespace cv;

#define LOAD_FS(m)   \
    {                \
        fs[#m] >> m; \
    }

template <typename T>
inline static bool floatEq(T a, T b)
{
    return (abs(a - b) / b) < 1e-5;
}

template <typename T1, typename T2>
inline static bool vectorEq(const vector<T1>& v1, const vector<T2>& v2)
{
    if (v1.size() != v2.size())
        return false;
    for (size_t i = 0; i < v1.size(); ++i)
        if (v1[i] != v2[i])
            return false;
    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout << "no input" << endl;
        return 0;
    }

    ORBVocabulary voc;
    auto time_start = high_resolution_clock::now();
    //    voc.loadFromBinaryFile(argv[1]);
    voc.loadFromFsBinFile("t.fsbin");
    auto time_end = high_resolution_clock::now();
    double time_cost = duration_cast<nanoseconds>(time_end - time_start).count() * 1e-6;
    cout << "time cost: " << time_cost << " ms" << endl;

    voc.saveToFsBinFile("t.fsbin");

    { // test
        FileStorage fs;
        if (!fs.open("t.yaml", FileStorage::READ)) {
            cout << "cannot load yaml" << endl;
            return 0;
        }
        Mat descriptors;
        vector<int> bv_idx;
        vector<double> bv_word;
        vector<int> fv_idx;
        vector<vector<int>> fv_feat;
        LOAD_FS(descriptors);
        LOAD_FS(bv_idx);
        LOAD_FS(bv_word);
        LOAD_FS(fv_idx);
        LOAD_FS(fv_feat);

        // transform
        vector<DBoW2::FORB::TDescriptor> descs(descriptors.rows);
        for (int i = 0; i < descriptors.rows; ++i)
            memcpy(&(descs[i]), descriptors.ptr<uchar>(i), 32);
        DBoW2::BowVector bv;
        DBoW2::FeatureVector fv;
        voc.transform(descs, bv, fv, 4);

        // check BoWVector
        for (size_t i = 0; i < bv_idx.size(); i++) {
            auto idx = bv_idx[i];
            auto word = bv_word[i];
            if (bv.find(idx) == bv.end()) {
                cout << "idx not found in bv: " << idx << endl;
                break;
            }
            if (!floatEq(bv[idx], word)) {
                cout << "word not match in bv: " << bv[idx] << " " << word << endl;
                break;
            }
        }

        // check FeatureVector
        for (size_t i = 0; i < fv_idx.size(); ++i) {
            auto idx = fv_idx[i];
            const auto& feat = fv_feat[i];
            if (fv.find(idx) == fv.end()) {
                cout << "idx not found in fv: " << idx << endl;
                break;
            }
            if (!vectorEq(fv[idx], feat)) {
                cout << "feature not match in fv: " << idx << endl;
                break;
            }
        }
    }

    return 0;
}
