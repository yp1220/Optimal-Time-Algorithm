

#pragma once


#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>


// One ride item available for purchase.
class RideItem
{
	//
public:

	//
	RideItem
	(
		const std::string& description,
		double cost_dollars,
		double time_minutes
	)
		:
		_description(description),
		_cost_dollars(cost_dollars),
		_time_minutes(time_minutes)
	{
		assert(!description.empty());
		assert(cost_dollars > 0);
	}

	//
	const std::string& description() const { return _description; }
	double cost() const { return _cost_dollars; }
	double rideTime() const { return _time_minutes; }

	//
private:

	// Human-readable description of the ride, e.g. "new enchanted world". Must be non-empty.
	std::string _description;

	// Ride cost, in units of dollars; Must be positive
	double _cost_dollars;

	// Ride time in minutes; most be non-negative.
	double _time_minutes;
};


// Alias for a vector of shared pointers to RideItem objects.
typedef std::vector<std::shared_ptr<RideItem>> RideVector;


// Load all the valid ride items from the CSV database
// Ride items that are missing fields, or have invalid values, are skipped.
// Returns nullptr on I/O error.
std::unique_ptr<RideVector> load_ride_database(const std::string& path)
{
	std::unique_ptr<RideVector> failure(nullptr);

	std::ifstream f(path);
	if (!f)
	{
		std::cout << "Failed to load ride database; Cannot open file: " << path << std::endl;
		return failure;
	}

	std::unique_ptr<RideVector> result(new RideVector);

	size_t line_number = 0;
	for (std::string line; std::getline(f, line); )
	{
		line_number++;

		// First line is a header row
		if (line_number == 1)
		{
			continue;
		}

		std::vector<std::string> fields;
		std::stringstream ss(line);

		for (std::string field; std::getline(ss, field, '^'); )
		{
			fields.push_back(field);
		}

		if (fields.size() != 3)
		{
			std::cout
				<< "Failed to load ride database: Invalid field count at line " << line_number << "; Want 3 but got " << fields.size() << std::endl
				<< "Line: " << line << std::endl
				;
			return failure;
		}

		std::string
			descr_field = fields[0],
			cost_dollars_field = fields[1],
			time_minutes_field = fields[2]
			;

		auto parse_dbl = [](const std::string& field, double& output)
		{
			std::stringstream ss(field);
			if (!ss)
			{
				return false;
			}

			ss >> output;

			return true;
		};

		std::string description(descr_field);
		double cost_dollars, time_minutes;
		if (
			parse_dbl(cost_dollars_field, cost_dollars)
			&& parse_dbl(time_minutes_field, time_minutes)
			)
		{
			result->push_back(
				std::shared_ptr<RideItem>(
					new RideItem(
						description,
						cost_dollars,
						time_minutes
					)
					)
			);
		}
	}

	f.close();

	return result;
}


// Convenience function to compute the total cost and time in an RideVector.
// Provide the RideVector as the first argument
// The next two arguments will return the cost and time back to the caller.
void sum_ride_vector
(
	const RideVector& rides,
	double& total_cost,
	double& total_time
)
{
	total_cost = total_time = 0;
	for (auto& ride : rides)
	{
		total_cost += ride->cost();
		total_time += ride->rideTime();
	}
}


// Convenience function to print out each RideItem in an RideVector,
// followed by the total kilocalories and protein in it.
void print_ride_vector(const RideVector& rides)
{
	std::cout << "*** ride Vector ***" << std::endl;

	if (rides.size() == 0)
	{
		std::cout << "[empty ride list]" << std::endl;
	}
	else
	{
		for (auto& ride : rides)
		{
			std::cout
				<< "Ye olde " << ride->description()
				<< " ==> "
				<< "Cost of " << ride->cost() << " dollars"
				<< "; time in minutes = " << ride->rideTime()
				<< std::endl
				;
		}

		double total_cost, total_time;
		sum_ride_vector(rides, total_cost, total_time);
		std::cout
			<< "> Grand total cost: " << total_cost << " dollars" << std::endl
			<< "> Grand total time: " << total_time
			<< std::endl
			;
	}
}


// Filter the vector source, i.e. create and return a new RideVector
// containing the subset of the ride items in source that match given
// criteria.
// This is intended to:
//	1) filter out ride with zero or negative time that are irrelevant to our optimization
//	2) limit the size of inputs to the exhaustive search algorithm since it will probably be slow.
//
// Each ride item that is included must have at minimum min_time and at most max_time.
//	(i.e., each included ride item's time must be between min_time and max_time (inclusive).
//
// In addition, the the vector includes only the first total_size ride items that match these criteria.
std::unique_ptr<RideVector> filter_ride_vector
(
	const RideVector& source,
	double min_time,
	double max_time,
	int total_size
)
{
	// TODO: implement this function, then delete the return statement below
	std::unique_ptr<RideVector> final(new RideVector);
	int size = 0;
	for (size_t i = 0; i < source.size(); i++)
	{
		if (size != total_size)
		{
			if (source[i]->rideTime() > min_time && source[i]->rideTime() < max_time)
			{
				final->push_back(source[i]);
				size++;
			}
		}
		else
		{
			break;
		}


	}
	return final;

}


// Compute the optimal set of ride items with a greedy algorithm.
// Specifically, among the ride items that fit within a total_cost dollar budget,
// choose the rides whose time is greatest.
// Repeat until no more ride items can be chosen, either because we've run out of ride items,
// or run out of dollars.
std::unique_ptr<RideVector> greedy_max_time
(
	const RideVector& rides,
	double total_cost
)
{
	// TODO: implement this function, then delete the return statement below
	RideVector copy_rides = rides;
	std::unique_ptr<RideVector> result(new RideVector);
	int result_cost = 0;
	double time_cost = 0;
	int index = 0;
	while (copy_rides.size() > 0)
	{
		for (size_t i = 0; i < copy_rides.size(); i++)
		{
			if ((copy_rides[i]->rideTime() / copy_rides[i]->cost()) > time_cost)
			{
				time_cost = copy_rides[i]->rideTime() / copy_rides[i]->cost();
				index = i;
			}
		}
		time_cost = 0;
		if ((result_cost + copy_rides[index]->cost()) <= total_cost)
		{
			result->push_back(copy_rides[index]);
			result_cost += copy_rides[index]->cost();
		}
		copy_rides.erase(copy_rides.begin() + index);
	}
	return result;
}


// Compute the optimal set of ride items with an exhaustive search algorithm.
// Specifically, among all subsets of ride items, return the subset whose cost
// in dollars fits within the total_cost budget and whose total time is greatest.
// To avoid overflow, the size of the ride items vector must be less than 64.
std::unique_ptr<RideVector> exhaustive_max_time
(
	const RideVector& rides,
	double total_cost
)
{
	// TODO: implement this function, then delete the return statement below
	std::unique_ptr<RideVector> best(new RideVector);
	for (uint64_t bits = 0; bits <= pow(2, rides.size()) - 1; bits++)
	{
		RideVector candidate;
		for (size_t j = 0; j < rides.size(); j++)
		{
			if (((bits >> j) & 1) == 1)
			{
				candidate.push_back(rides[j]);
			}
		}
		double candidate_cost = 0.0;
		double candidate_time = 0.0;
		double best_cost = 0.0;
		double best_time = 0.0;
		sum_ride_vector(candidate, candidate_cost, candidate_time);
		sum_ride_vector(*best, best_cost, best_time);
		if (candidate_cost <= total_cost)
		{
			if (best->empty() || candidate_time > best_time)
			{
				*best = candidate;
			}
		}
	}
	return best;
}
