#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){
	camera.initGrabber(640, 480);
}

//--------------------------------------------------------------
void ofApp::update(){
	camera.update();
	
	Mat matImage = toCv(camera.getPixelsRef()).clone();
	cv::erode(matImage, matImage, Mat());
	cv::erode(matImage, matImage, Mat());
	cv::erode(matImage, matImage, Mat());
	ofxCv::imitate(this->preview, matImage);
	ofxCv::copy(matImage, this->preview, 3);
	this->preview.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	this->preview.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
