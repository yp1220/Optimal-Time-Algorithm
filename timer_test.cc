
#include "iostream"

#include "maxtime.hh"
#include "rubrictest.hh"
#include "timer.hh"

using namespace std;

int main()
{
  for (int i = 0; i < 20; i++)
  {
         int total_size = i;
         auto all_rides = load_ride_database("ride.csv");
         auto filtered_rides = filter_ride_vector(*all_rides, 1, 1000, total_size);
         Timer timer;
         greedy_max_time(*filtered_rides, 2000);
         double elapsed = timer.elapsed();
         cout << i+1 << "-> "<< elapsed << endl;
  }
  for (int i = 0; i < 10; i++)
  {
         int total_size = i;
         auto all_rides = load_ride_database("ride.csv");
         auto filtered_rides = filter_ride_vector(*all_rides, 1, 1000, total_size);
         Timer timer;

         exhaustive_max_time(*filtered_rides, 2000);
         double elapsed = timer.elapsed();
         cout << i+1 << "-> " << elapsed << endl;
  }

}
