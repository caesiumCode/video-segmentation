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
    
    computeImagesetVar();
    texture_var.loadFromImage(image_var);
    sprite_var.setTexture(texture_var);
    
    // Setup sprites
    display_mode = 0;
    window_scale = getWindowScale(imageset_dim);
    sprite.scale(window_scale, window_scale);
    sprite_mean.scale(window_scale, window_scale);
    sprite_var.scale(window_scale, window_scale);
    
    // Setup window
    window.create(sf::VideoMode(window_scale*imageset_dim.x, window_scale*imageset_dim.y), TITLE);
    window.setFramerateLimit(30);
    
    // Setup thresholds
    log_threshold = -10.;
    delta_log_threshold = 0.;
    threshold = expf(log_threshold);
    threshold2 = expf(log_threshold + delta_log_threshold);
    
    // Compute segmentation mask
    std::cout << "INFO: Running segmentation algorithm - Maximum likelihood Estimator with normal distribution\n";
    dpestimator_mle.fit(imageset, "mle");
    
    std::cout << "INFO: Running segmentation algorithm - Kernel density Estimator with normal kernel\n";
    dpestimator_kde.fit(imageset, "kde");
    
    std::cout << "INFO: Extract segmentation mask\n";
    mask_mode = "mle";
    if (mask_mode.compare("mle") == 0)
        texture_segmentation.loadFromImage(dpestimator_mle.evaluate(imageset_index, threshold, threshold2));
    else if (mask_mode.compare("kde") == 0)
        texture_segmentation.loadFromImage(dpestimator_kde.evaluate(imageset_index, threshold, threshold2));
    else {
        std::cout << "ERROR: Unknown mask mode: " << mask_mode << std::endl;
    }
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
                window.draw(sprite_var);
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
                
                updateSegmentationImage();
                break;
            
            case sf::Keyboard::Left:
                imageset_index--;
                if (imageset_index < 0)
                    imageset_index = imageset_size - 1;
                image.loadFromFile(imageset[imageset_index]);
                texture.update(image);
                
                updateSegmentationImage();
                break;
                
            case sf::Keyboard::Up:
                updateThreshold(+1, 0);
                break;
                
            case sf::Keyboard::Down:
                updateThreshold(-1, 0);
                break;
                
            case sf::Keyboard::Numpad0:
                display_mode = 0;
                break;
                
            case sf::Keyboard::Num0:
                display_mode = 0;
                break;
                
            case sf::Keyboard::Numpad1:
                display_mode = 1;
                break;
                
            case sf::Keyboard::Num1:
                display_mode = 1;
                break;
                
            case sf::Keyboard::Numpad2:
                display_mode = 2;
                break;
                
            case sf::Keyboard::Num2:
                display_mode = 2;
                break;
                
            case sf::Keyboard::M:
                mask_mode = "mle";
                updateSegmentationImage();
                break;
                
            case sf::Keyboard::K:
                mask_mode = "kde";
                updateSegmentationImage();
                break;
                
            case sf::Keyboard::A:
                updateThreshold(0, +1);
                break;
                
            case sf::Keyboard::Q:
                updateThreshold(0, -1);
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
    std::cout << "INFO: Compute mean image\n";
    
    image_mean.create(imageset_dim.x, imageset_dim.y);
    std::vector<std::vector<Vector3>> temp_mean(imageset_dim.x, std::vector<Vector3>(imageset_dim.y, Vector3::Zeros()));
    
    // Sum all the pixels
    for (int k = 0; k < imageset_size; k++) {
        sf::Image image_temp;
        image_temp.loadFromFile(imageset[k]);
        
        for (int i = 0; i < imageset_dim.x; i++) {
            for (int j = 0; j < imageset_dim.y; j++) {
                temp_mean[i][j] = temp_mean[i][j] + Vector3(image_temp.getPixel(i, j));
            }
        }
    }
    
    // Compute the actual mean
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            image_mean.setPixel(i, j, (1./float(imageset_size) * temp_mean[i][j]).toColor());
        }
    }
}

void Program::computeImagesetVar() {
    std::cout << "INFO: Compute variance image\n";
    
    image_var.create(imageset_dim.x, imageset_dim.y);
    
    std::vector<std::vector<float>> var(imageset_dim.x, std::vector<float>(imageset_dim.y, 0.));
    float max_cov = 0;
    
    // Sum all variance
    for (int k = 0; k < imageset_size; k++) {
        sf::Image image_temp;
        image_temp.loadFromFile(imageset[k]);
        
        for (int i = 0; i < imageset_dim.x; i++) {
            for (int j = 0; j < imageset_dim.y; j++) {
                sf::Color col = image_temp.getPixel(i, j);
                sf::Color col_mean = image_mean.getPixel(i, j);
                
                var[i][j] += float(col.r - col_mean.r)*float(col.r - col_mean.r) +
                             float(col.g - col_mean.g)*float(col.g - col_mean.g) +
                             float(col.b - col_mean.b)*float(col.b - col_mean.b);
            }
        }
    }
    
    // Find the maximum value
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            var[i][j] = sqrtf(var[i][j]);
            if (var[i][j] > max_cov)
                max_cov = var[i][j];
        }
    }
    
    // Normalize the variances to fit into [0, 255]
    for (int i = 0; i < imageset_dim.x; i++) {
        for (int j = 0; j < imageset_dim.y; j++) {
            float val = var[i][j] / float(max_cov) * 255.;
            image_var.setPixel(i, j, sf::Color((int) val, (int) val, (int) val));
        }
    }
}

void Program::updateThreshold(int s1, int s2) {
    log_threshold += s1;
    if (s2 > 0 || delta_log_threshold > 0)
        delta_log_threshold += s2;
    
    threshold = expf(log_threshold);
    threshold2 = expf(log_threshold + delta_log_threshold);
    
    updateSegmentationImage();
}

void Program::updateSegmentationImage() {
    if (mask_mode.compare("mle") == 0)
        texture_segmentation.update(dpestimator_mle.evaluate(imageset_index, threshold, threshold2));
    else if (mask_mode.compare("kde") == 0)
        texture_segmentation.update(dpestimator_kde.evaluate(imageset_index, threshold, threshold2));
    else
        std::cout << "ERROR: Unknown mask mode: " << mask_mode << std::endl;
}
