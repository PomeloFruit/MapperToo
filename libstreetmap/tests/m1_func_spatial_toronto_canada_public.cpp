#include <algorithm>
#include <set>
#include <unittest++/UnitTest++.h>
#include "m1.h"
#include "unit_test_util.h"
#include "StreetsDatabaseAPI.h"

using ece297test::relative_error;


SUITE(spatial_queries_public_toronto_canada) {

    struct MapFixture {
        MapFixture() {
            //Load the map
            load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
        }
    
        ~MapFixture() {
            //Clean-up
            close_map();
        }
    };

    TEST_FIXTURE(MapFixture, closest_poi) {
        std::set<unsigned> valid_results;
        unsigned actual;
        
        valid_results = {368};
        actual = find_closest_point_of_interest(LatLon(43.64250946044921875, -79.35969543457031250));
        CHECK(valid_results.count(actual));
        
        valid_results = {723};
        actual = find_closest_point_of_interest(LatLon(43.71545028686523438, -79.45131683349609375));
        CHECK(valid_results.count(actual));
        
        valid_results = {1091};
        actual = find_closest_point_of_interest(LatLon(43.85248184204101562, -79.43147277832031250));
        CHECK(valid_results.count(actual));
        
        valid_results = {1098};
        actual = find_closest_point_of_interest(LatLon(43.77970123291015625, -79.60785675048828125));
        CHECK(valid_results.count(actual));
        
        valid_results = {1126};
        actual = find_closest_point_of_interest(LatLon(43.83801269531250000, -79.22557830810546875));
        CHECK(valid_results.count(actual));
        
        valid_results = {1224};
        actual = find_closest_point_of_interest(LatLon(43.77691268920898438, -79.58845520019531250));
        CHECK(valid_results.count(actual));
        
        valid_results = {1455};
        actual = find_closest_point_of_interest(LatLon(43.65697860717773438, -79.70158386230468750));
        CHECK(valid_results.count(actual));
        
        valid_results = {2434};
        actual = find_closest_point_of_interest(LatLon(43.56536483764648438, -79.34043121337890625));
        CHECK(valid_results.count(actual));
        
        valid_results = {2489};
        actual = find_closest_point_of_interest(LatLon(43.71529388427734375, -79.55553436279296875));
        CHECK(valid_results.count(actual));
        
        valid_results = {4203, 5753};
        actual = find_closest_point_of_interest(LatLon(43.74922943115234375, -79.02179718017578125));
        CHECK(valid_results.count(actual));
        
        valid_results = {4982};
        actual = find_closest_point_of_interest(LatLon(43.77373886108398438, -79.47676086425781250));
        CHECK(valid_results.count(actual));
        
        valid_results = {5037};
        actual = find_closest_point_of_interest(LatLon(43.71936035156250000, -79.52051544189453125));
        CHECK(valid_results.count(actual));
        
        valid_results = {5208};
        actual = find_closest_point_of_interest(LatLon(43.87590789794921875, -79.01165771484375000));
        CHECK(valid_results.count(actual));
        
        valid_results = {5228};
        actual = find_closest_point_of_interest(LatLon(43.57031250000000000, -79.68901062011718750));
        CHECK(valid_results.count(actual));
        
        valid_results = {5506};
        actual = find_closest_point_of_interest(LatLon(43.87495422363281250, -79.56262969970703125));
        CHECK(valid_results.count(actual));
        
        valid_results = {5516};
        actual = find_closest_point_of_interest(LatLon(43.90193939208984375, -79.57024383544921875));
        CHECK(valid_results.count(actual));
        
        valid_results = {6005};
        actual = find_closest_point_of_interest(LatLon(43.56804275512695312, -79.77233123779296875));
        CHECK(valid_results.count(actual));
        
        valid_results = {6118};
        actual = find_closest_point_of_interest(LatLon(43.69831466674804688, -79.65619659423828125));
        CHECK(valid_results.count(actual));
        
        valid_results = {6409};
        actual = find_closest_point_of_interest(LatLon(43.73253250122070312, -79.09117126464843750));
        CHECK(valid_results.count(actual));
        
        valid_results = {6498};
        actual = find_closest_point_of_interest(LatLon(43.81575393676757812, -79.77579498291015625));
        CHECK(valid_results.count(actual));
        
        valid_results = {6687};
        actual = find_closest_point_of_interest(LatLon(43.48199844360351562, -79.40149688720703125));
        CHECK(valid_results.count(actual));
        
        valid_results = {6687};
        actual = find_closest_point_of_interest(LatLon(43.59196853637695312, -79.37844085693359375));
        CHECK(valid_results.count(actual));
        
        valid_results = {6848};
        actual = find_closest_point_of_interest(LatLon(43.82280349731445312, -79.18561553955078125));
        CHECK(valid_results.count(actual));
        
        valid_results = {6941};
        actual = find_closest_point_of_interest(LatLon(43.71036148071289062, -79.60934448242187500));
        CHECK(valid_results.count(actual));
        
        valid_results = {7207};
        actual = find_closest_point_of_interest(LatLon(43.73324203491210938, -79.53833770751953125));
        CHECK(valid_results.count(actual));
        
        valid_results = {7337};
        actual = find_closest_point_of_interest(LatLon(43.83701705932617188, -79.14602661132812500));
        CHECK(valid_results.count(actual));
        
        valid_results = {8278};
        actual = find_closest_point_of_interest(LatLon(43.58732604980468750, -79.09316253662109375));
        CHECK(valid_results.count(actual));
        
        valid_results = {8392};
        actual = find_closest_point_of_interest(LatLon(43.60358428955078125, -79.29006958007812500));
        CHECK(valid_results.count(actual));
        
        valid_results = {8427};
        actual = find_closest_point_of_interest(LatLon(43.91736602783203125, -79.71571350097656250));
        CHECK(valid_results.count(actual));
        
        valid_results = {8455};
        actual = find_closest_point_of_interest(LatLon(43.89463424682617188, -79.20743560791015625));
        CHECK(valid_results.count(actual));
        
    } //closest_poi

    TEST_FIXTURE(MapFixture, closest_intersection) {
        std::set<unsigned> valid_results;
        unsigned actual;
        
        valid_results = {2136};
        actual = find_closest_intersection(LatLon(43.71936035156250000, -79.52051544189453125));
        CHECK(valid_results.count(actual));
        
        valid_results = {4253};
        actual = find_closest_intersection(LatLon(43.77373886108398438, -79.47676086425781250));
        CHECK(valid_results.count(actual));
        
        valid_results = {6084, 6093, 6100, 6101, 97673};
        actual = find_closest_intersection(LatLon(43.48199844360351562, -79.40149688720703125));
        CHECK(valid_results.count(actual));
        
        valid_results = {7137};
        actual = find_closest_intersection(LatLon(43.73253250122070312, -79.09117126464843750));
        CHECK(valid_results.count(actual));
        
        valid_results = {10260};
        actual = find_closest_intersection(LatLon(43.82280349731445312, -79.18561553955078125));
        CHECK(valid_results.count(actual));
        
        valid_results = {10432};
        actual = find_closest_intersection(LatLon(43.83701705932617188, -79.14602661132812500));
        CHECK(valid_results.count(actual));
        
        valid_results = {10545};
        actual = find_closest_intersection(LatLon(43.60358428955078125, -79.29006958007812500));
        CHECK(valid_results.count(actual));
        
        valid_results = {10549};
        actual = find_closest_intersection(LatLon(43.56536483764648438, -79.34043121337890625));
        CHECK(valid_results.count(actual));
        
        valid_results = {21784};
        actual = find_closest_intersection(LatLon(43.71036148071289062, -79.60934448242187500));
        CHECK(valid_results.count(actual));
        
        valid_results = {28348};
        actual = find_closest_intersection(LatLon(43.71545028686523438, -79.45131683349609375));
        CHECK(valid_results.count(actual));
        
        valid_results = {34716};
        actual = find_closest_intersection(LatLon(43.69831466674804688, -79.65619659423828125));
        CHECK(valid_results.count(actual));
        
        valid_results = {41421};
        actual = find_closest_intersection(LatLon(43.90193939208984375, -79.57024383544921875));
        CHECK(valid_results.count(actual));
        
        valid_results = {42815};
        actual = find_closest_intersection(LatLon(43.90982437133789062, -79.60358428955078125));
        CHECK(valid_results.count(actual));
        
        valid_results = {42935};
        actual = find_closest_intersection(LatLon(43.77970123291015625, -79.60785675048828125));
        CHECK(valid_results.count(actual));
        
        valid_results = {43170};
        actual = find_closest_intersection(LatLon(43.91736602783203125, -79.71571350097656250));
        CHECK(valid_results.count(actual));
        
        valid_results = {43259};
        actual = find_closest_intersection(LatLon(43.81575393676757812, -79.77579498291015625));
        CHECK(valid_results.count(actual));
        
        valid_results = {43659};
        actual = find_closest_intersection(LatLon(43.77691268920898438, -79.58845520019531250));
        CHECK(valid_results.count(actual));
        
        valid_results = {45565, 96913};
        actual = find_closest_intersection(LatLon(43.74922943115234375, -79.02179718017578125));
        CHECK(valid_results.count(actual));
        
        valid_results = {57597};
        actual = find_closest_intersection(LatLon(43.73324203491210938, -79.53833770751953125));
        CHECK(valid_results.count(actual));
        
        valid_results = {65218};
        actual = find_closest_intersection(LatLon(43.56804275512695312, -79.77233123779296875));
        CHECK(valid_results.count(actual));
        
        valid_results = {66201};
        actual = find_closest_intersection(LatLon(43.57031250000000000, -79.68901062011718750));
        CHECK(valid_results.count(actual));
        
        valid_results = {71302};
        actual = find_closest_intersection(LatLon(43.58732604980468750, -79.09316253662109375));
        CHECK(valid_results.count(actual));
        
        valid_results = {71564};
        actual = find_closest_intersection(LatLon(43.71529388427734375, -79.55553436279296875));
        CHECK(valid_results.count(actual));
        
        valid_results = {75116};
        actual = find_closest_intersection(LatLon(43.83801269531250000, -79.22557830810546875));
        CHECK(valid_results.count(actual));
        
        valid_results = {83449};
        actual = find_closest_intersection(LatLon(43.87590789794921875, -79.01165771484375000));
        CHECK(valid_results.count(actual));
        
        valid_results = {83714};
        actual = find_closest_intersection(LatLon(43.89463424682617188, -79.20743560791015625));
        CHECK(valid_results.count(actual));
        
        valid_results = {87011};
        actual = find_closest_intersection(LatLon(43.85248184204101562, -79.43147277832031250));
        CHECK(valid_results.count(actual));
        
        valid_results = {91612};
        actual = find_closest_intersection(LatLon(43.87495422363281250, -79.56262969970703125));
        CHECK(valid_results.count(actual));
        
        valid_results = {103748};
        actual = find_closest_intersection(LatLon(43.65697860717773438, -79.70158386230468750));
        CHECK(valid_results.count(actual));
        
        valid_results = {107411};
        actual = find_closest_intersection(LatLon(43.64250946044921875, -79.35969543457031250));
        CHECK(valid_results.count(actual));
        
    } //closest_intersection

} //spatial_queries_public_toronto_canada

