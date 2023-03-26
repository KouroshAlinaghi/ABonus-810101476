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


struct Places
{
    vector<string> name;
    vector<int> opening_time;
    vector<int> closing_time;
    vector<int> rank;
    vector<int> visited;
};


const string VISIT_FROM = "Visit from ";
const string UNTIL = " until ";


int get_number_of_places(vector<string> input_lines) {
    int num_of_places = input_lines.size() / 4 - 1;
    return num_of_places;
}

vector<int> get_order_of_informations(vector<string> input_lines)
{
    vector<int> order_of_informations;

    vector<string> info = { NAME, OPENING_TIME, CLOSING_TIME, RANK };

    for (int i=0;i<4;i++) for (int j = 0;j<4; j++) if (info[i] == input_lines[j])     order_of_informations.push_back(j);
    return order_of_informations;
}

int string_to_minutes(string string_time)
{
    int minutes = 0;
    minutes = stoi(string_time.substr(0, 2)) * 60 + stoi(string_time.substr(3, 2));
    return minutes;
}

Places generaete_places(vector<string> input_lines)
{
    Places place_info;
    int number_of_places = get_number_of_places(input_lines);
    vector<int> order_of_informations = get_order_of_informations(input_lines);
    for (int i = 1; i <= number_of_places; i++)
    {
        place_info.name.push_back(input_lines[order_of_informations[0] + i * 4]);
        place_info.opening_time.push_back(string_to_minutes(input_lines[order_of_informations[1] + i * 4]));
        place_info.closing_time.push_back(string_to_minutes(input_lines[order_of_informations[2] + i * 4]));
        int rank = stoi(input_lines[order_of_informations[3] + i * 4]);
        place_info.rank.push_back(rank);
        place_info.visited.push_back(0);
    }
    return place_info;
}

Places read_input(string filename)
{
    vector<string> input_lines;
    string curr_line;
    ifstream input_file(filename);
    while (getline(input_file, curr_line)) {
        istringstream line(curr_line);

        string file_element;

        while (getline(line, file_element, ',')) {
            input_lines.push_back(file_element);
        }
    }
    input_file.close();
    Places place_info = generaete_places(input_lines);
    return place_info;
}







string minutes_to_string(int input)
{
    string result = "";
    int hours = input / 60;
    int minutes = input % 60;
    if (hours >= 10)
        result += to_string(hours);
    else
    {

        result += "0";
        result += to_string(hours);
    }
    result += ":";
    if (minutes >= 10)
        result += to_string(minutes);
    else
    {
        result += "0";
        result += to_string(minutes);
    }
    return result;
}

void output_printer(int start, int finish, Places places, int place_index)
{
    cout << LOCK <<  places.name[place_index] << endl;
    cout << VISIT_FROM << minutes_to_string(start) << UNTIL;
    cout << minutes_to_string(finish) << endl << END_PRINT << endl;
}


bool is_place_open(int& start, int i, Places places)
{
    if (start >= places.opening_time[i] && start <= places.closing_time[i])
        return true;
    else
        return false;

}

bool is_place_visitable(int& start, int i, Places places)
{
    if (start + 30 + 15 <= places.closing_time[i])
        return true;
    else
        return false;
}

int change_visit_index(int start, int visit_index, int i, Places places)
{
    if (visit_index == -1)
        return i;
    else
    {
        if (start >= places.opening_time[i]) if (places.rank[i] <    places.rank[visit_index]) return i; else return visit_index;
        else
        {
            if (places.opening_time[i] < places.opening_time[visit_index]) return i;
            if (places.opening_time[i] == places.opening_time[visit_index]) if (places.rank[i] < places.rank[visit_index]) return i;
            return visit_index;
        }
    }
}

void change_start_and_finish(int& start,   int& finish,   Places places,int visit_index) {
    if (start + 30 < places.opening_time[visit_index])
        start = places.opening_time[visit_index];
    else
        start += 30;
    if (places.closing_time[visit_index] - start >= 60)
        finish = start + 60;
    else
        finish = places.closing_time[visit_index];
}

int visit_place_index(int &start,    int &finnish,   Places& places) {
    int visit_index = -1;
    for (int i = 0; i < places.name.size(); i++) if (is_place_open(start,i,places) && is_place_visitable(start, i, places) && places.visited[i] == 0) visit_index = change_visit_index(start,visit_index, i, places);
    if (visit_index == -1) for (int i = 0; i < places.name.size(); i++) if (start < places.opening_time[i] && is_place_visitable(start, i, places) && places.visited[i] == 0) visit_index = change_visit_index(start, visit_index, i, places);
    if (visit_index >= 0)
    {

        places.visited[visit_index] = 1;
        change_start_and_finish(start, finnish, places, visit_index);
    }  

    return visit_index;
}



int main(int argc, char* argv[])
{
    Places places = read_input(argv[1]);
    int finish = 8 * 60, start = 8 * 60 - 30;
    int index_of_place_to_visit = visit_place_index(start, finish, places);
    while (index_of_place_to_visit != -1)
    {
        output_printer(start, finish, places, index_of_place_to_visit);
        start = finish;
        index_of_place_to_visit = visit_place_index(start, finish, places);
    }


    return 0;
}