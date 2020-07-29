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

#include <SFML/Graphics.hpp>

#include "DPEstimator.hpp"

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
    
    void updateSegmentationImage(int);
    
    float getWindowScale(sf::Vector2u);
    
    void handleEvent(sf::Event);
    
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    float window_scale;
    
    std::vector<sf::Image> imageset;
    int imageset_size, imageset_index;
    sf::Vector2u imageset_dim;
    
    DPEstimator dpestimator;
    float threshold = 0.1;
    sf::Texture texture_segmentation;
    sf::Sprite sprite_segmentation;
};

#endif /* Program_hpp */
