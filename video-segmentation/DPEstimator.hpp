//
//  DPEstimator.hpp
//  video-segmentation
//
//  Created by Stephen Jaud on 29/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#ifndef DPEstimator_hpp
#define DPEstimator_hpp

#include <vector>
#include <string>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "LinearAlgebra.hpp"
#include "MLEstimator.hpp"

// Density Pixel Estimator
class DPEstimator {
public:
    DPEstimator();
    
    void loadData(const std::vector<sf::Image> &);
    void fit(std::string);
    
    sf::Image evaluate(int, float);
    
private:
    void fit_mle();
    void fit_kde();
    
    int N, WIDTH, HEIGHT;
    
    std::vector<std::vector<std::vector<Vector3>>> tensorPixel; // WIDTH x HEIGHT x N ( x 3 )
    std::vector<std::vector<std::vector<float>>> tensorDensity; // WIDTH x HEIGHT x N
};

#endif /* DPEstimator_hpp */
