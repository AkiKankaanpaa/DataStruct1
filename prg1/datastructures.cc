// Datastructures.cc

#include "datastructures.hh"
#include <cmath>
#include <random>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

Datastructures::Datastructures():
    coordinate_sorted_(false),
    alphabetical_sorted_(false),
    alphabetical_vector_ids_({}),
    coordinate_vector_ids_({}),
    places_by_id_({}),
    places_by_name_({}),
    places_by_type_({})
{
}

Datastructures::~Datastructures()
{
}

int Datastructures::place_count()
{
    return places_by_id_.size();
}

void Datastructures::clear_all()
{
    places_by_id_.clear();
    places_by_name_.clear();
    places_by_type_.clear();
    areas_by_id_.clear();
    alphabetical_sorted_ = false;
    coordinate_sorted_ = false;
}

std::vector<PlaceID> Datastructures::all_places()
{
    std::vector<PlaceID> place_vector = {};
    for (auto it = places_by_id_.begin(); it != places_by_id_.end(); it++) {
        place_vector.push_back(it->first);
    }
    return place_vector;
}

bool Datastructures::add_place(PlaceID id, const Name& name, PlaceType type, Coord xy)
{
    auto found_place = get_place(id);
    // Making sure that no place already exists with the same PlaceID
    if (found_place != nullptr) {
        return false;
    }
    auto added_place = std::make_shared<Place>(id, name, type, xy);

    places_by_id_.insert({id, added_place});
    places_by_name_.insert({name, added_place});
    places_by_type_.insert({type, added_place});

    // Since adding a value changes all Place-relevant datastructures, raise both flags
    coordinate_sorted_ = false;
    alphabetical_sorted_ = false;
    return true;
}

std::pair<Name, PlaceType> Datastructures::get_place_name_type(PlaceID id)
{
    std::shared_ptr<Place> wanted_place = get_place(id);
    if (wanted_place == nullptr) {
        return {NO_NAME, PlaceType::NO_TYPE};
    }
    return {wanted_place->name, wanted_place->type};
}

Coord Datastructures::get_place_coord(PlaceID id)
{
    std::shared_ptr<Place> wanted_place = get_place(id);
    if (wanted_place == nullptr) {
        return NO_COORD;
    }
    return wanted_place->coordinates;
}

bool Datastructures::add_area(AreaID id, const Name &name, std::vector<Coord> coords)
{
    auto found_area = get_area(id);
    if (found_area != nullptr) {
        return false;
    }
    auto added_area = std::make_shared<Area>(id, name, coords);
    areas_by_id_.insert({id, added_area});
    return true;
}

Name Datastructures::get_area_name(AreaID id)
{
    std::shared_ptr<Area> wanted_area = get_area(id);
    if (wanted_area == nullptr) {
        return NO_NAME;
    }
    return wanted_area->name;
}

std::vector<Coord> Datastructures::get_area_coords(AreaID id)
{
    std::shared_ptr<Area> wanted_area = get_area(id);
    if (wanted_area == nullptr) {
        return {NO_COORD};
    }
    return wanted_area->coordinates;
}

void Datastructures::creation_finished()
{
    // Currently this has no purpose
}


std::vector<PlaceID> Datastructures::places_alphabetically()
{
    if (!alphabetical_sorted_) {
        alphabetical_vector_ids_ = {};
        std::multimap<Name, std::shared_ptr<Place>> temp_map = {};
        // First pushing to temp_map which automatically becomes sorted
        for (auto it = places_by_id_.begin(); it != places_by_id_.end(); ++it) {
            temp_map.insert({it->second->name, it->second });
        }
        // Then simply pushing the ids in order to the vector
        for(auto it = temp_map.begin(); it != temp_map.end(); ++it) {
            alphabetical_vector_ids_.push_back(it->second->id);
        }
        alphabetical_sorted_ = true;
    }
    return alphabetical_vector_ids_;
}

std::vector<PlaceID> Datastructures::places_coord_order()
{
    if (!coordinate_sorted_) {
        coordinate_vector_ids_ = {};
        std::multimap<Coord, std::shared_ptr<Place>> temp_map = {};
        // First pushing to temp_map which automatically becomes sorted
        for (auto it = places_by_id_.begin(); it != places_by_id_.end(); ++it) {
            temp_map.insert({it->second->coordinates, it->second });
        }
        // Then simply pushing the ids in order to the vector
        for(auto it = temp_map.begin(); it != temp_map.end(); ++it) {
            coordinate_vector_ids_.push_back(it->second->id);
        }
        coordinate_sorted_ = true;
    }
    return coordinate_vector_ids_;
}

std::vector<PlaceID> Datastructures::find_places_name(Name const& name)
{
    std::vector<PlaceID> found_places;
    auto id_iterator_pair = places_by_name_.equal_range(name);
    for (auto it = id_iterator_pair.first; it != id_iterator_pair.second; ++it) {
        found_places.push_back(it->second->id);
    }
    return found_places;
}

std::vector<PlaceID> Datastructures::find_places_type(PlaceType type)
{
    std::vector<PlaceID> found_places;
    auto id_iterator_pair = places_by_type_.equal_range(type);
    for (auto it = id_iterator_pair.first; it != id_iterator_pair.second; ++it) {
        found_places.push_back(it->second->id);
    }
    return found_places;
}

bool Datastructures::change_place_name(PlaceID id, const Name& newname)
{
    auto found_place = get_place(id);
    if (found_place == nullptr) {
        return false;
    }

    Name old_name = found_place->name;
    auto id_iterator_pair = places_by_name_.equal_range(old_name);
    found_place->name = newname;

    for (auto it = id_iterator_pair.first; it != id_iterator_pair.second; ++it) {
        if (it->second->id == id) {
            auto wanted_element = places_by_name_.extract(it);
            wanted_element.key() = newname;
            places_by_name_.insert(std::move(wanted_element));
            break;
        }
    }
    alphabetical_sorted_ = false;
    return true;
}

bool Datastructures::change_place_coord(PlaceID id, Coord newcoord)
{
    auto found_place = get_place(id);
    if (found_place == nullptr) {
        return false;
    }

    found_place->coordinates = newcoord;
    coordinate_sorted_ = false;
    return true;
}

std::vector<AreaID> Datastructures::all_areas()
{
    std::vector<AreaID> area_vector = {};
    for (auto it = areas_by_id_.begin(); it != areas_by_id_.end(); it++) {
        area_vector.push_back(it->first);
    }
    return area_vector;
}

bool Datastructures::add_subarea_to_area(AreaID id, AreaID parentid)
{
    auto found_subarea = get_area(id);
    auto found_parent_area = get_area(parentid);
    if (found_subarea == nullptr || found_parent_area == nullptr) {
        return false;
    }
    // Done separately to prevent segmentation fault if found_subarea points to a nullptr
    if (found_subarea->parent_area != nullptr) {
        return false;
    }

    found_subarea->parent_area = found_parent_area;
    found_parent_area->subareas.push_back(found_subarea);
    return true;
}

std::vector<AreaID> Datastructures::subarea_in_areas(AreaID id)
{
    auto found_area = get_area(id);
    if (found_area == nullptr) {
        return {NO_AREA};
    }

    std::vector<AreaID> parents = {};
    // Going through all of the parents and adding them to parents vector
    while (found_area->parent_area != nullptr) {
        found_area = found_area->parent_area;
        parents.push_back(found_area->id);
    }
    return parents;
}

std::vector<PlaceID> Datastructures::places_closest_to(Coord xy, PlaceType type)
{
    std::unordered_multimap<PlaceType, std::shared_ptr<Place>>::iterator beginning;
    std::unordered_multimap<PlaceType, std::shared_ptr<Place>>::iterator ending;

    if(type == PlaceType::NO_TYPE){
        beginning = places_by_type_.begin();
        ending = places_by_type_.end();
    } else {
        auto it_range = places_by_type_.equal_range(type);
        beginning = it_range.first;
        ending = it_range.second;
    }

    std::pair<double, std::shared_ptr<Place>> first_closest = {NO_DISTANCE, nullptr};
    std::pair<double, std::shared_ptr<Place>> second_closest = {NO_DISTANCE, nullptr};
    std::pair<double, std::shared_ptr<Place>> third_closest = {NO_DISTANCE, nullptr};

    // During the loop, we first check if the closest-pairs yet have a value, and if they do, we compare them one by one to the value to determine if
    // the place should be within the top three and change the closest-pairs accordingly
    for(auto it = beginning; it != ending; it++){
        double distance = calculate_euclidean({xy.x - it->second->coordinates.x, xy.y - it->second->coordinates.y});
        if (first_closest.second == nullptr){
            first_closest = {distance, it->second};
        } else if(distance < first_closest.first || (distance == first_closest.first && first_closest.second->coordinates.y > it->second->coordinates.y)){
            third_closest = second_closest;
            second_closest = first_closest;
            first_closest = {distance, it->second};
        } else if (second_closest.second == nullptr){
            second_closest = {distance, it->second};
        } else if (distance < second_closest.first || (distance == second_closest.first && second_closest.second->coordinates.y > it->second->coordinates.y)){
            third_closest = second_closest;
            second_closest = {distance, it->second};
        } else if (third_closest.second == nullptr){
            third_closest = {distance, it->second};
        } else if (distance < third_closest.first || (distance == third_closest.first && third_closest.second->coordinates.y > it->second->coordinates.y)){
            third_closest = {distance, it->second};
        }
    }

    std::vector<PlaceID> close_places_in_order = {};
    if (third_closest.second != nullptr){
        close_places_in_order.push_back(first_closest.second->id);
        close_places_in_order.push_back(second_closest.second->id);
        close_places_in_order.push_back(third_closest.second->id);
    } else if (second_closest.second != nullptr){
        close_places_in_order.push_back(first_closest.second->id);
        close_places_in_order.push_back(second_closest.second->id);
    } else if (first_closest.second != nullptr){
        close_places_in_order.push_back(first_closest.second->id);
    }

    return close_places_in_order;
}

bool Datastructures::remove_place(PlaceID id)
{
    std::shared_ptr<Place> to_be_removed = get_place(id);
    if (to_be_removed == nullptr) {
        return false;
    }

    // Only iterating over a specific key in the multimaps
    auto name_range = places_by_name_.equal_range(to_be_removed->name);
    for (auto name_iter = name_range.first; name_iter != name_range.second; name_iter++) {
        if (name_iter->second->id == id) {
            places_by_name_.erase(name_iter);
            break;
        }
    }

    auto type_range = places_by_type_.equal_range(to_be_removed->type);
    for (auto type_iter = type_range.first; type_iter != type_range.second; type_iter++) {
        if (type_iter->second->id == id) {
            places_by_type_.erase(type_iter);
            break;
        }
    }

    places_by_id_.erase(id);
    coordinate_sorted_ = false;
    alphabetical_sorted_ = false;
    return true;
}

std::vector<AreaID> Datastructures::all_subareas_in_area(AreaID id)
{
    auto parent_area = get_area(id);
    if (parent_area == nullptr) {
        return {NO_AREA};
    }
    return get_children(parent_area);
}

AreaID Datastructures::common_area_of_subareas(AreaID id1, AreaID id2)
{
    auto first_area = get_area(id1);
    auto second_area = get_area(id2);
    if (first_area == nullptr || second_area == nullptr) {
        return NO_AREA;
    }

    auto first_parent_pointer = first_area->parent_area;
    auto second_parent_pointer = second_area->parent_area;
    std::vector<std::shared_ptr<Area>> first_parents;
    std::vector<std::shared_ptr<Area>> second_parents;

    // Pushing all of the parents one by one into the vectors first_parents and second_parents
    while (first_parent_pointer != nullptr) {
        first_parents.push_back(first_parent_pointer);
        first_parent_pointer = first_parent_pointer->parent_area;
    }

    while (second_parent_pointer != nullptr) {
        second_parents.push_back(second_parent_pointer);
        second_parent_pointer = second_parent_pointer->parent_area;
    }

    // Check for the first value that differs between the parents, starting from the root
    auto result = std::mismatch(first_parents.rbegin(), first_parents.rend(),
                                second_parents.rbegin(), second_parents.rend());

    // If the first value is different, the two areas have a different root and by so do not have a common area
    if (result.first == first_parents.rbegin()) {
        return NO_AREA;
    }
    // Change to the common parent instead of the first differing value by going back by one
    --result.first;
    return result.first->get()->id;
}

std::shared_ptr<Place> Datastructures::get_place(PlaceID id) {
    auto search_by_id = places_by_id_.find(id);
    if (search_by_id == places_by_id_.end()) {
        return nullptr;
    }
    return search_by_id->second;
}

std::shared_ptr<Area> Datastructures::get_area(AreaID id)
{
    auto search_by_id = areas_by_id_.find(id);
    if (search_by_id == areas_by_id_.end()) {
        return nullptr;
    }
    return search_by_id->second;
}

// This function is called recursively to find out all of the children
std::vector<AreaID> Datastructures::get_children(std::weak_ptr<Area> current_area)
{
    std::vector<AreaID> subareas = {};
    for (auto child: current_area.lock()->subareas) {
        // Add the child to the subareas vector
        subareas.push_back(child.lock()->id);
        // And then return all of the subareas of the child and push them into subareas vector
        auto children_subareas = get_children(child);
        for (auto child: children_subareas) {
            subareas.push_back(child);
        }
    }
    return subareas;
}

double calculate_euclidean(Coord coord)
{
    return std::sqrt(std::pow(coord.x, 2) + std::pow(coord.y, 2));
}
