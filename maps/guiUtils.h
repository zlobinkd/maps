#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <optional>

#include "bounds.h"
#include "node.h"
#include "way.h"
#include "map.h"

struct CallbackData {
	cv::Mat* image;
	Bounds& bounds;
	Map& map;
	const size_t imageSizeX;
	const size_t imageSizeY;
	// for shift
	std::optional<int> startMoveX;
	std::optional<int> startMoveY;
	// route
	std::optional<id_t> startRoutePt;
	std::optional<id_t> endRoutePt;
	std::vector<id_t> routeNodes;
};

cv::Scalar routeColor(std::string routeName);

bool skipType(const std::string& routeName);

// TODO: pack arguments to CallbackData
void updateImage(
	cv::Mat& img,
	const Map& map,
	size_t imageSizeX,
	size_t imageSizeY,
	const Bounds& bounds,
	const std::optional<id_t>& startRoutePt,
	const std::optional<id_t>& endRoutePt, 
	const std::vector<id_t>& routeNodes);

void onMouseEvent(int ev, int x, int y, int flags, void* userdata);

const cv::Scalar BLACK = { 0, 0, 0 };
const cv::Scalar RED = { 0, 0, 255 };
const cv::Scalar GREEN = { 0, 255, 0 };
const cv::Scalar BLUE = { 255, 0, 0 };
const cv::Scalar WHITE = { 255, 255, 255 };
const cv::Scalar GRAY = { 127, 127, 127 };
const cv::Scalar LIGHT_GRAY = { 175, 175, 175 };
const cv::Scalar BROWN = { 33, 67, 101 };
const cv::Scalar LIGHT_BROWN = { 87, 122, 185 };
const cv::Scalar ORANGE = { 0, 165, 255 };
const cv::Scalar YELLOW = { 128, 213, 255 };
const cv::Scalar BIEGE = { 176, 228, 239 };
const cv::Scalar PINK = { 203, 192, 255 };
const cv::Scalar CYAN = { 230, 216, 173 };
const cv::Scalar PURPLE = { 163, 73, 163 };
const cv::Scalar DARK_RED = { 21, 0, 136 };
