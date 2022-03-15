#ifndef CARRECORD_H
#define CARRECORD_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std; 


struct Car {
    int deleteNext; 
    unsigned int id;
    char model[3];
    unsigned int year;
    unsigned int price;
    char transmission[9];
    unsigned int mileage;
    char fuelType[6];
    double tax;
    double milesPerGaloon;
    double engineSize;

    Car(unsigned int id,
        string Model,
        unsigned int year,
        unsigned int price,
        string Transmission,
        unsigned int mileage,
        string FuelType,
        double tax,
        double milesPerGaloon,
        double engineSize) {
        this->id = id;
        strcpy(model, Model.c_str());
        this->year = year;
        this->price = price;
        strcpy(transmission, Transmission.c_str()); 
        this->mileage = mileage;
        strcpy(fuelType, FuelType.c_str()); 
        this->tax = tax;
        this->milesPerGaloon = milesPerGaloon;
        this->engineSize = engineSize;
        this->deleteNext = -2;
    }

    Car(){};

    void display(){
    cout << "\n ID: " << id << "\n model: " << string(model,sizeof(model)) << "\n year: " << year << "\n price: " << price << "\n transmission: " << string(transmission,sizeof(transmission)) << "\n mileage: " << mileage << "\n fuelType: " << string(fuelType,sizeof(fuelType)) << "\n tax: " << tax << "\n mpg: " << milesPerGaloon << "\n engineSize: " << engineSize;
  }


};

#endif
