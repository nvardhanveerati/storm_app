#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "defns.h"

using namespace std;

// clock_t timestamps[4];
// int tsize1;
// int tsize2;

// void writeTimestampsToFile(string sort)
// {
// 	ofstream out_file;
// 	out_file.open("time_analysis.txt",ios::app);
// 	out_file << sort << "," <<to_string(timestamps[1]-timestamps[0])<<","<<to_string(timestamps[3]-timestamps[2])<<","<<to_string(tsize1)<<","<<to_string(tsize2)<<"\n";
// 	out_file.close();
// }
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
	return (int)(stof(value.substr(0,value.length())));
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
void final_insertion_sort(int *output_event_ids, int *output_years, int *output_indices, int len_output)
{
	int i=0,j=0,key_ei=0,key_y=0,key_i=0;
	for(j=1;j<len_output;j++)
	{
		key_ei = output_event_ids[j];
		key_y = output_years[j];
		key_i = output_indices[j];
		i = j-1;
		while(i>=0 && output_event_ids[i]>key_ei)
		{
			output_event_ids[i+1]=output_event_ids[i];
			output_years[i+1]=output_years[i];
			output_indices[i+1]=output_indices[i];
			i = i - 1;
		}
		output_event_ids[i+1] = key_ei;
		output_years[i+1] = key_y;
		output_indices[i+1] = key_i;
	}

	i=0,j=0,key_ei=0,key_y=0,key_i=0;
	for(j=1;j<len_output;j++)
	{
		key_ei = output_event_ids[j];
		key_y = output_years[j];
		key_i = output_indices[j];
		i = j-1;
		while(i>=0 && output_years[i]>key_y)
		{
			output_event_ids[i+1]=output_event_ids[i];
			output_years[i+1]=output_years[i];
			output_indices[i+1]=output_indices[i];
			i = i - 1;
		}
		output_event_ids[i+1] = key_ei;
		output_years[i+1] = key_y;
		output_indices[i+1] = key_i;
	}
}

void final_merge(int *A, int *B, int *C, int l, int m, int r)
{
	int nl = m - l + 1;
	int nr = r - m;

	int *LA = new int[nl];
	int *LB = new int[nl];
	int *LC = new int[nl];
	int *RA = new int[nr];
	int *RB = new int[nr];
	int *RC = new int[nr];


	for (int i = 0; i < nl; i++)
	{	
		LA[i] = A[l + i];
		LB[i] = B[l + i];
		LC[i] = C[l + i];
	}
	for (int j = 0; j < nr; j++)
	{
		RA[j] = A[m + 1 + j];
		RB[j] = B[m + 1 + j];
		RC[j] = C[m + 1 + j];
	}

	int i=0, j=0,k = l;
	while (i < nl && j < nr) {
		if (LA[i] <= RA[j]) {
			A[k] = LA[i];
			B[k] = LB[i];
			C[k] = LC[i];
			i++;
		}
		else {
			A[k] = RA[j];
			B[k] = RB[j];
			C[k] = RC[j];
			j++;
		}
		k++;
	}
	while (i < nl) {
		A[k] = LA[i];
		B[k] = LB[i];
		C[k] = LC[i];
		i++;
		k++;
	}

	while (j < nr) {
		A[k] = RA[j];
		B[k] = RB[j];
		C[k] = RC[j];
		j++;
		k++;
	}
	delete[] LA;
	delete[] RB;
	delete[] LB;
	delete[] RA;
	delete[] LC;
	delete[] RC;
}

void final_mergesort(int *A, int *B, int *C, int l, int r)
{
	if(l>=r){
		return;
	}
	int m =l+ (r-l)/2;
	final_mergesort(A,B,C,l,m);
	final_mergesort(A,B,C,m+1,r);
	final_merge(A,B,C,l,m,r);
}

void final_merge_sort_call(int *output_event_ids, int *output_years, int *output_indices, int len_output)
{
	// cout << "Is this even being called?" << endl;
	final_mergesort(output_event_ids, output_years, output_indices, 0,len_output-1);
	final_mergesort(output_years, output_event_ids, output_indices, 0,len_output-1);
}

// Last method that prints the output in the right format
void print_final_damage_output(int *output_event_ids, int *output_years, int len_output ,struct annual_storm *annual_storm_array, int num_years_total)
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
						if(annual_storm_array[j].storm_events[k].cz_name != "0")
							cout << "\t\tCounty/Parish/Marine Name: "<<annual_storm_array[j].storm_events[k].cz_name<<endl;
						else
							cout << "\t\tCounty/Parish/Marine Name: "<<endl;
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

int get_number_of_fatality_events(int year, int event_id, struct annual_storm *annual_storm_array, int num_years_total)
{
	int index_year;
	int count=0;
	for(int i=0;i<num_years_total;i++)
	{
		if(year == annual_storm_array[i].year)
		{
			index_year=i;
		}
	}
	for(int i=0;i<annual_storm_array[index_year].no_fatalities;i++)
	{
		if(event_id == annual_storm_array[index_year].fatality_events[i].event_id)
		{
			count++;
		}
	}
	return count;
}

void get_fatality_event(struct fatality_event *f, int year, int event_id, struct annual_storm *annual_storm_array, int num_years_total, int num_of_fatality_events)
{
	int index_year;
	for(int i=0;i<num_years_total;i++)
	{
		if(year == annual_storm_array[i].year)
		{
			index_year = i;
		}
	}
	int j=0;
	for(int i=0;i<annual_storm_array[index_year].no_fatalities;i++)
	{
		if(event_id == annual_storm_array[index_year].fatality_events[i].event_id)
		{
			f[j] = annual_storm_array[index_year].fatality_events[i];
			j++;
		}
	}
}

void print_final_deaths_output(int *output_event_ids, int *output_years, int len_output, struct annual_storm *annual_storm_array, int num_years_total)
{
	int year_t=0;
	for(int i=0;i<len_output;i++)
	{
		if(year_t !=  output_years[i])
		{
			cout << "\t" << output_years[i]<< endl;
			year_t = output_years[i];
		}
		int num_of_fatality_events = get_number_of_fatality_events(output_years[i], output_event_ids[i], annual_storm_array, num_years_total);

		struct fatality_event *f = new struct fatality_event[num_of_fatality_events];
		get_fatality_event(f, output_years[i], output_event_ids[i], annual_storm_array, num_years_total, num_of_fatality_events);
		if(num_of_fatality_events == 0)
		{
			cout << "\t\tEvent Id: " << output_event_ids[i];
		}
		for(int j=0;j<num_of_fatality_events;j++)
		{
			if(j==0)
				cout << "\t\tEvent Id: " << f[j].event_id;
			if(j!=0)
				cout <<endl;
			if(f[j].fatality_type!='\0')
				cout << "\n\t\t\tFatality Type: " << f[j].fatality_type;
			if(f[j].fatality_date!="")
				cout << "\n\t\t\tFatality Date: " << f[j].fatality_date;
			if(f[j].fatality_age!=0)
				cout << "\n\t\t\tFatality Age: " << f[j].fatality_age;
			if(f[j].fatality_sex!='\0')
				cout << "\n\t\t\tFatality Sex: " << f[j].fatality_sex;
			if(f[j].fatality_location!="")
				cout << "\n\t\t\tFatality Location: " << f[j].fatality_location;
		}

		if(i!=(len_output-1))
		{
			cout << endl<<endl;
		}
		delete[] f;
	}
}

void merge_damage(struct damage *damage_array, int left, int mid, int right)
{
	int nl = mid - left + 1;
	int nr = right - mid;

	struct damage *L = new struct damage[nl];
	struct damage *R = new struct damage[nr];

	for (int i = 0; i < nl; i++)
		L[i] = damage_array[left + i];
	for (int j = 0; j < nr; j++)
		R[j] = damage_array[mid + 1 + j];

	int i=0, j=0,k = left;

	while (i < nl && j < nr) {
		if (L[i].damage_amount <= R[j].damage_amount) {
			damage_array[k].damage_amount = L[i].damage_amount;
			damage_array[k].year = L[i].year;
			damage_array[k].index = L[i].index;
			i++;
		}
		else {
			damage_array[k].damage_amount = R[j].damage_amount;
			damage_array[k].year = R[j].year;
			damage_array[k].index = R[j].index;
			j++;
		}
		k++;
	}

	while (i < nl) {
		damage_array[k].damage_amount = L[i].damage_amount;
		damage_array[k].year = L[i].year;
		damage_array[k].index = L[i].index;
		i++;
		k++;
	}


	while (j < nr) {
		damage_array[k].damage_amount = R[j].damage_amount;
		damage_array[k].year = R[j].year;
		damage_array[k].index = R[j].index;
		j++;
		k++;
	}
	delete[] L;
	delete[] R;
}


void mergesort_damage(struct damage *damage_array, int left, int right)
{
	if(left>=right)
	{
		return;
	}
	int mid =left+ (right-left)/2;
	mergesort_damage(damage_array,left,mid);
	mergesort_damage(damage_array,mid+1,right);
	merge_damage(damage_array,left,mid,right);
}

void merge_deaths(struct deaths *death_array, int left, int mid, int right)
{
	int nl = mid - left + 1;
	int nr = right - mid;

	struct deaths *L = new struct deaths[nl];
	struct deaths *R = new struct deaths[nr];

	for (int i = 0; i < nl; i++)
		L[i] = death_array[left + i];
	for (int j = 0; j < nr; j++)
		R[j] = death_array[mid + 1 + j];

	int i=0, j=0,k = left;

	while (i < nl && j < nr) {
		if (L[i].total_deaths <= R[j].total_deaths) {
			death_array[k].total_deaths = L[i].total_deaths;
			death_array[k].year = L[i].year;
			death_array[k].index = L[i].index;
			i++;
		}
		else {
			death_array[k].total_deaths = R[j].total_deaths;
			death_array[k].year = R[j].year;
			death_array[k].index = R[j].index;
			j++;
		}
		k++;
	}

	while (i < nl) {
		death_array[k].total_deaths = L[i].total_deaths;
		death_array[k].year = L[i].year;
		death_array[k].index = L[i].index;
		i++;
		k++;
	}

	while (j < nr) {
		death_array[k].total_deaths = R[j].total_deaths;
		death_array[k].year = R[j].year;
		death_array[k].index = R[j].index;
		j++;
		k++;
	}
	delete[] L;
	delete[] R;
}

void mergesort_deaths(struct deaths *death_array, int left, int right)
{
	if(left>=right)
	{
		return;
	}
	int mid =left+ (right-left)/2;
	mergesort_deaths(death_array,left,mid);
	mergesort_deaths(death_array,mid+1,right);
	merge_deaths(death_array,left,mid,right);
}

void insertionSort_death(struct deaths *death_array, int len_death_struct)
{
	int i=0,j=0;
	struct deaths key;
	for(j=1;j<len_death_struct;j++)
	{
		i=j-1;
		key = death_array[j];
		while(i>=0 && death_array[i].total_deaths>key.total_deaths)
		{
			death_array[i+1] = death_array[i];
			i = i-1;
		}
		death_array[i+1] = key;
	}
}

void format_output_death(struct deaths *death_array, int len_death_struct, string position, struct annual_storm *annual_storm_array, int num_years_total, char sort)
{
	int i=0,j=0;
	int *output_years;
	int *output_indices;
	int *output_event_ids;

	int count=0;
	int start=0;
	if(position == "max")
	{
		int largest_death_val = death_array[len_death_struct-1].total_deaths;
		for(i=len_death_struct-1;i>=0;i--)
		{
			if(death_array[i].total_deaths == largest_death_val)
			{
				count++;
			}
			else
			{
				break;
			}
		}

		output_years= new int[count];
		output_indices= new int[count];
		output_event_ids= new int[count];
		start = len_death_struct - count;
		for(i=0;i<count;i++)
		{
			output_years[i] = death_array[i+start].year;
			output_indices[i] = death_array[i+start].index;
		}
	}
	else if(position == "min")
	{
		int smallest_death_val = death_array[0].total_deaths;
		for(i=0;i<len_death_struct;i++)
		{
			if(death_array[i].total_deaths == smallest_death_val)
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
			output_years[i] = death_array[i].year;
			output_indices[i] = death_array[i].index;
		}
	}
	else
	{
		int k = string_to_int(position);
		int val_at_k = death_array[k-1].total_deaths; 
		for(i=k-1;i>=0;i--)
		{
			if(death_array[i].total_deaths == val_at_k)
			{
				count++;
			}
			else
			{
				break;
			}
		}
		start = k - count;
		for(i=k;i<len_death_struct;i++)
		{
			if(death_array[i].total_deaths == val_at_k)
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
			output_years[i] = death_array[i+start].year;
			output_indices[i] = death_array[i+start].index;
		}
	}
	// tsize2 = count;
	get_event_ids(output_event_ids, output_years, output_indices, count, annual_storm_array, num_years_total);
	if(sort == 'I')
	{
		// timestamps[2] = clock();
		final_insertion_sort(output_event_ids,output_years,output_indices,count);
		// timestamps[3] = clock();
	}
	else
	{
		// timestamps[2] = clock();
		final_merge_sort_call(output_event_ids, output_years, output_indices, count);
		// timestamps[3] = clock();
	}

	print_final_deaths_output(output_event_ids, output_years, count, annual_storm_array, num_years_total);
	delete[] output_years;
	delete[] output_indices;
	delete[] output_event_ids;
}

// Method that does the first sort on the array of damage structs 
void insertionSort_damage(struct damage *damage_array, int len_damage_struct)
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
}

void format_output_damage(struct damage *damage_array, int len_damage_struct, string position, struct annual_storm *annual_storm_array, int num_years_total, char sort)
{
	int i=0,j=0;
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
		int val_at_k = damage_array[k-1].damage_amount;
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
	// tsize2=count;
	get_event_ids(output_event_ids, output_years, output_indices, count, annual_storm_array, num_years_total);
	if(sort == 'I')
	{
		// timestamps[2] = clock();
		final_insertion_sort(output_event_ids, output_years, output_indices, count);
		// timestamps[3] = clock();
	}
	else
	{
		// timestamps[2] = clock();
		final_merge_sort_call(output_event_ids, output_years, output_indices, count);
		// timestamps[3] = clock();
	}
	print_final_damage_output(output_event_ids, output_years, count, annual_storm_array, num_years_total);
	delete[] output_years;
	delete[] output_event_ids;
	delete[] output_indices;
}

void executeFatalityQuery(struct annual_storm *annual_storm_array, int num_years_total, string position, int years_array[], int len_years, char sort)
{
	int len_death_struct;
	if(len_years == 1)
	{
		for(int i=0;i<num_years_total;i++)
		{
			if(years_array[0] == annual_storm_array[i].year)
			{
				len_death_struct = annual_storm_array[i].no_storms;
			}
		}
	} 
	else
	{
		for(int i =0;i<num_years_total;i++)
		{
			len_death_struct += annual_storm_array[i].no_storms; 
		}
	}

	struct deaths *death_array = new struct deaths[len_death_struct];
	int i=0,j=0,k=0, temp=0;
	if(len_years != 1)
	{
		while(j< len_years)
		{
			temp = annual_storm_array[j].no_storms;
			for(k=0;k<temp;k++)
			{
				death_array[i+k].total_deaths = annual_storm_array[j].storm_events[k].deaths_direct + annual_storm_array[j].storm_events[k].deaths_indirect;
				death_array[i+k].year = annual_storm_array[j].storm_events[k].year;
				death_array[i+k].index = k+1;
			}
			j = j+1;
			i = i+temp;
		}
	}
	else if(len_years == 1)
	{
		for(i=0;i<num_years_total;i++)
		{
			if(years_array[0] == annual_storm_array[i].year)
			{
				for(j=0;j<annual_storm_array[i].no_storms;j++)
				{
					death_array[j].year = years_array[0];
					death_array[j].index = j+1;
					death_array[j].total_deaths = annual_storm_array[i].storm_events[j].deaths_direct + annual_storm_array[i].storm_events[j].deaths_indirect;
				}
			}
		}
	}
	// tsize1 = len_death_struct;
	if(sort == 'I')
	{
		// timestamps[0] = clock();
		insertionSort_death(death_array, len_death_struct);
		// timestamps[1] = clock();
		format_output_death(death_array, len_death_struct, position, annual_storm_array, num_years_total,sort);
	}
	else
	{
		// timestamps[0] = clock();
		mergesort_deaths(death_array, 0, len_death_struct -1);
		// timestamps[1] = clock();
		format_output_death(death_array, len_death_struct, position, annual_storm_array, num_years_total,sort);
	}
	delete[] death_array;

}

// Method to execute the select query in their custom ways
void executeDamageQuery(struct annual_storm *annual_storm_array, int num_years_total, string position, int years_array[], int len_years, char damage, char sort)
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
	// tsize1 = len_damage_struct;
	if(sort == 'I')
	{
		// timestamps[0] = clock();
		insertionSort_damage(damage_array, len_damage_struct);
		// timestamps[1] = clock();
		format_output_damage(damage_array, len_damage_struct, position, annual_storm_array, num_years_total, sort);
	}
	else
	{
		// timestamps[0] = clock();
		mergesort_damage(damage_array, 0, len_damage_struct-1);
		// timestamps[1] = clock();
		format_output_damage(damage_array, len_damage_struct, position, annual_storm_array, num_years_total, sort);
	}
	delete[] damage_array;
}

// Method to parse the query passed
void parseQuery(string query, struct annual_storm *annual_storm_array, int num_years_total)
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

	if(query_parts[0] != "select")
	{
		return;
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
	char sort_type = 'I';
	if(query_parts[4] == "merge")
		sort_type = 'M';

	if(query_parts[3] == "damage_property" || query_parts[3] == "damage_crops")
	{
		char damage_type = 'P';
		if(query_parts[3] == "damage_crops")
		{
			damage_type = 'C';
		}
		executeDamageQuery(annual_storm_array, num_years_total, query_parts[1], query_years, n_years, damage_type, sort_type);
		// writeTimestampsToFile(query_parts[4]);
	}
	if(query_parts[3] == "fatality")
	{
		executeFatalityQuery(annual_storm_array, num_years_total, query_parts[1], query_years, n_years, sort_type);
		// writeTimestampsToFile(query_parts[4]);
	}
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
		parseQuery(all_queries[i], annual_storm_array, n);
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