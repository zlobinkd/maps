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
#include <map>

struct TrafficSignalInfo {
	id_t nodeId;
	std::vector<Connection> connections;
	double x;
	double y;
};

static int dumpStreetSignals() {
	std::ofstream file("C:\\Users\\Konstantin\\Desktop\\streetSignalsMoscow.csv");

	if (file.is_open())
	{
		std::map<id_t, TrafficSignalInfo> infos;
		const auto& bounds = MapData::instance().bounds();
		const auto& nodes = MapData::instance().nodes();
		for (const auto& way : MapData::instance().ways())
			for (size_t i = 0; i < way.refs().size(); i++)
			{
				const auto& node = nodes[way.refs()[i]];
				if (node.tagValue("highway") != "traffic_signals")
					continue;

				const auto& [x, y] = node.localCoords(bounds);
				const auto nextNode = i < way.refs().size() - 1 ?
					std::make_optional(nodes[way.refs()[i + 1]]) :
					std::nullopt;
				const auto prevNode = i > 0 ?
					std::make_optional(nodes[way.refs()[i - 1]]) :
					std::nullopt;

				if (prevNode.has_value()) {
					infos[node.id()].nodeId = node.id();
					infos[node.id()].x = x;
					infos[node.id()].y = y;
					infos[node.id()].connections.push_back(Connection{ way.id(),prevNode->id(), node.id() });
				}

				if (way.tagValue("oneway") == "yes")
					continue;

				if (nextNode.has_value()) {
					infos[node.id()].nodeId = node.id();
					infos[node.id()].x = x;
					infos[node.id()].y = y;
					infos[node.id()].connections.push_back(Connection{ way.id(), node.id(), nextNode->id() });
				}
			}

		for (const auto& [_, signalInfo] : infos)
		{
			file << signalInfo.nodeId << " " << signalInfo.x << " " << signalInfo.y * bounds.aspectRatio();
			for (const auto& connection : signalInfo.connections)
			{
				const auto& otherNode = connection.from() != signalInfo.nodeId ? 
					nodes[connection.from()] : nodes[connection.to()];
				const auto& [x, y] = otherNode.localCoords(bounds);
				file << " " << otherNode.id() << " " << x << " " << y * bounds.aspectRatio();
			}
			file << std::endl;
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