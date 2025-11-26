// ConsoleApplication_Windowing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <limits>
#include <cstdint>
#include <filesystem>
#include <chrono>


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

// Aufgabe 42: 
void optimize_using_cv(const cv::Mat& source, cv::Mat& destination, int lower, int upper) {
    cv::Mat clipped;
    cv::threshold(source, clipped, upper, upper, cv::THRESH_TRUNC); // truncate values above upper: pixel > upper, it is set to upper
    cv::threshold(clipped, clipped, lower, 0, cv::THRESH_TOZERO); // pixel < lower, it is set to 0 

    // Scale values from [lower, upper] → [0, 255]
    /*
    * destination(x,y) = alpha x clipped(x,y) + beta
    * FORMULAR above:float scaled = (wmax - wmin)*((float)(g-g_lowerThreshold)/(g_upperThreshold-g_lowerThreshold)) + wmin;
    * => alpha = (wmax-wmin)/(g_upperThreshold - g_lowerThreshold)
    * => beta = -(wmax-wmin)*g_lowerThreshold/(g_upperThreshold - g_lowerThreshold) + wmin 
    */
    float alpha = 255.0 / (upper - lower);
    float beta = -255.0 * lower / (upper - lower);
    clipped.convertTo(destination, CV_8U, alpha, beta); 
}
void updateOutputImage(cv::Mat& src) {
    cv::Mat output;
    optimize_using_cv(src, output, g_lowerThreshold, g_upperThreshold);
    cv::imshow("Output image", output);
}
// Aufgabe 26, 31, 32:

static void onLowerThresholdTrackbar(int pos, void* userdata)
{
    cv::Mat* src = static_cast<cv::Mat*>(userdata); // cast void* back to Mat*
    if (pos >= g_upperThreshold) {
        g_lowerThreshold = g_upperThreshold - 1;
    }
    else {
        g_lowerThreshold = pos;
    }
    if (g_lowerThreshold < 0) g_lowerThreshold = 0;
    cv::setTrackbarPos("Lower threshold", "Output image", g_lowerThreshold);
    updateOutputImage(*src);
    std::cout << "Lower threshold: " << g_lowerThreshold << ", Upper threshold: " << g_upperThreshold << std::endl;
}
static void onUpperThresholdTrackbar(int pos, void* userdata)
{
    cv::Mat* src = static_cast<cv::Mat*>(userdata);
    if (pos <= g_lowerThreshold) {
        g_upperThreshold = g_lowerThreshold + 1;
    }
    else {
        g_upperThreshold = pos;
    }
    if (g_upperThreshold > upperLimit) g_upperThreshold = upperLimit;
    cv::setTrackbarPos("Upper threshold", "Output image", g_upperThreshold);
    updateOutputImage(*src);
    std::cout << "Lower threshold: " << g_lowerThreshold << ", Upper threshold: " << g_upperThreshold << std::endl;
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
    cv::createTrackbar("Lower threshold", "Output image", nullptr, upperLimit, onLowerThresholdTrackbar, &m51);
    cv::createTrackbar("Upper threshold", "Output image", nullptr, upperLimit, onUpperThresholdTrackbar, &m51);
    // Aufgabe 28:
    cv::setTrackbarPos("Lower threshold", "Output image", g_lowerThreshold);
    cv::setTrackbarPos("Upper threshold", "Output image", g_upperThreshold);

    // Aufgabe 34: 
    bool farbmodus = false;
    cv::Mat write_output_grau(m51.size(), CV_8U);
    cv::Mat write_output_farb(m51.size(), CV_8UC3);
    // Aufgabe 42
    bool autoMode = true;
    bool performanceTestMode = true;
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
            auto t1 = std::chrono::high_resolution_clock::now();
            cv::Mat output_aufgabe24(m51.size(), CV_8U);
            for (int y = 0; y < m51.rows; y++)
            {
                for (int x = 0; x < m51.cols; x++)
                {
                    output_aufgabe24.at<uint8_t>(y, x) = static_cast<uint8_t>(grauwertspreizung(m51.at<uint16_t>(y, x)));
                }
            }
            write_output_grau = output_aufgabe24;
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration_loop = t2 - t1;
            if(performanceTestMode) std::cout << "Loop version time: " << duration_loop.count() << "\n";
            cv::imshow("Output image", output_aufgabe24);


            // Aufgabe 42:
            cv::Mat output_optimized;
            auto t3 = std::chrono::high_resolution_clock::now();
            optimize_using_cv(m51, output_optimized, g_lowerThreshold, g_upperThreshold);
            auto t4 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration_optimized = t4 - t3;
            if(performanceTestMode) std::cout << "optimized version time: " << duration_optimized.count() << "\n";
            cv::imshow("Output image", output_optimized);

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
        else if (key == 't')
        {
            performanceTestMode = !performanceTestMode;
            std::cout << (performanceTestMode? "Performance Test mode resumed." : "Performance Test mode paused.") << std::endl;
        }
    
    }
    /*
    * Aufgabe 41:
    * a. 32454 - 32903
    * b. Medizin, CT, MRI, ...
    * 
    * Aufgabe 42:
    * OpenCV optimized alternative:
    * cv::threshold() statt 2 geschachtelten Schleifen
    * 
    * Aufgabe 43:
    * Loop version time ~ 9ms/jedes mal ist upper/lower geändert 
    * Optimized version time ~ 3ms/jedes mal ist upper/lower geändert 
    * => 3 mal schneller
    */
    
}
