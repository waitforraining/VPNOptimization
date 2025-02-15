#include "Skeleton/RidgeFilter.h"
#include <filesystem>

using namespace cv;
using namespace std;

namespace ViewPointNetwork
{
	RidgeDetection::RidgeDetection(int ddepth, int dx, int dy, int ksize,
		int out_dtype, double scale, double delta, int borderType)
	{
		CV_Assert((ksize == 1 || ksize == 3 || ksize == 5 || ksize == 7));
		CV_Assert((ddepth == CV_32FC1 || ddepth == CV_64FC1));
		_ddepth = ddepth;
		_dx = dx;
		_dy = dy;
		_ksize = ksize;
		_scale = scale;
		_delta = delta;
		_borderType = borderType;
		_out_dtype = out_dtype;
	}

	void RidgeDetection::getRoughRidge(InputArray _img, OutputArray out)
	{
		Mat img = _img.getMat();
		CV_Assert(img.channels() == 1 || img.channels() == 3);

		if (img.channels() == 3)
			cvtColor(img, img, COLOR_BGR2GRAY);

		Mat sbx, sby;
		Sobel(img, sbx, _ddepth, _dx, 0, _ksize, _scale, _delta, _borderType);
		Sobel(img, sby, _ddepth, 0, _dy, _ksize, _scale, _delta, _borderType);

		Mat sbxx, sbyy, sbxy;
		Sobel(sbx, sbxx, _ddepth, _dx, 0, _ksize, _scale, _delta, _borderType);
		Sobel(sby, sbyy, _ddepth, 0, _dy, _ksize, _scale, _delta, _borderType);
		Sobel(sbx, sbxy, _ddepth, 0, _dy, _ksize, _scale, _delta, _borderType);

		Mat sb2xx, sb2yy, sb2xy;
		multiply(sbxx, sbxx, sb2xx);
		multiply(sbyy, sbyy, sb2yy);
		multiply(sbxy, sbxy, sb2xy);

		Mat sbxxyy;
		multiply(sbxx, sbyy, sbxxyy);

		Mat rootex;
		rootex = (sb2xx + (sb2xy + sb2xy + sb2xy + sb2xy) - (sbxxyy + sbxxyy) + sb2yy);
		Mat root;
		sqrt(rootex, root);
		Mat ridgexp;
		ridgexp = ((sbxx + sbyy) + root);
		ridgexp.convertTo(out, _out_dtype, 0.5);
	}


	void showImage(cv::Mat image, const std::string& windowName)
	{
		namedWindow(windowName, WINDOW_AUTOSIZE);
		imshow(windowName, image);
		waitKey(0); 
	}

	// Process the image and extract the skeleton
	cv::Mat RidgeDetection::getRidge(House& house, HeatMap& heat,
		cv::Mat image, const string& outresultpath, int distType, int markSize, int HouseType)
	{
		// Set the boundary size
		int border_size = 10;
		// Normalized image pixel value 0-255
		cv::normalize(image, image, 0, 255, cv::NORM_MINMAX);
		// Binary the image
		cv::Mat binary;
		cv::threshold(image, binary, 0, 255, cv::THRESH_BINARY);

		for (int i = 0; i < house.getEdges().size(); i++)
			cv::line(binary, cv::Point(house.getEdge(i).getBegPoint().X() / heat.getCell(), binary.rows - house.getEdge(i).getBegPoint().Y() / heat.getCell()),
				cv::Point(house.getEdge(i).getEndPoint().X() / heat.getCell(), binary.rows - house.getEdge(i).getEndPoint().Y() / heat.getCell()), cv::Scalar(0), 3);

		// Add a border around the image
		cv::copyMakeBorder(binary, binary, border_size, border_size, border_size, border_size, cv::BORDER_CONSTANT, 0);

		/*distType = 2;
		markSize = 5;*/
		
		// Calculate the distance transform
		cv::Mat dis;
		cv::distanceTransform(binary, dis, distType, markSize);
		//printMatProperties(dis);

		// Gets the size of the distance transform image
		int height = dis.rows;
		int width = dis.cols;
		// Remove the added boundary area
		cv::Rect roi(border_size, border_size, width - 2 * border_size, height - 2 * border_size);
		dis = dis(roi).clone();
		//dis = dis(roi);

		cv::normalize(dis, dis, 0, 255, cv::NORM_MINMAX);

		dis = 255 - dis;

		cv::Mat ridges;
		getRoughRidge(dis, ridges);

		cv::Mat profile = ridges.clone();

		int edgeSize = house.getEdges().size();
		for (int i = 0; i < edgeSize; i++)
			cv::line(profile, cv::Point(house.getEdge(i).getBegPoint().X() / heat.getCell(), profile.rows - house.getEdge(i).getBegPoint().Y() / heat.getCell()),
				cv::Point(house.getEdge(i).getEndPoint().X() / heat.getCell(), profile.rows - house.getEdge(i).getEndPoint().Y() / heat.getCell()), cv::Scalar(255), 1);

		//imwrite(outresultpath + "_distType" + to_string(distType) + "_markSize" + to_string(markSize) + "_ridges.png", profile);
		imwrite(outresultpath + "_ridges.png", profile);

		// Normalized ridge detection results
		cv::normalize(ridges, ridges, 0, 255, cv::NORM_MINMAX);
		// Binarize the ridge detection results
		//cv::Mat binary_ridges;
		cv::threshold(ridges, binary, 20, 255, cv::THRESH_BINARY);
		//cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		// Remove the skeleton outside the room in the binarization result
		binary.setTo(0, image == 0);


		Mat zhang = fastSkeletonize(binary);
		//// Displays the combined image
		cv::Mat profile_2 = zhang.clone();
		for (int i = 0; i < edgeSize; i++)
			cv::line(profile_2, cv::Point(house.getEdge(i).getBegPoint().X() / heat.getCell(), profile_2.rows - house.getEdge(i).getBegPoint().Y() / heat.getCell()),
				cv::Point(house.getEdge(i).getEndPoint().X() / heat.getCell(), profile_2.rows - house.getEdge(i).getEndPoint().Y() / heat.getCell()), cv::Scalar(255), 1);

		/*imwrite(outresultpath + "_distType" + to_string(distType) + "_markSize" + to_string(markSize) + "_thin_ridges.png", profile_2);
		imwrite(outresultpath + "_distType" + to_string(distType) + "_markSize" + to_string(markSize) + "_zhang_skeleton.png", zhang);*/
		imwrite(outresultpath + "_thin_ridges.png", profile_2);
		imwrite(outresultpath + "_zhang_skeleton.png", zhang);

		return zhang;
	}
	void printMatProperties(const cv::Mat& mat) {
		// Output image shape
		std::cout << "Image shape (height, width): (" << mat.rows << ", " << mat.cols << ")" << std::endl;

		// Number of output channels
		std::cout << "Number of channels: " << mat.channels() << std::endl;

		// Output data type
		std::cout << "Data type: " << mat.type() << std::endl;

		// Output stride
		std::cout << "Step (bytes per row): " << mat.step << std::endl;
	}

	// Process the image and extract the skeleton
	cv::Mat RidgeDetection::getRidge(cv::Mat image, const string& outresultpath, int distType, int markSize, int HouseType)
	{
		// Set the boundary size
		int border_size = 10;
		
		cv::normalize(image, image, 0, 255, cv::NORM_MINMAX);
		// Normalized image pixel value 0-255
		cv::Mat binary;
		//cv::threshold(normalized, binary, 0, 255, cv::THRESH_BINARY);
		cv::threshold(image, binary, 0, 255, cv::THRESH_BINARY);
		//showImage(binary, "house");

		// Add a border around the image
		cv::copyMakeBorder(binary, binary, border_size, border_size, border_size, border_size, cv::BORDER_CONSTANT, 0);

		/*distType = 2;
		markSize = 5;*/
		
		// Calculate the distance transform
		cv::Mat dis;
		cv::distanceTransform(binary, dis, distType, markSize);
		// Gets the size of the distance transform image
		int height = dis.rows;
		int width = dis.cols;
		// Remove the added boundary area
		cv::Rect roi(border_size, border_size, width - 2 * border_size, height - 2 * border_size);
		dis = dis(roi);

		// Normalized distance transform results
		cv::normalize(dis, dis, 0, 255, cv::NORM_MINMAX);

		// Invert the image
		dis = 255 - dis;

		cv::Mat ridges;
		getRoughRidge(dis, ridges);
		cv::Mat profile = ridges.clone();

		imwrite(outresultpath + "_ridges.png", profile);
		// Normalized ridge detection results
		cv::normalize(ridges, ridges, 0, 255, cv::NORM_MINMAX);
		// Binarize the ridge detection results
		//cv::Mat binary_ridges;
		cv::threshold(ridges, binary, 20, 255, cv::THRESH_BINARY);
		//cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		binary.setTo(0, image == 0);

		Mat zhang = fastSkeletonize(binary);
		//// Displays the combined image

		cv::Mat profile_2 = zhang.clone();

		imwrite(outresultpath + "_thin_ridges.png", profile_2);
		imwrite(outresultpath + "_zhang_skeleton.png", zhang);

		return zhang;
	}

	// Create a skeleton
	cv::Mat RidgeDetection::creatRidge(House& house, HeatMap& heat,
		const string& grayImgPath, const string& outresultpath, int houseType)
	{
		// Read the image
		cv::Mat image = imread(grayImgPath, IMREAD_GRAYSCALE);
		return getRidge(house, heat, image, outresultpath, RT_L2, 5, houseType);
	}

	// Skeleton refinement -- Modified zhang method, type cv::Mat
	cv::Mat fastSkeletonize(cv::Mat image) 
	{
		// Lookup Table (LUT) - one entry for every possible 8-bit binary neighborhood combination.
		int lut[256] = {
			0, 0, 0, 1, 0, 0, 1, 3, 0, 0, 3, 1, 1, 0, 1, 3, 0, 0, 0, 0, 0, 0,
			0, 0, 2, 0, 2, 0, 3, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 2, 2, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0,
			0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 2, 0, 0, 0, 3, 1,
			0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 3, 0, 0,
			1, 3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 2, 3, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3,
			0, 1, 0, 0, 0, 0, 2, 2, 0, 0, 2, 0, 0, 0
		};

		bool pixel_removed;
		bool first_pass;
		int neighbors;

		// Get the image size and add borders
		int rows = image.rows + 2;
		int cols = image.cols + 2;

		// Initialize the skeleton image and add borders
		cv::Mat skeleton = cv::Mat::zeros(rows, cols, CV_8UC1);

		// Copy the original image data to the skeleton image
		image.copyTo(skeleton(cv::Rect(1, 1, image.cols, image.rows)));
		//skeleton = image(cv::Rect(1, 1, image.cols - 2, image.rows - 2)) > 0;
		//skeleton(cv::Rect(1, 1, image.cols - 2, image.rows - 2)) = image;
		cv::Mat cleaned_skeleton = skeleton.clone();

		pixel_removed = true;

		// The algorithm repeats refinements until no further refinements occur
		while (pixel_removed) {
			pixel_removed = false;

			// There are two stages, in the first stage, the pixels marked 1 and 3 are removed;
			// In the second stage, the pixels labeled 2 and 3 are removed.
			for (int pass_num = 0; pass_num < 2; ++pass_num) {
				first_pass = (pass_num == 0);
				for (int row = 1; row < rows - 1; ++row) {
					for (int col = 1; col < cols - 1; ++col) {
						// Process foreground pixels
						if (skeleton.at<uchar>(row, col) > 0) {
							// Calculate the 8 neighborhood number
							neighbors =
								1 * (skeleton.at<uchar>(row - 1, col - 1) > 0) +
								2 * (skeleton.at<uchar>(row - 1, col) > 0) +
								4 * (skeleton.at<uchar>(row - 1, col + 1) > 0) +
								8 * (skeleton.at<uchar>(row, col + 1) > 0) +
								16 * (skeleton.at<uchar>(row + 1, col + 1) > 0) +
								32 * (skeleton.at<uchar>(row + 1, col) > 0) +
								64 * (skeleton.at<uchar>(row + 1, col - 1) > 0) +
								128 * (skeleton.at<uchar>(row, col - 1) > 0);

							// Determine whether to remove the pixel
							if ((lut[neighbors] == 1 && first_pass) ||
								(lut[neighbors] == 2 && !first_pass) ||
								(lut[neighbors] == 3)) {
								cleaned_skeleton.at<uchar>(row, col) = 0;

								pixel_removed = true;
							}
						}
					}
				}
				// Update the skeleton image
				cleaned_skeleton.copyTo(skeleton);

			}
		}
		skeleton = skeleton(cv::Rect(1, 1, image.cols, image.rows));

		cv::threshold(skeleton, skeleton, 0, 255, cv::THRESH_BINARY);

		// Returns the final skeleton image with the border removed
		return skeleton;
	}
}