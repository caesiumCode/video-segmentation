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

namespace fs = std::filesystem;


class Program {
public:
    Program(std::string);
    void run();
    
private:
    const std::vector<std::string> SUPPORTED_IMAGE_FORMATS = {".png", ".jpg", ".jpeg"};
    const std::string TITLE = "Video Segmentation";
    
    bool isSupported(std::string);
    void loadImageset(std::string);
    
    sf::RenderWindow window;
    
    std::vector<sf::Image> imageset;
    int imageset_size;
    sf::Vector2u imageset_dim;
};

#endif /* Program_hpp */
