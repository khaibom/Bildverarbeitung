// ConsoleApplication_Windowing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <limits>
#include <cstdint>
#include <filesystem>


// Aufgabe 12:
int upperLimit = std::numeric_limits<uint16_t>::max(); //2^16-1 = 65535

// Aufgabe 17:
int g_lowerThreshold = 0;
int g_upperThreshold;

// Aufgabe 19:
// lineare Interpolation
float grauwertspreizung(int g, float wmax=255.0, float wmin=0.0) {
    //if (g_upperThreshold == g_lowerThreshold) {
    //    return wmin;
    //}
    if (g <= g_lowerThreshold) return wmin;
    if (g >= g_upperThreshold) return wmax;
    float scaled = (wmax - wmin)*((float)(g-g_lowerThreshold)/(g_upperThreshold-g_lowerThreshold)) + wmin;
    return scaled;
}

// Aufgabe 26, 31, 32:
static void onLowerThresholdTrackbar(int pos, void*);
static void onUpperThresholdTrackbar(int pos, void*);

static void onLowerThresholdTrackbar(int pos, void*)
{
    if (pos == 65535) pos = 65534;
    if (pos < g_upperThreshold) {
        g_lowerThreshold = pos;
        std::cout << "new lower threshold : " << g_lowerThreshold << " upper threshold : " << g_upperThreshold << std::endl;
    }
    else {
        g_lowerThreshold = pos;
        int newupper = pos + 1;
        onUpperThresholdTrackbar(newupper, nullptr);
        cv::setTrackbarPos("Upper threshold", "Output image", g_upperThreshold);
        if (g_lowerThreshold < 0) g_lowerThreshold = 0;
        std::cout << "new lower threshold : " << g_lowerThreshold << " new upper threshold : " << g_upperThreshold << std::endl;
    }
}
static void onUpperThresholdTrackbar(int pos, void*)
{
    if (pos == 0) pos = 1;
    if (pos > g_lowerThreshold) {
        g_upperThreshold = pos;
        std::cout << "lower threshold : " << g_lowerThreshold << " new upper threshold : " << g_upperThreshold << std::endl;
    }
    else {
        g_upperThreshold = pos;
        int newlower = pos - 1;
        onLowerThresholdTrackbar(newlower, nullptr);
        cv::setTrackbarPos("Lower threshold", "Output image", g_lowerThreshold);
        if (g_upperThreshold > upperLimit) g_upperThreshold = upperLimit;
        std::cout << "new lower threshold : " << g_lowerThreshold << " new upper threshold : " << g_upperThreshold << std::endl;
    }
}

int main()
{
    // Aufgabe 15: 
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    //std::string inputPath = "D:\\FH Aachen\\Sem 5\\Bildverarbeitung\\Praktikum\\ConsoleApplication_Windowing\\m51.tif";
    std::string inputPath = "D:\\FH Aachen\\Sem 5\\Bildverarbeitung\\Praktikum\\ConsoleApplication_Windowing\\ct.dcm.tif";
    cv::Mat m51 = cv::imread(inputPath, cv::IMREAD_UNCHANGED);
    //std::cout << "Trying to load: " << inputPath << std::endl;
    //if (m51.empty()) {
    //    std::cerr << "Error: Could not open or find the image at " << inputPath << std::endl;
    //    return -1;
    //}
    std::filesystem::path p(inputPath);
    std::string baseName = p.stem().string();
    std::cout << baseName;
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
    // unsigned int upperLimit = std::numeric_limits<uint16_t>::max(); //2^16-1 = 65535
    std::cout << "Upper limit: " << upperLimit << std::endl; // 65535

    // Aufgabe 14:
    //cv::imshow("Input image", normalisiert);

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

    // Aufgabe 23:
    cv::namedWindow("Output image", cv::WINDOW_AUTOSIZE);
    //cv::createTrackbar("Lower threshold", "Output image", &g_lowerThreshold, upperLimit);
    //cv::createTrackbar("Upper threshold", "Output image", &g_upperThreshold, upperLimit);

    // Aufgabe 27:
    cv::createTrackbar("Lower threshold", "Output image", nullptr, upperLimit, onLowerThresholdTrackbar);
    cv::createTrackbar("Upper threshold", "Output image", nullptr, upperLimit, onUpperThresholdTrackbar);
    // Aufgabe 28:
    cv::setTrackbarPos("Lower threshold", "Output image", g_lowerThreshold);
    cv::setTrackbarPos("Upper threshold", "Output image", g_upperThreshold);

    // Aufgabe 34: 
    bool farbmodus = false;
    cv::Mat write_output_grau(m51.size(), CV_8U);
    cv::Mat write_output_farb(m51.size(), CV_8UC3);
    // Aufgabe 42
    bool autoMode = true;
    // Aufgabe 24: Interaktive Schleife
    while (true)
    {
        if (autoMode)
        {
            g_lowerThreshold += 250;
            if (g_lowerThreshold >= g_upperThreshold)
            {
                g_lowerThreshold = 0;
                g_upperThreshold += 500;
                if (g_upperThreshold >= upperLimit)
                {
                    g_upperThreshold = upperLimit;
                    autoMode = false; // stop when reaching max
                    std::cout << "Reached upper limit. Auto mode stopped.\n";
                }
            }

            // Update trackbars visually
            cv::setTrackbarPos("Lower threshold", "Output image", g_lowerThreshold);
            cv::setTrackbarPos("Upper threshold", "Output image", g_upperThreshold);
            std::cout << "Auto step -> lower: " << g_lowerThreshold << ", upper: " << g_upperThreshold << std::endl;
        }
        if (!farbmodus) {
            cv::Mat output_aufgabe24(m51.size(), CV_8U);
            for (int y = 0; y < m51.rows; y++)
            {
                for (int x = 0; x < m51.cols; x++)
                {
                    output_aufgabe24.at<uint8_t>(y, x) = static_cast<uint8_t>(grauwertspreizung(m51.at<uint16_t>(y, x)));
                }
            }
            write_output_grau = output_aufgabe24;
            cv::imshow("Aufgabe 24/33", output_aufgabe24);
        }
        else { // Aufgabe 33: BGR
            cv::Mat output_color(m51.size(), CV_8UC3);
            for (int y = 0; y < m51.rows; y++)
            {
                for (int x = 0; x < m51.cols; x++)
                {
                    uint16_t pixel = m51.at<uint16_t>(y, x);
                    if (pixel < g_lowerThreshold) { //Blau
                        output_color.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 0, 0);
                    }
                    else if (pixel > g_upperThreshold) { //Rot
                        output_color.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
                    }
                    else {
                        uint8_t grau = static_cast<uint8_t>(grauwertspreizung(pixel));
                        output_color.at<cv::Vec3b>(y, x) = cv::Vec3b(grau, grau, grau);
                    }
                }
            }
            write_output_farb = output_color;
            cv::imshow("Aufgabe 24/33", output_color);
        }

        char key = (char)cv::waitKey(10);
        if (key == 27) break; //ESC
        else if (key == 'm' || key == 'M')
        {
            farbmodus = !farbmodus; // Wechsel zwischen 0 und 1
            std::cout << "Mode switched to " << (farbmodus ? "Farbmodus" : "Grauwert") << std::endl;
        }
        else if (key == 's' || key == 'S') { // Aufgabe 36
            std::string modus = farbmodus ? "farbe" : "grau";
            std::string filename = baseName + "_" + modus + "_lowerTH" + std::to_string(g_lowerThreshold) + "_upperTH" + std::to_string(g_upperThreshold) + ".png";
            cv::imwrite(filename, farbmodus ? write_output_farb : write_output_grau);
            std::cout << "Bild gespeichert: " << filename << std::endl;
        }
        else if (key == ' ')
        {
            autoMode = !autoMode; // SPACE -> pause/resume
            std::cout << (autoMode ? "Auto mode resumed." : "Auto mode paused.") << std::endl;
        }

    }
    /*
    * Aufgabe 41:
    * a. 32454 - 32903
    * b. Medizin, CT, MRI, ...
    * 
    * Aufgabe 42:
    * automatisch inkrementiert lower und upper Threshold
    */
    
    

}
