#include "support/configCosmos.h"
#include "support/convertlib.h"
#include "support/timelib.h"
#include "support/jsondef.h"
#include "gtest/gtest.h"

//using namespace Convert;
using namespace std;
using namespace json11;

class initial_conditions {

  public:

	int				id;
	string			name;
	int				duration;
	vector<double>	initial_resource_levels;
	int				number_of_events;
	int				number_of_resources;

	// Argument constructor
	initial_conditions(
		const int id,
		const string& name,
		const int duration,
		const vector<double>& initial_resource_levels,
		const int number_of_events,
		const int number_of_resources
	) : id(id),
		name(name),
		duration(duration),
		initial_resource_levels(initial_resource_levels),
		number_of_events(number_of_events),
		number_of_resources(number_of_resources)
	{
			//validate_arguments();
	}

	// Default constructor
	initial_conditions() = default;

	// Method to convert an instance of the class to JSON format
	Json to_json() const {
		return Json::object {
			{ "id", id },
			{ "name", name },
			{ "duration", duration },
			{ "initial_resource_levels", initial_resource_levels_to_json() },
			{ "number_of_events", number_of_events },
			{ "number_of_resources", number_of_resources }
		};
	}

	// Method to initialize the class from JSON
	void from_json(const string& json_string) {
		string error;
		Json p = Json::parse(json_string, error);
		if(error.empty())	{
			if(p["id"].is_number())					id = p["id"].number_value();
			if(p["name"].is_string())				name = p["name"].string_value();
			if(p["duration"].is_number())			duration = p["duration"].number_value();
			if(p["number_of_events"].is_number())	number_of_events = p["number_of_events"].number_value();
			if(p["number_of_resources"].is_number())	number_of_resources = p["number_of_resources"].number_value();
			if(p["initial_resource_levels"].is_array()) {
				initial_resource_levels.clear();
				for (auto& item : p["initial_resource_levels"].array_items()) {
					initial_resource_levels.push_back(round(item.number_value()*10000.0)/10000.0);
				}
			}
		}

		// Validate all arguments
		//validate_arguments();
		return;
	}
	// Equality operator
	bool operator==(const initial_conditions& other) const {
    	const double delta = 0.00001;
    	if (id != other.id) { cout<<"oh shit"<<endl; return false;}
    	if (name != other.name) return false;
    	if (duration != other.duration) return false;
    	if (number_of_events != other.number_of_events) return false;
    	if (number_of_resources != other.number_of_resources) return false;
    	if (initial_resource_levels.size() != other.initial_resource_levels.size()) return false;
		cout<<"good"<<endl;
    	for (size_t i = 0; i < initial_resource_levels.size(); ++i) {
        	if (fabs(initial_resource_levels[i] - other.initial_resource_levels[i]) > delta) return false;
    	}
    	return true;
	}

private:
	// Helper function to validate arguments // this function throws
	void validate_arguments() {
		if (id <= 0) {
			throw invalid_argument("initial_conditions::id must be greater than 0.");
		}
		if (name.empty()) {
			throw invalid_argument("initial_conditions::name cannot be empty.");
		}
		if (duration <= 0) {
			throw invalid_argument("initial_conditions::duration must be greater than 0.");
		}
		if (number_of_events < 0) {
			throw invalid_argument("initial_conditions::number_of_events of events must be zero or greater.");
		}
		if (number_of_resources < 0) {
			throw invalid_argument("initial_conditions::number_of_resources must be zero or greater.");
		}
		if (initial_resource_levels.size() != static_cast<size_t>(number_of_resources)) {
			throw invalid_argument("initial_conditions::number_of_resources must be equal to initial_conditions::initial_resource_levels.size().");
		}
	}

	// Helper function to convert the vector<double> to JSON array format
	Json initial_resource_levels_to_json() const {
		vector<Json> json_array;
		for (const double& value : initial_resource_levels) {
			json_array.push_back(round(value*10000.0)/10000.0);
		}
		return json_array;
	}
};

// Example Usage
int main() {

	// Create an instance of initial_conditions using the argument constructor
	initial_conditions conditions_1(1, "Test Condition", 120, {10.5123678, 20.7123678, 15.3123678}, 5, 3);

	// Convert to JSON
	cout << "conditions_1.to_json(): " << conditions_1.to_json() << endl;

	// Parse JSON string to create an instance
	initial_conditions conditions_2;
	conditions_2.from_json(conditions_1.to_json().dump());

	cout << "conditions_2.to_json(): " << conditions_2.to_json() << endl;

	// define == / != ?
	bool huh=conditions_1==conditions_2;
	cout<<"huh = "<<huh<<endl;



	return 0;
}
