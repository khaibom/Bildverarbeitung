#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

// Calculate centroid of a contour using moments
cv::Point calcCentroid(const std::vector<cv::Point>& contour) {
    cv::Moments m = cv::moments(contour);
    if (m.m00 == 0) return cv::Point(-1, -1);
    return cv::Point(static_cast<int>(m.m10 / m.m00), static_cast<int>(m.m01 / m.m00));
}

// Function to find contours, calculate centroids, and visualize
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
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea) {
            maxArea = area;
            outerIdx = static_cast<int>(i);
        }
    }
    
    // Draw outer ring contour in BLUE
    cv::drawContours(result, contours, outerIdx, cv::Scalar(255, 0, 0), 2);
    
    // Calculate centroid of outer ring (Task 5a: position of object)
    cv::Point outerCentroid = calcCentroid(contours[outerIdx]);
    
    // Find the inner rectangular contour (small opening near "nose")
    int innerIdx = -1;
    double targetArea = maxArea * 0.01;
    double minDiff = maxArea;
    
    for (size_t i = 0; i < contours.size(); i++) {
        if (static_cast<int>(i) == outerIdx) continue;
        
        double area = cv::contourArea(contours[i]);
        
        // Filter by size: should be small but not too tiny
        if (area > 100 && area < maxArea * 0.1) {
            cv::RotatedRect rect = cv::minAreaRect(contours[i]);
            double rectArea = rect.size.width * rect.size.height;
            double rectangularity = area / rectArea;
            
            if (rectangularity > 0.5) {
                double diff = std::abs(area - targetArea);
                if (diff < minDiff || innerIdx == -1) {
                    minDiff = diff;
                    innerIdx = static_cast<int>(i);
                }
            }
        }
    }
    
    // Draw inner rectangular contour in GREEN and calculate centroid
    cv::Point innerCentroid(-1, -1);
    if (innerIdx >= 0) {
        cv::drawContours(result, contours, innerIdx, cv::Scalar(0, 255, 0), 2);
        // Calculate centroid of inner contour (Task 5b: orientation reference)
        innerCentroid = calcCentroid(contours[innerIdx]);
    }
    
    // Task 6: Draw RED connection line between both centroids
    if (outerCentroid.x >= 0 && innerCentroid.x >= 0) {
        cv::line(result, outerCentroid, innerCentroid, cv::Scalar(0, 0, 255), 2);
        
        // Draw centroid markers for better visibility
        cv::circle(result, outerCentroid, 5, cv::Scalar(255, 0, 0), -1);  // Blue filled circle
        cv::circle(result, innerCentroid, 5, cv::Scalar(0, 255, 0), -1);  // Green filled circle
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
