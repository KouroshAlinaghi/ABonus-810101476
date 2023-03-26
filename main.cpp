#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

const vector<string> INFO = { "name", "openingTime", "closingTime", "rank" };
const int NUMBER_OF_COLUMNS = 4;
const int DISTANCE_IN_TIME = 30;
const int MIN_TIME_TO_STAY = 15;
const int NICE_TIME_TO_STAY = 60;
const int START_TIME = 8*60;

struct Place {
    string name;
    int opening_time, closing_time, rank;
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

int string_to_minutes(string string_time) {
    return stoi(string_time.substr(0, 2)) * 60 + stoi(string_time.substr(3, 2));
}

Place generate_place(vector<string> input_lines, vector<int> order_of_informations, int i) {
    return {
        input_lines[order_of_informations[0] + i * NUMBER_OF_COLUMNS],
        string_to_minutes(input_lines[order_of_informations[1] + i * NUMBER_OF_COLUMNS]),
        string_to_minutes(input_lines[order_of_informations[2] + i * NUMBER_OF_COLUMNS]),
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

string minutes_to_string(int input) {
    string result = "";
    int hours = input / 60;
    int minutes = input % 60;
    if (hours >= 10)
        result += to_string(hours);
    else {
        result += "0";
        result += to_string(hours);
    }
    result += ":";
    if (minutes >= 10)
        result += to_string(minutes);
    else {
        result += "0";
        result += to_string(minutes);
    }
    
    return result;
}

void output_printer(int start, int finish, Place place) {
    cout << "Location " <<  place.name << endl;
    cout << "Visit from" << minutes_to_string(start) << " until ";
    cout << minutes_to_string(finish) << endl << "---" << endl;
}

bool is_place_open(int start, Place place) {
    return start >= place.opening_time && start <= place.closing_time;
}

bool is_place_visitable(int start, Place place) {
    return start + DISTANCE_IN_TIME + MIN_TIME_TO_STAY <= place.closing_time;
}

Place change_visit_place(int start, bool found_place, Place place, Place visit_place) {
    if (!found_place)
        return place;
    if (start >= place.opening_time)
        return place.rank < visit_place.rank ? place : visit_place;
    if (place.opening_time < visit_place.opening_time)
        return place;
    if (place.opening_time == visit_place.opening_time && place.rank < visit_place.rank)
        return place;
    return visit_place;
}

void change_start_and_finish(int& start, int& finish, Place place) {
    if (start + DISTANCE_IN_TIME < place.opening_time)
        start = place.opening_time;
    else
        start += DISTANCE_IN_TIME;

    if (place.closing_time - start >= NICE_TIME_TO_STAY)
        finish = start + NICE_TIME_TO_STAY;
    else
        finish = place.closing_time;
}

void mark_as_visited(Places& places, Place visited_place) {
    for (Place& place : places)
        if (place.name == visited_place.name) {
            place.visited = true;
            return;
        }
}

bool find_visit_place(int& start, int& finnish, Places& places, Place& visit_place) {
    bool found_place = false;
    for (Place place : places)
        if (is_place_open(start,place) && is_place_visitable(start, place) && !place.visited) {
            visit_place = change_visit_place(start,found_place, place, visit_place);
            found_place = true;
        }
    
    if (!found_place)
        for (Place place : places)
            if (start < place.opening_time && is_place_visitable(start, place) && !place.visited) {
                visit_place = change_visit_place(start,found_place, place, visit_place);
                found_place = true;
            }

    if (found_place) {   
        mark_as_visited(places, visit_place);
        change_start_and_finish(start, finnish, visit_place);
    }

    return found_place;
}

int main(int argc, char* argv[]) {
    Places places = read_input(argv[1]);
    int finish = START_TIME, start = START_TIME - DISTANCE_IN_TIME;
    Place visit_place;
    while (find_visit_place(start, finish, places, visit_place)) {
        output_printer(start, finish, visit_place);
        start = finish;
    }

    return 0;
}