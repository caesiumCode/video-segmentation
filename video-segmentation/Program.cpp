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
    texture.loadFromImage(imageset[imageset_index]);
    sprite.setTexture(texture);
    sprite.scale(window_scale, window_scale);
    
    // Setup window
    window.create(sf::VideoMode(window_scale*imageset_dim.x, window_scale*imageset_dim.y), TITLE);
    window.setFramerateLimit(30);
    
    // Setup thresholds
    log_threshold = -10.;
    threshold = expf(log_threshold);
    
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
                texture.update(imageset[imageset_index]);
                
                updateSegmentationImage(imageset_index);
                break;
            
            case sf::Keyboard::Left:
                imageset_index--;
                if (imageset_index < 0)
                    imageset_index = imageset_size - 1;
                texture.update(imageset[imageset_index]);
                
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

void Program::computeImagesetMean() {
    image_mean.create(imageset_dim.x, imageset_dim.y);
    
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            float r = 0., g = 0., b = 0.;
            for (int k = 0; k < imageset_size; k++) {
                sf::Color col = imageset[k].getPixel(i, j);
                r += (float) col.r;
                g += (float) col.g;
                b += (float) col.b;
            }
            r /= (float) imageset_size;
            g /= (float) imageset_size;
            b /= (float) imageset_size;
                        
            image_mean.setPixel(i, j, sf::Color((int) r, (int) g, (int) b));
        }
    }
}

void Program::computeImagesetCov() {
    image_cov.create(imageset_dim.x, imageset_dim.y);
    
    std::vector<std::vector<float>> cov(imageset_dim.x, std::vector<float>(imageset_dim.y, 0.));
    float max_cov = 0;
    
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            float r = 0., g = 0., b = 0.;
            for (int k = 0; k < imageset_size; k++) {
                sf::Color col = imageset[k].getPixel(i, j);
                sf::Color col_mean = image_mean.getPixel(i, j);
                
                r += float(col.r - col_mean.r)*float(col.r - col_mean.r);
                g += float(col.g - col_mean.g)*float(col.g - col_mean.g);
                b += float(col.b - col_mean.b)*float(col.b - col_mean.b);
            }
            r /= (float) imageset_size;
            g /= (float) imageset_size;
            b /= (float) imageset_size;
            
            cov[i][j] = sqrtf(r + g + b);
            if (cov[i][j] > max_cov)
                max_cov = cov[i][j];
        }
    }
    
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            cov[i][j] = cov[i][j] / max_cov * 255.;
            
            image_cov.setPixel(i, j, sf::Color((int) cov[i][j], (int) cov[i][j], (int) cov[i][j]));
        }
    }
}

void Program::updateSegmentationImage(int k) {
    texture_segmentation.update(dpestimator.evaluate(k, threshold));
}
