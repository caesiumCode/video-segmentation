//
//  DPEstimator.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 29/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include "DPEstimator.hpp"

// Helper functions
float max(float, float, float);
float max(float, float);
float min(float, float, float);
float min(float, float);

// DPEstimator member functions
DPEstimator::DPEstimator() {
    
}

void DPEstimator::fit(const std::vector<std::string> & imageset, std::string method) {
    // Get the tensor dimensions
    sf::Image image_info;
    image_info.loadFromFile(imageset[0]);
    N = (int) imageset.size();
    WIDTH = image_info.getSize().x;
    HEIGHT = image_info.getSize().y;
    
    // Initialize the tensor
    tensorDensity = std::vector<std::vector<std::vector<float>>>(WIDTH, std::vector<std::vector<float>>(HEIGHT, std::vector<float>(N, 0.)));
    
    // Fit the estimator
    if (!method.compare("mle")) {
        fit_mle(imageset);
    } else if (!method.compare("kde")) {
        fit_kde(imageset);
    } else {
        std::cout << "ERROR: unknown method : " << method << "\n";
    }
}

sf::Image DPEstimator::evaluate(int k, float s, float s2) {
    sf::Image image_density;
    image_density.create(WIDTH, HEIGHT, sf::Color::Transparent);
    
    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
            if (tensorDensity[i][j][k] <= s)
                spread(image_density, k, i, j, s2);
    
    return image_density;
}

void DPEstimator::spread(sf::Image & image, int k, int i, int j, float s) {
    if (image.getPixel(i, j) != sf::Color::Red) {
        image.setPixel(i, j, sf::Color::Red);
    
        if (i-1 >= 0 && tensorDensity[i-1][j][k] <= s)
            spread(image, k, i-1, j, s);
        if (i+1 < WIDTH && tensorDensity[i+1][j][k] <= s)
            spread(image, k, i+1, j, s);
        if (j-1 >= 0 && tensorDensity[i][j-1][k] <= s)
            spread(image, k, i, j-1, s);
        if (j+1 < HEIGHT && tensorDensity[i][j+1][k] <= s)
            spread(image, k, i, j+1, s);
    }
}

void DPEstimator::fit_mle(const std::vector<std::string> & imageset) {
    // Load temporary the images in the RAM for faster computation
    std::vector<sf::Image> tensorPixel = std::vector<sf::Image>(N, sf::Image());
    for (int k = 0; k < N; k++)
        tensorPixel[k].loadFromFile(imageset[k]);
    
    // Estimate the pixel density for each 'timepixel'
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            // Load the timepixel
            std::vector<Vector3> timePixel(N, Vector3::Zeros());
            for (int k = 0; k < N; k++)
                timePixel[k] = RGBtoHSL(tensorPixel[k].getPixel(i, j));
            
            // Fit the ML estimator
            MLEstimator mlestimator;
            mlestimator.fit(timePixel);
            
            // Estimate the (proportionnal) density for each pixel
            tensorDensity[i][j] = mlestimator.evaluate(timePixel, false);
        }
    }
}

void DPEstimator::fit_kde(const std::vector<std::string> & imageset) {
    // Load temporary the images in the RAM for faster computation
    std::vector<sf::Image> tensorPixel = std::vector<sf::Image>(N, sf::Image());
    for (int k = 0; k < N; k++)
        tensorPixel[k].loadFromFile(imageset[k]);
    
    // Estimate the pixel density for each 'timepixel'
    for (int i = 0; i < WIDTH; i++) {
        std::cout << i << " sur " << WIDTH-1 << std::endl;
        for (int j = 0; j < HEIGHT; j++) {
            // Load the timepixel
            std::vector<Vector3> timePixel(N, Vector3::Zeros());
            for (int k = 0; k < N; k++)
                timePixel[k] = RGBtoHSL(tensorPixel[k].getPixel(i, j));
            
            // Fit the KD estimator & estimate the density
            KDEstimator kdestimator;
            tensorDensity[i][j] = kdestimator.fit_evaluate(timePixel);
        }
    }
}

Vector3 DPEstimator::RGBtoYCbCr(sf::Color col) {
    float Y = 0.299 * float(col.r) + 0.587 * float(col.g) + 0.114 * float(col.b);
    float Cb = 128. - 0.1687 * float(col.r) - 0.3313 * float(col.g) + 0.5 * float(col.b);
    float Cr = 128. + 0.5 * float(col.r) - 0.4187 * float(col.g) - 0.0813 * float(col.b);
    
    return Vector3(Y, Cb, Cr);
}

Vector3 DPEstimator::RGBtoHSL(sf::Color col) {
    // Normalize RGB values
    float R = float(col.r)/255., G = float(col.g)/255., B = float(col.b)/255.;
    
    // Intermediate variables
    float V = max(R, G, B);
    float C = V - min(R, G, B);
    
    // Compute H, SL, H
    float L = V - C/2;
    
    float H = 0.;
    if (C == 0)
        H = 0;
    else if (V == R)
        H = 60. * (G - B) / C;
    else if (V == G)
        H = 60. * ( 2. + (B - R) / C );
    else if (V == B)
        H = 60. * ( 4. + (R - G) / C );
    
    float SL = 0.;
    if (L == 0. || L == 1.)
        SL = 0.;
    else
        SL = (V - L)/min(L, 1. - L);
    
    return Vector3(H/360., SL, L);
}

float max(float a, float b) {
    if (a > b)
        return a;
    else
        return b;
}

float max(float a, float b, float c) {
    return max(a, max(b, c));
}

float min(float a, float b) {
    if (a < b)
        return a;
    else
        return b;
}

float min(float a, float b, float c) {
    return min(a, min(b, c));
}
