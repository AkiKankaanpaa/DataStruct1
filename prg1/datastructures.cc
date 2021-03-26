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

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures():
    coordinate_sorted_(false),
    alphabetical_sorted_(false),
    alphabetical_vector_ids_({}),
    coordinate_vector_ids_({}),
    places_by_id_({}),
    places_by_name_({}),
    places_by_type_({})
{
    // Replace this comment with your implementation
}

Datastructures::~Datastructures()
{
    // Replace this comment with your implementation
}

int Datastructures::place_count()
{
    return places_by_id_.size();
}

void Datastructures::clear_all()
{
    places_by_id_.clear();
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
    // Making sure that no area already exists with the same AreaID
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
    // Replace this comment with your implementation
    // NOTE!! It's quite ok to leave this empty, if you don't need operations
    // that are performed after all additions have been done.
}


std::vector<PlaceID> Datastructures::places_alphabetically()
{
    if (!alphabetical_sorted_) {
        alphabetical_vector_ids_ = {};
        std::multimap<Name, std::shared_ptr<Place>> temp_map = {};
        for (auto it = places_by_id_.begin(); it != places_by_id_.end(); ++it) {
            temp_map.insert({it->second->name, it->second });
        }
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
        for (auto it = places_by_id_.begin(); it != places_by_id_.end(); ++it) {
            temp_map.insert({it->second->coordinates, it->second });
        }
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
        }
    }
    alphabetical_sorted_ = false;
    return true;
}

bool Datastructures::change_place_coord(PlaceID id, Coord newcoord)
{
    auto found_value = places_by_id_.find(id);
    if (found_value == places_by_id_.end()) {
        return false;
    }
    found_value->second->coordinates = newcoord;
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
    auto found_subarea = areas_by_id_.find(id);
    auto found_parent_area = areas_by_id_.find(parentid);
    auto end_of_areas = areas_by_id_.end();
    if (found_subarea == end_of_areas || found_parent_area == end_of_areas) {
        return false;
    }
    if (found_subarea->second->parent_area != nullptr) {
        return false;
    }

    found_subarea->second->parent_area = found_parent_area->second;
    found_parent_area->second->subareas.push_back(found_subarea->second);
    return true;
}

std::vector<AreaID> Datastructures::subarea_in_areas(AreaID id)
{
    auto found_area = get_area(id);
    if (found_area == nullptr) {
        return {NO_AREA};
    }

    std::vector<AreaID> higher_areas = {};
    while (found_area->parent_area != nullptr) {
        found_area = found_area->parent_area;
        higher_areas.push_back(found_area->id);
    }
    return higher_areas;
}

std::vector<PlaceID> Datastructures::places_closest_to(Coord xy, PlaceType type)
{
    // Replace this comment with your implementation
    return {};
}

bool Datastructures::remove_place(PlaceID id)
{
    // Replace this comment with your implementation
    return false;
}

std::vector<AreaID> Datastructures::all_subareas_in_area(AreaID id)
{
    // Replace this comment with your implementation
    return {NO_AREA};
}

AreaID Datastructures::common_area_of_subareas(AreaID id1, AreaID id2)
{
    // Replace this comment with your implementation
    return NO_AREA;
}

std::shared_ptr<Place> Datastructures::get_place(PlaceID id) {
    auto search_by_id = places_by_id_.find(id);
    if (search_by_id == places_by_id_.end()) {
        return nullptr;
    }
    return search_by_id->second;
}

std::shared_ptr<Area> Datastructures::get_area(PlaceID id)
{
    auto search_by_id = areas_by_id_.find(id);
    if (search_by_id == areas_by_id_.end()) {
        return nullptr;
    }
    return search_by_id->second;
}

double calculate_euclidean(Coord coord)
{
    return std::sqrt(std::pow(coord.x, 2) + std::pow(coord.y, 2));
}
