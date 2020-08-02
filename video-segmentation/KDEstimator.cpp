//
//  KDEstimator.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 30/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include "KDEstimator.hpp"

KDEstimator::KDEstimator() {

}

float KDEstimator::kernel(Vector3 u) {
    return expf(-0.5 * u * (H_inv * u));
}

std::vector<float> KDEstimator::fit_evaluate(const std::vector<Vector3> & data) {
    n = (int) data.size();
    
    // - - - - - FITTING - - - - -
    // Compute mean
    Vector3 mean = Vector3::Zeros();
    for (int k = 0; k < n; k++)
        mean = mean + data[k];
    mean = 1./float(n) * mean;
    
    // Estimate bandwith matrix
    H = Matrix3::Zeros();
    for (int k = 0; k < n; k++)
        H = H + outerp(data[k]);
    H = powf(float(n), -2./7.) / float(n-1) * (H - float(n) * outerp(mean)); // Scott's rule + Unbiased sample covariance
    H_inv = H.inverse();
    
    // - - - - - Evaluate - - - - -
    // Initialize K(xi - xj)
    std::vector<std::vector<float>> K(n, std::vector<float>(n, 0.));
    for (int i = 0; i < n; i++) {
        K[i][i] = 1;
        for (int j = i+1; j < n; j++) {
            K[i][j] = kernel(data[i] - data[j]);
            K[j][i] = K[i][j];
        }
    }
    
    // Sum the kernels
    std::vector<float> y(n, 0.);
    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i != j)
                y[i] += K[i][j];
    
    // Find de maxumum density
    float max_d = 0.;
    for (int k = 0; k < n; k++)
        if (y[k] > max_d)
            max_d = y[k];
    
    // Normalize the density
    for (int k = 0; k < n; k++)
        y[k] /= max_d;
    
    return y;
}
