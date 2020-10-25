#include "TrafficModel.h"

TrafficModel::TrafficModel() { }
TrafficModel::~TrafficModel(){ }

void TrafficModel::set_commands(vector<string> commands)
{
	this->commands = commands;
}

/* A helper function. 
 * Given a specific id as the input, it searches the lane change command stored in
 * "commands". Returns the value of the turn-light in the command for this car id.
 * 0 = no light, 1 = left light, 2 = right light.
 */
int TrafficModel::get_lane_change_command(int id)
{
	int count = commands.size();
	for (int i = 0; i < count; i++){
		size_t found = commands[i].find(',');
		string iter_id = commands[i].substr(0, found);
		string light = commands[i].substr(found+1, commands[i].size()-1);
		stringstream ss1(iter_id);
		int id_value;
		ss1 >> id_value;
		if (id_value == id){
			stringstream ss2(light);
			int light_value;
			ss2 >> light_value;
			return light_value;
		}
	}
	return 0;
}

/*
 * The function that updates the vehicle positions and states. 
 */
void TrafficModel::update()
{

  Car *car1; //main car
  Car *car2; //car in front 
  Car *car3; //car to the side
  Car *car4; 
  Car *car5; //next in line
  bool right;
  bool left;
  bool forward;
  int direction;
  unsigned int static_size;


  //reset the moved modifiers for all cars
  for(unsigned int i = 0; i < platoons.size(); i++) {
    car4 = platoons[i].get_head();
    for(unsigned int j = 0; j < platoons[i].size(); j++) {
      car4->set_moved(false);
      if(car4->get_next() != NULL) {
        car4 = car4->get_next();
      }
    }
  }

  //cycling through the platoons
  for(unsigned int i = 0; i < platoons.size(); i++) {
    
    
    //set up car instances
    car1 = platoons[i].get_head();

    //need to set size here since platoon size may change in loop
    static_size = platoons[i].size();


    //cycling through each car in each platoon to determine its actions 
    for(unsigned int j = 0; j < static_size; j++) {

      //cout << "New car id " << car1->get_id() << "has modifier " << car1->get_moved() << endl;
      
      //find the car that will be updated after the current one 
      car5 = car1->get_next();
      if(car5 != NULL) {
        while (car5->get_moved() == true) {
          car5 = car5->get_next();
        }
      }
      
      //ensure a car does not move twice in a tick
      if (car1->get_moved() != true) {

      
        right = true;
        left = true;
        forward = true;
        direction = this->get_lane_change_command(car1->get_id());
        

        //check whether the car can drive forward


        //set the second car instance
        car2 = car1->get_prev();
        
        //check if there is a car right infront of car1
        if(car2 != NULL && car2->get_position() == car1->get_position() + 1) {
          forward = false;    
        }

        
        //cout << "Forward Check done" << endl;
        
        //check whether the car can turn right 
        //check for car and lane
        

        //this if statement checks whether there is a platoon to the right
        if (i != platoons.size() - 1) { 
          
          //checking that the platoon to the right has cars
          //if not, bool right should remain true so no code required
          if(platoons[i+1].size() != 0) { 
            
            car3 = platoons[i+1].get_head();
            
            //this loop checks through the cars in the platoon on the right to see if any are in the same position
            for(unsigned int k = 0; k < platoons[i+1].size(); k++) {
              if(car1->get_position() == car3->get_position()) {
                right = false;
              }
              car3 = car3->get_next();
            }

          }
        } else {
          right = false;
        }
        
        //cout << "Right Check done" << endl;

        //check whether the car can turn left 
        //check for car and lane
        

        //this if statement checks whether there is a platoon to the left
        if (i != 0) { 
          
          //checking that the platoon to the left has cars
          //if not, bool left should remain true so no code required
          if(platoons[i-1].size() != 0) { 

            car3 = platoons[i-1].get_head();

            //this loop checks through the cars in the platoon on the right to see if any are in the same position
            for(unsigned int k = 0; k < platoons[i-1].size(); k++) {
              if(car1->get_position() == car3->get_position()) {
                left = false;
              }
              car3 = car3->get_next();
            }
          }
        } else { //this is left most platoon
          left = false;
        }
        //cout << "Left Check done" << endl;

        //check and execute car command 

        
        //if the car is doing a legal right turn
        if (direction == 2 && right == true) { 
          
            // remove car from this list 
          
            platoons[i].remove(car1);
        
            //add to list to the right
            platoons[i+1].insert(car1);
            //cout << "Right Turn done" << endl;
        }
        
        //if the car is doing a legal left turn
        else if (direction == 1 && left == true) {
          
          // remove car from this list 
          
            platoons[i].remove(car1);

            //add to list to the left
            platoons[i-1].insert(car1);
            //cout << "Left Turn done" << endl;
        }
        
        //car will drive forward if it can and didn't turn
        else if (forward == true) {
          
          //move car1 forward on cell
          car1->set_position(car1->get_position()+1);
          //cout << "Forward done" << endl;
        }

        
        //set next car
        //cout << "Ending loop for car id " << car1->get_id() << endl;
        
        if(car5 != NULL) {
          car1 = car5;
        }
        
        /*
        if(car1 != NULL) {
          cout << "New ID should be " << car1->get_id() << endl;
        } else {
           cout << "New car is NULL " << endl; 
        }*/

      }
    }
  }
}

/*
 * Initialization based on the input information
 */
void TrafficModel::initialize(vector<string> info)
{
	int lane_count = info.size();
	for (int i = 0; i < lane_count; i++){
		Platoon p = Platoon(info[i]);
		platoons.push_back(p);
	}
}

//
// IMPORTANT: DO NOT CHANGE THE FUNCTIONS BELOW THIS LINE
//

// Returns all the vehicle states in the system
vector<string> TrafficModel::get_system_state()
{
	vector<string> output;
	int size = platoons.size();
	for (int i = 0; i < size; i++){
		// get the last vehicle in the platoon
		Car* temp = platoons[i].get_tail();
		string s = "";
		ostringstream out;
		while (temp != NULL){
			out << ";(" << temp->get_id() << "," << i << "," << temp->get_position() << \
					 "," << get_lane_change_command(temp->get_id()) << ")";
			temp = temp->get_prev();
		}

		output.push_back(out.str());
	}
	return output;
}

//Get the state of cars as a 3D vector representing car data in lane/pos
vector<vector<vector<int>>> TrafficModel::get_car_state(){

	vector<string> state = get_system_state();
	vector<vector<vector<int>>> cars;
	string remainingCars;
	string newCar;

	//Parse state into vector of car data
	for (unsigned int i = 0; i < state.size(); i++){
		vector<vector<int>> carRow;
		remainingCars = state[i];
		remainingCars.push_back(';');

		//Parse string of entire lane into individual car data
		while (remainingCars.size() > 1) {
			remainingCars = remainingCars.substr(1);
			size_t pos = remainingCars.find(";");
			newCar = remainingCars.substr(1,pos-2);

			carRow.push_back(parse_car_data(newCar));

			if(pos!=string::npos){
				remainingCars = remainingCars.substr(pos);
			} else {
				remainingCars = "";
			}
		}
		cars.push_back(carRow);
	}
	return cars;
}

//Parse string in form (id,lane,pos,turn) into vector of ints
vector<int> TrafficModel::parse_car_data (string& car){
	vector<int> carData;
	string delimiter = ",";
	size_t last = 0;
	size_t next = 0;
	int index = 0;

	while ((next = car.find(delimiter, last)) != string::npos) {
		carData.push_back(stoi(car.substr(last, next-last)));
		last = next + 1;
		index++;
	}
	carData.push_back(stoi(car.substr(last)));
	return carData;
}
