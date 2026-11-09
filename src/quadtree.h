#pragma once
#ifndef PROJECT2_QUADTREE_H
#define PROJECT2_QUADTREE_H

#include <vector>
#include "sighting.h"

// Axis aligned bounding box for a region of lat-lon space
struct BoundingBox
{
    double minLat, maxLat;
    double minLon, maxLon;

    BoundingBox(double minLat, double maxLat, double minLon, double maxLon)
        : minLat(minLat), maxLat(maxLat), minLon(minLon), maxLon(maxLon) {}

    bool contains(double lat, double lon) const;
    bool intersects(const BoundingBox& other) const;
};

class QuadTree
{
    static constexpr int MAX_CAPACITY = 4; // Max sightings per node before splitting
    static constexpr int MAX_DEPTH = 20; // Max height of the tree - 1 (doesn't include root)

    struct Node
    {
        BoundingBox bounds;
        std::vector<int> indices;   // Indices into the master sightings vector
        Node* children[4];          // NW, NE, SW, SE
        bool divided;

        explicit Node(BoundingBox bounds);
        ~Node();
    };

    Node* root;
    const std::vector<Sighting>* sightings; 

    void insert(Node* node, int index, int depth);
    void subdivide(Node* node);
    void radiusSearch(const Node* node, double lat, double lon,
                      double radius, std::vector<int>& results) const;

public:
    QuadTree();
    ~QuadTree();

    void build(const std::vector<Sighting>& sightings);
    void insertSighting(const Sighting& sighting, int index);
    std::vector<int> radiusSearch(const std::vector<Sighting>& sightings,
                                  double lat, double lon, double radius) const;
};

#endif // PROJECT2_QUADTREE_H