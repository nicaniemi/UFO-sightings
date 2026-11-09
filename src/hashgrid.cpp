#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>

#include "hashgrid.h"
#include "geo.h"

// HashGrid constructor, we ended up changing our mind so user does not define cellSize anymore, so main automatically initializes to 1
HashGrid::HashGrid(double cellSize) : cellSize(cellSize) // Initialize cell size, ensure greater than 0
{
  if (cellSize <= 0)
  {
    throw std::invalid_argument("Cell size must be greater than 0.");
  }
}

std::string HashGrid::coordToCellHelper(const double lat, const double lon) const // Calculate the cell a coordinate is in
{

  if (lat < -90 || lat > 90) // Check latitude bounds
    throw std::invalid_argument("Latitude out of bounds.");
  if (lon < -180 || lon > 180) // Check longitude bounds
    throw std::invalid_argument("Longitude out of bounds.");

  // Convert cell x and y to a whole number, remove decimals
  int x = static_cast<int>(std::floor(lat / cellSize));
  int y = static_cast<int>(std::floor(lon / cellSize));

  const std::string xCoordinate = std::to_string(x); // Coordinates stored as strings
  const std::string yCoordinate = std::to_string(y);

  return xCoordinate + "," + yCoordinate;
}

void HashGrid::insertSighting(const Sighting &sighting, const int index) // Insert sighting into table
{
  std::string coord = coordToCellHelper(sighting.lat, sighting.lon); // Get coordinates of sighting
  cells[coord].push_back(index); // Append index of sighting
}

std::vector<int> HashGrid::findCell(const double lat, const double lon) const // See if a sighting exists in the table in a specified coordinate
{
  std::string const coord = coordToCellHelper(lat, lon); // Get coordinate of sighting

  auto const doesExist = cells.find(coord); // Does the sighting exist

  if (doesExist == cells.end()) // If sighting not in the table, return empty vector
    return {};
  else // If this sighting is found, return the index it's stored at
  {
    return doesExist->second;
  }
}

std::vector<int> HashGrid::radiusSearch(const std::vector<Sighting> &sightings, double lat, double lon, double radius) const
{

  if (radius < 0) // Ensure valid radius
  {
    throw std::invalid_argument("Radius must be greater than or equal to 0.");
  }

  if (lat < -90 || lat > 90)
    throw std::invalid_argument("Latitude out of bounds.");
  if (lon < -180 || lon > 180)
    throw std::invalid_argument("Longitude out of bounds.");

  double latChange = (radius / 6371) * (180 / M_PI); // Convert latChange to degrees

  double minLat = lat - latChange; // Southern edge of search
  if (minLat < -90)
    minLat = -90;
  double maxLat = lat + latChange; // Northern edge of search
  if (maxLat > 90)
    maxLat = 90;

  double minLon;
  double maxLon;

  if (minLat == -90 || maxLat == 90) // If latitudes are min or max valued, lon also automatically set to min and max values
  {
    minLon = -180;
    maxLon = 180;
  }

  else
  {
    auto val = sin(radius/6371) / cos(toRadians(lat)); // Compute val for lonChange

    // Clamp val values to prevent out of range error for arcsin
    if (val < -1.0)
      val = -1.0;
    if (val > 1.0)
      val = 1.0;

    auto lonChange = asin(val); // Take arcsin of val

    lonChange = lonChange * 180.0 / M_PI; // Convert to degrees

    minLon = lon - lonChange; // Western edge of search
    maxLon = lon + lonChange; // Eastern edge of search
  }

  if (minLon < - 180 || maxLon > 180) // If the min or max lon goes past the min/max value, set both to the min/max value
  {
    minLon = -180;
    maxLon = 180;
  }

  // Convert lat and lon into cell coordinates, static cast to int for hash table searching
  int minLatCell = static_cast<int>(std::floor(minLat / cellSize));
  int maxLatCell = static_cast<int>(std::floor(maxLat / cellSize));
  int minLonCell = static_cast<int>(std::floor(minLon / cellSize));
  int maxLonCell = static_cast<int>(std::floor(maxLon / cellSize));

  std::vector<int> sightingsInRadius; // Create a vector to store all sightings that are in the table and within the given radius as indices

  for (int i = minLatCell; i <= maxLatCell; i++)
  {
    for (int j = minLonCell; j <= maxLonCell; j++)
    {
      auto cellCoord = std::to_string(i) + "," + std::to_string(j); // Create the cell key for the cell we're checking

      const auto found = cells.find(cellCoord); // Check to see if the cell exists in the table

      if (found != cells.end()) // If the cell does exist
      {
        const auto& foundCell = found->second; // Vector of sightings inside the cell

        for (std::size_t k = 0; k < foundCell.size(); k++)
        {
          auto current = foundCell[k]; // Index of the sighting
          const auto dist = haversineKm(sightings[current].lat, sightings[current].lon, lat, lon); // Calculate distance from viewpoint to sighting
          if (dist <= radius) // If the distance of the sighting is within the radius we want to check
          {
            sightingsInRadius.push_back(current); // Add sighting to the vector
          }
        }
      }
    }
  }
  return sightingsInRadius;
}