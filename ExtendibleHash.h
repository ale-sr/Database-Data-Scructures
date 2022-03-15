#include <iostream>
#include <bitset>
#include <cmath>
//#include <bits/stdc++.h>
#include <string>
#include <cstdio>
#include <stdio.h>
#include <set>
#include <filesystem>
#define M 64
#include "carRecord.h"
 
using namespace std;

class extendibleHash {
private:
  string hashIndexFile;
  int  GlobalDepth;
  int BlockingFactor;
  set<string> buckets;

  string convertBinary(long key,  int length){
    string binary= bitset<M>(key).to_string();
    return binary.substr(M - length, length);
  }

  template <typename KeyType>
  string generateHash(KeyType key,  int length){
    if (typeid(key) == typeid(int()) || typeid(key) == typeid(long()) || typeid(key) == typeid(double()) ||
    typeid(key) == typeid(float()) || typeid(key)== typeid(char())){
    long newKey = long (key) % int(pow(2, GlobalDepth));
    return convertBinary(newKey, length);
    }
    else if (typeid(key)== typeid(string())){
      long newKey = long (stoi("key") ) % int( pow(2, GlobalDepth));
      return convertBinary(newKey, length);
    }
    else{
      return convertBinary(key, length); 
    }
  }


  void initializeBuckets(){
    //cabecerea
    int deleteNext = -1;
    int totalRecords = 0;
    //files
    fstream bucketOne;
    fstream bucketZero;
    bucketOne.open("1.dat", ios::binary | ios::out | fstream::out);
    bucketOne.write((char *) &totalRecords, sizeof(int));
    bucketOne.write((char *) &deleteNext, sizeof(int));
    bucketZero.open("0.dat", ios::binary | ios::out | fstream::out );
    bucketZero.write((char *) &totalRecords, sizeof(int));
    bucketZero.write((char *) &deleteNext, sizeof(int));
    bucketOne.close();
    bucketZero.close();

    buckets.insert("0");
    buckets.insert("1");
    
    return;
  }

  public:
    
  void deleteInfoFile(string indexFile) {
    int registers =0; int Delete = -1; 
    ofstream file; file.open(indexFile,  ofstream::trunc |ofstream::out );
    // Trunc to 0 registers, and deleteNext file -1
    file.write((char *) &registers, sizeof( int));
    file.write((char *) &Delete, sizeof(int));
    file.close();
  }

  void deleteBuckets(){
    for(auto bucket:buckets){
      char* char_array;
      auto file_name = bucket + ".dat";
      char_array = &file_name[0];
      std::remove(char_array);
    }
  }

  extendibleHash( int GlobalDepth, const string& hashIndexFile,   int BlockingFactor) {
    this->GlobalDepth = GlobalDepth;
    this->BlockingFactor = BlockingFactor;
    initializeBuckets();
  }

  void splitBucket(string bucket) {
    //create new buckets 
    string bucketZero, bucketOne, bucketInitial;
    fstream fileZero, fileOne, file; 
    int totalRecords = 0; int deleteNext = -1;
    bucketZero = "0" + bucket + ".dat";
    bucketOne = "1" + bucket + ".dat";

    buckets.insert("0" + bucket);
    buckets.insert("1" + bucket);
    buckets.erase(bucket);

    bucketInitial =  bucket+".dat";

    Car record;
    fileZero.open(bucketZero, ios::binary | ios::out | fstream::out);
    fileOne.open(bucketOne, ios::binary | ios::out | fstream::out);
    fileZero.write((char *) &totalRecords, sizeof(int));
    fileZero.write((char *) &deleteNext, sizeof(int));
    fileOne.write((char *) &totalRecords, sizeof(int));
    fileOne.write((char *) &deleteNext, sizeof(int));
    fileZero.close();
    fileOne.close();

    file.open(bucketInitial, ios::binary | ios::out | fstream::in);
    Car r;
    file.seekg(2*sizeof(int), ios::beg);
    while(file.read((char*)&r, sizeof(r))){
      if(r.deleteNext == -2){
        insertRecord(r);
      }
    }

    char* char_array;

    char_array = &bucketInitial[0];

    std::remove(char_array);

  }

  string getBucket(Car carRecord){
    string binary = generateHash(carRecord.id, GlobalDepth);
    for(auto bucket:buckets){
      int blength = bucket.length();
      if(binary.compare(binary.length()-blength,blength, bucket) == 0){
        return bucket;
      }
    }
    throw std::runtime_error("Bucket not found");
  }

  string getBucket(int key){
    string binary = generateHash(key, GlobalDepth);
    for(auto bucket:buckets){
      int blength = bucket.length();
      if(binary.compare(binary.length()-blength,blength, bucket) == 0){
        return bucket;
      }
    }
    throw std::runtime_error("Bucket not found");
  }
  
  void insertRecord(Car carRecord) {
    
    string bucket= getBucket(carRecord);
    string bucketFile = bucket +".dat";
    
    fstream fileOfBucket(bucketFile, ios::binary | ios::in | ios::out);
    int totalRecords, deleteNext;
    fileOfBucket.read((char *) &totalRecords, sizeof(int));
    fileOfBucket.read((char *) &deleteNext, sizeof(int));
    if (totalRecords >= BlockingFactor) {
      if (bucket.length() < GlobalDepth) { 
        /*if reach number of records allow in a block and the local depth didn´t
          exceeded the GlobalDepth, splitBucket the buckets      */      
        splitBucket(bucket);
        insertRecord(carRecord);
      }
      else {
        //if no more splitBuckets are allow due to the GlobalDepth add to the end 
        ++totalRecords;
        fileOfBucket.seekg(0, ios::beg);
        fileOfBucket.write((char *) &totalRecords, sizeof(int));
        fileOfBucket.seekg(0, ios::end);
        fileOfBucket.write((char *) &carRecord, sizeof(Car));
      }
    } 
    else {    
      //update the number of total records
      ++totalRecords; 
      fileOfBucket.seekg(0, ios::beg);
      fileOfBucket.write((char *) &totalRecords, sizeof(int));
      // if there's no deleted record add to the end 
      if (deleteNext == -1){
        fileOfBucket.seekg(0, ios::end);
        fileOfBucket.write((char *) &carRecord, sizeof(Car));
      } 
      else {
        fileOfBucket.seekg(deleteNext, ios::beg);
        Car deletedRecord;
        // LIFO technique, so insert in the first deleted record 
        fileOfBucket.read((char *) &deletedRecord, sizeof(Car));
        int deletedeleteNextetedRecord = deletedRecord.deleteNext;
        fileOfBucket.seekg(deleteNext, ios::beg);
        fileOfBucket.write((char *) &carRecord, sizeof(Car));
        fileOfBucket.seekg(sizeof(int));
        fileOfBucket.write((char *) &deletedeleteNextetedRecord, sizeof(int));
      }
    }
    fileOfBucket.close();
  }

  void removeRegister(int key){
    fstream file;
    int totalRecords, deleteNext;
    Car record; 
    string bucket= getBucket(key);
    string bucketFile = bucket +".dat";
    
    file.open(bucketFile, ios::binary | ios::out | ios::in );
    file.read((char *) &totalRecords, sizeof(int));
    file.read((char *) &deleteNext, sizeof(int));
    
    --totalRecords;
    bool flag = false;

    if(totalRecords <= 0){
      if(file.tellp() == file.eof()){
        cerr<< "Key not found in remove"<<endl; 
        return;
      }
      while(file.read((char *) &record, sizeof(record))) {
        if (record.id == key && record.deleteNext == -2){
          flag = true;
          break;
        }
      }
      if(!flag){
        cerr<< "Key not found in remove"<<endl; 
        return;
      }

      char* char_array;

      char* char_array2;

      char_array = &bucketFile[0];

      std::remove(char_array);

      buckets.erase(bucket);

      string newBucket = bucketFile.substr(1,bucketFile.length()-1);
      
      if(bucketFile[0] == '0'){
        bucketFile[0] = '1';
      }else{
        bucketFile[0] = '0';
      }

      buckets.erase(bucket);
      buckets.insert(newBucket);
      
      char_array = &bucketFile[0];
      char_array2 = &newBucket[0];

      std::rename(char_array, char_array2);

      return;
    }else{

     while(file.read((char *) &record, sizeof(record))) {
        if (record.id == key && record.deleteNext == -2){
          record.deleteNext = deleteNext;
          deleteNext = int(file.tellp())-sizeof(record);
          flag = true;
          break;
        }
      }
      if(!flag){
        cerr<< "Key not found in remove"<<endl;
        return;
      }

      record.deleteNext = deleteNext;

      file.close();

      file.open(bucketFile, ios::binary | ios::out | fstream::out);

      file.seekg(deleteNext, ios::beg);

      file.write((char*)&record, sizeof(record));

      file.seekg(0,ios::beg);
      file.write((char *) &totalRecords, sizeof(int));
      file.write((char *) &deleteNext, sizeof(int));
      
      return;
    }
  }
    
  vector<Car> search(int key) {
    Car record;
    int totalRecords, deleteNext;
    vector<Car> result;
    fstream file; 

    string bucketName= getBucket(key); 
    string bucket = bucketName +".dat";
    
    file.open(bucket, ios::binary | ios::out | ios::in );
    file.read((char *) &totalRecords, sizeof(int));
    file.read((char *) &deleteNext, sizeof(int));
    for (unsigned int i = 0; i < totalRecords; i++) {
      file.read((char *) &record, sizeof(record));
    // -1 means that the record  is not deleted
      if (record.id == key && record.deleteNext == -2)
        {result.push_back(record);}
    }
    if (result.empty()){
      cerr<<"Key not found in search "<<endl;
    }
    file.close();
    return result;
  }

}; 