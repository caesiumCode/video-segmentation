//
//  GMEstimator.hpp
//  video-segmentation
//
//  Created by Stephen Jaud on 02/08/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#ifndef GMEstimator_hpp
#define GMEstimator_hpp

#include <vector>
#include <cmath>

#include "LinearAlgebra.hpp"

// Gaussian mixture estimator
class GMEstimator {
public:
    GMEstimator();
    
    void fit(const std::vector<Vector3> &);
    
    std::vector<float> evaluate(const std::vector<Vector3> &);
    
private:
    const float GC = 1./sqrtf(8. * M_PI * M_PI * M_PI);
    
    int K;
    std::vector<float> tau;
    std::vector<Vector3> mu;
    std::vector<Matrix3> sigma, sigma_inv;
};

#endif /* GMEstimator_hpp */
