#include "myXmlParser.h"
#include "myXmlWriter.h"
#include "mapFiltering.h"
#include "guiUtils.h"
#include "benchmark.h"
#include "mapData.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>

static int dumpStreetSignals() {
	std::ofstream file("path\\to\\file");

	if (file.is_open())
	{
		for (const auto& node : MapData::instance().nodes())
			if (node.hasTag("highway") && node.tagValue("highway") == "traffic_signals")
			{
				const auto& bounds = MapData::instance().bounds();
				const auto& coords = node.localCoords(bounds);
				file << node.id() << " " << coords[0] << " " << coords[1] * bounds.aspectRatio() << std::endl;
			}

		return 0;
	}
	return 1;
}

static int browseMap() {
	Map map;

	auto boundsCopy = MapData::instance().bounds();

	const size_t imageSizeX = 1000;
	const size_t imageSizeY = (size_t)((double)imageSizeX * boundsCopy.aspectRatio());
	cv::Mat image;

	auto callbackData = CallbackData{ &image, boundsCopy, map,
										imageSizeX, imageSizeY };

	updateImage(callbackData);
	cv::imshow("Display window", image);
	cv::setMouseCallback("Display window", onMouseEvent, &callbackData);
	int k = cv::waitKey(0);

	return 0;
}

static int compressXml() {
	const auto& [compressedNodes, compressedWays, compressedRelations] = dropUntraversableNodes(
		MapData::instance().nodes(), MapData::instance().ways(), MapData::instance().relations());
	xmlWriter writer;
	writer.write("path\\to\\file", compressedNodes, compressedWays, compressedRelations, MapData::instance().bounds());
}



static int benchmark() {
	// ?..
	const std::string filename = "path\\to\\file";
	auto res = executeAndShowElapsedTime<std::optional<XmlOutputTuple>>(&parseXML, filename);
	if (!res.has_value())
		return 1;

	auto& [nodes, ways, relations, bounds] = *res;

	auto map = executeAndShowElapsedTime<Map>([&]() { return Map{}; });

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