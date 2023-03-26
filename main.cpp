#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

#define NUMBER_OF_COLUMNS 4
#define DISTANCE_IN_TIME 30
#define MIN_TIME_TO_STAY 15
#define NICE_TIME_TO_STAY 60
#define START_TIME 8*60

const vector<string> INFO = { "name", "openingTime", "closingTime", "rank" };

typedef int TimeInMinutes;
typedef pair<TimeInMinutes, TimeInMinutes> TimeRange;

struct Place {
    string name;
    TimeRange opening_range;
    int rank;
    bool visited;
};

typedef vector<Place> Places;

int get_number_of_places(vector<string> input_lines) {
    return input_lines.size() / NUMBER_OF_COLUMNS - 1;
}

vector<int> get_order_of_informations(vector<string> input_lines) {
    vector<int> order_of_informations;

    for (int i = 0; i < NUMBER_OF_COLUMNS; i++)
        for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
            if (INFO[i] == input_lines[j])
                order_of_informations.push_back(j);

    return order_of_informations;
}

TimeInMinutes string_to_minutes(string string_time) {
    return stoi(string_time.substr(0, 2)) * 60 + stoi(string_time.substr(3, 2));
}

Place generate_place(vector<string> input_lines, vector<int> order_of_informations, int i) {
    return {
        input_lines[order_of_informations[0] + i * NUMBER_OF_COLUMNS],
        make_pair(
            string_to_minutes(input_lines[order_of_informations[1] + i * NUMBER_OF_COLUMNS]), 
            string_to_minutes(input_lines[order_of_informations[2] + i * NUMBER_OF_COLUMNS])
        ),
        stoi(input_lines[order_of_informations[3] + i * NUMBER_OF_COLUMNS]),
        false
    };
}

Places generaete_places(vector<string> input_lines) {
    Places places;
    int number_of_places = get_number_of_places(input_lines);
    vector<int> order_of_informations = get_order_of_informations(input_lines);
    for (int i = 1; i <= number_of_places; i++)
        places.push_back(generate_place(input_lines, order_of_informations, i));

    return places;
}

vector<string> get_input_lines(string filename) {
    vector<string> input_lines;
    string curr_line;
    ifstream input_file(filename);
    while (getline(input_file, curr_line)) {
        istringstream line(curr_line);
        string file_element;
        while (getline(line, file_element, ','))
            input_lines.push_back(file_element);
    }
    input_file.close();
    return input_lines;
}

Places read_input(string filename) {
    return generaete_places(get_input_lines(filename));
}

string time_to_string(TimeInMinutes input) {
    stringstream ss;
    ss << setw(2) << std::setfill('0') << (int)(input/60) << ":" << setw(2) << std::setfill('0') << (int)(input%60);
    return ss.str();
}

void output_printer(TimeRange time_range, Place place) {
    cout << "Location " <<  place.name << endl;
    cout << "Visit from" << time_to_string(time_range.first) << " until ";
    cout << time_to_string(time_range.second) << endl << "---" << endl;
}

bool is_place_open(TimeInMinutes start, Place place) {
    return start >= place.opening_range.first && start <= place.opening_range.second;
}

bool is_place_visitable(TimeInMinutes start, Place place) {
    return start + DISTANCE_IN_TIME + MIN_TIME_TO_STAY <= place.opening_range.second;
}

Place change_visit_place(TimeInMinutes start, bool found_place, Place place, Place visit_place) {
    if (!found_place)
        return place;
    if (start >=place.opening_range.first)
        return place.rank < visit_place.rank ? place : visit_place;
    if (place.opening_range.first < visit_place.opening_range.first)
        return place;
    if (place.opening_range.first == visit_place.opening_range.first && place.rank < visit_place.rank)
        return place;
    return visit_place;
}

void change_start_and_finish(TimeRange& time_range, Place place) {
    if (time_range.first + DISTANCE_IN_TIME < place.opening_range.first)
        time_range.first = place.opening_range.first;
    else
        time_range.first += DISTANCE_IN_TIME;

    if (place.opening_range.second - time_range.first >= NICE_TIME_TO_STAY)
        time_range.second = time_range.first + NICE_TIME_TO_STAY;
    else
        time_range.second = place.opening_range.second;
}

void mark_as_visited(Places& places, Place visited_place) {
    for (Place& place : places)
        if (place.name == visited_place.name) {
            place.visited = true;
            return;
        }
}

bool find_visit_place(TimeRange& time_range, Places& places, Place& visit_place) {
    bool found_place = false;
    for (Place place : places)
        if (is_place_open(time_range.first, place) && is_place_visitable(time_range.first, place) && !place.visited) {
            visit_place = change_visit_place(time_range.first, found_place, place, visit_place);
            found_place = true;
        }
    
    if (!found_place)
        for (Place place : places)
            if (time_range.first < place.opening_range.first && is_place_visitable(time_range.first, place) && !place.visited) {
                visit_place = change_visit_place(time_range.first, found_place, place, visit_place);
                found_place = true;
            }

    if (found_place) {   
        mark_as_visited(places, visit_place);
        change_start_and_finish(time_range, visit_place);
    }

    return found_place;
}

int main(int argc, char* argv[]) {
    Places places = read_input(argv[1]);
    TimeRange start_and_finish = make_pair(START_TIME - DISTANCE_IN_TIME, START_TIME);
    Place visit_place;
    while (find_visit_place(start_and_finish, places, visit_place)) {
        output_printer(start_and_finish, visit_place);
        start_and_finish.first = start_and_finish.second;
    }

    return 0;
}