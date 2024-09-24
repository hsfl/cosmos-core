#include "support/configCosmos.h"
#include "support/stringlib.h"
#include "support/convertdef.h"
#include "support/convertlib.h"
#include "support/jsondef.h"
#include <fstream>
#include <algorithm>  // For max_element and sort
#include <dirent.h>
#include <sys/types.h>


using namespace json11;

// Program to output overpass events for a given satellite and it's targets (as defined by minimum elevation angle)

// Function to compute the elevation angle in degrees
double elevation_angle(const double target[], const double satellite[]) {
	// Line-of-sight vector from target to satellite
	double los[3];
	los[0] = satellite[0] - target[0];
	los[1] = satellite[1] - target[1];
	los[2] = satellite[2] - target[2];

	// Compute magnitudes
	double los_mag = std::sqrt(los[0]*los[0] + los[1]*los[1] + los[2]*los[2]);
	double tgt_mag = std::sqrt(target[0]*target[0] + target[1]*target[1] + target[2]*target[2]);

	// Normalize vectors
	double los_unit[3] = { los[0]/los_mag, los[1]/los_mag, los[2]/los_mag };
	double up_unit[3] = { target[0]/tgt_mag, target[1]/tgt_mag, target[2]/tgt_mag };

	// Compute dot product
	double dot = los_unit[0]*up_unit[0] + los_unit[1]*up_unit[1] + los_unit[2]*up_unit[2];

	// Ensure dot product is within valid range for asin()
	if (dot > 1.0) dot = 1.0;
	if (dot < -1.0) dot = -1.0;

	// Compute elevation angle in radians
	double elevation_rad = std::asin(dot);

	// Convert to degrees
	return elevation_rad * (180.0 / M_PI);
}


int main(int argc, char *argv[])
{

  // Open the CSV files // todo change to command line args // how to differntiate targets and satellites?
	// Satellite name remains the same for new
	std::string satellite_name = "Mothership";

	// Vectors to hold satellite and target names (if needed)
	//std::vector<std::string> satellite_names; // to be implemented
	std::vector<std::string> target_names;

	// load target file names from target directory
	std::string targets_directory = "../python/plot_orbit/targets/";
	std::string satellites_directory = "../python/plot_orbit/satellites/";
	DIR* dir = opendir(targets_directory.c_str());
	if (dir != nullptr) {
		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr) {
			std::string filename(entry->d_name);

			// Skip "." and ".." entries
			if (filename == "." || filename == "..")
				continue;

			// Check if the filename ends with ".eci"
			if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".eci") {
				// Remove the ".eci" extension to get the target name
				std::string target_name = filename.substr(0, filename.size() - 4);
				target_names.push_back(target_name);
			}
		}
		closedir(dir);
	} else {
		// Handle error opening directory
		std::cerr << "Failed to open directory: " << targets_directory << std::endl;
	}

	// Create an empty JSON array to hold all pass events (i.e. satellites over any target)
	json11::Json::array pass_events;

	// for all target
	for(size_t id = 0; id < target_names.size(); ++id)	{

   			std::string satLine, tgtLine;
   			int lineNumber = 0;
			bool inSequence = false;
			vector<vector<double>> passes;
			vector<double> pass;
			vector<vector<double>> timestamps;
			vector<double> timestamp;

			// Open the target file
			std::ifstream target_file(targets_directory+target_names[id]+".eci");
			if (!target_file.is_open()) {
				std::cerr << "Failed to open target file: " << targets_directory+target_names[id]+".eci" << std::endl;
				continue; // Skip to the next file
			}

			// Open the satellite file
			std::ifstream satellite_file(satellites_directory + satellite_name + ".eci");
			if (!satellite_file.is_open()) {
				std::cerr << "Failed to open satellite file: " << satellites_directory + satellite_name + ".eci" << std::endl;
				return 1;
			}

  			while (std::getline(satellite_file, satLine) && std::getline(target_file, tgtLine)) {
	  			lineNumber++;

	  			// Parse the satellite line
	  			std::stringstream satStream(satLine);
	  			std::string satItem;
	  			std::vector<double> satData;
	  			while (std::getline(satStream, satItem, ',')) {
		  			try {
			  			satData.push_back(std::stod(satItem));
		  			} catch (const std::invalid_argument& e) {
			  			std::cerr << "Invalid data in satellite file at line " << lineNumber << std::endl;
			  			break;
		  			}
	  			}
	
	  			if (satData.size() < 4) {
		  			std::cerr << "Not enough data in satellite file at line " << lineNumber << std::endl;
		  			continue;
	  			}
	
	  			double satellite[3] = { satData[0], satData[1], satData[2] };
			double sat_t = satData[3]; // MJD timestamp
	
	  			// Parse the target line
	  			std::stringstream tgtStream(tgtLine);
	  			std::string tgtItem;
	  			std::vector<double> tgtData;
	  			while (std::getline(tgtStream, tgtItem, ',')) {
		  			try {
			  			tgtData.push_back(std::stod(tgtItem));
		  			} catch (const std::invalid_argument& e) {
			  			std::cerr << "Invalid data in target file at line " << lineNumber << std::endl;
			  			break;
		  			}
	  			}
	
	  			if (tgtData.size() < 4) {
		  			std::cerr << "Not enough data in target file at line " << lineNumber << std::endl;
		  			continue;
	  			}

	  			double target[3] = { tgtData[0], tgtData[1], tgtData[2] };
			//double target_t = tgtData[3]; // MJD timestamp

	  			// Calculate the elevation angle
	  			double elevation = elevation_angle(target, satellite);

	  			// Check if elevation meets the condition
	  			if (elevation > 0) {
		  			if (!inSequence) {
			  			// Starting a new sequence
					pass.clear();
					timestamp.clear();
			  			inSequence = true;
		  			}
				// store the result
				pass.push_back(elevation);
				timestamp.push_back(sat_t);

	  			} else {
		  			// Elevation does not meet the condition
	
				if(!pass.empty())	{
					passes.push_back(pass);
					pass.clear();
					timestamps.push_back(timestamp);
					timestamp.clear();
				}
		  			inSequence = false;
	  			}
  			}

		// if you were in the middle of a sequence
		if (inSequence)	{
			passes.push_back(pass);
			pass.clear();
			timestamps.push_back(timestamp);
			timestamp.clear();
		}

		// now make some json objects
		double max_elevation = 0.0;

		for(size_t i = 0; i < passes.size(); ++i)	{
			max_elevation = *std::max_element(passes[i].begin(), passes[i].end());
			double start_mjd = timestamps[i][0];
			double end_mjd = *timestamps[i].rbegin();
			string start_iso = utc2iso8601(start_mjd);
			string end_iso = utc2iso8601(end_mjd);

 			// Format max_elevation in the name string
			std::ostringstream name_stream;
			name_stream << satellite_name << " over " << target_names[id] << " @ " << start_iso << " (max elev = " << std::fixed << std::setprecision(1) << max_elevation << " deg)";
			std::string name = name_stream.str();
			int duration = timestamps[i].size()-1;

			json11::Json::object pass_event{
				{"name", name},
				{"target", target_names[id]},
				{"start_mjd", start_mjd},
				{"end_mjd", end_mjd},
				{"start_iso8601", start_iso},
				{"end_iso8601", end_iso},
				{"duration", duration},
				{"max_elevation", max_elevation}
			};
			// Add the pass_event object to the pass_events array
			pass_events.push_back(pass_event);
		}

   		satellite_file.close();
   		target_file.close();
	} // end for all target .eci files

	// now sort the pass_events array by start_mjd ascending
	// Define the comparator function
	auto comparator = [](const json11::Json& a, const json11::Json& b) {
		return a["start_mjd"].number_value() < b["start_mjd"].number_value();
	};

	// Sort the pass_events array by pass start time
	std::sort(pass_events.begin(), pass_events.end(), comparator);


	json11::Json::object pass_events_object = {{ "Pass Events for "+satellite_name, pass_events }};
	cout<<pass_events_object<<endl;

	// now extract the observation windows

	// Create an array to hold the observation windows
	json11::Json::array observation_windows;

	// Create a set to hold unique target names in the current observation window
	std::vector<std::string> current_targets;

	if (!pass_events.empty()) {
		// Initialize the first observation window with the first pass event
		double current_start_mjd = pass_events[0]["start_mjd"].number_value();
		double current_end_mjd = pass_events[0]["end_mjd"].number_value();
		// Insert the first target name in order of observation, if not already present
		std::string target_name = pass_events[0]["target"].string_value();
		if (std::find(current_targets.begin(), current_targets.end(), target_name) == current_targets.end()) {
			current_targets.push_back(target_name);
		}

		// Optionally, store the earliest start_iso8601 and latest end_iso8601
		std::string current_start_iso = pass_events[0]["start_iso8601"].string_value();
		std::string current_end_iso = pass_events[0]["end_iso8601"].string_value();

		// Iterate over the pass events starting from the second one
		for (size_t i = 1; i < pass_events.size(); ++i) {
			double next_start_mjd = pass_events[i]["start_mjd"].number_value();
			double next_end_mjd = pass_events[i]["end_mjd"].number_value();

			// Check if the next pass overlaps with the current observation window
			if (next_start_mjd <= current_end_mjd) {
				// Overlaps, so extend the current observation window
				if (next_end_mjd > current_end_mjd) {
					current_end_mjd = next_end_mjd;
					current_end_iso = pass_events[i]["end_iso8601"].string_value();
				}
				// Insert the overlapping target name in order of observation, if not already present
				std::string target_name = pass_events[i]["target"].string_value();
				if (std::find(current_targets.begin(), current_targets.end(), target_name) == current_targets.end()) {
					current_targets.push_back(target_name);
				}
			} else {
				// No overlap, finalize the current observation window
				int duration_seconds = static_cast<int>((current_end_mjd - current_start_mjd) * 86400. + 0.5);

				// Create the observation window JSON object
				json11::Json::object observation_window{
					{"name", satellite_name + " Observation Window #" + std::to_string(observation_windows.size())},
					{"start_mjd", current_start_mjd},
					{"end_mjd", current_end_mjd},
					{"start_iso8601", current_start_iso},
					{"end_iso8601", current_end_iso},
					{"duration", duration_seconds},
					{"targets", json11::Json::array(current_targets.begin(), current_targets.end())} // Preserve the order of observation
		};

				// Add the observation window to the array
				observation_windows.push_back(observation_window);

				// Start a new observation window with the next pass event
				current_start_mjd = next_start_mjd;
				current_end_mjd = next_end_mjd;
				current_start_iso = pass_events[i]["start_iso8601"].string_value();
				current_end_iso = pass_events[i]["end_iso8601"].string_value();
			 	current_targets.clear(); // Reset for the new observation window
				current_targets.push_back(pass_events[i]["target"].string_value()); // Insert the first target of new window
			}
		}

		// Add the last observation window
		int duration_seconds = static_cast<int>((current_end_mjd - current_start_mjd) * 86400. + 0.5);

		json11::Json::array targets_json;
		for (const auto& target : current_targets) {
			targets_json.push_back(target);
		}

		json11::Json::object observation_window{
			{"name", satellite_name + " Observation Window #" + std::to_string(observation_windows.size())},
			{"start_mjd", current_start_mjd},
			{"end_mjd", current_end_mjd},
			{"start_iso8601", current_start_iso},
			{"end_iso8601", current_end_iso},
			{"duration", duration_seconds},
			{"targets", json11::Json::array(current_targets.begin(), current_targets.end())} // Preserve the order of observation
		};

		observation_windows.push_back(observation_window);
	}


	// Wrap the observation windows in a JSON object with a descriptive key
	json11::Json::object output{
		{"Observation Windows for " + satellite_name, observation_windows}
	};

	// Output the observation windows
	std::cout << output << std::endl;

	cout<<"number of targets = "<<target_names.size()<<endl;
	cout<<"number of passes over targets = "<<pass_events.size()<<endl;
	cout<<"number of observation windows = "<<observation_windows.size()<<endl;

	return 0;

}
