//id	model	year	price	transmission	mileage	fuelType	tax	mpg	engineSize
#ifndef REGISTRO_H
#define REGISTRO_H

#include <string>
#define ui unsigned int

using namespace std;

struct Registro{
  ui id;
  char model[3];
  int year;
  ui price;
  char transmission[9];
  ui mileage;
  char fuelType[6];
  double tax;
  double mpg;
  double engineSize;

  int pos;
  char location;
  int nextPos;
  char nextLocation;

  Registro(){};

  Registro(ui id, string model, int year, ui price, string transmission, ui mileage, string fuelType, double tax, double mpg, double engineSize ){
    this->id = id;
    strcpy(this->model, model.c_str());
    this->year = year;
    this->price = price;
    strcpy(this->transmission, transmission.c_str());
    this->mileage = mileage;
    strcpy(this->fuelType, fuelType.c_str());
    this->tax = tax;
    this->mpg = mpg;
    this->engineSize = engineSize;
  }

  void display(){
    cout << "\n ID: " << id << "\n model: " << string(model,sizeof(model)) << "\n year: " << year << "\n price: " << price << "\n transmission: " << string(transmission,sizeof(transmission)) << "\n mileage: " << mileage << "\n fuelType: " << string(fuelType,sizeof(fuelType)) << "\n tax: " << tax << "\n mpg: " << mpg << "\n engineSize: " << engineSize;
  }

};

#endif