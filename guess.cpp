#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<iostream>
#include<map>
#include<set>
#include<string>
#include<vector>

using namespace std;

int MONEY_LARGE = 0, MONEY_BIG = 2250, MONEY_MEDIUM = 1000, MONEY_SMALL = 450;
int CREDIT_BIG = 105, CREDIT_SMALL = 70;

int dif_to_points(int dif)
{
	return (dif * dif - 15 * dif + 58) / 2;
}

int legend_points(int points, int trainer_number)
{
	if (trainer_number <= 8)
		return (points >= 203);
	if (trainer_number == 9 || trainer_number == 10)
		return (points >= 200);
	if (trainer_number == 11 || trainer_number == 12)
		return (points >= 190);
	if (trainer_number >= 13)
		return (points >= 180);
}

int pokemon_points(int points, int trainer_number)
{
	if (trainer_number <= 8)
		return (points >= 190);
	if (trainer_number == 9 || trainer_number == 10)
		return (points >= 180);
	if (trainer_number == 11 || trainer_number == 12)
		return (points >= 165);
	if (trainer_number >= 13)
		return (points >= 150);
}

int points_to_money(int points, int trainer_number)
{
	if (trainer_number <= 8)
	{
		if (points >= 203)
			return 0;
		if (points >= 190)
			return MONEY_LARGE;
		if (points >= 175)
			return MONEY_BIG;
		if (points >= 165)
			return MONEY_MEDIUM;
		if (points >= 155)
			return MONEY_SMALL;
	}
	if (trainer_number == 9 || trainer_number == 10)
	{
		if (points >= 200)
			return 0;
		if (points >= 180)
			return MONEY_LARGE;
		if (points >= 160)
			return MONEY_BIG;
		if (points >= 150)
			return MONEY_MEDIUM;
		if (points >= 140)
			return MONEY_SMALL;
	}
	if (trainer_number == 11 || trainer_number == 12)
	{
		if (points >= 190)
			return 0;
		if (points >= 165)
			return MONEY_LARGE;
		if (points >= 140)
			return MONEY_BIG;
		if (points >= 130)
			return MONEY_MEDIUM;
		if (points >= 120)
			return MONEY_SMALL;
	}
	if (trainer_number >= 13)
	{
		if (points >= 180)
			return 0;
		if (points >= 150)
			return MONEY_LARGE;
		if (points >= 120)
			return MONEY_BIG;
		if (points >= 110)
			return MONEY_MEDIUM;
		if (points >= 100)
			return MONEY_SMALL;
	}
	return 0;
}

int points_to_credit(int points, int trainer_number)
{
	if (trainer_number <= 8)
	{
		if (points >= 190)
			return CREDIT_BIG;
		if (points >= 175)
			return CREDIT_SMALL;
	}
	if (trainer_number == 9 || trainer_number == 10)
	{
		if (points >= 180)
			return CREDIT_BIG;
		if (points >= 160)
			return CREDIT_SMALL;
	}
	if (trainer_number == 11 || trainer_number == 12)
	{
		if (points >= 165)
			return CREDIT_BIG;
		if (points >= 140)
			return CREDIT_SMALL;
	}
	if (trainer_number >= 13)
	{
		if (points >= 150)
			return CREDIT_BIG;
		if (points >= 120)
			return CREDIT_SMALL;
	}
	return 0;
}


int main()
{	
	ifstream cwin("winners.txt", std::ifstream::in);
	vector<int> trainer_number(2);
	vector<string> empty(8);
	vector<vector<string> > winner(2, empty);
	for (int i = 0; i < 2; ++i)
	{
		cwin>>trainer_number[i];
		for (int j = 0; j < min(8, trainer_number[i]); ++j)
			cwin>>winner[i][j];
	}
	ifstream clate("late.txt", std::ifstream::in);
	set<string> late;
	string s;
	clate>>s;
	while (s != "end")
	{
		late.insert(s);
		clate>>s;
	}
	ifstream cpred("predicts.txt", std::ifstream::in);
	vector<map<string, int> > points(2);
	map<string, int> late_points;
	vector<set<string> > first(3);	
	for (int i = 0; i < 2; ++i)
	{
		cpred>>s;
		while (s != "end")
		{
			string name;
			while (s != "Турнир")
			{
				name += (s + " ");
				cpred>>s;
			}
			if (points[i].find(name) == points[i].end())
			{
				points[i][name] = 0;
				//if (trainer_number[i] < 8) points[i][name] = dif_to_points(0);
			}
			if (late_points.find(name) == late_points.end())
				late_points[name] = 0;
			while (s != "фазе\"")
				cpred>>s;
			int place;
			cpred>>place;
			cpred>>s;
			if (late.find(s) != late.end())
				++late_points[name];
			else
			{
				place -= late_points[name];
				if (place < 9)
				{
					for (int j = 0; j < min(8, trainer_number[i]); ++j)
					{
						if (s == winner[i][j])
						{
							points[i][name] += dif_to_points(abs(j - place + 1));
						}
					}
					if (place == 1 && winner[i][0] == s)
					{
						if (first[0].find(name) != first[0].end())
						{
							first[0].erase(first[0].find(name));
							first[2].insert(name);
						}
						else
							first[i].insert(name);
					}
				}
			}
			cpred>>s;
		}
		late_points.clear();
	}
	//freopen("output.txt", "w", stdout);
	for (int i = 0; i < 2; ++i)
	{
		for (map<string, int>::iterator it = points[i].begin(); it != points[i].end(); ++it)
			cout<<it->first<<" "<<it->second<<" "<<points_to_money(it->second, trainer_number[i])<<"\n";
		cout<<"\n";
	}

	set<string> legendary;
	set<string> pokemon;
	map<string, int> money;
	map<string, int> credits;
	for (int i = 0; i < 2; ++i)
	{
		for (map<string, int>::iterator it = points[i].begin(); it != points[i].end(); ++it)
		{
			if (legend_points(it->second, trainer_number[i]))
				legendary.insert(it->first);
			if (pokemon_points(it->second, trainer_number[i]))
				pokemon.insert(it->first);
			if (money.find(it->first) == money.end())
				money[it->first] = 0;
			money[it->first] += points_to_money(it->second, trainer_number[i]);
			if (credits.find(it->first) == credits.end())
				credits[it->first] = 0;
			credits[it->first] += points_to_credit(it->second, trainer_number[i]);
		}
	}
	map<int, vector<string> > money_list;
	empty.clear();
	for (map<string, int>::iterator it = money.begin(); it != money.end(); ++it)
	{
		if (money_list.find(it->second) == money_list.end())
			money_list[it->second] = empty;
		money_list[it->second].push_back(it->first);
	}
	map<int, vector<string> > credit_list;
	empty.clear();
	for (map<string, int>::iterator it = credits.begin(); it != credits.end(); ++it)
	{
		if (credit_list.find(it->second) == credit_list.end())
			credit_list[it->second] = empty;
		credit_list[it->second].push_back(it->first);
	}


	//freopen("prizes.txt", "w", stdout);
	if (! legendary.empty())
	{
		cout<<"Легенда + Лоск + титул \"Сорвавший джекпот\": ";
		for (set<string>::iterator it = legendary.begin(); it != legendary.end(); ++it)
			cout<<(*it)<<", ";
		cout<<"\n";
	}
	if (! pokemon.empty())
	{
		cout<<"Покемон + 1500 Поке-иен: ";
		for (set<string>::iterator it = pokemon.begin(); it != pokemon.end(); ++it)
			cout<<(*it)<<", ";
		cout<<"\n";
	}
	for (map<int, vector<string> >::iterator it = money_list.begin(); it != money_list.end(); ++it)
	{
		if (it->first > 0)
		{
			cout<<it->first<<" Поке-иен: ";
			for (int i = 0; i < it->second.size(); ++i)
				cout<<it->second[i]<<", ";
			cout<<"\n";
		}
	}
	for (map<int, vector<string> >::iterator it = credit_list.begin(); it != credit_list.end(); ++it)
	{
		if (it->first > 0)
		{
			cout<<it->first<<" Кредитов: ";
			for (int i = 0; i < it->second.size(); ++i)
				cout<<it->second[i]<<", ";
			cout<<"\n";
		}
	}
	if (! first[2].empty())
	{
		cout<<"3 Редких конфеты: ";
		for (set<string>::iterator it = first[2].begin(); it != first[2].end(); ++it)
			cout<<(*it)<<", ";
		cout<<"\n";
	}
	if (! first[0].empty())
	{
		cout<<"1 Редкая конфета (за победителя старшей лиги): ";
		for (set<string>::iterator it = first[0].begin(); it != first[0].end(); ++it)
			cout<<(*it)<<", ";
		cout<<"\n";
	}
	if (! first[1].empty())
	{
		cout<<"1 Редкая конфета (за победителя младшей лиги): ";
		for (set<string>::iterator it = first[1].begin(); it != first[1].end(); ++it)
			cout<<(*it)<<", ";
	}
	return 0;
}
