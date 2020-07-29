//
//  MLEstimator.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 29/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include "MLEstimator.hpp"

MLEstimator::MLEstimator() {
    
}

float MLEstimator::evaluate(Vector3 x, bool log) {
    Vector3 temp = x - mean;
    if (!log) {
        return expf(-1./2. * temp * (cov_inv * temp));
    } else {
        return -1./2. * temp * (cov_inv * temp);
    }
}

void MLEstimator::fit(std::vector<Vector3> & data) {
    n = (int) data.size();
    
    // Estimate the mean
    mean = Vector3::Zeros();
    for (int k = 0; k < n; k++)
        mean = mean + data[k];
    mean = 1./float(n) * mean;
    
    // Estimate the covariance
    cov = Matrix3::Zeros();
    for (int k = 0; k < n; k++)
        cov = cov + data[k].outerp();
    cov = 1./float(n) * cov;
    cov = cov - mean.outerp();
    cov_inv = cov.inverse();
}

