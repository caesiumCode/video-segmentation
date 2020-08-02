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
    // Get the input path
    std::string inputPath = "";
    for (int i = 0; i < argc; i++)
        if (argc > i+1 && std::strcmp(argv[i], "-i") == 0)
            inputPath = std::string(argv[i+1]);
    
    // If input path is given we run the program
    if (inputPath.compare("") != 0) {
        Program program(inputPath);
        program.run();
    } else
        std::cout << "ERROR: input path not given\n";
    
    return 0;
}
