# Installation
1. Install visual studio 2022 with at least C++20 and v143
2. Install OpenCV (download a compiled version and define OPENCV_DIR in environment variables)
3. Open maps.sln in VS2022 and press ctrl+B

# Data and Usage
The program uses the OSM xml-like files. Those can be downloaded from the OSM website or its mirror.
1. Go to www.openstreetmap.org
2. Zoom into the map part that needs exporting.
3. Press export -> Overpass API. The path to this file must be set in mapData.cpp
4. Use compressXml() function in main.cpp to remove the irrelevant data and reassign the IDs. I don't remember why, but it's better to do it a couple times (apparently some reassignment bug).
5. Use dumpStreetSignals() function to create a file with traffic signal positions.
6. Cluster the traffic signals with clustering.py. The path to resulting file must be set in mapData.h
7. Use browseMap() function to interactively calculate routes. RMB - set route start/end.

# Features
Map browser, route planer, XML compresser (deletes irrelevant data entries) and benchmark tester.
