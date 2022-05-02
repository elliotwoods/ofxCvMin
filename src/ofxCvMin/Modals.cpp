#include "Modals.h"

namespace ofxCv {
	namespace Modals {
		void
			showImage(const cv::Mat& image
				, const std::string& windowTitle
				, const WindowProperties& windowProperties)
		{
			// shallow copy to start with
			auto renderImage = image;

			// pyramid down until we fit within the intended bounds
			while (renderImage.cols > windowProperties.maxWidth
				|| renderImage.rows > windowProperties.maxHeight) {
				cv::pyrDown(renderImage, renderImage);
			}

			if (windowProperties.normalizeColors) {
				cv::normalize(renderImage, renderImage, 0, 255, cv::NORM_MINMAX);
			}

			// show the window
			cv::imshow(windowTitle, renderImage);

			// wait for user to exit
			cv::waitKey(0);
			cv::destroyWindow(windowTitle);
		}
	}
}