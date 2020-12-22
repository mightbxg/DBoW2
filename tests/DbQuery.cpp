#include <DBoW2/DBoW2.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

using OrbDescriptor = OrbVocabulary::TDescriptor;

vector<OrbDescriptor> cvtDescriptors(const cv::Mat& m)
{
    CV_Assert(m.rows > 0 && m.cols == 32);
    CV_Assert(m.type() == CV_8UC1 && m.isContinuous());
    vector<OrbDescriptor> ret(m.rows);
    memcpy(&ret[0], m.ptr(), m.rows * 32);
    return ret;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        cout << "Args: <vocabulary> <image_folder>" << endl;
        return 0;
    }
    OrbVocabulary voc;
    if (!voc.loadFromFsBinFile(argv[1])) {
        cout << "cannot load vocabulary from " << argv[1] << endl;
        return -1;
    }
    OrbDatabase db;
    db.setVocabulary(voc, false, 0);

    vector<String> fns;
    glob(string(argv[2]) + "/*.png", fns);
    if (fns.empty()) {
        cout << "no image in " << argv[2] << endl;
        return -2;
    }

    auto orb = cv::ORB::create(1000, 1.2f, 3, 31, 0, 2, ORB::HARRIS_SCORE, 31, 7);
    ofstream out("QueryResults.txt", ios::out);
    for (size_t idx = 0; idx < fns.size(); ++idx) {
        Mat image = imread(fns[idx], IMREAD_GRAYSCALE);
        if (image.empty()) {
            cout << "cannot load image " << fns[idx] << endl;
            break;
        }
        vector<KeyPoint> kps;
        Mat descs_mat;
        orb->detectAndCompute(image, noArray(), kps, descs_mat);
        auto descs = cvtDescriptors(descs_mat);

        DBoW2::BowVector bv;
        DBoW2::FeatureVector fv;
        voc.transform(descs, bv, fv, 4);
        DBoW2::QueryResults ret;
        db.query(bv, ret, 4);
        db.add(bv);
        out << idx << " -----------------" << endl;
        for (const auto& r : ret)
            out << "  " << r.Id << " " << r.Score << endl;
        cout << "." << flush;
    }
    out.close();
    cout << "done" << endl;

    return 0;
}
