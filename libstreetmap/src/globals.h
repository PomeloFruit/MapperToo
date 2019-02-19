
#pragma once

////unordered map of <street ids, street segments with the same street ids>
//#include <vector>
//#include <string>
//#include <map>
//#include <unordered_map>
//
//class g{
//    public:
//        typedef std::vector<unsigned> IDVector;
//
//
//        double find_street_length_preload(unsigned street_id);
//        double find_street_segment_length_preload(unsigned street_segment_id);
//        double find_street_segment_travel_time_preload(unsigned street_segment_id);
//
//        //unordered map of <street ids, street segments with the same street ids>
//        std::unordered_map<int, std::vector<unsigned>> streetIDMap; 
//
//        //map of <partial street name, street ids with the same partial street name>
//        //(ie. the name Bloor becomes five vectors: b, bl, blo, bloo, bloor)
//        std::map<std::string, std::vector<unsigned>> partialStreetNameMap;
//
//        //vector of vectors of intersections on a street 
//        std::vector<std::vector<unsigned>> streetIntersectionsVector; 
//
//        //vector of street segment ID indexed on intersection ID
//        std::vector<std::vector<unsigned>> intersectionSegIDVector;
//
//        //vector of street segment names indexed on intersection Id
//        std::vector<std::vector<std::string>> intersectionSegNameVector;
//
//        //vector of street segment lengths indexed on segment ID
//        std::vector<double> segLengthVector;
//
//        //vector of street lengths indexed on street ID
//        std::vector<double> streetLengthVector;
//
//        //vector of street segment travel times indexed on segment ID
//        std::vector<double> segTravelTimeVector;
//
//        
//};