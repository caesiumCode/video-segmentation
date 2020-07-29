//
//  Program.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 21/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include "Program.hpp"

Program::Program(std::string inputPath) {
    // Load imageset
    loadImageset(inputPath);
    
    // Setup scale
    window_scale = getWindowScale(imageset_dim);
    
    // Setup sprite
    texture.loadFromImage(imageset[imageset_index]);
    sprite.setTexture(texture);
    sprite.scale(window_scale, window_scale);
    
    // Setup window
    window.create(sf::VideoMode(window_scale*imageset_dim.x, window_scale*imageset_dim.y), TITLE);
    window.setFramerateLimit(30);
    
    // Compute segmentation mask
    std::cout << "INFO: Transforming imageset\n";
    dpestimator.loadData(imageset);
    
    std::cout << "INFO: Running segmentation algorithm\n";
    dpestimator.fit("mle");
    
    std::cout << "INFO: Extract segmentation mask\n";
    texture_segmentation.loadFromImage(dpestimator.evaluate(imageset_index, threshold));
    sprite_segmentation.setTexture(texture_segmentation);
    sprite_segmentation.scale(window_scale, window_scale);
}

float Program::getWindowScale(sf::Vector2u dim) {
    float scalex = (float) MAX_WIDTH/dim.x, scaley = (float) MAX_HEIGHT/dim.y;
    
    if (scalex < scaley)
        return scalex;
    else
        return scaley;
}

void Program::run() {    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            handleEvent(event);
        }
        
        window.clear();
        
        window.draw(sprite);
        window.draw(sprite_segmentation);
        
        window.display();
    }
}

void Program::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed)
        window.close();
    
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Right:
                imageset_index = (imageset_index + 1)%imageset_size;
                texture.update(imageset[imageset_index]);
                
                std::cout << "INFO: Extract segmentation mask\n";
                texture_segmentation.update(dpestimator.evaluate(imageset_index, threshold));
                break;
            
            case sf::Keyboard::Left:
                imageset_index--;
                if (imageset_index < 0)
                    imageset_index = imageset_size - 1;
                texture.update(imageset[imageset_index]);
                
                std::cout << "INFO: Extract segmentation mask\n";
                texture_segmentation.update(dpestimator.evaluate(imageset_index, threshold));
                break;
                
            default:
                break;
        }
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
    
    // - - - Set variable - - -
    imageset_size = (int) filenames.size();
    imageset_index = 0;
    
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
