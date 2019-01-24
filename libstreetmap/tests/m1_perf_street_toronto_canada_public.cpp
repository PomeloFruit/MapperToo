#include <algorithm>
#include <set>
#include <random>
#include <unittest++/UnitTest++.h>
#include "unit_test_util.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"

using ece297test::relative_error;


SUITE(street_queries_perf_public_toronto_canada) {

    struct MapFixture {
        MapFixture() {
            //Load the map
            load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
    
            //Initialize random number generators
            rng = std::minstd_rand(3);
            rand_intersection = std::uniform_int_distribution<unsigned>(0, getNumIntersections()-1);
            rand_street = std::uniform_int_distribution<unsigned>(1, getNumStreets()-1);
            rand_segment = std::uniform_int_distribution<unsigned>(0, getNumStreetSegments()-1);
            rand_poi = std::uniform_int_distribution<unsigned>(0, getNumPointsOfInterest()-1);
            rand_lat = std::uniform_real_distribution<double>(43.479999542, 43.919979095);
            rand_lon = std::uniform_real_distribution<double>(-79.789978027, -79.000007629);
        }
    
        ~MapFixture() {
            //Clean-up
            close_map();
        }
    
        std::minstd_rand rng;
        std::uniform_int_distribution<unsigned> rand_intersection;
        std::uniform_int_distribution<unsigned> rand_street;
        std::uniform_int_distribution<unsigned> rand_segment;
        std::uniform_int_distribution<unsigned> rand_poi;
        std::uniform_real_distribution<double> rand_lat;
        std::uniform_real_distribution<double> rand_lon;
    };
    
    TEST_FIXTURE(MapFixture, street_street_segments_perf) {
        //Verify Functionality
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;
        
        expected = {2, 9542, 9543, 16187, 16188};
        actual = find_street_street_segments(2);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {14984, 14987, 130539};
        actual = find_street_street_segments(3791);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {16344, 16345, 68447};
        actual = find_street_street_segments(4118);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {19163, 19164, 19165, 84068, 84865};
        actual = find_street_street_segments(4893);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {21257};
        actual = find_street_street_segments(5414);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {22140};
        actual = find_street_street_segments(5701);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {23148};
        actual = find_street_street_segments(5903);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {48380};
        actual = find_street_street_segments(9098);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {52160};
        actual = find_street_street_segments(10487);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {55944};
        actual = find_street_street_segments(11357);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {56794};
        actual = find_street_street_segments(11564);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {58761};
        actual = find_street_street_segments(12077);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {59363};
        actual = find_street_street_segments(12181);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {65197, 65201, 65259, 65260, 65261, 65266, 65302, 65307, 78070};
        actual = find_street_street_segments(13368);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {69652, 69655, 69667, 69668};
        actual = find_street_street_segments(14321);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {70222, 70223, 70224, 70226};
        actual = find_street_street_segments(14456);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {71790, 71798, 71799, 71801, 71805};
        actual = find_street_street_segments(14823);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {72743};
        actual = find_street_street_segments(14953);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {74465, 74496, 74549, 74658, 74835, 74854, 74928, 74956, 74964, 74966, 75012, 75013, 75188};
        actual = find_street_street_segments(15291);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {76910};
        actual = find_street_street_segments(15846);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {79764};
        actual = find_street_street_segments(16463);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {80816, 80847, 80859, 80874, 80875, 80876};
        actual = find_street_street_segments(16738);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {81945, 81946, 81947, 81948};
        actual = find_street_street_segments(17063);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {95171};
        actual = find_street_street_segments(18719);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {95613, 95712};
        actual = find_street_street_segments(18773);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {96768, 96784, 96785, 96786, 96802, 96803, 96808};
        actual = find_street_street_segments(18915);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {100936};
        actual = find_street_street_segments(19096);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {101527, 101528, 101529};
        actual = find_street_street_segments(19181);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {118605, 128569, 128570};
        actual = find_street_street_segments(20351);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {140321, 140322};
        actual = find_street_street_segments(20909);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        //Generate random inputs
        std::vector<unsigned> street_ids;
        for(size_t i = 0; i < 8000000; i++) {
            street_ids.push_back(rand_street(rng));
        }
        {
            //Timed Test
            ECE297_TIME_CONSTRAINT(1010);
            std::vector<unsigned> result;
            for(size_t i = 0; i < 8000000; i++) {
                result = find_street_street_segments(street_ids[i]);
            }
        }
    } //street_street_segments_perf

    TEST_FIXTURE(MapFixture, all_street_intersections_perf) {
        //Verify Functionality
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;
        
        expected = {3, 4, 10371, 10372, 16929, 16930};
        actual = find_all_street_intersections(2);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {893, 1717, 25346, 31186, 64484, 64640, 64737, 64738, 64839, 65008, 65025, 65026, 65069, 65081};
        actual = find_all_street_intersections(15291);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {6530, 21891};
        actual = find_all_street_intersections(11357);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {15418, 15424, 15427, 61743, 61744};
        actual = find_all_street_intersections(14456);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {15806, 15807, 15812, 15813};
        actual = find_all_street_intersections(3791);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {16200, 16201, 16202, 19964, 19965, 19966};
        actual = find_all_street_intersections(4893);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {17103, 17104, 17105, 60617};
        actual = find_all_street_intersections(4118);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {17996, 101669};
        actual = find_all_street_intersections(20909);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {22003, 22004};
        actual = find_all_street_intersections(5414);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {22808, 61409, 61410, 61411, 61418};
        actual = find_all_street_intersections(14321);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {22919, 22921};
        actual = find_all_street_intersections(5701);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {23884, 23889};
        actual = find_all_street_intersections(5903);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {29398, 50561, 58475, 58476, 58479, 58506, 58511, 58512, 58513};
        actual = find_all_street_intersections(13368);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {39426, 39428, 68882, 68884, 68896, 68897, 68920};
        actual = find_all_street_intersections(16738);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {40762, 40766, 69636, 69640, 69642};
        actual = find_all_street_intersections(17063);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {45837};
        actual = find_all_street_intersections(9098);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {47523, 49356};
        actual = find_all_street_intersections(10487);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {48410, 63383};
        actual = find_all_street_intersections(14953);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {48443, 72290};
        actual = find_all_street_intersections(19096);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {49530, 78720, 78737, 78741, 78750, 78756, 78757, 78760};
        actual = find_all_street_intersections(18915);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {52835, 52836};
        actual = find_all_street_intersections(11564);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {54263, 54264};
        actual = find_all_street_intersections(12077);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {54619, 54620};
        actual = find_all_street_intersections(12181);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {62779, 62783, 62789, 62791, 62792, 62793};
        actual = find_all_street_intersections(14823);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {66263, 66264};
        actual = find_all_street_intersections(15846);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {68161, 68179};
        actual = find_all_street_intersections(16463);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {77747, 77761};
        actual = find_all_street_intersections(18719);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {77957, 78074, 78075};
        actual = find_all_street_intersections(18773);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {81498, 81499, 81500, 81501};
        actual = find_all_street_intersections(19181);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {90773, 90775, 90776, 90791};
        actual = find_all_street_intersections(20351);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        //Generate random inputs
        std::vector<unsigned> street_ids;
        for(size_t i = 0; i < 1600000; i++) {
            street_ids.push_back(rand_street(rng));
        }
        {
            //Timed Test
            ECE297_TIME_CONSTRAINT(1506);
            std::vector<unsigned> result;
            for(size_t i = 0; i < 1600000; i++) {
                result = find_all_street_intersections(street_ids[i]);
            }
        }
    } //all_street_intersections_perf

    TEST_FIXTURE(MapFixture, intersection_ids_from_street_ids_perf) {
        //Verify Functionality
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(4014, 6108);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(5657, 17966);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(7230, 12986);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(7964, 9631);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(8421, 16535);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(9607, 16236);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(9627, 8414);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(12603, 17220);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(13198, 2713);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(13792, 4356);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(14642, 17299);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(17872, 16195);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(18394, 382);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(18582, 20636);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(20038, 19050);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {4};
        actual = find_intersection_ids_from_street_ids(2, 4083);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {18790, 18791, 18792, 19968, 44284};
        actual = find_intersection_ids_from_street_ids(4563, 0);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {21754};
        actual = find_intersection_ids_from_street_ids(5346, 5344);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {26961};
        actual = find_intersection_ids_from_street_ids(15641, 6543);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {28291};
        actual = find_intersection_ids_from_street_ids(6718, 0);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {39426, 39428};
        actual = find_intersection_ids_from_street_ids(16738, 0);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {39494, 39496};
        actual = find_intersection_ids_from_street_ids(7796, 7794);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {48759, 48986};
        actual = find_intersection_ids_from_street_ids(10571, 9782);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {49356};
        actual = find_intersection_ids_from_street_ids(10487, 10290);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {62792, 62793};
        actual = find_intersection_ids_from_street_ids(14823, 14825);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {66263};
        actual = find_intersection_ids_from_street_ids(15846, 15848);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {68235};
        actual = find_intersection_ids_from_street_ids(16494, 16496);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {69367, 69388};
        actual = find_intersection_ids_from_street_ids(16949, 16923);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {81499, 81500};
        actual = find_intersection_ids_from_street_ids(19181, 19202);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {83973};
        actual = find_intersection_ids_from_street_ids(19638, 19655);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        //Generate random inputs
        std::vector<unsigned> street_ids_1;
        std::vector<unsigned> street_ids_2;
        for(size_t i = 0; i < 900000; i++) {
            unsigned street_id1 = rand_street(rng);
            unsigned street_id2 = rand_street(rng);
            street_ids_1.push_back(street_id1);
            street_ids_2.push_back(street_id2);
        }
        {
            //Timed Test
            ECE297_TIME_CONSTRAINT(410);
            std::vector<unsigned> result;
            for(size_t i = 0; i < 900000; i++) {
                result = find_intersection_ids_from_street_ids(street_ids_1[i], street_ids_2[i]);
            }
        }
    } //intersection_ids_from_street_ids_perf

    TEST_FIXTURE(MapFixture, street_ids_from_partial_street_name_perf) {
        //Verify Functionality
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;
        
        expected = {1, 2, 3, 4, 18, 65, 66, 67, 154, 155, 161, 162, 164, 165, 167, 180, 181, 182, 183, 184, 186, 187, 188, 190, 191, 192, 194, 202, 204, 205, 208, 209, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 309, 390, 391, 501, 505, 506, 639, 705, 760, 761, 900, 904, 905, 907, 1052, 1062, 1064, 1072, 1073, 1074, 1078, 1079, 1080, 1081, 1097, 1099, 1527, 1535, 1654, 1897, 1898, 1899, 1900, 1903, 1906, 1907, 1908, 2043, 2044, 2045, 2046, 2129, 2130, 2136, 2137, 2138, 2184, 2489, 2503, 2504, 2510, 2601, 2816, 2817, 2818, 2911, 3093, 3094, 3109, 3135, 3136, 3137, 3415, 3419, 3421, 3440, 3549, 3550, 3551, 4693, 4694, 4827, 4834, 4865, 4917, 5100, 5101, 5103, 5184, 5185, 5186, 5410, 5412, 5413, 5414, 5416, 5417, 5801, 5896, 5956, 6067, 6068, 6074, 6396, 6399, 6401, 6402, 6405, 6406, 6407, 6408, 6416, 6417, 6418, 6420, 6421, 6422, 6423, 6424, 6426, 6427, 6428, 6429, 6782, 6824, 6837, 6838, 6839, 6840, 6855, 6913, 6914, 6954, 7001, 7185, 7186, 7730, 9033, 9035, 11357, 18837, 18971, 19145, 20099, 20100, 20107, 20108, 20261, 20281, 20292, 20294, 20304, 20308, 20310, 20311, 20312, 20315, 20396, 20558, 20796, 20816, 20818, 20819, 20846, 20848, 21187};
        actual = find_street_ids_from_partial_street_name("Highway");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {88, 888, 1766, 1974, 2157, 2575, 3624, 3966, 5761, 6143, 6472, 6586, 6592, 6654, 6662, 7089, 7092, 7155, 7437, 7439, 7613, 7946, 7947, 8011, 8666, 8748, 8820, 8960, 9068, 9471, 9544, 9785, 10453, 11015, 11089, 12023, 12410, 14124, 15095, 15539, 15816, 16095, 16161, 16186, 16663, 16716, 16992, 17163, 17365, 17369, 17901, 17945, 18035, 18068, 18468, 18731, 19249, 19600};
        actual = find_street_ids_from_partial_street_name("Cre");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {93, 97, 971, 1375, 14362};
        actual = find_street_ids_from_partial_street_name("Roxbo");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {250, 544, 640, 865, 916, 964, 1251, 1389, 1697, 1993, 2231, 2238, 2353, 2371, 2560, 2921, 3224, 3321, 3498, 3744, 4613, 4645, 4857, 4900, 4907, 5630, 5935, 6087, 6203, 6260, 6502, 6522, 6550, 6883, 6970, 7295, 7423, 7536, 7994, 8238, 8427, 8799, 9049, 9113, 9198, 9261, 9312, 9347, 9627, 9650, 10039, 10177, 10272, 10285, 10311, 10389, 10415, 10449, 10611, 10624, 10710, 10912, 11130, 11427, 11432, 11488, 11543, 11559, 11573, 11708, 12037, 12068, 12258, 12262, 12287, 12295, 12299, 12606, 12630, 12692, 12728, 12806, 12847, 12851, 12905, 13008, 13335, 13431, 13502, 13838, 13995, 14088, 14171, 14274, 14337, 14841, 14856, 14864, 14907, 14923, 14930, 14999, 15094, 15154, 15250, 15280, 15376, 15727, 15892, 15938, 15943, 15978, 16070, 16077, 16162, 16363, 16701, 16741, 17030, 17114, 17158, 17180, 17220, 17559, 17951, 18109, 18546, 18822, 18835, 18893, 19373, 19439, 19719, 20450, 20596, 20618, 20652, 20676, 20902};
        actual = find_street_ids_from_partial_street_name("Cha");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {1431};
        actual = find_street_ids_from_partial_street_name("Chippewa Avenue");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {1458, 1578, 15517, 15813, 17577, 18466};
        actual = find_street_ids_from_partial_street_name("Wells");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {2375};
        actual = find_street_ids_from_partial_street_name("Wyandot");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {3791};
        actual = find_street_ids_from_partial_street_name("Woodside Avenue");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {4247};
        actual = find_street_ids_from_partial_street_name("Cassel");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {4625};
        actual = find_street_ids_from_partial_street_name("Spears Stree");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {4893};
        actual = find_street_ids_from_partial_street_name("Forest Glen Crescent");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {5008};
        actual = find_street_ids_from_partial_street_name("Kittredge ");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {5903};
        actual = find_street_ids_from_partial_street_name("Rondeen Road");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {5961};
        actual = find_street_ids_from_partial_street_name("City Centre Transit Ter");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {6760};
        actual = find_street_ids_from_partial_street_name("Odessa Avenue");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {8200};
        actual = find_street_ids_from_partial_street_name("Meeting House Road");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {8421};
        actual = find_street_ids_from_partial_street_name("Ravineview Drive");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {11357};
        actual = find_street_ids_from_partial_street_name("Highways 27, 401, 427");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {11545};
        actual = find_street_ids_from_partial_street_name("Belinda Square");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {11564, 11701, 12318, 12673, 14709, 18257};
        actual = find_street_ids_from_partial_street_name("Faw");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {12151};
        actual = find_street_ids_from_partial_street_name("Lilia");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {12181, 12193, 12211};
        actual = find_street_ids_from_partial_street_name("Townley Av");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {12654, 13379, 16949};
        actual = find_street_ids_from_partial_street_name("Ashwood ");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {14236};
        actual = find_street_ids_from_partial_street_name("Ayma");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {14321};
        actual = find_street_ids_from_partial_street_name("Vaughan Bouleva");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {14953};
        actual = find_street_ids_from_partial_street_name("Ardmore Crescent");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {15846};
        actual = find_street_ids_from_partial_street_name("Blue Beech Crescent");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {16195};
        actual = find_street_ids_from_partial_street_name("Wildfield Crescent");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {18394};
        actual = find_street_ids_from_partial_street_name("Pompano Place");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {20636};
        actual = find_street_ids_from_partial_street_name("Capella Way");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        //Generate random inputs
        std::vector<std::string> street_names_prefixes;
        for(size_t i = 0; i < 1000000; i++) {
            std::string rand_street_name = getStreetName(rand_street(rng));
            unsigned rand_prefix_end_pos = (1.0*rng()/rng.max())*rand_street_name.length();
            street_names_prefixes.push_back(rand_street_name.substr(0, rand_prefix_end_pos));
        }
        {
            //Timed Test
            ECE297_TIME_CONSTRAINT(22710);
            std::vector<unsigned> result;
            for(size_t i = 0; i < 1000000; i++) {
                result = find_street_ids_from_partial_street_name(street_names_prefixes[i]);
            }
        }
    } //street_ids_from_partial_street_name_perf

} //street_queries_perf_public_toronto_canada

