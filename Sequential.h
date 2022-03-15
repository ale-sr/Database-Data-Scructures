#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include "Registro.h"

using namespace std;

struct comparator{
  inline bool operator() (const Registro& registro1, const Registro& registro2){
    if(registro1.id < registro2.id)
    return true;
    return false;
  }
};

class Sequential_File{
  private:
  int mainSize = 0;
  int auxSize = 0;

  public:

  Registro readLineFromCsv(string line){
    auto id = stoul(line.substr(0,6));
    auto model = line.substr(7,9);
    auto year = stoi(line.substr(11,14));
    auto price = stoul(line.substr(16,20));
    auto transmission = line.substr(23,31);
    auto mileage = stoul(line.substr(33,37));
    auto fuelType = line.substr(40,45);
    auto tax = stod(line.substr(46,48));
    auto mpg = stod(line.substr(50,53));
    auto engineSize = stod(line.substr(56,58));
    Registro r = Registro(id, model, year, price, transmission, mileage, fuelType, tax, mpg, engineSize);
    return r;
  }

  void insertAll(vector<Registro> registros){
    mainSize= 0;
    auxSize = 0;
    //Borrado de datos
    ofstream files;
    files.open("datos.bin", std::ofstream::out | std::ofstream::trunc | ios::binary);
    files.close();
    files.open("aux.bin", std::ofstream::out | std::ofstream::trunc);
    files.close();

    sort(registros.begin(), registros.end(), comparator());

    ofstream datos;
    datos.open("datos.bin", ios::binary | ios::in);
    if(datos.is_open()){
      for(auto registro:registros){
        registro.pos = datos.tellp()/78;
        registro.location = 'm';
        registro.nextPos = int(datos.tellp())/78 + 1;
        registro.nextLocation = 'm';
        datos.write((char*)&registro, sizeof(Registro));
        mainSize++;
      }
    }else{
      cout<<"No se pudo abrir el archivo";
    }
    datos.close();
  }

  vector<Registro> getAll(){
    ifstream datos;
    datos.open("datos.bin", ios::binary);
    vector<Registro> registros = {};
    Registro registro;
    if (datos.is_open()) {
      datos.seekg(0, ios::beg);
      while (datos.read((char *) &registro, sizeof(Registro))) {
        if(registro.location != '-'){
          registros.push_back(registro);
        }
      }
    } else { cout << "\nNo se pudo abrir el archivo"; }

    datos.open("aux.bin", ios::binary);
    if (datos.is_open()) {
      datos.seekg(0, ios::beg);
      while (datos.read((char *) &registro, sizeof(Registro))) {
        if(registro.location != '-'){
          registros.push_back(registro);
        }
      }
      } else { cout << "\nNo se pudo abrir el archivo"; }

    datos.close();
    return registros;
  }

  vector<Registro> getAllFromCsv(){
    ifstream datos;
    datos.open("fl_data.csv");
    vector<Registro> registros = {};
    Registro registro;
    string line;

    if(datos.is_open()){
      datos.seekg(0,ios::beg);
      while(getline(datos, line)){
        Registro r = readLineFromCsv(line);
        registros.push_back(r);
      }
    }
    else{cout<<"\nNo se pudo abrir el archivo";}
    datos.close();
    return registros;
  }
  
  void update(Registro& registro){
    ofstream datos;
    if (registro.location == 'm') {
      datos.open("datos.bin", ios::binary | ios::in);
    } else {
      datos.open("aux.bin", ios::binary | ios::in);
    }

    if (datos.is_open()) {
      int pos = registro.pos;
      datos.seekp(pos*sizeof(Registro), ios::beg);
      datos.write((char *) &registro, sizeof(Registro));
    } else {
      cout << "No se pudo abrir el archivo";
    }
    datos.close();
  }

  bool readNext(Registro & registro){
    ifstream datos;

    if(registro.nextLocation == 'm'){
      datos.open("datos.bin", ios::binary);
    }else{
      datos.open("aux.bin", ios::binary);
    }

    datos.seekg(registro.nextPos*sizeof(Registro), ios::beg);
		if(datos.read((char*)&registro, sizeof(Registro))){
      datos.close();
      return true;
    }
    return false;
  }
  
  void add(Registro registro){
    ofstream datos;
    datos.open("aux.bin", ios::binary | ios::app);
    if(datos.is_open()){
      if(auxSize == 5){
        vector<Registro> registros = getAll();
        insertAll(registros);
      }
      else{
        auto prev = search(registro.id);

        registro.location = 'a';
        registro.pos = datos.tellp();

        registro.nextPos = prev.nextPos;
        registro.nextLocation = prev.nextLocation;

        prev.nextPos = datos.tellp();
        prev.nextLocation = 'a';

        update(prev);
        datos.write((char*)&registro, sizeof(Registro));
        auxSize++;
      }
    }else{
      cout<<"No se pudo abrir el archivo";
    }
    datos.close();
  }
  
  Registro search(unsigned int key){
    ifstream datos;
    datos.open("datos.bin", ios::binary);

    Registro registro;
    int a = 0;
    int b = mainSize-1;
    int m;

    do{     
      m = ceil((a+b)/2);
      datos.seekg(m*sizeof(Registro), ios::beg);
      datos.read((char*)&registro, sizeof(Registro));
      if(registro.id > key){
		    b = m-1;
      }
      else if (registro.id < key) {
	      a = m;
      }
      else if(registro.id == key && registro.location != '-'){
        return registro;
      }
      else{
        b = m-1;
      }
    }while(a+1<b);

    m = ceil((a+b)/2);
    datos.seekg(m*sizeof(Registro), ios::beg);
    datos.read((char*)&registro, sizeof(Registro));
    Registro registroMin;

    if(registro.location == '-'){
      Registro registroMin;
      datos.seekg(0, ios::beg);
      while (datos.read((char *) &registro, sizeof(Registro))) {
        if(registro.location != '-' && registro.id < key){
          registroMin = registro;
          break;
        }
      }
      datos.open("aux.bin", ios::binary);
      datos.seekg(0, ios::beg);
      while (datos.read((char *) &registro, sizeof(Registro))) {
        if(registro.nextLocation != '-' && registro.id < key){
          registroMin = registro;
          break;
        }
      }
    }

    registroMin = registro;
    Registro registroNext = registroMin;
    while(registro.id < key && readNext(registroNext)){
      if(registroNext.id==key){
        return registroNext;
      }else if(registroNext.id > key){
        return registro;
      }
      registro = registroNext; 
    }

    datos.close();
    return registro;
  }
  
  vector<Registro> rangeSearch(unsigned int begin, unsigned int end){

    vector<Registro> registros = {};
    Registro registro = search(begin);

    if(registro.id < begin){
      readNext(registro);
    }
    while(registro.id >= begin && registro.id <= end){
      registros.push_back(registro);
      if(!readNext(registro)){
        break;
      }
    }
    return registros;
  }

  bool deleteRegistro(unsigned int key){
    Registro registro = search(key-1);

    Registro registroNext = registro;

    while(registro.id < key && readNext(registroNext)){
      if(registroNext.id==key){
        registro.nextPos = registroNext.nextPos;
        registro.nextLocation = registroNext.nextLocation;
        update(registro);
        registroNext.nextPos = -1;
        registroNext.nextLocation = '-';
        update(registroNext);
        registroNext.display();
        return true;
      }
      registro = registroNext;
    }
    return false;
  }
  
};