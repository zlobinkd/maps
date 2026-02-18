#include "myXmlParser.h"
#include "myXmlWriter.h"
#include "mapFiltering.h"
#include "guiUtils.h"
#include "benchmark.h"

#include <cmath>
#include <iostream>
#include <chrono>

static int browseMap() {
	auto res = parseXML("path\\to\\file");

	if (!res.has_value())
		return 1;

	auto& [nodes, ways, relations, bounds] = *res;

	Map map{ nodes, ways, relations, bounds };

	nodes.clear();
	ways.clear();
	relations.clear();

	const size_t imageSizeX = 1000;
	const size_t imageSizeY = (size_t)((double)imageSizeX * bounds.aspectRatio());
	cv::Mat image;

	auto boundsCopy = bounds;

	auto callbackData = CallbackData{ &image, boundsCopy, map,
										imageSizeX, imageSizeY };

	updateImage(callbackData);
	cv::imshow("Display window", image);
	cv::setMouseCallback("Display window", onMouseEvent, &callbackData);
	int k = cv::waitKey(0);

	return 0;
}

static int compressXml() {
	const auto res = parseXML("path\\to\\file");
	if (!res.has_value())
		return 1;

	const auto& [nodes, ways, relations, bounds] = *res;
	const auto& [compressedNodes, compressedWays, compressedRelations] = dropUntraversableNodes(nodes, ways, relations);
	xmlWriter writer;
	writer.write("path\\to\\file", compressedNodes, compressedWays, compressedRelations, bounds);
}



static int benchmark() {
	const std::string filename = "path\\to\\file";
	auto res = executeAndShowElapsedTime<std::optional<XmlOutputTuple>>(&parseXML, filename);
	if (!res.has_value())
		return 1;

	auto& [nodes, ways, relations, bounds] = *res;

	auto map = executeAndShowElapsedTime<Map>([&]() { return Map{ nodes, ways, relations, bounds }; });

	nodes.clear();
	ways.clear();
	relations.clear();

	const auto showImage = [&]() {
		const size_t imageSizeX = 1000;
		const size_t imageSizeY = (size_t)((double)imageSizeX * bounds.aspectRatio());
		cv::Mat image;

		auto boundsCopy = bounds;

		auto callbackData = CallbackData{ &image, boundsCopy, map,
										imageSizeX, imageSizeY };
		updateImage(callbackData);
		cv::imshow("Display window", image);
		int k = cv::pollKey(); };

	executeAndShowElapsedTime(showImage);

	executeAndShowElapsedTime([&map]() { map.shortestPath(359797, 645321); });
	
	return 0;
}

int main()
{
	browseMap();
}