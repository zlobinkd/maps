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