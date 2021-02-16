#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "defns.h"

using namespace std;

// Method to safely convert string to int
int string_to_int(string s)
{
	if(s.empty() || s=="\r")
	{
		return 0;
	}
	return stoi(s);
}

// Method that returns the length of a file (to get the number of records in that file)
int csvLen(string filepath)
{
	string command = "wc -l "+filepath;
	FILE* file_path_output = popen(command.c_str(), "r");

	char buffer[128];
	string size = "";
	if(fgets(buffer, 128, file_path_output)!=NULL)
		size = buffer;
	int size_int = stoi(size);
	pclose(file_path_output);
	return size_int+1;
}

// Method that parses values that have K or M in the end to indicate Thousand or Million.  
int parseValue(string value)
{
	char c = value[value.length()-1];
	if(c == 'K' || c == 'k')
	{
		return (1000* stof(value.substr(0,value.length()-1)));
	}
	else if(c == 'M' || c == 'm')
	{
		return (1000000* stof(value.substr(0,value.length()-1)));
	}
	return (stof(value.substr(0,value.length()-1)));
}

// Method that tokenizes a record to create a storm_event struct
struct storm_event form_storm_struct(string line)
{
	char* token;
	char str[line.length()];
	strcpy(str, line.c_str());
	token = strtok(str, ",");

	string record_s[13];
	int i=0;
	while (token != NULL)
	{
		record_s[i] = token;
		token = strtok(NULL, ",");
		i++;
	}
	struct storm_event temp_struct_event = {string_to_int(record_s[0]), record_s[1], string_to_int(record_s[2]), record_s[3], record_s[4], record_s[5][0], record_s[6], string_to_int(record_s[7]), string_to_int(record_s[8]), string_to_int(record_s[9]), string_to_int(record_s[10]), parseValue(record_s[11]), string_to_int(record_s[12])};
	return temp_struct_event;
}

// Method that tokenizes a record to create a fatality_event struct
struct fatality_event form_fatality_struct(string line)
{
	char* token;
	char str[line.length()];
	strcpy(str, line.c_str());
	token = strtok(str, ",");

	string record_s[6];
	int i=0;
	while (token != NULL)
	{
		record_s[i] = token;
		token = strtok(NULL, ",");
		i++;
	}
	struct fatality_event temp_fatality_event = {string_to_int(record_s[0]), record_s[1][0], record_s[2], string_to_int(record_s[3]), record_s[4][0], record_s[5]};
	return temp_fatality_event;
}

// Method to read a damage/details file
void storm_readInputFile(string file, struct storm_event *storm_event_input)
{
	ifstream inFile;
    inFile.open(file);
    string token;
    int i=0;
    int len = csvLen(file);
    string store[len];

    while(inFile.good())
    {
    	if(i<=len)
    	{
        	getline(inFile,token,'\n');
        	store[i] = token;
        	i++;
    	}
    }
    inFile.close();
    
    for(i=1;i<len;i++)
    {
    	storm_event_input[i-1] = form_storm_struct(store[i]); 
    }
}

// Method to read a fatality file
void fatalities_readInputFile(string file, struct fatality_event *fatality_event_input)
{
	ifstream inFile;
    inFile.open(file);
    string token;
    int i=0;
    int len = csvLen(file);
    string store[len];

    while(inFile.good())
    {
    	if(i<=len)
    	{
        	getline(inFile,token,'\n');
        	store[i] = token;
        	i++;
    	}
    }
    inFile.close();

    for(i=1;i<len;i++)
    {
    	fatality_event_input[i-1] = form_fatality_struct(store[i]); 
    }
}

// Method to get the event IDs from the indices and years data of a specific damage amount
void get_event_ids(int *output_event_ids, int *output_years, int *output_indices, int len_output, struct annual_storm *annual_storm_array, int num_years_total)
{
	for(int i=0;i<len_output;i++)
	{
		for(int j=0;j<num_years_total;j++)
		{
			if(annual_storm_array[j].year == output_years[i])
			{
				output_event_ids[i] = annual_storm_array[j].storm_events[(output_indices[i] - 1)].event_id;
			}
		}
	}
}

// Custom Insertion Sort to sort Years and EventIDs
void final_insertion_sort(int *output_event_ids, int *output_years, int len_output)
{
	int i=0,j=0,key_ei=0,key_y=0;
	for(j=1;j<len_output;j++)
	{
		key_ei = output_event_ids[j];
		key_y = output_years[j];
		i = j-1;
		while(i>=0 && output_event_ids[i]>key_ei)
		{
			output_event_ids[i+1]=output_event_ids[i];
			output_years[i+1]=output_years[i];
			i = i - 1;
		}
		output_event_ids[i+1] = key_ei;
		output_years[i+1] = key_y;
	}

	i=0,j=0,key_ei=0,key_y=0;
	for(j=1;j<len_output;j++)
	{
		key_ei = output_event_ids[j];
		key_y = output_years[j];
		i = j-1;
		while(i>=0 && output_years[i]>key_y)
		{
			output_event_ids[i+1]=output_event_ids[i];
			output_years[i+1]=output_years[i];
			i = i - 1;
		}
		output_event_ids[i+1] = key_ei;
		output_years[i+1] = key_y;
	}
}

// Last method that prints the output in the right format
void print_final_output(int *output_event_ids, int *output_years, int len_output ,struct annual_storm *annual_storm_array, int num_years_total)
{
	int year_t=0;
	for(int i=0;i<len_output;i++)
	{
		for(int j=0;j<num_years_total;j++)
		{
			if(output_years[i] == annual_storm_array[j].year)
			{
				for(int k=0;k<annual_storm_array[j].no_storms;k++)
				{
					if(output_event_ids[i] == annual_storm_array[j].storm_events[k].event_id)
					{
						if(year_t !=  annual_storm_array[j].year)
						{
							cout << "\t" << annual_storm_array[j].storm_events[k].year << endl;
							year_t = annual_storm_array[j].year;
						}
						cout << "\t\tEvent Id: "<<annual_storm_array[j].storm_events[k].event_id<<endl;
						cout << "\t\tState: "<<annual_storm_array[j].storm_events[k].state<<endl;
						cout << "\t\tYear: "<<annual_storm_array[j].storm_events[k].year<<endl;
						cout << "\t\tMonth: "<<annual_storm_array[j].storm_events[k].month_name<<endl;
						cout << "\t\tEvent Type: "<<annual_storm_array[j].storm_events[k].event_type<<endl;
						cout << "\t\tCounty/Parish/Marine: "<<annual_storm_array[j].storm_events[k].cz_type<<endl;
						cout << "\t\tCounty/Parish/Marine Name: "<<annual_storm_array[j].storm_events[k].cz_name<<endl;
						cout << "\t\tInjuries Direct: "<<annual_storm_array[j].storm_events[k].injuries_direct<<endl;
						cout << "\t\tInjuries Indirect: "<<annual_storm_array[j].storm_events[k].injuries_indirect<<endl;
						cout << "\t\tDeaths Direct: "<<annual_storm_array[j].storm_events[k].deaths_direct<<endl;
						cout << "\t\tDeaths Indirect: "<<annual_storm_array[j].storm_events[k].deaths_indirect<<endl;
						cout << "\t\tDamage to Property: $"<<annual_storm_array[j].storm_events[k].damage_property<<endl;
						cout << "\t\tDamage to Crops: $"<<annual_storm_array[j].storm_events[k].damage_crops;
					}
				}
			}
		}
		if(i!=(len_output-1)){
			cout << endl << endl;
		}
	}

}

// Method that does the first sort on the array of damage structs 
void insertionSort(struct damage *damage_array, int len_damage_struct, string position, struct annual_storm *annual_storm_array, int num_years_total)
{
	int i=0,j=0;
	struct damage key;
	for(j=1;j<len_damage_struct;j++)
	{
		i=j-1;
		key = damage_array[j];
		while(i>=0 && damage_array[i].damage_amount>key.damage_amount)
		{
			damage_array[i+1] = damage_array[i];
			i = i-1;
		}
		damage_array[i+1] = key;
	}

	int *output_years;
	int *output_indices;
	int *output_event_ids;
	int count=0;
	int start=0;
	if(position == "max")
	{
		int largest_dam_val = damage_array[len_damage_struct-1].damage_amount;
		for(i=len_damage_struct-1;i>=0;i--)
		{
			if(damage_array[i].damage_amount == largest_dam_val)
			{
				count++;
			}
			else
			{
				break;
			}
		}

		output_years = new int[count];
		output_indices = new int[count];
		output_event_ids = new int[count];

		start = len_damage_struct - count;

		for(i=0;i<count;i++)
		{
			output_years[i] = damage_array[i+start].year;
			output_indices[i] = damage_array[i+start].index;
		}
	}

	else if(position == "min")
	{
		int smallest_dam_val = damage_array[0].damage_amount;
		for(i=0;i<len_damage_struct;i++)
		{
			if(damage_array[i].damage_amount == smallest_dam_val)
			{
				count++;
			}
			else
			{
				break;
			}
		}

		output_years = new int[count];
		output_indices = new int[count];
		output_event_ids = new int[count];

		for(i=0;i<count;i++)
		{
			output_years[i] = damage_array[i].year;
			output_indices[i] = damage_array[i].index;
		}
	}

	else
	{
		int k = string_to_int(position);
		int val_at_k = damage_array[k-1].damage_amount; // check if it is k or k+1 or k-1
		for(i=k-1;i>=0;i--)
		{
			if(damage_array[i].damage_amount == val_at_k)
			{
				count++;
			}
			else
			{
				break;
			}
		}
		start = k - count;
		for(i=k;i<len_damage_struct;i++)
		{
			if(damage_array[i].damage_amount == val_at_k)
			{
				count++;
			}
			else
			{
				break;
			}
		}
		output_years = new int[count];
		output_indices = new int[count];
		output_event_ids = new int[count];

		for(i=0;i<count;i++)
		{
			output_years[i] = damage_array[i+start].year;
			output_indices[i] = damage_array[i+start].index;
		}
	}
	get_event_ids(output_event_ids, output_years, output_indices, count, annual_storm_array, num_years_total);
	final_insertion_sort(output_event_ids,output_years,count);
	print_final_output(output_event_ids, output_years, count, annual_storm_array, num_years_total);
	delete[] output_years;
	delete[] output_event_ids;
	delete[] output_indices;
}

// Method to execute the select query in their custom ways
void executeDamageQuery(struct annual_storm *annual_storm_array, int num_years_total, string position, int years_array[], int len_years, char damage)
{
	int len_damage_struct;
	if(len_years == 1)
	{
		for(int i=0;i<num_years_total;i++)
		{
			if(years_array[0] == annual_storm_array[i].year)
			{
				len_damage_struct = annual_storm_array[i].no_storms;
			}
		}
	}
	else
	{
		for(int i=0;i<num_years_total;i++)
		{
			len_damage_struct += annual_storm_array[i].no_storms;
		}
	}

	// Create array of damage structs
	struct damage *damage_array = new struct damage[len_damage_struct];
	int i=0,j=0,k=0,temp=0;
	if(len_years != 1)
	{
		while(j < len_years)
		{
			temp = annual_storm_array[j].no_storms;
			for(k=0;k<temp;k++)
			{
				if(damage == 'P')
				{
					damage_array[i+k].damage_amount = annual_storm_array[j].storm_events[k].damage_property;
				}
				else if(damage == 'C')
				{
					damage_array[i+k].damage_amount = annual_storm_array[j].storm_events[k].damage_crops;	
				}
				damage_array[i+k].year = annual_storm_array[j].storm_events[k].year;
				damage_array[i+k].index = k+1;
			}
			j = j+1;
			i = i+temp;
		}
	}
	if(len_years == 1)
	{
		for(i=0;i<num_years_total;i++)
		{
			if(years_array[0] == annual_storm_array[i].year)
			{
				for(j=0;j<annual_storm_array[i].no_storms;j++)
				{
					damage_array[j].year = years_array[0];
					damage_array[j].index = j+1;
					if(damage == 'P')
					{
						damage_array[j].damage_amount = annual_storm_array[i].storm_events[j].damage_property;
					}
					else if(damage == 'C')
					{
						damage_array[j].damage_amount = annual_storm_array[i].storm_events[j].damage_crops;
					}
				}
			}
		}
	}
	insertionSort(damage_array, len_damage_struct, position, annual_storm_array, num_years_total);
	delete[] damage_array;
}

// Method to parse the query passed
void parseDamageQuery(string query, struct annual_storm *annual_storm_array, int num_years_total)
{
	char* token;
	char str[query.length()];
	strcpy(str, query.c_str());
	token = strtok(str, " ");

	string query_parts[5];
	int i=0;
	while (token != NULL)
	{
		query_parts[i] = token;
		token = strtok(NULL, " ");
		i++;
	}

	if(query_parts[0] != "select" || query_parts[4] != "insertion")
	{
		return;
	}
	if(query_parts[3] != "damage_property" && query_parts[3] != "damage_crops" )
	{
		return;
	}
	char damage_type = 'P';
	if(query_parts[3] == "damage_crops")
	{
		damage_type = 'C';
	}
	int n_years = 1;
	if(query_parts[2] == "all")
	{
		n_years = num_years_total;
	}
	int query_years[n_years];
	if(query_parts[2] == "all")
	{
		for(int i=0;i<n_years;i++)
		{
			query_years[i] = annual_storm_array[i].year;
		}
	}
	else
	{
		query_years[0] = string_to_int(query_parts[2]);
	}
	executeDamageQuery(annual_storm_array, num_years_total, query_parts[1], query_years, n_years, damage_type);

}


int main(int argc, char *argv[]) 
{
	int n = atoi(argv[1]);
	int years[n];
	for(int i=0;i<n;i++)
	{
		years[i] = atoi(argv[i+2]);
	}

	//////////////////////////////
	// Read in the CSV files and store data
	//////////////////////////////
	struct annual_storm *annual_storm_array = new struct annual_storm[n];

	for(int i=0;i<n;i++)
	{
		string details_inputfile = "Data/details-"+to_string(years[i])+".csv";
		int len_details = csvLen(details_inputfile);
		struct storm_event *storm_event_input = new struct storm_event[len_details-1];
		storm_readInputFile(details_inputfile, storm_event_input);

		string fatalities_inputfile = "Data/fatalities-"+to_string(years[i])+".csv";
		int len_fatal = csvLen(fatalities_inputfile);
		struct fatality_event *fatality_event_input = new struct fatality_event[len_fatal-1];
		fatalities_readInputFile(fatalities_inputfile, fatality_event_input);

		struct annual_storm temp_AS = {years[i], len_details-1, storm_event_input,len_fatal-1 ,fatality_event_input};
		annual_storm_array[i] = temp_AS;

	}
	
	//////////////////////////////
	// Read queries
	//////////////////////////////
	int n_queries;
	cin >> n_queries;
	cin >> ws;
	string all_queries[n_queries];
	for(int i=0;i<n_queries;i++)
	{
		string temp = "";
		getline(cin,temp);
		all_queries[i] = temp;
	}
	for(int i=0;i<n_queries;i++)
	{
		if(i!=0)
		{
			cout << "\n\n";
		}
		cout << "Query: " << all_queries[i] << endl;
		parseDamageQuery(all_queries[i], annual_storm_array, n);
	}
	// Deleting arrays of storm and fatality events before deleting the annual storm array.
	for(int i=0;i<n;i++)
	{
		delete[] annual_storm_array[i].storm_events;
		delete[] annual_storm_array[i].fatality_events;
	}
	delete[] annual_storm_array;
	return 0;
}
