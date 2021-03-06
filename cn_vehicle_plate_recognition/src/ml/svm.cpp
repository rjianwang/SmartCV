/* \file svm.cpp
 * An implementation of SVM classifier
 */

#include "../../include/ml/svm.h"
#include "../../include/tool/tool.h"
#include "../../include/core/resource.h"
#include "../../include/core/feature.h"

/* \namespace pr
 * Namespace wherea all C++ Plate Recognition functionality resides
 */
namespace pr
{

SVMClassifier::SVMClassifier()
{
    SVM_params.svm_type = CvSVM::C_SVC;
    SVM_params.kernel_type = CvSVM::RBF; // CvSVM::LINEAR
    SVM_params.degree = 0.1; // 0
    SVM_params.gamma = 0.1;  // 1
    SVM_params.coef0 = 0.1;  // 0
    SVM_params.C = 1;
    SVM_params.nu = 0.1;     // 0
    SVM_params.p = 0.1;      // 0
    SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 0.00001);

    svmClassifier = new CvSVM;
}

SVMClassifier::~SVMClassifier()
{
    if (svmClassifier != NULL)
        delete svmClassifier;
}

// Load training datas from directory
// For EXAMPLE: 
//     There is a directory "train" and two sub-directories in it,
//     that is "plates0", "plates1". 
//     We load datas from "train".
void SVMClassifier::load_plate(const std::string filepath)
{
    if (DEBUG_MODE)
        std::cout << "Loading training data for SVM classifier..." << std::endl;

    std::string paths[2] = {filepath + "/plates0/", filepath + "plates1/"};
    for (int n = 0; n < 2; n++)
    {
        std::vector<std::string> files;
        files = getFiles(paths[n]);

        if (files.size() == 0)
            std::cout << "Load trainging data ERROR: empty directory" << std::endl;

        for (int i = 0; i < files.size(); i++)
        {
            std::string path = paths[n] + files[i];
            cv::Mat img = cv::imread(path.c_str());
            cv::cvtColor(img, img, CV_BGR2GRAY);

            if (img.cols == 0)
               std::cout << "Fail to load images " << files[i] << std::endl;

            // resized to 36 * 136
            cv::Mat resized;
            resized.create(36, 136, CV_32FC1);
            resize(img, resized, resized.size(), 0, 0, cv::INTER_CUBIC);

            resized = resized.reshape(1, 1);
            trainData.push_back(resized);
            labelData.push_back(n);
        }
    }
}

void SVMClassifier::load_cn(const std::string filepath)
{
    if (DEBUG_MODE)
    {
        std::cout << "Loading training data(Chinese Characters) for SVM classifier." 
            << std::endl;
    }

    for (int n = 0; n < Resources::numCNCharacters; n++)
    {
        std::vector<std::string> files;
        files = getFiles(filepath + Resources::cn_chars[n] + "/");

        if (files.size() == 0)
            std::cout << "Loading trainning data(Chinese Characters) ERROR. "
                << "Directory \"" << filepath + Resources::cn_chars[n] << "\" is empty." 
                << std::endl;

        for (int i = 0; i < files.size(); i++)
        {
            std::string path = filepath + Resources::cn_chars[n] + "/" + files[i];
            cv::Mat img = cv::imread(path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

            if (img.cols == 0)
                std::cout << "Fail to load images " << path << std::endl;
            
//            cv::threshold(img, img, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

            cv::Mat f = features(img);
            trainData.push_back(f);
            labelData.push_back(n);
        }
    }
}

void SVMClassifier::load_char(const std::string filepath)
{
    if (DEBUG_MODE)
    {
        std::cout << "Loading training data(digits & letters) for SVM classifier." 
            << std::endl;
    }

    for (int n = 0; n < Resources::numCharacters; n++)
    {
        std::vector<std::string> files;
        files = getFiles(filepath + Resources::chars[n] + "/");

        if (files.size() == 0)
            std::cout << "Loading trainning data ERROR. " 
                << "Directory \"" << filepath + Resources::chars[n] << "\" is empty."
                << std::endl;

        for (int i = 0; i < files.size(); i++)
        {
            std::string path = filepath + Resources::chars[n] + "/" + files[i];
            cv::Mat img = cv::imread(path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

            if (img.cols == 0)
                std::cout << "Fail to load images " << path << std::endl;
            
 //           cv::threshold(img, img, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
            
            //cv::Mat f = features(img);
            cv::Mat resized;
            resized.create(24, 12, CV_32FC1);
            cv::resize(img, resized, resized.size());
            resized = resized.reshape(1, 1);

            trainData.push_back(resized);
            labelData.push_back(n);
        }
    }
}

// Load training model
void SVMClassifier::load_model(std::string filename)
{
    if (DEBUG_MODE)
        std::cout << "Loading model for SVM classifier." << std::endl;
    
    svmClassifier->load(filename.c_str());
}

// train
bool SVMClassifier::train()
{
    if (DEBUG_MODE)
    {
        std::cout << "Training ..." << std::endl;
        std::cout << "\ttrainData size: " << trainData.size() << std::endl;
        std::cout << "\tlabelData size: " << labelData.size() << std::endl;
    }
    
    trainData.convertTo(trainData, CV_32FC1);
    svmClassifier = new CvSVM(trainData, labelData, cv::Mat(), cv::Mat(), SVM_params);
}

void SVMClassifier::save(std::string filepath)
{
    if (DEBUG_MODE)
    {
        std::cout << "Saving training model..." << std::endl;
        std::cout << "\tSVM_cn.xml" << std::endl;
    }

    cv::FileStorage fs(filepath.c_str(), cv::FileStorage::WRITE);
    svmClassifier->write(*fs, "svm");
}

// predict
float SVMClassifier::predict(const cv::Mat &sample)
{
    if (DEBUG_MODE)
    {
        std::cout << "Predicting..." << std::endl;
        std::cout << "\tSample size: " << sample.size() << std::endl;
    }

    float response = svmClassifier->predict(sample);
    return response;
}

} /* ends for namespace pr */
