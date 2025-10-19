// ConsoleApplication_Windowing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
int main()
{
    cv::Mat image(500, 500, CV_8UC1);
    image = 255;
    std::cout << "Hello World!\n";
    std::cout << "test";

    cv::imshow("Gray Image", image);  // display the image in a window
    cv::waitKey(0);                   // wait for a key press before closing

}
