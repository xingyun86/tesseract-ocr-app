// tesseract-ocr-demo.cpp : Defines the entry point for the application.
//

#include "tesseract-ocr-demo.h"

#include <tesseract/baseapi.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif // _MSC_VER

void dealImg(const char* path);

int main(int argc, char** argv)
{
#ifdef _MSC_VER
    std::string root = argv[0];
    if (root.rfind('\\') != std::string::npos)
    {
        root = root.substr(0, root.rfind('\\'));
    }
    else
    {
        root = ".";
    }
    SetCurrentDirectoryA(root.c_str());
#endif // _MSC_VER

    std::string outText;
    std::string imPath = "sfzfm.jpg";
    //imPath = "test.png";
    imPath = "test.jpg";
    imPath = "bbb.png";
    imPath = "aaa.jpg";
    imPath = "zzz.jpg";
    imPath = "ttt.png";
    if (argc > 1)
    {
        imPath = argv[1];
    }
    //dealImg(imPath.c_str());

    /*{
        cv::Mat imag, result;
        imag = cv::imread(imPath, 0);	//将读入的彩色图像直接以灰度图像读入
        cv::namedWindow("原图", 1);
        cv::imshow("原图", imag);
        result = imag.clone();
        //进行二值化处理，选择30，200.0为阈值
        cv::threshold(imag, result, 30, 200.0, cv::THRESH_BINARY);
        cv::namedWindow("二值化图像");
        cv::imshow("二值化图像", result);
        cv::waitKey();
    }*/
    std::string languagePath = root + "\\tessdata";
    std::string languageType = "chi_sim+eng";
    //const char* languageType = "chi_sim";
    // Create Tesseract object
    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
    /*
      Initialize OCR engine to use English (eng) and The LSTM
      OCR engine.


      There are four OCR Engine Mode (oem) available

      OEM_TESSERACT_ONLY             Legacy engine only.
      OEM_LSTM_ONLY                  Neural nets LSTM engine only.
      OEM_TESSERACT_LSTM_COMBINED    Legacy + LSTM engines.
      OEM_DEFAULT                    Default, based on what is available.
     */
     // 初始化
    int nRet = ocr->Init(languagePath.c_str(), languageType.c_str(), tesseract::OEM_LSTM_ONLY);
    //int nRet = ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
    if (nRet != 0) {
        printf("TessBaseAPI init failed！");
        return -1;
    }

    // Set Page segmentation mode to PSM_AUTO (3)
    // Other important psm modes will be discussed in a future post.
    // 设置分割模式
    ocr->SetPageSegMode(tesseract::PSM_AUTO);

    // Open input image using OpenCV
    //cv::Mat im = cv::imread(imPath, cv::IMREAD_COLOR);

    // Set image data
    //ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
    cv::Mat colorImage, grayImage, binaryImage;
    {
        //将读入的彩色图像直接以灰度图像读入
        colorImage = cv::imread(imPath, cv::IMREAD_COLOR);
        //将读入的彩色图像直接以灰度图像读入
        //grayImage = cv::imread(imPath, cv::IMREAD_GRAYSCALE);
        //cv::namedWindow("原图", 1);
        //cv::imshow("原图", grayImage);
        //binaryImage = grayImage.clone();
        //进行二值化处理，选择30，200.0为阈值
        //cv::threshold(grayImage, binaryImage, 30, 200.0, cv::THRESH_OTSU | cv::THRESH_BINARY);
        //cv::namedWindow("二值化图像");
        //cv::imshow("二值化图像", binaryImage);
        //cv::waitKey();
         // Set image data
        ocr->SetImage(colorImage.data, colorImage.cols, colorImage.rows, 3, colorImage.step);
        //ocr->SetImage(grayImage.data, grayImage.cols, grayImage.rows, 3, grayImage.step);
        //ocr->SetImage(binaryImage.data, binaryImage.cols, binaryImage.rows, 3, binaryImage.step);
    }
    // Run Tesseract OCR on image
    outText = std::string(ocr->GetUTF8Text());

    // print recognized text
    std::cout << outText << std::endl;

    // Destroy used object and release memory
    ocr->End();
    delete ocr;

    return 0;
}

void dealImg(const char* path)
{
    cv::Mat src = cv::imread(path);
    // 结果图
    cv::Mat dst;
    // 显示原图
    cv::imshow("原图", src);

    cv::cvtColor(src, dst, cv::COLOR_RGB2GRAY);
    // 高斯模糊，主要用于降噪
    cv::GaussianBlur(dst, dst, cv::Size(3, 3), 0);
    cv::imshow("GaussianBlur图", dst);
    // 二值化图，主要将灰色部分转成白色，使内容为黑色
    cv::threshold(dst, dst, 165, 255, cv::THRESH_BINARY);
    cv::imshow("threshold图", dst);
    // 中值滤波，同样用于降噪
    cv::medianBlur(dst, dst, 3);
    cv::imshow("medianBlur图", dst);
    // 腐蚀操作，主要将内容部分向高亮部分腐蚀，使得内容连接，方便最终区域选取
    cv::erode(dst, dst, cv::Mat(9, 9, CV_8U));
    cv::imshow("erode图", dst);

    //定义变量
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dst, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat result;

    for (int i = 0; i < hierarchy.size(); i++)
    {
        cv::Rect rect = cv::boundingRect(contours.at(i));
        cv::rectangle(src, rect, cv::Scalar(255, 0, 255));
        // 定义身份证号位置大于图片的一半，并且宽度是高度的6倍以上
        if (rect.y > src.rows / 2 && rect.width / rect.height > 6)
        {
            result = src(rect);
            cv::imshow("身份证号", result);
        }
    }

    cv::imshow("轮廓图", src);
    cv::waitKey();
}