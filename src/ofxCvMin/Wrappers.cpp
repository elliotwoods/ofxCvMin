#include "Wrappers.h"

namespace ofxCv {

	using namespace cv;

	void loadMat(Mat& mat, string filename) {
		FileStorage fs(ofToDataPath(filename), FileStorage::READ);
		fs["Mat"] >> mat;
	}

	void saveMat(Mat mat, string filename) {
		FileStorage fs(ofToDataPath(filename), FileStorage::WRITE);
		fs << "Mat" << mat;
	}

	void saveImage(Mat& mat, string filename) {
		if (mat.depth() == CV_8U) {
			ofPixels pix8u;
			toOf(mat, pix8u);
			ofSaveImage(pix8u, filename);
		}
		else if (mat.depth() == CV_16U) {
			ofShortPixels pix16u;
			toOf(mat, pix16u);
			ofSaveImage(pix16u, filename);
		}
		else if (mat.depth() == CV_32F) {
			ofFloatPixels pix32f;
			toOf(mat, pix32f);
			ofSaveImage(pix32f, filename);
		}
	}

	Vec3b convertColor(Vec3b color, int code) {
		Mat_<Vec3b> mat(1, 1, CV_8UC3);
		mat(0, 0) = color;
		cvtColor(mat, mat, code);
		return mat(0, 0);
	}

	ofColor convertColor(ofColor color, int code) {
		Vec3b cvColor(color.r, color.g, color.b);
		Vec3b result = convertColor(cvColor, code);
		return ofColor(result[0], result[1], result[2], color.a);
	}

	ofPolyline convexHull(const ofPolyline& polyline) {
		vector<cv::Point2f> contour = toCv(polyline);
		vector<cv::Point2f> hull;
		convexHull(Mat(contour), hull);
		return toOfPolyline(hull);
	}

	// this should be replaced by c++ 2.0 api style code once available
	vector<cv::Vec4i> convexityDefects(const vector<cv::Point>& contour) {
		vector<int> hullIndices;
		convexHull(Mat(contour), hullIndices, false, false);
		vector<cv::Vec4i> convexityDefects;
		if (hullIndices.size() > 0 && contour.size() > 0) {
			CvMat contourMat = cvMat(1, contour.size(), CV_32SC2, (void*)&contour[0]);
			CvMat hullMat = cvMat(1, hullIndices.size(), CV_32SC1, (void*)&hullIndices[0]);
			CvMemStorage* storage = cvCreateMemStorage(0);
			CvSeq* defects = cvConvexityDefects(&contourMat, &hullMat, storage);
			for (int i = 0; i < defects->total; i++){
				CvConvexityDefect* cur = (CvConvexityDefect*)cvGetSeqElem(defects, i);
				cv::Vec4i defect;
				defect[0] = cur->depth_point->x;
				defect[1] = cur->depth_point->y;
				defect[2] = (cur->start->x + cur->end->x) / 2;
				defect[3] = (cur->start->y + cur->end->y) / 2;
				convexityDefects.push_back(defect);
			}
			cvReleaseMemStorage(&storage);
		}
		return convexityDefects;
	}

	vector<cv::Vec4i> convexityDefects(const ofPolyline& polyline) {
		vector<cv::Point2f> contour2f = toCv(polyline);
		vector<cv::Point2i> contour2i;
		Mat(contour2f).copyTo(contour2i);
		return convexityDefects(contour2i);
	}

	cv::RotatedRect minAreaRect(const ofPolyline& polyline) {
		return minAreaRect(Mat(toCv(polyline)));
	}

	cv::RotatedRect fitEllipse(const ofPolyline& polyline) {
		return fitEllipse(Mat(toCv(polyline)));
	}

	void fitLine(const ofPolyline& polyline, ofVec2f& point, ofVec2f& direction) {
		Vec4f line;
		fitLine(Mat(toCv(polyline)), line, CV_DIST_L2, 0, .01, .01);
		direction.set(line[0], line[1]);
		point.set(line[2], line[3]);
	}

	ofMatrix4x4 estimateAffine3D(vector<ofVec3f>& from, vector<ofVec3f>& to, float accuracy) {
		if (from.size() != to.size() || from.size() == 0 || to.size() == 0) {
			return ofMatrix4x4();
		}
		vector<unsigned char> outliers;
		return estimateAffine3D(from, to, outliers, accuracy);
	}

	ofMatrix4x4 estimateAffine3D(vector<ofVec3f>& from, vector<ofVec3f>& to, vector<unsigned char>& outliers, float accuracy) {
		Mat fromMat(1, from.size(), CV_32FC3, &from[0]);
		Mat toMat(1, to.size(), CV_32FC3, &to[0]);
		Mat affine;
		estimateAffine3D(fromMat, toMat, affine, outliers, 3, accuracy);
		ofMatrix4x4 affine4x4;
		affine4x4.set(affine.ptr<double>());
		affine4x4(3, 0) = 0;
		affine4x4(3, 1) = 0;
		affine4x4(3, 2) = 0;
		affine4x4(3, 3) = 1;
		Mat affine4x4Mat(4, 4, CV_32F, affine4x4.getPtr());
		affine4x4Mat = affine4x4Mat.t();
		affine4x4.set(affine4x4Mat.ptr<float>());
		return affine4x4;
	}

	bool findChessboardCornersPreTest(cv::Mat image, cv::Size patternSize, vector<cv::Point2f> & corners, int testResolution) {
		if (image.rows > testResolution || image.cols > testResolution) {
			cv::Mat lowRes;
			cv::resize(image, lowRes, cv::Size(testResolution, testResolution));
			vector<cv::Point2f> lowResPoints;
			if (cv::findChessboardCorners(lowRes, patternSize, lowResPoints)) {
				int maxX = 0;
				int maxY = 0;
				int minX = testResolution;
				int minY = testResolution;

				//find bounding box (roi) of found board
				for (auto & lowResPoint : lowResPoints) {
					if (lowResPoint.x < minX) {
						minX = lowResPoint.x;
					}
					if (lowResPoint.y < minY) {
						minY = lowResPoint.y;
					}
					if (lowResPoint.x > maxX) {
						maxX = lowResPoint.x;
					}
					if (lowResPoint.y > maxY) {
						maxY = lowResPoint.y;
					}
				}

				//move these coords into original image space
				maxX = maxX * image.cols / testResolution;
				maxY = maxY * image.rows / testResolution;
				minX = minX * image.cols / testResolution;
				minY = minY * image.rows / testResolution;

				//create a buffer around found points by 1 square size
				int boardResolutionMin = MIN(patternSize.width, patternSize.height);
				int strideX = (maxX - minX) / boardResolutionMin;
				int strideY = (maxY - minY) / boardResolutionMin;

				//apply buffer to bounds
				minX -= strideX * 4;
				maxX += strideX * 4;
				minY -= strideY * 4;
				maxY += strideY * 4;

				//clamp new bounds
				if (minX < 0)
				{
					minX = 0;
				}
				if (minY < 0)
				{
					minY = 0;
				}
				if (maxX > image.cols - 1)
				{
					maxX = image.cols - 1;
				}
				if (maxY > image.rows - 1)
				{
					maxY = image.rows - 1;
				}

				//copy roi into new matrix
				auto roiRect = cv::Rect(minX, minY, maxX - minX, maxY - minY);
				auto croppedFromLarge = image(roiRect);

				//find in cropped image
				vector<Point2f> croppedCorners;
				bool foundInCropped = findChessboardCorners(croppedFromLarge, patternSize, croppedCorners);

				if (foundInCropped) {
					//'uncrop' corners
					corners.clear();
					for (auto & corner : croppedCorners) {
						corners.push_back(corner + Point2f(minX, minY));
					}

					refineCheckerboardCorners(image, patternSize, corners);
					return true;
				}
				else {
					ofLogWarning("ofxCv::findChessboardCornersPreTest") << "Could find in low res, but not high res";
					corners.clear();
					for (auto & lowResCorner : lowResPoints) {
						auto corner = lowResCorner;
						corner.x *= (float)image.cols / (float)lowRes.cols;
						corner.y *= (float)image.rows / (float)lowRes.rows;
						corners.push_back(corner);
					}

					refineCheckerboardCorners(image, patternSize, corners);
					return true;
				}
			}
			else {
				return false;
			}
		}
		else {
			return findChessboardCorners(image, patternSize, corners);
		}
	}

	SimpleBlobDetector::Params getDefaultFindCircleBlobDetectorParams(Mat image, float minBlobWidthPct, float maxBlobWidthPct) {
		float minArea = pow(minBlobWidthPct * image.cols, 2);
		float maxArea = pow(maxBlobWidthPct * image.cols, 2);

		SimpleBlobDetector::Params params;
		params.minArea = minArea;
		params.maxArea = maxArea;

		return params;
	}

	bool findAsymmetricCircles(cv::Mat image, cv::Size patternSize, vector<cv::Point2f> & results, Ptr<FeatureDetector> featureDetector, int blockSize) {

		if (!featureDetector) {
			featureDetector = new SimpleBlobDetector(getDefaultFindCircleBlobDetectorParams(image));
		}

		if (blockSize == 0) {
			//blockSize = image.cols * 0.05f;

			//hack for Light Barrier
			blockSize = 100;
		}
		blockSize = (blockSize / 2) * 2 + 1;
		if (blockSize <= 1) {
			blockSize = 3;
		}

		Mat thresholded;
		cv::adaptiveThreshold(image, thresholded, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, blockSize, 2);

		return findCirclesGrid(thresholded, patternSize, results, CALIB_CB_ASYMMETRIC_GRID | CALIB_CB_CLUSTERING, featureDetector);
	}

	bool findBoard(cv::Mat image, BoardType boardType, cv::Size patternSize, vector<cv::Point2f> & results, bool useOptimisers) {
		switch (boardType) {
		case BoardType::Checkerboard:
			if (useOptimisers) {
				return findChessboardCornersPreTest(image, patternSize, results);
			} else {
				return findChessboardCorners(image, patternSize, results);
			}
			break;
		case BoardType::AsymmetricCircles:
			return findAsymmetricCircles(image, patternSize, results);
		default:
			return false;
		}
	}

	bool refineCheckerboardCorners(cv::Mat image, cv::Size patternSize, vector<cv::Point2f> & corners, int desiredHalfWindowSize /*= 5*/) {
		int windowSize = desiredHalfWindowSize;

		//make sure search size isn't too large
		{
			auto boundsOfCornerFinds = cv::boundingRect(corners);
			auto cornerFindsMinorAxis = MIN(boundsOfCornerFinds.width, boundsOfCornerFinds.height);
			auto boardMajorAxis = MAX(patternSize.width, patternSize.height);
			auto spacingBetweenCornersInImage = cornerFindsMinorAxis / (float)boardMajorAxis;

			if (spacingBetweenCornersInImage / 4 < windowSize) {
				windowSize = spacingBetweenCornersInImage / 4;
				if (windowSize % 2 == 0) {
					windowSize++;
				}
			}

			if (windowSize < 3) {
				//window size is too small to use
				return false;
			}
		}

		int ignoreCenterPixelsSize = windowSize / 5;

		auto subPixResults = corners;
		try {
			cv::cornerSubPix(image, subPixResults, Size(windowSize, windowSize), Size(ignoreCenterPixelsSize, ignoreCenterPixelsSize), TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 50, 1e-5));
			if (corners.size() != subPixResults.size()) {
				return false;
			}
		}
		catch (cv::Exception e) {
			ofLogWarning("ofxCvMin") << "Couldn't perform sub-pixel refinement of checkerboard find : " << e.what();
			return false;
		}

		//make sure none of the corners have walked outside their starting window
		for (int i = 0; i < corners.size(); i++) {
			auto difference = corners[i] - subPixResults[i];
			if (difference.x * difference.x + difference.y * difference.y > windowSize * windowSize) {
				//we walked too far!
				return false;
			}
		}

		corners = subPixResults;
		return true;
	}

	ofVec2f undistortPoint(const ofVec2f & distortedPoint, cv::Mat cameraMatrix, cv::Mat distotionCoefficients) {
		vector<Point2f> distortedPoints(1, toCv(distortedPoint));
		vector<Point2f> undistortedPoints(1);

		cv::undistortPoints(distortedPoints, undistortedPoints, cameraMatrix, distotionCoefficients);

		return toOf(undistortedPoints[0]);
	}

	float calibrateProjector(cv::Mat & cameraMatrixOut
		, cv::Mat & rotationOut, cv::Mat & translationOut
		, vector<ofVec3f> world, vector<ofVec2f> projectorPoints
		, int projectorWidth, int projectorHeight
		, bool projectorPointsAreNormalized
		, float initialLensOffset, float initialThrowRatio
		, bool trimOutliers, int flags) {
		vector<cv::Point2f> projector;
		if (projectorPointsAreNormalized) {
			for (const auto & projectorNormalisedPoint : projectorPoints) {
				auto projectorPoint = ofVec2f(
					ofMap(projectorNormalisedPoint.x, -1, +1, 0, projectorWidth),
					ofMap(projectorNormalisedPoint.y, -1, +1, 0, projectorHeight));
				projector.push_back(toCv(projectorPoint));
			}
		}
		else {
			projector = toCv(projectorPoints);
		}
		

		//we have to intitialise a basic camera matrix for it to start with (this will get changed by the function call calibrateCamera)
		cameraMatrixOut = Mat::eye(3, 3, CV_64F);
		cameraMatrixOut.at<double>(0, 0) = projectorWidth * initialThrowRatio; // default at 1.4 : 1.0f throw ratio
		cameraMatrixOut.at<double>(1, 1) = projectorHeight * initialThrowRatio;
		cameraMatrixOut.at<double>(0, 2) = projectorWidth / 2.0f;
		cameraMatrixOut.at<double>(1, 2) = projectorHeight * (0.50f - initialLensOffset / 2.0f); // default at 40% lens offset

		//same again for distortion
		Mat distortionCoefficients = Mat::zeros(5, 1, CV_64F);

		float error;
		if (trimOutliers) {
			error = ofxCv::calibrateCameraWorldRemoveOutliers(toCv(world), projector,
				cv::Size(projectorWidth, projectorHeight),
				cameraMatrixOut, distortionCoefficients,
				rotationOut, translationOut, flags, 100.0f);
		}
		else {
			vector<Mat> rotations, translations;
			error = cv::calibrateCamera(vector<vector<Point3f>>(1, toCv(world)), vector<vector<Point2f>>(1, projector)
				, cv::Size(projectorWidth, projectorHeight)
				, cameraMatrixOut, distortionCoefficients
				, rotations, translations
				, flags
				, TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 1000, DBL_EPSILON));
			rotationOut = rotations[0];
			translationOut = translations[0];
		}

		return error;
	}

	float calibrateProjector(ofMatrix4x4 & viewOut, ofMatrix4x4 & projectionOut
		, vector<ofVec3f> world, vector<ofVec2f> projectorPoints
		, int projectorWidth, int projectorHeight
		, bool projectorPointsAreNormalized
		, float initialLensOffset, float initialThrowRatio
		, bool trimOutliers, int flags) {
		cv::Mat cameraMatrix, rotation, translation;
		float error = calibrateProjector(cameraMatrix, rotation, translation, world, projectorPoints, projectorWidth, projectorHeight, projectorPointsAreNormalized, initialLensOffset, initialThrowRatio, trimOutliers, flags);
		viewOut = makeMatrix(rotation, translation);
		projectionOut = makeProjectionMatrix(cameraMatrix, cv::Size(projectorWidth, projectorHeight));
		return error;
	}

	float calibrateCameraWorldRemoveOutliers(vector<Point3f> pointsWorld, vector<Point2f> pointsImage, cv::Size size, cv::Mat & cameraMatrix, cv::Mat & distortionCoefficients, cv::Mat & rotation, cv::Mat & translation, int flags, float maxError) {
		const int pointCount = pointsWorld.size();

		auto cameraMatrixCopy = cameraMatrix;

		vector<Mat> rotations, translations;
		float rmsErrorAll = cv::calibrateCamera(vector<vector<Point3f>>(1, pointsWorld), vector<vector<Point2f>>(1, pointsImage),
			size, cameraMatrix, distortionCoefficients,
			rotations, translations, flags);

		vector<Point2f> projectedPoints;
	
		cv::projectPoints(pointsWorld, rotations[0], translations[0], cameraMatrix, distortionCoefficients, projectedPoints);
		set<int> indicesToRemove;

		for (int i = 0; i < pointCount; i++) {
			auto error = toOf(projectedPoints[i] - pointsImage[i]).length();
			if (error > maxError) {
				ofLogNotice("ofxCvMin::calibrateCameraWorldRemoveOutliers") << "Removing point [" << i << "] = [" << pointsWorld[i] << "]->[" << pointsImage[i] << "] because its error is too high [" << error << "]";
				indicesToRemove.insert(i);
			}
		}

		vector<Point3f> trimmedPointsWorld;
		vector<Point2f> trimmedPointsImage;

		for (int i = 0; i < pointCount; i++) {
			if (indicesToRemove.find(i) == indicesToRemove.end()) {
				trimmedPointsWorld.push_back(pointsWorld[i]);
				trimmedPointsImage.push_back(pointsImage[i]);
			}
		}

		cameraMatrix = cameraMatrixCopy.clone();
		
		//ensure principal point inside image
		{
			if (cameraMatrix.at<double>(0, 2) < 0 || cameraMatrix.at<double>(0, 2) >= size.width - 1) {
				cameraMatrix.at<double>(0, 2) = size.width / 2.0f;
			}
			if (cameraMatrix.at<double>(1, 2) < 0 || cameraMatrix.at<double>(1, 2) >= size.height - 1) {
				cameraMatrix.at<double>(1, 2) = size.height / 2.0f;
			}
		}

		float rmsErrorTrimmed = cv::calibrateCamera(vector<vector<Point3f>>(1, trimmedPointsWorld), vector<vector<Point2f>>(1, trimmedPointsImage)
			, size
			, cameraMatrix, distortionCoefficients
			, rotations, translations
			, flags);

		if (rmsErrorTrimmed != rmsErrorAll) {
			ofLogNotice("ofxCvMin::calibrateCameraWorldRemoveOutliers") << "Removing " << indicesToRemove.size() << "/" << pointCount << " points changed resprojection error from " << rmsErrorAll << "px to " << rmsErrorTrimmed << "px";
		}

		rotation = rotations[0];
		translation = translations[0];
		return rmsErrorTrimmed;
	}
}