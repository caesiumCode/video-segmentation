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
        return expf(-0.5 * temp * (cov_inv * temp));
    } else {
        return -0.5 * temp * (cov_inv * temp);
    }
}

std::vector<float> MLEstimator::evaluate(const std::vector<Vector3> & x, bool log) {
    int x_size = (int) x.size();
    std::vector<float> y(x_size, 0.);
    
    if (!log) {
        for (int k = 0; k < x_size; k++) {
            Vector3 temp = x[k] - mean;
            y[k] = expf(-0.5 * temp * (cov_inv * temp));
        }
    } else {
        for (int k = 0; k < x_size; k++) {
            Vector3 temp = x[k] - mean;
            y[k] = -0.5 * temp * (cov_inv * temp);
        }
    }
    
    return y;
}

void MLEstimator::fit(const std::vector<Vector3> & data) {
    n = (int) data.size();
    
    // Estimate the mean
    mean = Vector3::Zeros();
    for (int k = 0; k < n; k++)
        mean = mean + data[k];
    mean = 1./float(n) * mean;
    
    // Estimate the covariance
    cov = Matrix3::Zeros();
    for (int k = 0; k < n; k++)
        cov = cov + outerp(data[k]);
    cov = 1./float(n-1) * (cov - float(n) * outerp(mean)); // Unbiased sample covariance
    cov_inv = cov.inverse();
}

