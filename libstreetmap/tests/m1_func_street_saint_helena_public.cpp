#include <algorithm>
#include <set>
#include <unittest++/UnitTest++.h>
#include "m1.h"
#include "unit_test_util.h"
#include "StreetsDatabaseAPI.h"

using ece297test::relative_error;


SUITE(street_queries_public_saint_helena) {

    struct MapFixture {
        MapFixture() {
            //Load the map
            load_map("/cad2/ece297s/public/maps/saint-helena.streets.bin");
        }
    
        ~MapFixture() {
            //Clean-up
            close_map();
        }
    };

    TEST_FIXTURE(MapFixture, street_street_segments) {
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;
        
        expected = {49};
        actual = find_street_street_segments(1);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {61};
        actual = find_street_street_segments(2);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {62, 63};
        actual = find_street_street_segments(3);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {138, 174, 175};
        actual = find_street_street_segments(4);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {190, 306, 319};
        actual = find_street_street_segments(5);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214};
        actual = find_street_street_segments(6);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {215, 295};
        actual = find_street_street_segments(7);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {255, 256, 257, 258};
        actual = find_street_street_segments(8);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {296};
        actual = find_street_street_segments(9);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {297, 298};
        actual = find_street_street_segments(10);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {299};
        actual = find_street_street_segments(11);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {300, 301};
        actual = find_street_street_segments(12);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {307, 308};
        actual = find_street_street_segments(13);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {316};
        actual = find_street_street_segments(15);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {317, 318};
        actual = find_street_street_segments(16);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
    } //street_street_segments

    TEST_FIXTURE(MapFixture, all_street_intersections) {
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;
        
        expected = {60, 61};
        actual = find_all_street_intersections(1);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {60, 311};
        actual = find_all_street_intersections(11);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {62, 66};
        actual = find_all_street_intersections(9);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {62, 286, 287, 288, 289};
        actual = find_all_street_intersections(8);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {63, 309, 310};
        actual = find_all_street_intersections(10);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {64, 230, 231, 299};
        actual = find_all_street_intersections(5);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {90, 91};
        actual = find_all_street_intersections(2);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {92, 93, 94};
        actual = find_all_street_intersections(3);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {105, 208, 250};
        actual = find_all_street_intersections(7);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {155, 182, 221, 229, 234, 235, 237, 239, 241, 242, 243, 244, 245, 246, 247, 248, 249};
        actual = find_all_street_intersections(6);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {180, 181, 211, 212};
        actual = find_all_street_intersections(4);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {289, 309, 311};
        actual = find_all_street_intersections(16);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {311, 315, 317};
        actual = find_all_street_intersections(13);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {312, 313, 314};
        actual = find_all_street_intersections(12);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {316, 318};
        actual = find_all_street_intersections(15);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
    } //all_street_intersections

    TEST_FIXTURE(MapFixture, intersection_ids_from_street_ids) {
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(4, 10);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(5, 9);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(8, 6);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(11, 6);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(13, 5);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(13, 9);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {};
        actual = find_intersection_ids_from_street_ids(14, 5);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {60};
        actual = find_intersection_ids_from_street_ids(1, 11);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {62, 287, 288};
        actual = find_intersection_ids_from_street_ids(8, 0);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {93, 94};
        actual = find_intersection_ids_from_street_ids(3, 0);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {311};
        actual = find_intersection_ids_from_street_ids(11, 13);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {311};
        actual = find_intersection_ids_from_street_ids(13, 11);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {311};
        actual = find_intersection_ids_from_street_ids(13, 16);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {313};
        actual = find_intersection_ids_from_street_ids(12, 14);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {316, 318};
        actual = find_intersection_ids_from_street_ids(15, 0);
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
    } //intersection_ids_from_street_ids

    TEST_FIXTURE(MapFixture, street_ids_from_partial_street_name) {
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;
        
        expected = {1};
        actual = find_street_ids_from_partial_street_name("Fi");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {2};
        actual = find_street_ids_from_partial_street_name("Castle Terrace");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {2, 3, 5, 6, 9};
        actual = find_street_ids_from_partial_street_name("C");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {3};
        actual = find_street_ids_from_partial_street_name("Church Lane");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {4};
        actual = find_street_ids_from_partial_street_name("Sandy Bay Road");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {5};
        actual = find_street_ids_from_partial_street_name("Constitution H");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {5};
        actual = find_street_ids_from_partial_street_name("Constitution Hill");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {6};
        actual = find_street_ids_from_partial_street_name("Casons");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {9};
        actual = find_street_ids_from_partial_street_name("China Lane");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {10};
        actual = find_street_ids_from_partial_street_name("The R");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {11};
        actual = find_street_ids_from_partial_street_name("Sid");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {11};
        actual = find_street_ids_from_partial_street_name("Side Pat");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {12};
        actual = find_street_ids_from_partial_street_name("Seafront");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {13};
        actual = find_street_ids_from_partial_street_name("Nap");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
        expected = {13};
        actual = find_street_ids_from_partial_street_name("Napoleon");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);
        
    } //street_ids_from_partial_street_name

} //street_queries_public_saint_helena

