#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

// Function to find and draw contours
cv::Mat processImage(const cv::Mat& img) {
    cv::Mat result = img.clone();
    cv::Mat gray, binary;
    
    // Convert to grayscale
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    
    // Apply threshold to get binary image
    cv::threshold(gray, binary, 128, 255, cv::THRESH_BINARY);
    
    // Find all contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    
    if (contours.empty()) return result;
    
    // Find the largest contour (outer ring)
    int outerIdx = 0;
    double maxArea = 0;
    for (int i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea) {
            maxArea = area;
            outerIdx = i;
        }
    }
    
    // Draw outer ring contour in BLUE
    cv::drawContours(result, contours, outerIdx, cv::Scalar(255, 0, 0), 2);
    
    // Find the inner rectangular contour (small opening near "nose")
    // Look for small contours with rectangular shape
    int innerIdx = -1;
    double targetArea = maxArea * 0.01; // Inner opening is much smaller
    double minDiff = maxArea;
    
    for (int i = 0; i < contours.size(); i++) {
        if (i == outerIdx) continue;
        
        double area = cv::contourArea(contours[i]);
        
        // Filter by size: should be small but not too tiny
        if (area > 100 && area < maxArea * 0.1) {
            // Check if contour is somewhat rectangular using bounding rect ratio
            cv::RotatedRect rect = cv::minAreaRect(contours[i]);
            double rectArea = rect.size.width * rect.size.height;
            double rectangularity = area / rectArea; // How rectangular (closer to 1 = more rectangular)
            
            // Prefer more rectangular shapes
            if (rectangularity > 0.5) {
                double diff = std::abs(area - targetArea);
                if (diff < minDiff || innerIdx == -1) {
                    minDiff = diff;
                    innerIdx = i;
                }
            }
        }
    }
    
    // Draw inner rectangular contour in GREEN
    if (innerIdx >= 0) {
        cv::drawContours(result, contours, innerIdx, cv::Scalar(0, 255, 0), 2);
    }
    
    return result;
}

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

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
        images.push_back(img);
        std::cout << "Loaded: " << name << std::endl;
    }
    std::cout << "\nLoaded " << images.size() << " images successfully." << std::endl;
    std::cout << "Press 'n' for next image, 'p' for previous image, 'q' or ESC to quit." << std::endl;
   
    std::string windowName = "P4 - Ring Detection";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    
    int currentIndex = 0;
    
    while (true) {
        cv::setWindowTitle(windowName, windowName + " - " + imageNames[currentIndex]);
        
        // Process image and show with contours
        cv::Mat processed = processImage(images[currentIndex]);
        cv::imshow(windowName, processed);
        
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
