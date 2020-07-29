//
//  MLEstimator.hpp
//  video-segmentation
//
//  Created by Stephen Jaud on 29/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#ifndef MLEstimator_hpp
#define MLEstimator_hpp

#include <vector>
#include <cmath>

#include "LinearAlgebra.hpp"

// Maximum Likelihood Estimator (Normal distribution)
class MLEstimator {
public:
    MLEstimator();
    
    void fit(std::vector<Vector3> &);
    
    float evaluate(Vector3, bool);
    
private:
    int n;
    
    Vector3 mean;
    Matrix3 cov, cov_inv;
};

#endif /* MLEstimator_hpp */
