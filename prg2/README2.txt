## COMP.CS.300 Project 2, Aki Kankaanpää

### Briefer

This file will contain the README portion of the project's second phase, which includes explanations on the chosen data structures as well as decisions made for the sake of efficiency.

### Data structures

The data is stored in the following two structs:

* Way: Stores it's defining unique ID, name and coordinates, a shared_ptr pointer to a parent if one has been assigned (nullptr when created) as well as a vector (empty when created) filled with weak_ptr pointers (to prevent referencial loops).
* Crossroad_data: Stores the coordinates, current visited-status (true / false), as well as the minimum distance to itself (ultimately not used for it's named purpose)

The datastructures class uses a total of three different main datastructures, which are:

* std::unordered_map<WayID / Coord, std::shared_ptr<Way> / std::shared_ptr<Crossroad_data>: Used to store the Ways as well as Crossroad_data structs, with the key being their unique WayID or Coord depending on the used struct. This was chosen as finding and returning a pointer to an element behind a key when searching with the key has a worst-case of being linear, an average case of being constant, as find() used by get_way is used by several operations in some way. It is also convenient for storing the Crossroad_data, as you can very easily access it on average in constant time when looking for the data using the keys.
* std::unordered_multimap<Coord, std::shared_ptr<Way>>: Used to store the Areas and Places, with the key being their not unique coordinates, so it was necessary to create them as multimaps. Still has an effective search, as if you know the key, you may use std::equal_range to make the search be linear to the amount of elements with the same name / placetype, worstcase still being linear with the container size, and adding a new element to the map is efficient since it is unordered.
* std::vector<std::tuple<Coord, WayID, Distance / std::tuple<Coord, WayID>: Used to return the data asked by the route-functions. The type was defined by the function so the choice was rather obvious, and even with our implimentation of having to reverse it it is still rather inexpensive.

### Relevant Efficiency Choices

* We made the conscious decision to slow down the add_way method to further increase the speed of the other operations by using several data structures.
* The route algorithms both stop their search immediately when a proper result is found, usually avoiding the worst-cases by a long shot.
* Using find() or equal_range(), searching throughout the project is at worst linear, although almost always constant.