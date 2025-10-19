// ConsoleApplication_Windowing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <limits>
#include <cstdint>

// Aufgabe 17:
int g_lowerThreshold = 0;
int g_upperThreshold;

// Aufgabe 19:
// lineare Interpolation
float grauwertspreizung(int g, float wmax=255.0, float wmin=0.0) {
    float scaled = (wmax - wmin)*((g-g_lowerThreshold)/(g_upperThreshold-g_lowerThreshold)) + wmin;
    return scaled;
}

int main()
{
    // Aufgabe 15: 
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    cv::Mat m51 = cv::imread("D:\\FH Aachen\\Sem 5\\Bildverarbeitung\\Praktikum\\ConsoleApplication_Windowing\\m51.tif", cv::IMREAD_UNCHANGED);
    /*
    * Aufgabe 10: 
    * a. 320x510
    * 1 x UINT16 ~ CV_16U
    * b. nur schwarz (cv::IMREAD_UNCHANGED)
    * c. nur schwarz (cv::IMREAD_GRAYSCALE)
    * d. Nach der Normalisierung kann man die Sterne sehen
    */
    std::cout << "Bildgroesse: " << m51.cols << "x" << m51.rows << std::endl;
    cv::Mat normalisiert;
    cv::normalize(m51, normalisiert, 0, 65535, cv::NORM_MINMAX);
    normalisiert.convertTo(normalisiert, CV_16U);

    //cv::imshow("normalized image", normalisiert);
    //cv::imshow("original image", m51);

    
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
    std::cout << m51.depth() << std::endl; //2 -> CV_16U
    std::cout << normalisiert.depth() << std::endl; //2 -> CV_16U

    // Aufgabe 12:
    unsigned int upperLimit = std::numeric_limits<uint16_t>::max(); //2^16-1 = 65535
    std::cout << "Upper limit: " << upperLimit << std::endl; // 65535

    // Aufgabe 14:
    cv::imshow("Input image", normalisiert);

    // Aufgabe 18:
    g_upperThreshold = upperLimit / 16;

    // Aufgabe 19: 
    cv::Mat output(m51.size(), CV_8U); //output is 8bits instead of 16

    
    for (int y = 0; y < m51.rows; y++) {
        for (int x = 0; x < m51.cols; x++) {
            uint16_t pixel = m51.at<uint16_t>(y, x);
            if (pixel < g_lowerThreshold) {
                output.at<uint8_t>(y, x) = 0;
            }
            else if (pixel > g_upperThreshold) {
                output.at<uint8_t>(y, x) = 255;
            }
            else {
                // c. lineare Interpolation und Rundung
                float scaled = grauwertspreizung(pixel);
                output.at<uint8_t>(y, x) = static_cast<uint8_t>(scaled);
            }
        }
    }
    cv::imshow("Output image", output);

    /*
    * Aufgabe 21:
    * a. Input: Sterne können schwach sein aber Hintergrundinformation bleibt 
    * b. Output: Sterne oberhalb Threshold leuchen deutlicher. Der Hintergrund wurde ünterdrück.
    */ 

    cv::waitKey(0);                   
    
}
