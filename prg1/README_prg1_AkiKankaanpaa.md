## COMP.CS.300 Project 1, Aki Kankaanpää

### Briefer

This file will contain the README portion of the project, which includes explanations on the chosen data structures as well as decisions made for the sake of efficiency.

### Data structures

The data is stored in the following two structs:

* Area: Stores it's defining unique ID, name and coordinates, a shared_ptr pointer to a parent if one has been assigned (nullptr when created) as well as a vector (empty when created) filled with weak_ptr pointers (to prevent referencial loops).
* Place: Stores it's defining unique ID, name, type and coordinates.

The datastructures class uses a total of four different main datastructures, which are:

* std::unordered_map<PlaceID / AreaID, std::shared_ptr<Place / Area>>: Used to store the Areas and Places, with the key being their unique PlaceID or AreaID depending on the used struct. This was chosen as finding and returning a pointer to an element behind a key when searching with the key has a worst-case of being linear, although has an average case of being constant, as find() through get_place() / get_area() is used by most operations in some way, and adding a new element to the map is efficient since it is unordered.
* std::unordered_multimap<PlaceID, std::shared_ptr<Place>>: Used to store the Areas and Places, with the key being their not-unique Name or PlaceType, so it was necessary to create them as multimaps. Still has an effective search, as if you know the key, you may use std::equal_range to make the search be linear to the amount of elements with the same name / placetype, worstcase still being linear with the container size, and adding a new element to the map is efficient since it is unordered.
* std::vector<PlaceID>: Used to store the IDs of places, more importantly in order after being sorted. The sorted-condition is kept up by the coordinate_sorted_ and alphabetical_sorted_ boolean flags. Sorting this vector has a complexity of n log n, as shown by the last chosen data structure
* std::multimap<Coord / Name, std::shared_ptr<Place>>: Used to sort the vectors, as the sorting requires you to sort it through the unordered map where the data is originally sorted by it's key (Name and Coord, coord has an inline defined <-operator to use for this operation) and then transferred onto a vector. Only used within the sorting methods places_coord_order and places_alphabetically, and is deleted after it has fully been run.

### Relevant Efficiency Choices

* We made the conscious decision to slow down the add_place method by having several maps that we could use at a whim to make some of the other methods, such as finding with the key being a name or type, faster.
* Both of the sorting algorithms for the places have been made so that if the data is wished to be given, it keeps the current order and only sorts it if any relevant data has been changed.
* Using find() or equal_range() and iterating through it, most of the searching functions have been able to be in the worst case linear, most often constant.