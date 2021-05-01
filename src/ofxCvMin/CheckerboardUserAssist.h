#pragma once
#include "opencv2/opencv.hpp"

#include "Helpers.h"

#include "ofMain.h"

namespace ofxCv {
	bool findBoardWithAssistant(cv::Mat image
		, BoardType
		, cv::Size patternSize
		, vector<cv::Point2f> & results);

	bool findBoardWithAssistant(cv::Mat image
		, std::function<bool(cv::Mat, vector<cv::Point2f>&)> findFunction
		, vector<cv::Point2f>& results);

	bool selectROI(cv::Mat image, cv::Rect& roi);
}