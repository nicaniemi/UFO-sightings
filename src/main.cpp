#include <iostream>
#include <vector>
#include <chrono>

#include "sighting.h"
#include "loader.h"
#include "hashgrid.h"
#include "geo.h"
#include "quadtree.h"

using namespace std;

int main()
{
  auto sightings = loadSightings("data/close_encounters_simplified.csv"); // Load and parse CSV file of sightings, store all sightings

  HashGrid grid(1.0); // Initialize the hash grid with cell size of 1 degree
  for (size_t i = 0; i < sightings.size(); i++)
  {
    grid.insertSighting(sightings[i], i); // Insert each sighting into the table
  }

  // initialize quadtree and build it with the sightings
  QuadTree quadtree;
  quadtree.build(sightings);

  // Menu commands

  bool printMenu = true;
  bool endProgram = false;

  cout << endl;
  cout << "Welcome to the Close Encounters Library!" << endl;
  cout << "To get started, make a selection!" << endl;
  cout << endl;
  cout << "1. Search for a sighting using our linear search" << endl;
  cout << "2. Search for a sighting using our hash grid" << endl;
  cout << "3. Search for a sighting using our quadtree" << endl;
  cout << "4. Exit" << endl;
  cout << endl;
  cout << "Input selection: " << " ";

  printMenu = false;

  while (!endProgram)
  {
    if (printMenu)
    {
      cout << "Make another selection, or 4 to exit" << endl;
      cout << endl;
      cout << "1. Search for a sighting using our linear search" << endl;
      cout << "2. Search for a sighting using our hash grid" << endl;
      cout << "3. Search for a sighting using our quadtree" << endl;
      cout << "4. Exit" << endl;
      cout << endl;
      cout << "Input selection: " << " ";
    }

    int selection;
    cin >> selection;

    if (selection == 1) // Baseline linear search
    {
      printMenu = false;
      double lat;
      double lon;
      double radius;
      string shapeFilter;
      int yearFilter;

      cout << "Input latitude of viewpoint (Ex. 0.0): ";
      cin >> lat;
      if (lat < -90 || lat > 90 || !cin) // Codex helped me determine !cin for the last check, I wasn't sure how to verify if the cin was valid or not
      {
        cout << "Error: Invalid latitude!" << endl;
        cout << endl;
        printMenu = true;
        cin.clear();
        cin.ignore();
        continue;
      }
      cout << endl;

      cout << "Input longitude of viewpoint (Ex. 0.0): ";
      cin >> lon;
      if (lon < -180 || lon > 180 || !cin)
      {
        cout << "Error: Invalid longitude!" << endl;
        cout << endl;
        printMenu = true;
        cin.clear();
        cin.ignore();
        continue;
      }

      cout << "Input radius (kilometers) of search (Ex. 120): ";
      cin >> radius;
      if (!cin)
      {
        cout << "Error: Invalid radius!" << endl;
        cout << endl;
        printMenu = true;
        continue;
      }

      cout << "Enter a shape (lowercase) (ALL for no filter): ";
      cin >> shapeFilter;

      cout << "Enter a year of the sighting (0 for no filter): ";
      cin >> yearFilter;
      if (!cin)
      {
        cout << "Error: Invalid year filter" << endl;
        cout << endl;
        printMenu = true;
        cin.clear();
        cin.ignore();
        continue;
      }

      // Codex did help with learning about std::chrono
      auto timeBegin = std::chrono::high_resolution_clock::now();
      auto found = sightingsInDistance(sightings, lat, lon, radius);
      auto timeEnd = std::chrono::high_resolution_clock::now();
      auto timeFinal = (timeEnd - timeBegin) / 1000;

      int count = 0;

      if (found.empty() == true)
      {
        cout << "No sightings found based on these parameters" << endl;
        cout << endl;
      }

      if (found.empty() == false)
      {
        for (int i : found)
        {
          const auto &sighting = sightings[i]; // Pull the Sighting from the vector

          if (shapeFilter != "ALL" && sighting.shape != shapeFilter)
          {
            continue;
          }

          if (yearFilter != 0 && sighting.year != yearFilter)
          {
            continue;
          }

          // Print out sighting information

          cout << "Latitude of sighting: " << sighting.lat << endl;
          cout << "Longitude of sighting: " << sighting.lon << endl;
          cout << "Shape of sighting: " << sighting.shape << endl;
          cout << "Year of sighting: " << sighting.year << endl;
          cout << "Duration of sighting (seconds): " << sighting.duration << endl;
          cout << "City where sighting was reported: " << sighting.city << endl;
          cout << "State where sighting was reported: " << sighting.state << endl;
          cout << "Country where sighting was reported: " << sighting.country << endl;
          cout << endl;
          count += 1;
        }
      }
      cout << "Total sightings in distance: " << found.size() << endl;
      cout << "Total sightings matching filters: " << count << endl;
      cout << "Total time taken to filter through sightings: " << timeFinal.count() << " microseconds" << endl;
      cout << endl;
      printMenu = true;
    }

    if (selection == 2) // Grid search
    {
      printMenu = false;
      double lat;
      double lon;
      double radius;
      string shapeFilter;
      int yearFilter;

      cout << "Input latitude of viewpoint (Ex. 0.0): ";
      cin >> lat;
      if (lat < -90 || lat > 90 || !cin)
      {
        cout << "Error: Invalid Latitude!" << endl;
        cout << endl;
        printMenu = true;
        cin.clear();
        cin.ignore();
        continue;
      }
      cout << endl;

      cout << "Input longitude of viewpoint (Ex. 0.0): ";
      cin >> lon;
      if (lon < -180 || lon > 180 || !cin)
      {
        cout << "Error: Invalid Longitude!" << endl;
        cout << endl;
        printMenu = true;
        cin.clear();
        cin.ignore();
        continue;
      }

      cout << "Input radius (kilometers) of search (Ex. 120): ";
      cin >> radius;
      if (!cin)
      {
        cout << "Error: Invalid Radius!" << endl;
        cout << endl;
        printMenu = true;
        cin.clear();
        cin.ignore();
        continue;
      }

      cout << "Enter a shape (lowercase) (ALL for no filter): ";
      cin >> shapeFilter;

      cout << "Enter a year of the sighting (0 for no filter): ";
      cin >> yearFilter;
      if (!cin)
      {
        cout << "Error: Invalid year filter" << endl;
        cout << endl;
        printMenu = true;
        cin.clear();
        cin.ignore();
        continue;
      }

      auto timeBegin = std::chrono::high_resolution_clock::now();
      auto found = grid.radiusSearch(sightings, lat, lon, radius); // Create a vector of all sightings within radius of viewpoint
      auto timeEnd = std::chrono::high_resolution_clock::now();
      auto timeFinal = (timeEnd - timeBegin) / 1000;

      int count = 0;

      if (found.empty() == true)
      {
        cout << "No sightings found based on these parameters" << endl;
        cout << endl;
      }

      if (found.empty() == false)
      {
        for (int i : found)
        {
          const auto &sighting = sightings[i]; // Pull the Sighting from the vector

          if (shapeFilter != "ALL" && sighting.shape != shapeFilter)
          {
            continue;
          }

          if (yearFilter != 0 && sighting.year != yearFilter)
          {
            continue;
          }

          // Print out sighting information

          cout << "Latitude of sighting: " << sighting.lat << endl;
          cout << "Longitude of sighting: " << sighting.lon << endl;
          cout << "Shape of sighting: " << sighting.shape << endl;
          cout << "Year of sighting: " << sighting.year << endl;
          cout << "Duration of sighting (seconds): " << sighting.duration << endl;
          cout << "City where sighting was reported: " << sighting.city << endl;
          cout << "State where sighting was reported: " << sighting.state << endl;
          cout << "Country where sighting was reported: " << sighting.country << endl;
          cout << endl;
          count += 1;
        }
      }
      cout << "Total sightings in radius: " << found.size() << endl;
      cout << "Total sightings matching filters: " << count << endl;
      cout << "Total time taken to filter through sightings: " << timeFinal.count() << " microseconds" << endl;
      cout << endl;
      printMenu = true;
    }

    if (selection == 3) // Quadtree search
    {
      double lat;
      double lon;
      double radius;
      string shapeFilter;
      int yearFilter;

      cout << "Input latitude of viewpoint (Ex. 0.0): ";
      cin >> lat;
      cout << endl;

      cout << "Input longitude of viewpoint (Ex. 0.0): ";
      cin >> lon;

      cout << "Input radius (kilometers) of search (Ex. 120): ";
      cin >> radius;

      cout << "Enter a shape (lowercase) (ALL for no filter): ";
      cin >> shapeFilter;

      cout << "Enter a year of the sighting (0 for no filter): ";
      cin >> yearFilter;

      auto timeBegin = std::chrono::high_resolution_clock::now();
      auto found = quadtree.radiusSearch(sightings, lat, lon, radius);
      auto timeEnd = std::chrono::high_resolution_clock::now();
      auto timeFinal = (timeEnd - timeBegin) / 1000;

      if (found.empty() == true)
      {
        cout << "No sightings found based on these parameters" << endl;
        cout << endl;
      }

      if (found.empty() == false)
      {
        for (int i : found)
        {
          const auto &sighting = sightings[i];

          if (shapeFilter != "ALL" && sighting.shape != shapeFilter)
            continue;

          if (yearFilter != 0 && sighting.year != yearFilter)
            continue;

          cout << "Latitude of sighting: " << sighting.lat << endl;
          cout << "Longitude of sighting: " << sighting.lon << endl;
          cout << "Shape of sighting: " << sighting.shape << endl;
          cout << "Year of sighting: " << sighting.year << endl;
          cout << "Duration of sighting (seconds): " << sighting.duration << endl;
          cout << "City where sighting was reported: " << sighting.city << endl;
          cout << "State where sighting was reported: " << sighting.state << endl;
          cout << "Country where sighting was reported: " << sighting.country << endl;
          cout << endl;
        }
      }
      cout << "Total sightings in distance: " << found.size() << endl;
      cout << "Total time taken to filter through sightings: " << timeFinal.count() << " microseconds" << endl;
      printMenu = true;
    }

    if (selection == 4)
    {
      cout << "Thank you for using the Close Encounters Library!" << endl;
      printMenu = false;
      endProgram = true;
    }
  }
}