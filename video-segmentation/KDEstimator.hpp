//
//  KDEstimator.hpp
//  video-segmentation
//
//  Created by Stephen Jaud on 30/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#ifndef KDEstimator_hpp
#define KDEstimator_hpp

#include <vector>
#include <cmath>

#include "LinearAlgebra.hpp"

class KDEstimator {
public:
    KDEstimator();
        
    std::vector<float> fit_evaluate(const std::vector<Vector3> &);
        
private:
    float kernel(Vector3);
    
    Matrix3 H, H_inv;
    int n;
};

#endif /* KDEstimator_hpp */
