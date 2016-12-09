#pragma once
#include "opencv2/opencv.hpp"

#include "Helpers.h"

#include "ofMain.h"

namespace ofxCv {
	bool findBoardWithAssistant(cv::Mat image, BoardType, cv::Size patternSize, vector<cv::Point2f> & results);
}