#include "CheckerboardUserAssist.h"

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

	auto midValue = cv::mean(assistState.preview(assistState.roi));
	cv::threshold(assistState.preview, assistState.preview, midValue[0], 255, THRESH_BINARY);
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
		if (boardType != BoardType::Checkerboard) {
			ofLogError("ofxCvMin") << "findBoardWithAssist is only supported for checkerboards.";
			return false;
		}
		AssistState assistState;

		assistState.windowName = "Board finder assistant";
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
		setMouseCallback("Board finder assistant", onMouse, &assistState);
		imshow("Board finder assistant", image);
		
		//createTrackbar("blockSize", assistState.windowName, &assistState.blockSize, image.size().width, onTrackbar, &assistState);



		waitKey(0);

		auto selectedImagePortion = image(assistState.roi);

		//cv::adaptiveThreshold(selectedImagePortion, selectedImagePortion, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, assistState.blockSize * 2 + 1, 0.0);

		auto midValue = cv::mean(assistState.preview(assistState.roi));
		cv::threshold(assistState.preview, assistState.preview, midValue[0], 255, THRESH_BINARY);

		
		auto success = cv::findChessboardCorners(selectedImagePortion, patternSize, results, CV_CALIB_CB_NORMALIZE_IMAGE);
		if (!success) {
			//try without threshold applied
			selectedImagePortion = image(assistState.roi);
			success = cv::findChessboardCorners(selectedImagePortion, patternSize, results);
		}

		if (success) {
			for (auto & result : results) {
				result.x += assistState.roi.x;
				result.y += assistState.roi.y;
			}

			auto subPixResults = results;
			cv::cornerSubPix(image, subPixResults, Size(5, 5), Size(1, 1), TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 50, 1e-5));
			if (results.size() == subPixResults.size()) {
				results = subPixResults;
			}
		}

		cv::destroyWindow(assistState.windowName);

		return success;
	}
}