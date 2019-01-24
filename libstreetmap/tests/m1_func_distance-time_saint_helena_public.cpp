#include <algorithm>
#include <set>
#include <unittest++/UnitTest++.h>
#include "m1.h"
#include "unit_test_util.h"
#include "StreetsDatabaseAPI.h"

using ece297test::relative_error;


SUITE(distance_time_queries_public_saint_helena) {

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

    TEST_FIXTURE(MapFixture, distance_between_two_points) {
        double expected;
        double actual;
        
        expected = 42131.46180899130558828;
        actual = find_distance_between_two_points(LatLon(-11.68280792236328125, -11.81244564056396484), LatLon(-11.35332679748535156, -11.62172412872314453));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 56241.57087921761558391;
        actual = find_distance_between_two_points(LatLon(-9.70925903320312500, -7.70836591720581055), LatLon(-9.44832706451416016, -7.26911783218383789));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 115338.13176310292328708;
        actual = find_distance_between_two_points(LatLon(-13.01890945434570312, -9.63973808288574219), LatLon(-13.73339653015136719, -8.86724281311035156));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 155721.22809826550655998;
        actual = find_distance_between_two_points(LatLon(-11.77335262298583984, -12.40951538085937500), LatLon(-12.75326442718505859, -13.43280696868896484));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 166330.88012566044926643;
        actual = find_distance_between_two_points(LatLon(-8.75191974639892578, -11.89122009277343750), LatLon(-9.16448402404785156, -10.43607902526855469));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 230533.81292540708091110;
        actual = find_distance_between_two_points(LatLon(-10.50049591064453125, -12.39301395416259766), LatLon(-11.67993831634521484, -10.65622806549072266));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 296483.99684088258072734;
        actual = find_distance_between_two_points(LatLon(-11.36636924743652344, -6.66052150726318359), LatLon(-14.03187465667724609, -6.68261575698852539));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 300358.95126025506760925;
        actual = find_distance_between_two_points(LatLon(-14.38582324981689453, -14.21776580810546875), LatLon(-11.99445438385009766, -12.92930889129638672));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 487759.62171078263781965;
        actual = find_distance_between_two_points(LatLon(-14.34416866302490234, -13.29333782196044922), LatLon(-10.60994338989257812, -10.93852043151855469));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 522334.18411655724048615;
        actual = find_distance_between_two_points(LatLon(-11.60812187194824219, -11.42399120330810547), LatLon(-8.39069080352783203, -7.95042800903320312));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 525218.86689772817771882;
        actual = find_distance_between_two_points(LatLon(-10.55166912078857422, -12.17775917053222656), LatLon(-14.43496608734130859, -9.42596817016601562));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 619435.47167796839494258;
        actual = find_distance_between_two_points(LatLon(-9.43003654479980469, -8.15176296234130859), LatLon(-7.97343206405639648, -13.58964157104492188));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 683813.44527001294773072;
        actual = find_distance_between_two_points(LatLon(-8.73443222045898438, -5.77834224700927734), LatLon(-8.25660419464111328, -11.97569370269775391));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 906920.29224308708216995;
        actual = find_distance_between_two_points(LatLon(-8.11186981201171875, -12.34554195404052734), LatLon(-15.96532535552978516, -10.10347843170166016));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 925034.32448217854835093;
        actual = find_distance_between_two_points(LatLon(-11.05981922149658203, -5.89088439941406250), LatLon(-9.83863544464111328, -14.25615596771240234));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
    } //distance_between_two_points

    TEST_FIXTURE(MapFixture, street_segment_length) {
        double expected;
        double actual;
        
        expected = 3.96597143884154457;
        actual = find_street_segment_length(95);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 14.44067095243671695;
        actual = find_street_segment_length(90);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 54.74380950785471356;
        actual = find_street_segment_length(0);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 88.60965028773023278;
        actual = find_street_segment_length(280);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 90.10398470052959397;
        actual = find_street_segment_length(276);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 109.26947531035480665;
        actual = find_street_segment_length(242);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 125.12081508593892920;
        actual = find_street_segment_length(321);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 195.15771889682184792;
        actual = find_street_segment_length(286);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 205.79085584125428454;
        actual = find_street_segment_length(202);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 276.79768194393716385;
        actual = find_street_segment_length(193);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 297.67290480362589733;
        actual = find_street_segment_length(82);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 327.85262022318028130;
        actual = find_street_segment_length(152);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 338.64117741834195385;
        actual = find_street_segment_length(240);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 354.79578905664658350;
        actual = find_street_segment_length(265);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1744.61687924442571784;
        actual = find_street_segment_length(69);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
    } //street_segment_length

    TEST_FIXTURE(MapFixture, street_length) {
        double expected;
        double actual;
        
        expected = 46.50967211883510544;
        actual = find_street_length(9);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 68.19403613744451320;
        actual = find_street_length(3);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 234.74717201570786074;
        actual = find_street_length(16);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 244.30895054000075106;
        actual = find_street_length(13);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 251.05458600096653754;
        actual = find_street_length(12);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 274.75603497021086241;
        actual = find_street_length(2);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 702.55426006606637657;
        actual = find_street_length(15);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 936.62353624561819743;
        actual = find_street_length(10);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 944.83312595036329640;
        actual = find_street_length(8);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1255.35608656692261320;
        actual = find_street_length(11);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1391.77540091191349347;
        actual = find_street_length(5);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1945.62104694721165288;
        actual = find_street_length(1);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 2919.67030285654800537;
        actual = find_street_length(4);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 5818.22372649587123306;
        actual = find_street_length(7);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 8117.87950647245634173;
        actual = find_street_length(6);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
    } //street_length

    TEST_FIXTURE(MapFixture, street_segment_travel_time) {
        double expected;
        double actual;
        
        expected = 0.35693742949573898;
        actual = find_street_segment_travel_time(95);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1.29966038571930453;
        actual = find_street_segment_travel_time(90);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 4.92694285570692436;
        actual = find_street_segment_travel_time(0);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 6.37989482071657665;
        actual = find_street_segment_travel_time(280);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 6.48748689843813153;
        actual = find_street_segment_travel_time(276);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 9.00869868618760350;
        actual = find_street_segment_travel_time(321);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 9.83425277793193153;
        actual = find_street_segment_travel_time(242);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 14.81694162057030972;
        actual = find_street_segment_travel_time(202);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 17.56419470071396560;
        actual = find_street_segment_travel_time(286);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 19.92943309996347523;
        actual = find_street_segment_travel_time(193);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 21.43244914586106376;
        actual = find_street_segment_travel_time(82);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 25.54529681207855774;
        actual = find_street_segment_travel_time(265);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 29.50673582008623086;
        actual = find_street_segment_travel_time(152);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 30.47770596765077045;
        actual = find_street_segment_travel_time(240);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 125.61241530559865964;
        actual = find_street_segment_travel_time(69);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
    } //street_segment_travel_time

} //distance_time_queries_public_saint_helena

