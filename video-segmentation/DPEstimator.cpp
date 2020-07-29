//
//  DPEstimator.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 29/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include "DPEstimator.hpp"

DPEstimator::DPEstimator() {
    
}

void DPEstimator::loadData(const std::vector<sf::Image> & imageset) {
    // Get tensor dimensions
    N = (int) imageset.size();
    WIDTH = imageset[0].getSize().x;
    HEIGHT = imageset[0].getSize().y;
    
    // Initialize tensors
    tensorPixel = std::vector<std::vector<std::vector<Vector3>>>(WIDTH, std::vector<std::vector<Vector3>>(HEIGHT, std::vector<Vector3>(N, Vector3())));
    tensorDensity = std::vector<std::vector<std::vector<float>>>(WIDTH, std::vector<std::vector<float>>(HEIGHT, std::vector<float>(N, 0.)));
    
    // Convert imageset into tensor
    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
            for (int k = 0; k < N; k++)
                tensorPixel[i][j][k] = Vector3(imageset[k].getPixel(i, j));
}

void DPEstimator::fit(std::string method) {
    if (!method.compare("mle")) {
        fit_mle();
    } else if (!method.compare("kde")) {
        fit_kde();
    } else {
        std::cout << "ERROR: unknown method : " << method << "\n";
    }
}

void DPEstimator::fit_mle() {
    
}

void DPEstimator::fit_kde() {
    
}