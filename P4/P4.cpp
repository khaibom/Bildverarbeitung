

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

int main()
{
    std::vector<std::string> imageNames = {
        "stellung0.bmp",
        "stellung0a.bmp",
        "stellung1.bmp",
        "stellung2.bmp",
        "stellung2a.bmp",
        "stellung3.bmp",
        "stellung4.bmp",
        "stellung5.bmp",
        "stellung6.bmp"
    };
    
    std::string basePath = "images/";
    
    std::vector<cv::Mat> images;
    for (const auto& name : imageNames) {
        cv::Mat img = cv::imread(basePath + name);
        if (img.empty()) {
            std::cerr << "Error: Could not load image " << name << std::endl;
            return -1;
        }
        images.push_back(img);
        std::cout << "Loaded: " << name << std::endl;
    }
    
    std::cout << "\nLoaded " << images.size() << " images successfully." << std::endl;
    std::cout << "Press 'n' for next image, 'p' for previous image, 'q' or ESC to quit." << std::endl;
    
    // Display window
    std::string windowName = "P4 - Ring Detection";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    
    int currentIndex = 0;
    
    while (true) {
        cv::setWindowTitle(windowName, windowName + " - " + imageNames[currentIndex]);
        cv::imshow(windowName, images[currentIndex]);
        int key = cv::waitKey(0);
        
        if (key == 'n' || key == 'N') {
            currentIndex = (currentIndex + 1) % images.size();
        }
        else if (key == 'p' || key == 'P') {
            currentIndex = (currentIndex - 1 + images.size()) % images.size();
        }
        else if (key == 'q' || key == 'Q' || key == 27) {
            break;
        }
    }
    
    cv::destroyAllWindows();
    return 0;
}
