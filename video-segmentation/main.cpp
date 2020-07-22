//
//  main.cpp
//  video-segmentation
//
//  Created by Stephen Jaud on 20/07/2020.
//  Copyright © 2020 Stephen Jaud. All rights reserved.
//

#include <string>
#include <iostream>

#include "Program.hpp"

int main(int argc, const char * argv[]) {
    std::string path;
    for (int i = 0; i < argc; i++)
        std::cout << argv[i] << std::endl;
        
    Program program;
    program.run();
    
    return 0;
}
