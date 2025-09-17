#include "guiUtils.h"
#include "way.h"
#include "node.h"

#include <iostream>

cv::Scalar routeColor(std::string routeName) {
	if (routeName == "footway" || routeName == "path" || routeName == "pedestrian"
		|| routeName == "steps" || routeName == "elevator" || routeName == "platform"
		|| routeName == "bridleway") {
		return BROWN;
	}
	else if (routeName == "motorway" || routeName == "primary") {
		return ORANGE;
	}
	else if (routeName == "motorway_link" || routeName == "primary_link") {
		return YELLOW;
	}
	else if (routeName == "unclassified" || routeName == "tertiary" || routeName == "secondary"
		|| routeName == "secondary_link" || routeName == "tertiary_link" || routeName == "residential") {
		return WHITE;
	}
	else if (routeName == "track" || routeName == "construction" || routeName == "service") {
		return PINK;
	}
	else if (routeName == "cycleway") {
		return GREEN;
	}
	else if (routeName == "trunk") {
		return BLUE;
	}
	else if (routeName == "trunk_link") {
		return CYAN;
	}
	else if (routeName == "living_street") {
		return GRAY;
	}
	return LIGHT_GRAY;
}

bool skipType(const std::string& routeName) {
	if (routeName == "" || routeName == "cycleway"
		|| routeName == "footway" || routeName == "service"
		|| routeName == "pedestrian" || routeName == "track"
		|| routeName == "path" || routeName == "steps"
		|| routeName == "elevator" || routeName == "platform"
		|| routeName == "rest_area" || routeName == "raceway"
		|| routeName == "corridor" || routeName == "bridleway")
	{
		return true;
	}

	return false;
}

static void drawLine(
	cv::Mat& img, 
	const std::vector<id_t>& pointIds, 
	const Map& map, 
	const Bounds& bounds,
	const size_t imageSizeX,
	const size_t imageSizeY,
	const cv::Scalar& color) {
	std::vector<cv::Point> pts;
	for (const id_t ref : pointIds) {
		const auto& node = map.node(ref);
		const auto coords = node.localCoords(bounds);
		const auto x = int(coords[0] * imageSizeX);
		const auto y = int(coords[1] * imageSizeY);

		// wtf is this comment? why is X inverted in the second img
		// opencv refers to horizontal axis as the first coordinate
		// *---->         /\ X
		// |     X        |    Bounds
		// |   opencv     |     Y
		// V              *---->
		//   Y            
		pts.emplace_back(cv::Point{ y, x });
	}

	int ptsSize = pts.size();
	const auto* const d = pts.data();
	cv::polylines(img, &d, &ptsSize, 1, false, color, 3);
}

void updateImage(
	cv::Mat& img,
	const Map& map,
	size_t imageSizeX,
	size_t imageSizeY,
	const Bounds& bounds,
	const std::optional<id_t>& startRoutePt,
	const std::optional<id_t>& endRoutePt,
	const std::vector<id_t>& routeNodes) {
	img = cv::Mat(imageSizeX, imageSizeY, CV_8UC3, LIGHT_GRAY);

	for (const auto& wayId : map.waysToVisualize(bounds)) {
		const auto& way = map.way(wayId);
		if (!way.hasTag("highway"))
			continue;

		drawLine(img, way.refs(), map, bounds, imageSizeX, imageSizeY, routeColor(way.tagValue("highway")));
	}

	if (startRoutePt.has_value()) {
		const auto& startNode = map.node(*startRoutePt);
		const auto coords = startNode.localCoords(bounds);
		const auto x = int(coords[0] * imageSizeX);
		const auto y = int(coords[1] * imageSizeY);
		cv::circle(img, cv::Point(y, x), 5, GREEN, 3);
	}

	if (endRoutePt.has_value()) {
		const auto& endNode = map.node(*endRoutePt);
		const auto coords = endNode.localCoords(bounds);
		const auto x = int(coords[0] * imageSizeX);
		const auto y = int(coords[1] * imageSizeY);
		cv::circle(img, cv::Point(y, x), 5, RED, 3);
	}

	if (routeNodes.size() > 1)
		drawLine(img, routeNodes, map, bounds, imageSizeX, imageSizeY, RED);

	//cv::Mat output;
	// looks better? idk
	//cv::Laplacian(img, output, -1, 1);
	//cv::add(img, output, img);
}

// capture the route start- and end points
// TODO: better logic
static void onMouseRButtonUpEvent(int x, int y, void* userdata) {
	auto* data = static_cast<CallbackData*>(userdata);

	const auto coords = data->bounds.globalCoords(double(y) / data->imageSizeX, double(x) / data->imageSizeY);
	if (!data->startRoutePt.has_value() || data->endRoutePt.has_value())
		data->startRoutePt = data->map.closestPoint(coords[0], coords[1], data->bounds);
	else
		data->endRoutePt = data->map.closestPoint(coords[0], coords[1], data->bounds);

	if (data->startRoutePt.has_value() && data->endRoutePt.has_value())
		data->routeNodes = data->map.shortestPath(data->startRoutePt.value(), data->endRoutePt.value());

	auto& img = *(data->image);
	updateImage(img, data->map, data->imageSizeX, data->imageSizeY, data->bounds, data->startRoutePt, data->endRoutePt, data->routeNodes);
	cv::imshow("Display window", img);
}

// zoom in/out
static void onMouseWheelEvent(int x, int y, int flags, void* userdata) {
	auto* data = static_cast<CallbackData*>(userdata);
	auto& img = *(data->image);
	// x and y are switched here: in opencv, "X" or the first dimension is image columns (horizontal), whereas 
	// for spherical coordinates we use the latitude as the first dimension. (convention in this code)
	//
	// need to reflect the latitude coords, as the image lines are counted downwards and the latitude is counted
	// upwards
	if (flags > 0)
		data->bounds.zoomIn(1. - (double)y / (double)data->imageSizeX, double(x) / (double)data->imageSizeY);
	else
		data->bounds.zoomOut(1. - (double)y / (double)data->imageSizeX, double(x) / (double)data->imageSizeY);

	updateImage(img, data->map, data->imageSizeX, data->imageSizeY, data->bounds, data->startRoutePt, data->endRoutePt, data->routeNodes);
	cv::imshow("Display window", img);
}

// apply a shift to the image
static void onMouseLButtonUpEvent(int x, int y, void* userdata) {
	auto* data = static_cast<CallbackData*>(userdata);
	if (!data->startMoveX.has_value() || !data->startMoveY.has_value())
		return;

	auto& img = *(data->image);

	// x and y are switched here: in opencv, "X" or the first dimension is image columns (horizontal), whereas 
	// for spherical coordinates we use the latitude as the first dimension. (convention in this code)
	//
	// need to reflect the latitude coords, as the image lines are counted downwards and the latitude is counted
	// upwards
	data->bounds.shift(double(y - data->startMoveY.value()) / (double)data->imageSizeX,
					   double(data->startMoveX.value() - x) / (double)data->imageSizeY);
	updateImage(img, data->map, data->imageSizeX, data->imageSizeY, data->bounds, data->startRoutePt, data->endRoutePt, data->routeNodes);
	cv::imshow("Display window", img);

	// reset the shift source point -> no image movement with mouse move event
	data->startMoveX.reset();
	data->startMoveY.reset();
}

// apply image shift (no map update for performance)
static void onMouseMoveEvent(int x, int y, void* userdata) {
	const auto* data = static_cast<CallbackData*>(userdata);
	if (!data->startMoveX.has_value() || !data->startMoveY.has_value())
		return;

	const auto& img = *(data->image);
	cv::Mat tempImg = cv::Mat::zeros(data->image->size(), data->image->type());
	tempImg.setTo(LIGHT_GRAY);

	// if the move is moved left, the ROI moves to the right...
	const int shiftX = data->startMoveX.value() - x;
	const int shiftY = data->startMoveY.value() - y;

	if (std::abs(shiftX) >= data->imageSizeY || std::abs(shiftY) >= data->imageSizeX)
		return;

	// new ROI bounding box (initial image)
	const int endX = std::min(data->image->cols, data->image->cols + shiftX);
	const int endY = std::min(data->image->rows, data->image->rows + shiftY);
	const int startX = std::max(0, shiftX);
	const int startY = std::max(0, shiftY);

	// new ROI bounding box (resulting image)
	const int targetEndX = std::min(data->image->cols, data->image->cols - shiftX);
	const int targetEndY = std::min(data->image->rows, data->image->rows - shiftY);
	const int targetStartX = std::max(0, -shiftX);
	const int targetStartY = std::max(0, -shiftY);

	//     init                    res
	//  -----------            -----------
	// |     |     |          |           |
	// |-----      |    ->    |      -----|
	// |           |          |     |     |
	//  -----------            -----------
	const auto initImageRoi = cv::Rect{ startX, startY, endX - startX, endY - startY };
	const auto resImageRoi = cv::Rect{ targetStartX, targetStartY, targetEndX - targetStartX, targetEndY - targetStartY };
	img(initImageRoi).copyTo(tempImg(resImageRoi));
	cv::imshow("Display window", tempImg);
}

// general callback function
void onMouseEvent(int ev, int x, int y, int flags, void* userdata) {
	if (ev == cv::EVENT_MOUSEWHEEL) {
		onMouseWheelEvent(x, y, flags, userdata);
	}
	else if (ev == cv::EVENT_LBUTTONDOWN) {
		// save the clicked point
		auto* data = static_cast<CallbackData*>(userdata);
		data->startMoveX = x;
		data->startMoveY = y;
	}
	else if (ev == cv::EVENT_LBUTTONUP) {
		onMouseLButtonUpEvent(x, y, userdata);
	}
	else if (ev == cv::EVENT_MOUSEMOVE) {
		onMouseMoveEvent(x, y, userdata);
	}
	else if (ev == cv::EVENT_RBUTTONUP) {
		onMouseRButtonUpEvent(x, y, userdata);
	}
}