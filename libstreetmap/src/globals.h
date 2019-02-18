/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   globals.h
 * Author: sartori2
 *
 * Created on February 17, 2019, 5:44 AM
 */

#ifndef GLOBALS_H
#define GLOBALS_H
//unordered map of <street ids, street segments with the same street ids>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

extern std::unordered_map<int, std::vector<unsigned>> streetIDMap; 

//map of <partial street name, street ids with the same partial street name>
//(ie. the name Bloor becomes five vectors: b, bl, blo, bloo, bloor)
extern std::map<std::string, std::vector<unsigned>> partialStreetNameMap;

//vector of vectors of intersections on a street 
extern std::vector<std::vector<unsigned>> streetIntersectionsVector; 

//vector of street segment ID indexed on intersection ID
extern std::vector<std::vector<unsigned>> intersectionSegIDVector;

//vector of street segment names indexed on intersection Id
extern std::vector<std::vector<std::string>> intersectionSegNameVector;

//vector of street segment lengths indexed on segment ID
extern std::vector<double> segLengthVector;

//vector of street lengths indexed on street ID
extern std::vector<double> streetLengthVector;

//vector of street segment travel times indexed on segment ID
extern std::vector<double> segTravelTimeVector;



#endif /* GLOBALS_H */

