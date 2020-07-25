//
//  Program.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 21/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include "Program.hpp"

Program::Program(std::string inputPath) {
    loadImageset(inputPath);
    
    window.create(sf::VideoMode(imageset_dim.x, imageset_dim.y), TITLE);
    window.setFramerateLimit(30);
}

void Program::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.display();
    }
}

bool Program::isSupported(std::string ext) {
    for (const auto& exti : SUPPORTED_IMAGE_FORMATS)
        if (ext.compare(exti) == 0)
            return true;
    
    return false;
}

void Program::loadImageset(std::string inputPath) {
    // - - - Initialise filenames - - -
    std::vector<std::string> filenames = {};
    
    for (const auto& entry : fs::directory_iterator(inputPath)) {
        std::string file_extension = entry.path().extension();
        std::string file_name = entry.path().string();
        if (isSupported(file_extension))
            filenames.push_back(file_name);
    }
    
    std::sort(filenames.begin(), filenames.end());
    
    // - - - Set imageset_size variable - - -
    imageset_size = (int) filenames.size();
    
    // - - - Load imageset - - -
    std::cout << "INFO: Loading imageset\n";
    for (int i = 0; i < imageset_size; i++) {
        sf::Image image;
        if (!image.loadFromFile(filenames[i])) {
            std::cout << "\nLOADING ERROR : " << filenames[i] << "\n";
        } else {
            imageset.push_back(image);
        }
    }
    std::cout << "\n";
    
    // - - - Set imageset_dim variable - - -
    imageset_dim = imageset[0].getSize();
}
