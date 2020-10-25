#include "Platoon.h"

Platoon::Platoon(string init)
{
    // the string 'init' looks like:
    // a,b ; c,d ; ... where, a and c are the vehicle id, b and d are positions.
    stringstream ss(init);
	string segment;
	while(getline(ss, segment, ';'))
	{
		size_t found = segment.find(",");
		if (found!=std::string::npos){
			string id = segment.substr(0, found);
			string pos = segment.substr(found+1, segment.size()-1);

			// conversion to integer
			stringstream s1(id);
			int id_value;
			s1 >> id_value;
			stringstream s2(pos);
			int pos_value;
			s2 >> pos_value;
			Car* car = new Car(id_value, pos_value);

            if (head == NULL)
            {
                head = car;
                tail = car;
            }
            else
            {
                /*
                tail->set_next(car);
                car->set_prev(tail);
                tail = car;
                car->set_next(NULL); */
                this->insert(car);
            }
		}
	}
}

Car* Platoon::get_tail()
{
	return tail;
}

Car* Platoon::get_head()
{
	return head;
}

//returns the number of cars in the platoon
unsigned int Platoon::size() {
  unsigned int i = 1;
  Car *car1 = head;

  if (head == NULL) { //if there are no cars in the list
    return 0;
  }

  while(car1 != tail) {
    car1 = car1->get_next();
    i++;
  }

  return i;
}

//remove a car from the platoon
void Platoon::remove(Car* c) {
  
  int pos = c->get_position();
  

  //might need to play around
  Car *car1 = this->head;
  int i = car1->get_position();

  //to remove car c, we need to link the car before to the car after 
  //assumes that car that is being removed is in the platoon
  while(i != pos){
    car1 = car1->get_next();
    i = car1->get_position();
  }


  Car *car2 = NULL;
  Car *car3 = NULL;

  //once the right car is found, remove it by linking the next and previous
  if (car1->get_prev() != NULL && car1->get_next() != NULL) { //there are cars infront and behind of car1
    
    car2 = car1->get_prev();
    car3 = car1->get_next();

    car2->set_next(car3);
    car3->set_prev(car2);

    car1->set_prev(NULL);
    car1->set_next(NULL);
    //car1 = NULL;

  } else if (car1->get_prev() != NULL) { //if car1 is the tail
    
    car2 = car1->get_prev();
    car2->set_next(NULL);
    tail = car2;

    car1->set_next(NULL);
    car1->set_prev(NULL);
    //car1 = NULL;

  } else if (car1->get_next() != NULL) { //if car1 is the head
    
    car2 = car1->get_next();
    car2->set_prev(NULL);
    head = car2;

    car1->set_next(NULL);
    car1->set_prev(NULL);
    //car1 = NULL;

  } else { //if car1 is the head and tail
    
    car1->set_next(NULL);
    car1->set_prev(NULL);
    //car1 = NULL;
    tail = NULL;
    head = NULL;

  }
}

void Platoon::append(Car* c){
  Car *car1 = c;
  if(this->tail != NULL) {
    car1->set_next(this->tail);
  }
  tail = car1;
}

//Add a car to the start of the platoon
void Platoon::prepend(Car* c){
  Car *car1 = c;
  if(this->head != NULL) {
    car1->set_next(this->head);
  }
  head = car1;
}



//insert a car into the linked list, at the correct position between the other cars
void Platoon::insert(Car* c){

  //ensure the car doesn't move twice
  c->set_moved(true);

  //cout << "0";
  if(this->head == NULL && this->tail == NULL) {
    //if platoon is empty
    //cout << "1";
    tail = c;
    head = c;
    c->set_next(NULL);
    c->set_prev(NULL);

  } else if (this->tail->get_position() > c->get_position()) {
    //if car c has the lowest position, insert at back 
    //cout << "2";
    c->set_next(NULL);
    c->set_prev(this->tail);
    this->tail->set_next(c);
    this->tail = c;

  } else if (this->head->get_position() < c->get_position()) {
    //if car c has the highest position, insert at front
    //cout << "3";
    c->set_next(this->head);
    c->set_prev(NULL);
    this->head->set_prev(c);
    head = c;
    
  } else {
    //if car c is somewhere in the middle
    //cout << "4";
    int pos = c->get_position();
    Car *car1 = this->tail;
    int i = car1->get_position();

    while(pos > i) {
      car1 = car1->get_prev();
      i = car1->get_position();
    }
     //the loop continues until the position of car1 is greater than the position of car c, meaning car c should be inserted next after car1. 
     //cout << "insert last part" << endl;
     Car *car2 = car1->get_next();

     car2->set_prev(c);
     car1->set_next(c);

     c->set_next(car2);
     c->set_prev(car1);
  } 
}



