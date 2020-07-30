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
    
    // Get mean/cov images
    computeImagesetMean();
    texture_mean.loadFromImage(image_mean);
    sprite_mean.setTexture(texture_mean);
    
    computeImagesetCov();
    texture_cov.loadFromImage(image_cov);
    sprite_cov.setTexture(texture_cov);
    
    // Setup scale
    display_mode = 0;
    window_scale = getWindowScale(imageset_dim);
    sprite_mean.scale(window_scale, window_scale);
    sprite_cov.scale(window_scale, window_scale);
    
    // Setup sprite
    sprite.scale(window_scale, window_scale);
    
    // Setup window
    window.create(sf::VideoMode(window_scale*imageset_dim.x, window_scale*imageset_dim.y), TITLE);
    window.setFramerateLimit(30);
    
    // Setup thresholds
    log_threshold = -10.;
    threshold = expf(log_threshold);
    
    // Compute segmentation mask
    std::cout << "INFO: Running segmentation algorithm\n";
    dpestimator.fit(imageset, "mle");
    
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
        
        switch (display_mode) {
            case 0:
                window.draw(sprite);
                break;
                
            case 1:
                window.draw(sprite_mean);
                break;
                
            case 2:
                window.draw(sprite_cov);
                break;
                
            default:
                break;
        }
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
                image.loadFromFile(imageset[imageset_index]);
                texture.update(image);
                
                updateSegmentationImage(imageset_index);
                break;
            
            case sf::Keyboard::Left:
                imageset_index--;
                if (imageset_index < 0)
                    imageset_index = imageset_size - 1;
                image.loadFromFile(imageset[imageset_index]);
                texture.update(image);
                
                updateSegmentationImage(imageset_index);
                break;
                
            case sf::Keyboard::Up:
                log_threshold++;
                threshold = expf(log_threshold);
                updateSegmentationImage(imageset_index);
                break;
                
            case sf::Keyboard::Down:
                log_threshold--;
                threshold = expf(log_threshold);
                updateSegmentationImage(imageset_index);
                break;
                
            case sf::Keyboard::Numpad0:
                display_mode = 0;
                break;
                
            case sf::Keyboard::Numpad1:
                display_mode = 1;
                break;
                
            case sf::Keyboard::Numpad2:
                display_mode = 2;
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
    imageset = filenames;
    imageset_size = (int) filenames.size();
    imageset_index = 0;
    
    // - - - Set image variable - - -
    image.loadFromFile(filenames[imageset_index]);
    texture.loadFromImage(image);
    sprite.setTexture(texture);
    
    imageset_dim = image.getSize();
}

void Program::computeImagesetMean() {
    image_mean.create(imageset_dim.x, imageset_dim.y);
    std::vector<std::vector<Vector3>> temp_mean(imageset_dim.x, std::vector<Vector3>(imageset_dim.y, Vector3::Zeros()));
    
    for (int k = 0; k < imageset_size; k++) {
        sf::Image image_temp;
        image_temp.loadFromFile(imageset[k]);
        
        for (int i = 0; i < imageset_dim.x; i++) {
            for (int j = 0; j < imageset_dim.y; j++) {
                temp_mean[i][j] = temp_mean[i][j] + Vector3(image_temp.getPixel(i, j));
            }
        }
    }
    
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            image_mean.setPixel(i, j, (1./float(imageset_size) * temp_mean[i][j]).toColor());
        }
    }
}

void Program::computeImagesetCov() {
    image_cov.create(imageset_dim.x, imageset_dim.y);
    
    std::vector<std::vector<float>> cov(imageset_dim.x, std::vector<float>(imageset_dim.y, 0.));
    float max_cov = 0;
    
    for (int k = 0; k < imageset_size; k++) {
        sf::Image image_temp;
        image_temp.loadFromFile(imageset[k]);
        
        for (int i = 0; i < imageset_dim.x; i++) {
            for (int j = 0; j < imageset_dim.y; j++) {
                sf::Color col = image_temp.getPixel(i, j);
                sf::Color col_mean = image_mean.getPixel(i, j);
                
                cov[i][j] += float(col.r - col_mean.r)*float(col.r - col_mean.r) +
                             float(col.g - col_mean.g)*float(col.g - col_mean.g) +
                             float(col.b - col_mean.b)*float(col.b - col_mean.b);
            }
        }
    }
    
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            cov[i][j] = sqrtf(cov[i][j] / float(imageset_size));
            if (cov[i][j] > max_cov)
                max_cov = cov[i][j];
        }
    }
    
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            float val = cov[i][j] / float(max_cov);
            image_cov.setPixel(i, j, sf::Color((int) val, (int) val, (int) val));
        }
    }
}

void Program::updateSegmentationImage(int k) {
    texture_segmentation.update(dpestimator.evaluate(k, threshold));
}
