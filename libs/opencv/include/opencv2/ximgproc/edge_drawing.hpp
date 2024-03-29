// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef __OPENCV_EDGE_DRAWING_HPP__
#define __OPENCV_EDGE_DRAWING_HPP__

#include <opencv2/core.hpp>

namespace cv
{
namespace ximgproc
{

//! @addtogroup ximgproc_edge_drawing
//! @{

/** @brief Class implementing the ED (EdgeDrawing) @cite topal2012edge, EDLines @cite akinlar2011edlines, EDPF @cite akinlar2012edpf and EDCircles @cite akinlar2013edcircles algorithms

EDGE DRAWING LIBRARY FOR GEOMETRIC FEATURE EXTRACTION AND VALIDATION

Edge Drawing (ED) algorithm is an proactive approach on edge detection problem. In contrast to many other existing edge detection algorithms which follow a subtractive
approach (i.e. after applying gradient filters onto an image eliminating pixels w.r.t. several rules, e.g. non-maximal suppression and hysteresis in Canny), ED algorithm
works via an additive strategy, i.e. it picks edge pixels one by one, hence the name Edge Drawing. Then we process those random shaped edge segments to extract higher level
edge features, i.e. lines, circles, ellipses, etc. The popular method of extraction edge pixels from the thresholded gradient magnitudes is non-maximal supressiun that tests
every pixel whether it has the maximum gradient response along its gradient direction and eliminates if it does not. However, this method does not check status of the
neighboring pixels, and therefore might result low quality (in terms of edge continuity, smoothness, thinness, localization) edge segments. Instead of non-maximal supression,
ED points a set of edge pixels and join them by maximizing the total gradient response of edge segments. Therefore it can extract high quality edge segments without need for
an additional hysteresis step.
*/

class CV_EXPORTS_W EdgeDrawing : public Algorithm
{
public:

    enum GradientOperator
    {
        PREWITT = 0,
        SOBEL = 1,
        SCHARR = 2,
        LSD = 3
    };

    struct CV_EXPORTS_W_SIMPLE Params
    {
        CV_WRAP Params();
        //! Parameter Free mode will be activated when this value is true.
        CV_PROP_RW bool PFmode;
        //! indicates the operator used for gradient calculation.The following operation flags are available(cv::ximgproc::EdgeDrawing::GradientOperator)
        CV_PROP_RW int EdgeDetectionOperator;
        //! threshold value used to create gradient image.
        CV_PROP_RW int GradientThresholdValue;
        //! threshold value used to create gradient image.
        CV_PROP_RW int AnchorThresholdValue;
        CV_PROP_RW int ScanInterval;
        //! minimun connected pixels length processed to create an edge segment.
        CV_PROP_RW int MinPathLength;
        //! sigma value for internal GaussianBlur() function.
        CV_PROP_RW float Sigma;
        CV_PROP_RW bool SumFlag;
        //! when this value is true NFA (Number of False Alarms) algorithm will be used for line and ellipse validation.
        CV_PROP_RW bool NFAValidation;
        //! minimun line length to detect.
        CV_PROP_RW int MinLineLength;
        CV_PROP_RW double MaxDistanceBetweenTwoLines;
        CV_PROP_RW double LineFitErrorThreshold;
        CV_PROP_RW double MaxErrorThreshold;

        void read(const FileNode& fn);
        void write(FileStorage& fs) const;
    };

    /** @brief Detects edges and prepares them to detect lines and ellipses.

    @param src input image
    */
    CV_WRAP virtual void detectEdges(InputArray src) = 0;
    CV_WRAP virtual void getEdgeImage(OutputArray dst) = 0;
    CV_WRAP virtual void getGradientImage(OutputArray dst) = 0;

    CV_WRAP virtual std::vector<std::vector<Point> > getSegments() = 0;

    /** @brief Detects lines.

    @param lines  output Vec<4f> contains start point and end point of detected lines.
    @note you should call detectEdges() method before call this.
    */
    CV_WRAP virtual void detectLines(OutputArray lines) = 0;

    /** @brief Detects circles and ellipses.

    @param ellipses  output Vec<6d> contains center point and perimeter for circles.
    @note you should call detectEdges() method before call this.
    */
    CV_WRAP virtual void detectEllipses(OutputArray ellipses) = 0;

    CV_WRAP Params params;

    /** @brief sets parameters.

    this function is meant to be used for parameter setting in other languages than c++.
    */
    CV_WRAP void setParams(const EdgeDrawing::Params& parameters);
    virtual ~EdgeDrawing() { }
};

/** @brief Creates a smart pointer to a EdgeDrawing object and initializes it
*/
CV_EXPORTS_W Ptr<EdgeDrawing> createEdgeDrawing();
//! @}

}
}

#endif /* __OPENCV_EDGE_DRAWING_HPP__ */
