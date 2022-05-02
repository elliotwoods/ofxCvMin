#pragma once
/*
 Modals are pop up windows that allow the user to perform functions.
 Note : this is the first 'refactoring' of the original ofxCv layout
 */

#pragma once

#include "ofMain.h"
#include "opencv2/opencv.hpp"
#include <glm/glm.hpp>

namespace ofxCv {
	namespace Modals {
		struct WindowProperties {
			int maxWidth = ofGetScreenWidth();
			int maxHeight = ofGetScreenHeight();
			bool normalizeColors = false;
		};

		void showImage(const cv::Mat& image
			, const std::string& windowTitle
			, const WindowProperties& windowProperties = WindowProperties());
	}
}