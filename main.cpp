#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

using namespace std;

const string LOCK = "Location ";
const string NAME = "name";
const string OPENING_TIME = "openingTime";
const string CLOSING_TIME = "closingTime";
const string RANK = "rank";
const string END_PRINT = "---";
const string VISIT_FROM = "Visit from ";
const string UNTIL = " until ";

struct Place {
    string name;
    int opening_time, closing_time, rank;
    bool visited;
};

typedef vector<Place> Places;

int get_number_of_places(vector<string> input_lines) {
    int num_of_places = input_lines.size() / 4 - 1;
    return num_of_places;
}

vector<int> get_order_of_informations(vector<string> input_lines) {
    vector<int> order_of_informations;
    vector<string> info = { NAME, OPENING_TIME, CLOSING_TIME, RANK };

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (info[i] == input_lines[j])
                order_of_informations.push_back(j);

    return order_of_informations;
}

int string_to_minutes(string string_time) {
    int minutes = 0;
    minutes = stoi(string_time.substr(0, 2)) * 60 + stoi(string_time.substr(3, 2));
    return minutes;
}

Place generate_place(vector<string> input_lines, vector<int> order_of_informations, int i) {
    Place place = {
        input_lines[order_of_informations[0] + i * 4],
        string_to_minutes(input_lines[order_of_informations[1] + i * 4]),
        string_to_minutes(input_lines[order_of_informations[2] + i * 4]),
        stoi(input_lines[order_of_informations[3] + i * 4]),
        false
    };

    return place;
}

Places generaete_places(vector<string> input_lines) {
    Places places;
    int number_of_places = get_number_of_places(input_lines);
    vector<int> order_of_informations = get_order_of_informations(input_lines);
    for (int i = 1; i <= number_of_places; i++)
        places.push_back(generate_place(input_lines, order_of_informations, i));

    return places;
}

Places read_input(string filename) {
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

    Places place_info = generaete_places(input_lines);
    return place_info;
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
    cout << LOCK <<  place.name << endl;
    cout << VISIT_FROM << minutes_to_string(start) << UNTIL;
    cout << minutes_to_string(finish) << endl << END_PRINT << endl;
}


bool is_place_open(int start, Place place) {
    if (start >= place.opening_time && start <= place.closing_time)
        return true;
    else
        return false;
}

bool is_place_visitable(int start, Place place) {
    if (start + 30 + 15 <= place.closing_time)
        return true;
    else
        return false;
}

Place change_visit_place(int start, bool found_place, Place place, Place visit_place) {
    if (!found_place)
        return place;
    else {
        if (start >= place.opening_time)
            if (place.rank < visit_place.rank)
                return place;
            else 
                return visit_place;
        else {
            if (place.opening_time < visit_place.opening_time)
                return place;
            if (place.opening_time == visit_place.opening_time)
                if (place.rank < visit_place.rank)
                    return place;
            return visit_place;
        }
    }
}

void change_start_and_finish(int& start, int& finish, Place place) {
    if (start + 30 < place.opening_time)
        start = place.opening_time;
    else
        start += 30;

    if (place.closing_time - start >= 60)
        finish = start + 60;
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
    int finish = 8 * 60, start = 8 * 60 - 30;
    Place visit_place;
    while (find_visit_place(start, finish, places, visit_place)) {
        output_printer(start, finish, visit_place);
        start = finish;
    }

    return 0;
}