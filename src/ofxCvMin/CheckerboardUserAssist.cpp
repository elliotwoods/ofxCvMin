#include "CheckerboardUserAssist.h"
#include "Wrappers.h"

using namespace cv;

struct AssistState {
	string windowName;
	bool mouseDown = false;
	cv::Size imageSize;
	cv::Rect roi;
	cv::Mat mask;
	cv::Mat image;
	cv::Mat preview;
	int windowScale = 1;
	int blockSize = 100;
};

static void redraw(AssistState & assistState) {
	assistState.mask = 0;
	assistState.mask(assistState.roi) = 1;
	assistState.preview = 0;
	assistState.image.copyTo(assistState.preview, assistState.mask);

	try {
		auto midValue = cv::mean(assistState.preview(assistState.roi));
		cv::threshold(assistState.preview, assistState.preview, midValue[0], 255, THRESH_BINARY);
	}
	catch (...) {

	}
	
	//cv::adaptiveThreshold(assistState.preview, assistState.preview, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, assistState.blockSize * 2 + 1, 0.0);
	imshow(assistState.windowName, assistState.preview);
}

static void onMouse(int event, int x, int y, int, void * assistStateRaw)
{
	auto & assistState = * static_cast<AssistState*>(assistStateRaw);

	//dragging
	if (assistState.mouseDown) {
		auto oldRoi = assistState.roi;
		try {
			auto oldRoi = assistState.roi;
			assistState.roi.width = x - assistState.roi.x;
			assistState.roi.height = y - assistState.roi.y;

			redraw(assistState);
		}
		catch (...) {
			assistState.roi = oldRoi;
		}
	}

	switch (event) {
	case EVENT_LBUTTONDOWN:
		assistState.mouseDown = true;
		assistState.roi.x = x;
		assistState.roi.y = y;
		assistState.roi.width = 1;
		assistState.roi.height = 1;
		break;
	case EVENT_LBUTTONUP:
		assistState.mouseDown = false;
		break;
	default:
		break;
	}	
}

static void onTrackbar(int value, void * assistStateRaw) {
	auto & assistState = *static_cast<AssistState*>(assistStateRaw);
	//redraw(assistState);
}

namespace ofxCv {
	using namespace cv;

	bool findBoardWithAssistant(cv::Mat image, BoardType boardType, cv::Size patternSize, vector<cv::Point2f> & results)
	{
		auto success = findBoardWithAssistant(image, [&boardType, &patternSize, &results](cv::Mat image, vector<cv::Point2f>& results) {
			return ofxCv::findBoard(image
				, boardType
				, patternSize
				, results
				, false);
			}, results);

		if (success && boardType == BoardType::Checkerboard) {
			refineCheckerboardCorners(image
				, patternSize
				, results);
		}
		return success;
	}

	bool findBoardWithAssistant(cv::Mat image, std::function<bool(cv::Mat, vector<cv::Point2f>&)> findFunction, vector<cv::Point2f> & results) {
		cv::Rect roi;
		if (!selectROI(image, roi)) {
			return false;
		}

		auto selectedImagePortion = image(roi);

		auto midValue = cv::mean(image(roi));
		cv::Mat thresholded;
		cv::threshold(image, thresholded, midValue[0], 255, THRESH_BINARY);

		auto success = findFunction(selectedImagePortion, results);

		if (!success) {
			//try without threshold applied
			selectedImagePortion = image(roi);
			success = findFunction(selectedImagePortion, results);
		}

		if (success) {
			for (auto& result : results) {
				result.x += roi.x;
				result.y += roi.y;
			}
		}

		return success;
	}

	bool selectROI(cv::Mat image, cv::Rect& roi) {
		AssistState assistState;

		assistState.windowName = "ROI Assistant";
		assistState.imageSize = image.size();
		assistState.image = image;
		assistState.mask = cv::Mat::ones(image.size(), CV_8U);
		assistState.preview = image.clone();
		assistState.blockSize = image.size().width / 10;

		while (assistState.imageSize.height / assistState.windowScale > 1024) {
			assistState.windowScale *= 2;
		}

		namedWindow(assistState.windowName, WINDOW_NORMAL);
		resizeWindow(assistState.windowName, assistState.imageSize.width / assistState.windowScale, assistState.imageSize.height / assistState.windowScale);
		setMouseCallback(assistState.windowName, onMouse, &assistState);
		imshow(assistState.windowName, image);

		bool success = false;

		auto key = waitKey(0);
		cv::destroyWindow(assistState.windowName);
		roi = assistState.roi;
		if (roi.empty()) {
			roi.x = 0;
			roi.y = 0;
			roi.width = image.cols;
			roi.height = image.rows;
		}

		if (key == OF_KEY_ESC) {
			return false;
		}
		else {
			return true;
		}
	}
}