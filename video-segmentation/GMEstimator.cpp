//
//  GMEstimator.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 02/08/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include "GMEstimator.hpp"

GMEstimator::GMEstimator() {
    K = 2;
    
    tau = std::vector<float>(K, 0.);
    mu = std::vector<Vector3>(K, Vector3::Zeros());
    sigma = std::vector<Matrix3>(K, Matrix3::Zeros());
    sigma_inv = std::vector<Matrix3>(K, Matrix3::Zeros());
}

std::vector<float> GMEstimator::evaluate(const std::vector<Vector3> & x) {
    // Initialize variables
    int n = (int) x.size();
    std::vector<float> y(n, 0.);
    
    // Compute the estimate density at each point
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < K; i++) {
            Vector3 temp = x[k] - mu[i];
            y[k] += tau[i] * GC * expf(-0.5 * temp * (sigma_inv[i] * temp)) / sqrtf(sigma[i].det());
        }
    }
    
    return y;
}
