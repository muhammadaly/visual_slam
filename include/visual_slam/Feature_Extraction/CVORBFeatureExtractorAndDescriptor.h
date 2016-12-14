#ifndef CVORBFEATUREEXTRACTORANDDESCRIPTOR_H
#define CVORBFEATUREEXTRACTORANDDESCRIPTOR_H

#include "FeatureExtractorAndDescriptor.h"
#include <opencv2/features2d/features2d.hpp>

class CVORBFeatureExtractorAndDescriptor : public FeatureExtractorAndDescriptor
{
public:
    CVORBFeatureExtractorAndDescriptor();
    cv::Mat computeDescriptors(FrameData pFrameData , std::vector<cv::KeyPoint>& tkeypoint ,cv::Mat& tdescriptors);
private:
    cv::Ptr<cv::ORB> orb ;
};

#endif // CVORBFEATUREEXTRACTORANDDESCRIPTOR_H
