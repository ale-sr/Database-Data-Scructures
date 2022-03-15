#include "Sequential.h"
#include "ExtendibleHash.h"
#include "carRecord.h"
#include <chrono>
#include <cstdio>
#include <random>

using namespace std::chrono;
using namespace std;

Car readLineFromCsv(string line){
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
  Car c = Car(id, model, year, price, transmission, mileage, fuelType, tax, mpg, engineSize);
  return c;
}

vector<Car> getAllFromCsv(){
  ifstream datos;
  datos.open("fl_data.csv");
  vector<Car> registros = {};
  Car registro;
  string line;

  if(datos.is_open()){
    datos.seekg(0,ios::beg);
    while(getline(datos, line)){
      Car r = readLineFromCsv(line);
      registros.push_back(r);
    }
  }
  else{cout<<"\nNo se pudo abrir el archivo";}
  datos.close();
  return registros;
}

std::random_device dev;
template<class T>
double Rand(T first, T last) {
    std::default_random_engine eng(dev());
    std::uniform_real_distribution<double> dis(first, last);
    return dis(eng);
}

void sequential_tests(){
  Sequential_File sf;
  auto registros = sf.getAllFromCsv();
  ofstream file("SF_Execution_Times.csv");
  //Tiempos medidos en microsegundos
  for(int i = 0; i < 100; ++i){
    auto indice = Rand<int>(0, registros.size()-1);
    auto temp_reg = registros[indice];
    //Tiempo para busqueda
    auto start = high_resolution_clock::now();
    sf.search(temp_reg.id);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    file << to_string(duration.count()) << ";";
     //Tiempo para añadir
    temp_reg.id=Rand(430000, 50000);
    auto start1 = high_resolution_clock::now();
    sf.add(temp_reg);
    auto stop1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(stop1 - start1);
    file << to_string(duration1.count()) << ";";
    //Tiempo para eliminar
    auto start2 = high_resolution_clock::now();
    sf.deleteRegistro(temp_reg.id);
    auto stop2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    file << to_string(duration2.count()) << "\n";    
  }
  file.close();
}

void hashing_tests(){
  auto registros = getAllFromCsv();
  ofstream file("EH_Execution_Times.csv");
  extendibleHash eH(10, "indexfile.dat", 10);
  for(int i = 0 ; i < 1000; ++i){
    eH.insertRecord(registros[i]);
  }
  for(int i = 1001; i < 1101; ++i){
    auto indice = Rand<int>(0, 1000);
    auto temp_reg = registros[indice];
    //Tiempo para búsqueda
    auto start = high_resolution_clock::now();
    eH.search(registros[indice].id);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    file << to_string(duration.count()) << ";";
    //Tiempor para añadir
    auto start1 = high_resolution_clock::now();
    eH.insertRecord(registros[i]);
    auto stop1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(stop1 - start1);
    file << to_string(duration1.count()) << ";";
    //Tiempo para eliminar
    auto start2 = high_resolution_clock::now();
    eH.removeRegister(temp_reg.id);
    auto stop2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    file << to_string(duration2.count()) << "\n";
  }
  //eH.deleteBuckets();
  file.close();
}

void menu(){
  int option;

  cout<<"Seleccione la estructura que desee:"<<endl; 
  cout<<"1) Sequential File"<<endl; 
  cout<<"2) Extendible Hash"<<endl;

  cin>>option;

  switch(option){
    case 1:{

      Sequential_File sf;
      auto registros = sf.getAllFromCsv();
  
      sf.insertAll(registros);

      while(true){
        cout<<"Elige el método de testeo: "<<endl; 
        cout<<"1) Inserción de registro "<<endl; 
        cout<<"2) Búsqueda por rango "<<endl;
        cout<<"3) Búsqueda específica"<<endl; 
        cout<<"4) Eliminar registro"<<endl;
        cout<<"5) Salir"<<endl;

        int metodo;
        cin>>metodo; 

        switch(metodo){
        case 1:{
            auto start1 = high_resolution_clock::now();
            cout<<"Se estan insertando 100 registros..." <<endl;
            for(int i = 1000; i < 1100; ++i){
              //Tiempo para añadir
              sf.add(registros[i]);
              cout<<"Se esta insertando el registro con id: "<<registros[i].id<<endl; 
            }
            auto stop1 = high_resolution_clock::now();
            auto duration1 = duration_cast<microseconds>(stop1 - start1);
            cout<<"Tiempo para insertar 100 registros: "<< to_string(duration1.count()) <<" microsegundos"<<endl;
            break; 
        }
        case 2:{
            cout<<"Se estan buscando 100 registros..." <<endl;
            auto start = high_resolution_clock::now();
            for(int i = 0; i < 100; ++i){
              auto indice = Rand<int>(0, registros.size()-1);
              auto temp_reg = registros[indice];
              //Tiempo para busqueda
              sf.search(temp_reg.id);
              cout<<"Se esta buscando el registro con id: "<<registros[i].id<<endl; 
              }
              auto stop = high_resolution_clock::now();
              auto duration = duration_cast<microseconds>(stop - start);
              cout<<"Tiempo para buscar 100 registros: "<<to_string(duration.count())  <<" microsegundos"<<endl;
              break;
        }

        case 3:{
           cout<<"Se estan buscando 100 registros..." <<endl;
            auto start = high_resolution_clock::now();
            for(int i = 0; i < 100; ++i){
              //Tiempo para busqueda
              sf.rangeSearch(438103,i+438199);
              cout<<"Se esta buscando el registro en el rango de : "<<"438103 a "<< i+438199 <<endl; 
              }
              auto stop = high_resolution_clock::now();
              auto duration = duration_cast<microseconds>(stop - start);
              cout<<"Tiempo para buscar 100 registros: "<< to_string(duration.count()) <<" microsegundos"<<endl;
              break; 

        }
        case 4:{

          cout<<"Se estan eliminando 100 registros..." <<endl;
            auto start = high_resolution_clock::now();
            for(int i = 0; i < 100; ++i){
              auto indice = Rand<int>(0, registros.size()-1);
              auto temp_reg = registros[indice];
              //Tiempo para busqueda
              sf.deleteRegistro(temp_reg.id);
              cout<<"Se esta eliminando el registro con id : "<<registros[i].id<<endl; 
              }
              auto stop = high_resolution_clock::now();
              auto duration = duration_cast<microseconds>(stop - start);
              cout<<"Tiempo para eliminar 100 registros: "<< to_string(duration.count())<<" microsegundos"<<endl;
              break; 

        }
        case 5:{
          cout<<"Saliendo del Programa...";
          return;}  
          
      }
      }         
    }
    case 2:{

      auto registros = getAllFromCsv();
      extendibleHash e(10, "indexfile.dat", 10);
      
      for(int i = 0 ; i < 1000; ++i){
        e.insertRecord(registros[i]);
      }
      
      while(true){
        cout<<"Elige el método de testeo: "<<endl;
        cout<<"1) Inserción de registro "<<endl; 
        cout<<"2) Búsqueda específica"<<endl; 
        cout<<"3) Eliminar registro"<<endl;
        cout<<"4) Salir"<<endl;

        int metodo;
        cin>>metodo;

        switch(metodo){
          
          case 1:{
            auto start1 = high_resolution_clock::now();
            cout<<"Se estan insertando 100 registros..."<<endl;
            for(int i = 1000; i < 1100; ++i){
              //Tiempo para añadir
              e.insertRecord(registros[i]);
              cout<<"Se esta insertando el registro con id: "<< registros[i].id<<endl;
            }
            auto stop1 = high_resolution_clock::now();
            auto duration1 = duration_cast<microseconds>(stop1 - start1);
            cout<<"Tiempo para insertar 100 registros: "<< to_string(duration1.count()) <<" microsegundos"<<endl;
            break;
          }
          case 2:{
            auto start1 = high_resolution_clock::now();
            cout<<"Se estan buscando 100 registros..."<<endl;
            for(int i = 0; i < 100; ++i){
              //Tiempo para añadir
              e.search(registros[i].id);
              cout<<"Se esta buscando el registro con id: "<< registros[i].id<<endl;
            }
            auto stop1 = high_resolution_clock::now();
            auto duration1 = duration_cast<microseconds>(stop1 - start1);
            cout<<"Tiempo para buscar 100 registros: "<< to_string(duration1.count()) <<" microsegundos"<<endl;
            break;
          }
          case 3:{
            auto start1 = high_resolution_clock::now();
            cout<<"Se estan eliminando 100 registros..."<<endl;
            for(int i = 0; i < 100; ++i){
              //Tiempo para añadir
              e.removeRegister(registros[i].id);
              cout<<"Se esta eliminado el registro con id: "<< registros[i].id<<endl;
            }
            auto stop1 = high_resolution_clock::now();
            auto duration1 = duration_cast<microseconds>(stop1 - start1);
            cout<<"Tiempo para eliminar 100 registros: "<< to_string(duration1.count()) <<" microsegundos"<<endl;
            break;
          }
          case 4:{
            cout<<"Saliendo del Programa...";
            return;
          }
          
        }
      } 
    }
  }
}