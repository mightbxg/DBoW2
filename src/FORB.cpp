/**
 * File: FORB.cpp
 * Date: June 2012
 * Author: Dorian Galvez-Lopez
 * Description: functions for ORB descriptors
 * License: see the LICENSE.txt file
 */

#include <limits.h>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

#include "DBoW2/FORB.h"

using namespace std;

namespace DBoW2 {

// --------------------------------------------------------------------------

void FORB::meanValue(const std::vector<FORB::pDescriptor>& descriptors, FORB::TDescriptor& mean)
{
    mean.reset();

    if (descriptors.empty())
        return;

    const int N2 = descriptors.size() / 2;

    vector<int> counters(FORB::L_bits, 0);

    vector<FORB::pDescriptor>::const_iterator it;
    for (it = descriptors.begin(); it != descriptors.end(); ++it) {
        const auto& desc = **it;
        for (int i = 0; i < FORB::L_bits; ++i) {
            if (desc[i])
                counters[i]++;
        }
    }

    for (int i = 0; i < FORB::L_bits; ++i) {
        if (counters[i] > N2)
            mean.set(i);
    }
}

// --------------------------------------------------------------------------

double FORB::distance(const FORB::TDescriptor& a, const FORB::TDescriptor& b)
{
    return (double)(a ^ b).count();
}

// --------------------------------------------------------------------------

std::string FORB::toString(const FORB::TDescriptor& a)
{
    return a.to_string();
}

// --------------------------------------------------------------------------

void FORB::fromString(FORB::TDescriptor& a, const std::string& s)
{
    stringstream ss(s);
    ss >> a;
}

// --------------------------------------------------------------------------

void FORB::toMat32F(const std::vector<TDescriptor>& descriptors, cv::Mat& mat)
{
    if (descriptors.empty()) {
        mat.release();
        return;
    }

    const int N = descriptors.size();

    mat.create(N, FORB::L_bits, CV_32F);

    for (int i = 0; i < N; ++i) {
        const TDescriptor& desc = descriptors[i];
        float* p = mat.ptr<float>(i);
        for (int j = 0; j < FORB::L_bits; ++j, ++p) {
            *p = (desc[j] ? 1 : 0);
        }
    }
}

// --------------------------------------------------------------------------

void FORB::toMat32F(const cv::Mat& descriptors, cv::Mat& mat)
{
    descriptors.convertTo(mat, CV_32F);
    return;

    if (descriptors.empty()) {
        mat.release();
        return;
    }

    const int N = descriptors.rows;
    const int C = descriptors.cols;

    mat.create(N, FORB::L * 8, CV_32F);
    float* p = mat.ptr<float>(); // p[i] == 1 or 0

    const unsigned char* desc = descriptors.ptr<unsigned char>();

    for (int i = 0; i < N; ++i, desc += C) {
        for (int j = 0; j < C; ++j, p += 8) {
            p[0] = (desc[j] & (1 << 7) ? 1 : 0);
            p[1] = (desc[j] & (1 << 6) ? 1 : 0);
            p[2] = (desc[j] & (1 << 5) ? 1 : 0);
            p[3] = (desc[j] & (1 << 4) ? 1 : 0);
            p[4] = (desc[j] & (1 << 3) ? 1 : 0);
            p[5] = (desc[j] & (1 << 2) ? 1 : 0);
            p[6] = (desc[j] & (1 << 1) ? 1 : 0);
            p[7] = desc[j] & (1);
        }
    }
}

// --------------------------------------------------------------------------

void FORB::toMat8U(const std::vector<TDescriptor>& descriptors, cv::Mat& mat)
{
    mat.create(descriptors.size(), FORB::L, CV_8U);
    for (int i = 0; i < mat.rows; ++i) {
        auto src = &(descriptors[i]);
        auto dst = mat.ptr<uchar>(i);
        memcpy((void*)src, dst, FORB::L);
    }
}

// --------------------------------------------------------------------------

} // namespace DBoW2
