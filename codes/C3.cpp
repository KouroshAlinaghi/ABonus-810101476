#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<fstream>
#include<algorithm>
using namespace std;

#define name_pos 0
#define openingTime_pos 1
#define closingTime_pos 2
#define rank_pos 3
#define WHITE_SPACE ' '

struct place_str
{
	string name;
	int rank;
	int opening_time;
	int closing_time;
};

struct visit_str
{
	string name;
	int arrive_time;
	int leave_time;
};

typedef vector<struct place_str> place;
typedef vector<struct visit_str> visit;

// this function converts hour:minute to minutes
int time(string str)
{
	int time=0,i=0;
	string del;
	istringstream s(str);
    while(getline(s, del, ':'))
    {
    	i++;
    	if(i==1)
    		time+=60*stoi(del);
    	else
    		time+=stoi(del);
    }
    return time;
}
// this function converts minutes to hour:minute format
string inverseTime(int t)
{
	string hour,minute;
	minute=to_string(t%60);
	hour=to_string(t/60);
	if((t%60)<10)
		minute="0"+minute;
	if((t/60)<10)
		hour="0"+hour;
	return hour+":"+minute;
}

string PutSpace(string str)
{
	for(int i=0;i<str.length();i++)
		if(str[i]=='&')
			str[i]=WHITE_SPACE;
	return str;
}

vector<string> splitt(string str)
{
	vector<string> words;


	string word;
	istringstream s(str);
	while(getline(s, word, ' ')) words.push_back(word);
	return words;
}

void
reads_csv(string file_name,vector<struct place_str> &places)
{
	struct place_str place;
	string line,word;
	int position[4],i=0,line_number=0;
    ifstream file(file_name);
	while(getline(file,line))
	{
		for(int i=0;i<line.length();i++) // replace space with &
			if(line[i]==WHITE_SPACE) {line[i]='&';}
		istringstream s(line);
		i=0;
		if(line_number==0)
		{
			while (getline(s, word, ','))
			{
				if(word=="name")
					position[i]=name_pos;
				else if(word=="openingTime")
					position[i]=openingTime_pos;
				else if(word=="closingTime")
					position[i]=closingTime_pos;
				else if(word=="rank")
					position[i]=rank_pos;
				i+=1;
			}
			line_number=1;
		}
		else
		{
			i=0;
			while (getline(s, word, ','))
			{
				switch (position[i])
				{
					case name_pos:
						place.name=word;
						break;
					case openingTime_pos:
						place.opening_time=time(word);
						break;
					case closingTime_pos:
						place.closing_time=time(word);
						break;
					case rank_pos:
						place.rank=stoi(word);
						break;
					default:
						break;
				}
				i++;
			}
			places.push_back(place);
		}
	}
	file.close();
}

void remove_place(place &places , struct place_str selectedPlace)
{
	for(int i=0;i<places.size();i++) {
		if( places[i].name == selectedPlace.name )
			places.erase(places.begin()+i);
	}
}
int visit_duration(struct place_str place,int t)
{
	if( (place.closing_time - t) < 60 )
		return (place.closing_time-t);
	else return 60;
}
void swap(struct place_str &p1,struct place_str &p2)
{
	struct place_str temp;
	temp=p2;
	p2=p1;
	p1=temp;
}

void sortFirstByTime_andThenByRank(place& closest_places)
{
	for(int i=0;i<closest_places.size();i++){
		for(int j=0;j<closest_places.size()-i-1;j++)
		{
			if( closest_places[j].opening_time > closest_places[j+1].opening_time )
				swap(closest_places[j],closest_places[j+1]);
		}
	}
	for(int i=0;i<closest_places.size();i++)
	{
		for(int j=0;j<closest_places.size()-i-1;j++)
		{
			if( closest_places[j].opening_time == closest_places[j+1].opening_time &&closest_places[j].rank > closest_places[j+1].rank)
				swap(closest_places[j],closest_places[j+1]);}
	}
}


bool findopenplace(place &places,int &t,struct visit_str & next_visit)
{
	place open_places;
	int duration;
	for(int i=0;i<places.size();i++)
		if( places[i].opening_time<=t &&t<places[i].closing_time && (places[i].closing_time-t)>=15 )
			open_places.push_back(places[i]);
	if(open_places.size()==0)
		return false;
	else
	{
		for(int i=0;i<open_places.size();i++) // sort places by rank
		{
			for(int j=0;j<open_places.size()-i-1;j++){
				if( open_places[j].rank > open_places[j+1].rank )
					swap(open_places[j],open_places[j+1]);
			}
		}
		next_visit.name=open_places[0].name;
		next_visit.arrive_time=t;
		duration = visit_duration(open_places[0],t);
		next_visit.leave_time=t+duration;
		t=next_visit.leave_time+30;
		remove_place(places,open_places[0]);
		return true;

	}
}
bool findClosestPlace(place &places,int &t,struct visit_str &next_visit)
{
	place closest_places;
	int duration;
	for(int i=0;i<places.size();i++)
		if((places[i].closing_time-places[i].opening_time)>=15&&t<places[i].opening_time)
			closest_places.push_back(places[i]);
	if (closest_places.size()==0 )
		return false;
	else{

		sortFirstByTime_andThenByRank(closest_places);
		next_visit.name=closest_places[0].name;
		t=closest_places[0].opening_time;
		next_visit.arrive_time=closest_places[0].opening_time;
		duration=visit_duration(closest_places[0],t);
		next_visit.leave_time=closest_places[0].opening_time+duration;
		t=next_visit.leave_time+30;
		remove_place(places,closest_places[0]);
		return true;
	}

}

void Scheduler(place places,visit &visits)
{
	int t = 0;
	struct visit_str next_visit;
	while(1)
		if(findopenplace(places,t,next_visit)){
			visits.push_back(next_visit);
		}
		else if(findClosestPlace(places,t,next_visit))
		{
			visits.push_back(next_visit);
		}
		else
			break;
}







int main(int argc,char *argv[])
{
	place places;
	visit visits;
	reads_csv(string(argv[1]),places);
	Scheduler(places,visits);
	for(int i=0;i<visits.size();i++)
	{
		cout<<"Location "<<PutSpace(visits[i].name)<<endl;
		cout<<"Visit from "<<inverseTime(visits[i].arrive_time)<<" "
			<<"until "<<inverseTime(visits[i].leave_time);
		cout<<endl<<"---"<<endl;
	}
	return 0;
}