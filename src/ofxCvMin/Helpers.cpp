#include "Helpers.h"
#include "Utilities.h"

namespace ofxCv {
	
	using namespace cv;
	
	ofMatrix4x4 makeMatrix(Mat rotation, Mat translation) {
		Mat rot3x3;
		if(rotation.rows == 3 && rotation.cols == 3) {
			rot3x3 = rotation;
		} else {
			Rodrigues(rotation, rot3x3);
		}
		double* rm = rot3x3.ptr<double>(0);
		double* tm = translation.ptr<double>(0);
		return ofMatrix4x4(rm[0], rm[3], rm[6], 0.0f,
											 rm[1], rm[4], rm[7], 0.0f,
											 rm[2], rm[5], rm[8], 0.0f,
											 tm[0], tm[1], tm[2], 1.0f);
	}

	//a reference : http://strawlab.org/2011/11/05/augmented-reality-with-OpenGL/#the_opengl_projection_matrix_from_hz_intrinsic_parameters
	ofMatrix4x4 makeProjectionMatrix(Mat cameraMatrix, cv::Size imageSize) {
		float focalLengthX = cameraMatrix.at<double>(0, 0);
		float focalLengthY = cameraMatrix.at<double>(1, 1);
		float ppx = cameraMatrix.at<double>(0, 2);
		float ppy = cameraMatrix.at<double>(1, 2);

		ofMatrix4x4 projection;
		projection(0, 0) = 2.0f * focalLengthX / (float)imageSize.width;
		projection(1, 1) = -2.0f * focalLengthY / (float)imageSize.height;
		projection(2,3) = 1.0f;
		projection(3,3) = 0.0f;

		const ofVec3f lensOffset(2 * (ppx / (float) imageSize.width) - 1.0f, 1.0f - 2 * (ppy / (float) imageSize.height), 0.0f);
		projection.postMultTranslate(lensOffset);

		return projection;
	}
	
	vector<Point3f> makeCheckerboardPoints(cv::Size size, float spacing, bool centered) {
		vector<ofVec3f> corners;
		
		ofVec3f offset;
		if (centered) {
			offset = - ofVec3f(size.width - 1, size.height - 1, 0) * spacing * 0.5f;
		}
		else {
			offset = ofVec3f(spacing, spacing, 0.0f); // first inner corner is 1 square in
		}
		
		for(int j=0; j<size.height; j++) {
			for(int i=0; i<size.width; i++) {
				corners.push_back(ofVec3f(i, j, 0) * spacing + offset);
			}
		}
		return toCv(corners);
	}

	ofMesh makeCheckerboardMesh(cv::Size size, float spacing, bool centered) {
		ofMesh mesh;

		ofVec3f center;
		if (centered) {
			center = ofVec3f(size.width + 1, size.height + 1, 0) * spacing * 0.5f;
		}
		
		//board face
		const auto topLeft = ofVec3f(-1, -1, 0.0f) * spacing - center;
		const auto topRight = ofVec3f(2 + size.width, -1, 0.0f) * spacing - center;
		const auto bottomLeft = ofVec3f(-1, size.height + 2, 0.0f) * spacing - center;
		const auto bottomRight = ofVec3f(2 + size.width, size.height + 2, 0.0f) * spacing - center;
		
		mesh.addVertex(bottomLeft);
		mesh.addVertex(topRight);
		mesh.addVertex(topLeft);

		mesh.addVertex(bottomRight);
		mesh.addVertex(topRight);
		mesh.addVertex(bottomLeft);

		mesh.addColor(ofColor(255));
		mesh.addColor(ofColor(255));
		mesh.addColor(ofColor(255));

		mesh.addColor(ofColor(255));
		mesh.addColor(ofColor(255));
		mesh.addColor(ofColor(255));

		const auto z = -spacing / 100.0f;
		for (int i = 0; i<size.width + 1; i++) {
			for(int j=0; j<size.height + 1; j++) {
				auto black = i % 2 == j % 2;
				auto squareTopLeft = ofVec3f(i, j, z) * spacing - center;
				
				mesh.addVertex(squareTopLeft);
				mesh.addVertex(squareTopLeft + ofVec3f(0, spacing, 0));
				mesh.addVertex(squareTopLeft + ofVec3f(spacing, 0, 0));

				mesh.addVertex(squareTopLeft + ofVec3f(spacing, 0, 0));
				mesh.addVertex(squareTopLeft + ofVec3f(0, spacing, 0));
				mesh.addVertex(squareTopLeft + ofVec3f(spacing, spacing, 0));

				for(int c=0; c<6; c++) {
					mesh.addColor(ofFloatColor(black ? 0.0f : 1.0f));
				}
			}
		}

		mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
		return mesh;
	}

	vector<Point3f> makeAsymmetricCirclePoints(cv::Size size, float spacing, bool centered) {
		vector<ofVec3f> points;

		ofVec3f center;
		if (centered) {
			center = ofVec3f(size.width * 2.0f - 1.0f, size.height - 1.0f, 0) * spacing * 0.5f;
		}

		for (int j = 0; j<size.height; j++) {
			for (int i = 0; i<size.width; i++) {
				points.push_back(ofVec3f(
					i * 2 + (j % 2),
					j,
					0
					) * spacing - center);
			}
		}
		return toCv(points);
	}

	ofMesh makeAsymmetricCircleMesh(cv::Size size, float spacing, bool centered) {
		ofMesh mesh;
		const auto points = toOf(makeAsymmetricCirclePoints(size, spacing, centered));
		
		ofVec3f center;
		if (centered) {
			center = ofVec3f(size.width * 2.0f, size.height, 0) * spacing * 0.5f;
		}

		//board face
		const auto topLeft = ofVec3f(-1, -1, 0.0f) * spacing - center;
		const auto topRight = ofVec3f(1 + size.width * 2, -1, 0.0f) * spacing - center;
		const auto bottomLeft = ofVec3f(-1, size.height + 1, 0.0f) * spacing - center;
		const auto bottomRight = ofVec3f(1 + size.width * 2, size.height + 1, 0.0f) * spacing - center;

		mesh.addVertex(bottomLeft);
		mesh.addVertex(topRight);
		mesh.addVertex(topLeft);

		mesh.addVertex(bottomRight);
		mesh.addVertex(topRight);
		mesh.addVertex(bottomLeft);

		mesh.addColor(ofColor(255));
		mesh.addColor(ofColor(255));
		mesh.addColor(ofColor(255));

		mesh.addColor(ofColor(255));
		mesh.addColor(ofColor(255));
		mesh.addColor(ofColor(255));

		const auto r = spacing / 10.0f;
		const auto z = (-spacing / 100.0f) / r; // /r since *r later
		for (auto & point : points) {
			mesh.addVertex(point + ofVec3f(-1, +1, z) * r);
			mesh.addVertex(point + ofVec3f(+1, -1, z) * r);
			mesh.addVertex(point + ofVec3f(-1, -1, z) * r);
			mesh.addVertex(point + ofVec3f(-1, +1, z) * r);
			mesh.addVertex(point + ofVec3f(+1, +1, z) * r);
			mesh.addVertex(point + ofVec3f(+1, -1, z) * r);
			mesh.addColor(ofColor(0));
			mesh.addColor(ofColor(0));
			mesh.addColor(ofColor(0));
			mesh.addColor(ofColor(0));
			mesh.addColor(ofColor(0));
			mesh.addColor(ofColor(0));
		}

		return mesh;
	}

	vector<Point3f> makeBoardPoints(BoardType boardType, cv::Size size, float spacing, bool centered) {
		switch (boardType) {
		case BoardType::AsymmetricCircles:
			return makeAsymmetricCirclePoints(size, spacing, centered);
			break;
		case BoardType::Checkerboard:
			return makeCheckerboardPoints(size, spacing, centered);
			break;
		default:
			return vector<Point3f>();
		}
	}

	ofMesh makeBoardMesh(BoardType boardType, cv::Size size, float spacing, bool centered) {
		switch (boardType) {
		case BoardType::AsymmetricCircles:
			return makeAsymmetricCircleMesh(size, spacing, centered);
			break;
		case BoardType::Checkerboard:
			return makeCheckerboardMesh(size, spacing, centered);
			break;
		default:
			return ofMesh();
		}
	}
	
	vector<Point2f> undistortPixelCoordinates(const vector<Point2f> & distortedPixelCoordinates, cv::Mat cameraMatrix, cv::Mat distortionCoefficients) {
		vector<Point2f> normalisedPoints;
		cv::undistortPoints(distortedPixelCoordinates, normalisedPoints, cameraMatrix, distortionCoefficients);
		auto fx = cameraMatrix.at<double>(0, 0);
		auto fy = cameraMatrix.at<double>(1, 1);
		auto cx = cameraMatrix.at<double>(0, 2);
		auto cy = cameraMatrix.at<double>(1, 2);
		
		vector<Point2f> undistortedPixelCoordinates(distortedPixelCoordinates.size());
		auto undistortedPixelCoordinatesIterator = undistortedPixelCoordinates.begin();
		for (const auto & normalisedPoint : normalisedPoints) {
			auto & undistorted = *undistortedPixelCoordinatesIterator++;
			undistorted.x = normalisedPoint.x * fx + cx;
			undistorted.y = normalisedPoint.y * fy + cy;
		}

		return undistortedPixelCoordinates;
	}

	void drawMat(Mat& mat, float x, float y) {
		drawMat(mat, x, y, mat.cols, mat.rows);
	}
	
    // experimental special case of copying into ofTexture, which acts different
    // might be able to rewrite this in terms of getDepth() but right now this
    // function loses precision with CV_32F -> CV_8U
    template <class S>
    void copy(S& src, ofTexture& tex) {
        Mat mat = toCv(src);
		int glType;
		Mat buffer;
		if(mat.depth() != CV_8U) {
			copy(mat, buffer, CV_8U);
		} else {
			buffer = mat;
		}
		if(mat.channels() == 1) {
			glType = GL_LUMINANCE;
		} else {
			glType = GL_RGB;
		}
		int w = buffer.cols;
		int h = buffer.rows;
		tex.allocate(w, h, glType);
		tex.loadData(buffer.ptr(), w, h, glType);
    }
    
	void drawMat(Mat& mat, float x, float y, float width, float height) {
        ofTexture tex;
        copy(mat, tex);
		tex.draw(x, y, width, height);
	}
	
	void drawCorners(vector<Point2f> & points) {
		vector<ofVec2f> ofPoints = toOf(points);
		drawCorners(ofPoints);
	}
	
	void drawCorners(vector<ofVec2f> & points) {
		ofMesh line;
		line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINE_STRIP);

		ofFloatColor color(0.5f, 0.5f, 0.5f);
		for(auto & point : points) {
			line.addVertex(point);
			line.addColor(color);
			color.r += 0.5f / (float)points.size();
		}
		ofPushStyle();
		ofEnableSmoothing();
		
		ofSetLineWidth(4.0f);
		line.disableColors();
		ofSetColor(0, 0, 0);
		line.draw();
		
		ofSetLineWidth(2.0f);
		line.enableColors();
		ofSetColor(255, 255, 255);
		line.draw();
		
		ofPopStyle();
	}
	
	void applyMatrix(const ofMatrix4x4& matrix) {
		glMultMatrixf((GLfloat*) matrix.getPtr());
	}
	
	int forceOdd(int x) {
		return (x / 2) * 2 + 1;
	}
	
	int findFirst(const Mat& arr, unsigned char target) {
		for(int i = 0; i < arr.rows; i++) {
			if(arr.at<unsigned char>(i) == target) {
				return i;
			}
		}
		return 0;
	}
	
	int findLast(const Mat& arr, unsigned char target) {
		for(int i = arr.rows - 1; i >= 0; i--) {
			if(arr.at<unsigned char>(i) == target) {
				return i;
			}
		}
		return 0;
	}
	
	float weightedAverageAngle(const vector<Vec4i>& lines) {
		float angleSum = 0;
		ofVec2f start, end;
		float weights = 0;
		for(int i = 0; i < lines.size(); i++) {
			start.set(lines[i][0], lines[i][1]);
			end.set(lines[i][2], lines[i][3]);
			ofVec2f diff = end - start;
			float length = diff.length();
			float weight = length * length;
			float angle = atan2f(diff.y, diff.x);
			angleSum += angle * weight;
			weights += weight;
		}
		return angleSum / weights;
	}
	
	vector<cv::Point2f> getConvexPolygon(const vector<cv::Point2f>& convexHull, int targetPoints) {
		vector<cv::Point2f> result = convexHull;
		
		static const unsigned int maxIterations = 16;
		static const double infinity = numeric_limits<double>::infinity();
		double minEpsilon = 0;
		double maxEpsilon = infinity;
		double curEpsilon = 16; // good initial guess
		
		// unbounded binary search to simplify the convex hull until it's targetPoints
		if(result.size() > targetPoints) {
			for(int i = 0; i < maxIterations; i++) {
				approxPolyDP(Mat(convexHull), result, curEpsilon, true);
				if(result.size() == targetPoints) {
					break;
				}
				if(result.size() > targetPoints) {
					minEpsilon = curEpsilon;
					if(maxEpsilon == infinity) {
						curEpsilon = curEpsilon *  2;
					} else {
						curEpsilon = (maxEpsilon + minEpsilon) / 2;
					}
				}
				if(result.size() < targetPoints) {
					maxEpsilon = curEpsilon;
					curEpsilon = (maxEpsilon + minEpsilon) / 2;
				}
			}
		}
		
		return result;
	}
	
	void drawHighlightString(string text, ofPoint position, ofColor background, ofColor foreground) {
		drawHighlightString(text, position.x, position.y, background, foreground);
	}
	
	void drawHighlightString(string text, int x, int y, ofColor background, ofColor foreground) {
		vector<string> lines = ofSplitString(text, "\n");
		int textLength = 0;
		for(int i = 0; i < lines.size(); i++) {
			// tabs are not rendered
			int tabs = count(lines[i].begin(), lines[i].end(), '\t');
			int curLength = lines[i].length() - tabs;
			// after the first line, everything is indented with one space
			if(i > 0) {
				curLength++;
			}
			if(curLength > textLength) {
				textLength = curLength;
			}
		}
		
		int padding = 4;
		int fontSize = 8;
		float leading = 1.7;
		int height = lines.size() * fontSize * leading - 1;
		int width = textLength * fontSize;
		
#ifdef TARGET_OPENGLES
		// This needs to be refactored to support OpenGLES
		// Else it will work correctly
#else
		glPushAttrib(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		ofPushStyle();
		ofSetColor(background);
		ofFill();
		ofRect(x, y, width + 2 * padding, height + 2 * padding);
		ofSetColor(foreground);
		ofNoFill();
		ofPushMatrix();
		ofTranslate(padding, padding);
		ofDrawBitmapString(text, x + 1, y + fontSize + 2);
		ofPopMatrix();
		ofPopStyle();
		glPopAttrib();
#endif         
         
	}
}