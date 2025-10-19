// ConsoleApplication_Windowing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <limits>
#include <cstdint>

int main()
{
    cv::Mat image(500, 500, CV_8UC1);
    image = 255;

    cv::Mat m51 = cv::imread("D:\\FH Aachen\\Sem 5\\Bildverarbeitung\\Praktikum\\ConsoleApplication_Windowing\\m51.tif", cv::IMREAD_GRAYSCALE);
    /*
    * Aufgabe 10: 
    * a. 320x510
    * 1 x UINT8 ~ CV_8U
    * b. nur schwarz (cv::IMREAD_UNCHANGED)
    * c. nur schwarz (cv::IMREAD_GRAYSCALE)
    * d. Nach der Normalisierung kann man die Sterne sehen
    */
    cv::Mat normalisiert;
    cv::normalize(m51, normalisiert, 0, 255, cv::NORM_MINMAX);
    normalisiert.convertTo(normalisiert, CV_8U);

    cv::imshow("normalized image", normalisiert);
    cv::imshow("original image", m51);
    /*
    * Aufgabe 11:
    * CV Depth	Code returned by .depth()
        CV_8U	0
        CV_8S	1
        CV_16U	2
        CV_16S	3
        CV_32S	4
        CV_32F	5
        CV_64F	6
    */
    std::cout << m51.depth() << std::endl; //0 -> CV_8U
    std::cout << normalisiert.depth() << std::endl; //0 -> CV_8U

    // Aufgabe 12:
    unsigned int upperLimit = std::numeric_limits<uint8_t>::max(); //2^8-1 = 255
    std::cout << "Upper limit: " << upperLimit << std::endl; // 255

    cv::waitKey(0);                   
    
}
