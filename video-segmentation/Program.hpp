//
//  Program.hpp
//  video-segmentation
//
//  Created by Stephen Jaud on 21/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#ifndef Program_hpp
#define Program_hpp

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "DPEstimator.hpp"
#include "LinearAlgebra.hpp"

namespace fs = std::filesystem;


class Program {
public:
    Program(std::string);
    void run();
    
private:
    const std::vector<std::string> SUPPORTED_IMAGE_FORMATS = {".png", ".jpg", ".jpeg"};
    const std::string TITLE = "Video Segmentation";
    const int MAX_WIDTH = 2000, MAX_HEIGHT = 1000;
    
    bool isSupported(std::string);
    void loadImageset(std::string);
    void computeImagesetMean();
    void computeImagesetCov();
    
    void updateSegmentationImage(int);
    
    float getWindowScale(sf::Vector2u);
    
    void handleEvent(sf::Event);
    
    sf::RenderWindow window;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    float window_scale;
    
    std::vector<std::string> imageset;
    int imageset_size, imageset_index;
    sf::Vector2u imageset_dim;
    sf::Image image_mean, image_cov;
    sf::Texture texture_mean, texture_cov;
    sf::Sprite sprite_mean, sprite_cov;
    int display_mode;
    
    DPEstimator dpestimator;
    float threshold, log_threshold;
    sf::Texture texture_segmentation;
    sf::Sprite sprite_segmentation;
};

#endif /* Program_hpp */
