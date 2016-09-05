#include <stdio.h>

#include <FlyCapture2.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "TagDetector.h"


int init_camera(FlyCapture2::Camera &camera, double exposure, double gain)
{
    FlyCapture2::Error error;
    FlyCapture2::Property property;
    FlyCapture2::Property gain_property;


    // connect
    error = camera.Connect(0);
    if (error != FlyCapture2::PGRERROR_OK) {
        printf("ERROR! Failed to connect to camera!\n");
        return -1;
    } else {
        printf("Firefly camera connected!\n");
    }

    // configure exposure
    property.type = FlyCapture2::AUTO_EXPOSURE;
    property.onOff = true;
    property.autoManualMode = false;
    property.absControl = true;
    property.absValue = exposure;

    error = camera.SetProperty(&property);
    if (error != FlyCapture2::PGRERROR_OK) {
        printf("ERROR! Failed to configure camera exposure!\n");
        return -1;
    } else {
        printf("Firefly camera exposure configured!\n");
    }

    // configure gain
    property.type = FlyCapture2::GAIN;
    property.onOff = true;
    property.autoManualMode = false;
    property.absControl = true;
    property.absValue = gain;

    error = camera.SetProperty(&property);
    if (error != FlyCapture2::PGRERROR_OK) {
        printf("ERROR! Failed to configure camera exposure!\n");
        return -1;
    } else {
        printf("Firefly camera gain configured!\n");
    }

    // start camera
    error = camera.StartCapture();
    if (error != FlyCapture2::PGRERROR_OK) {
        printf("ERROR! Failed start camera!\n");
        return -1;
    } else {
        printf("Firefly initialized!\n");
    }

    return 0;
}

int get_frame(FlyCapture2::Camera &camera, cv::Mat &image)
{
    double data_size;
    double data_rows;
    unsigned int row_bytes;

    FlyCapture2::Image raw_img;
    FlyCapture2::Image rgb_img;
    FlyCapture2::Error error;

    error = camera.RetrieveBuffer(&raw_img);
    if (error != FlyCapture2::PGRERROR_OK) {
        printf("Video capture error!\n");
        return -1;
    }

    // convert to rgb
    raw_img.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgb_img);

    // convert to opencv mat
    data_size = rgb_img.GetReceivedDataSize();
    data_rows = rgb_img.GetRows();
    row_bytes = data_size / data_rows;
    cv::Mat(
        rgb_img.GetRows(),
        rgb_img.GetCols(),
        CV_8UC3,
        rgb_img.GetData(),
        row_bytes
    ).copyTo(image);

    return 0;
}

int main(void)
{
    int retval;

    TagFamily family("Tag16h5");
    TagDetectorParams params;
    TagDetector detector(family, params);
    FlyCapture2::Camera *camera;
    cv::Mat frame;

    // setup
    camera  = new FlyCapture2::Camera();
    retval = init_camera(*camera, 1.0, 10.0);
    if (retval != 0) {
        return -1;
    }

    while (1) {
        get_frame(*camera, frame);
        cv::imshow("test", frame);
        cv::waitKey(1);
    }


    return 0;
}
