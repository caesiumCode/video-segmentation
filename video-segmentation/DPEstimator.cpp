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

void DPEstimator::fit(const std::vector<std::string> & imageset, std::string method) {
    if (!method.compare("mle")) {
        fit_mle(imageset);
    } else if (!method.compare("kde")) {
        fit_kde(imageset);
    } else {
        std::cout << "ERROR: unknown method : " << method << "\n";
    }
}

sf::Image DPEstimator::evaluate(int k, float s) {
    sf::Image image_density;
    image_density.create(WIDTH, HEIGHT, sf::Color(0, 0, 0, 0));
    
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (tensorDensity[i][j][k] <= s)
                image_density.setPixel(i, j, sf::Color::Red);
            else
                image_density.setPixel(i, j, sf::Color::Transparent);
        }
    }
    
    return image_density;
}

void DPEstimator::fit_mle(const std::vector<std::string> & imageset) {
    // Get the tensor dimensions
    sf::Image image_info;
    image_info.loadFromFile(imageset[0]);
    N = (int) imageset.size();
    WIDTH = image_info.getSize().x;
    HEIGHT = image_info.getSize().y;
    
    // Initialize the tensors
    tensorDensity = std::vector<std::vector<std::vector<float>>>(WIDTH, std::vector<std::vector<float>>(HEIGHT, std::vector<float>(N, 0.)));
    std::vector<sf::Image> tensorPixel = std::vector<sf::Image>(N, sf::Image());
    
    // Load temporary the images in the RAM for faster computation
    for (int k = 0; k < N; k++)
        tensorPixel[k].loadFromFile(imageset[k]);
    
    // Estimate the pixel density for each 'timepixel'
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            // Load the timepixel
            std::vector<Vector3> timePixel(N, Vector3::Zeros());
            for (int k = 0; k < N; k++)
                timePixel[k] = Vector3(tensorPixel[k].getPixel(i, j));
            
            // Fit the ML estimator
            MLEstimator mlestimator;
            mlestimator.fit(timePixel);
            
            // Estimate the (proportionnal) density for each pixel
            for (int k = 0; k < N; k++)
                tensorDensity[i][j][k] = mlestimator.evaluate(timePixel[k], false);
        }
    }
}

void DPEstimator::fit_kde(const std::vector<std::string> & imageset) {
    
}
