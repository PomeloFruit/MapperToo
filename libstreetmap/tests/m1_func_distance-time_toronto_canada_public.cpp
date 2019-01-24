#include <algorithm>
#include <set>
#include <unittest++/UnitTest++.h>
#include "m1.h"
#include "unit_test_util.h"
#include "StreetsDatabaseAPI.h"

using ece297test::relative_error;


SUITE(distance_time_queries_public_toronto_canada) {

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

    TEST_FIXTURE(MapFixture, distance_between_two_points) {
        double expected;
        double actual;
        
        expected = 2356.55189424585660163;
        actual = find_distance_between_two_points(LatLon(43.79279327392578125, -79.40152740478515625), LatLon(43.78813934326171875, -79.37289428710937500));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 2428.15175589732825756;
        actual = find_distance_between_two_points(LatLon(43.71529388427734375, -79.55553436279296875), LatLon(43.73324203491210938, -79.53833770751953125));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 2949.60709968117043900;
        actual = find_distance_between_two_points(LatLon(43.59196853637695312, -79.37844085693359375), LatLon(43.59408187866210938, -79.41493988037109375));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 3548.21484590875252252;
        actual = find_distance_between_two_points(LatLon(43.82280349731445312, -79.18561553955078125), LatLon(43.83701705932617188, -79.14602661132812500));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 7083.20920347177525400;
        actual = find_distance_between_two_points(LatLon(43.64250946044921875, -79.35969543457031250), LatLon(43.60358428955078125, -79.29006958007812500));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 9272.78419905099690368;
        actual = find_distance_between_two_points(LatLon(43.54529571533203125, -79.10993194580078125), LatLon(43.56031036376953125, -79.22308349609375000));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 9502.64656263026699889;
        actual = find_distance_between_two_points(LatLon(43.71036148071289062, -79.60934448242187500), LatLon(43.65697860717773438, -79.70158386230468750));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 10259.69455752463727549;
        actual = find_distance_between_two_points(LatLon(43.59600830078125000, -79.11286163330078125), LatLon(43.52816772460937500, -79.19911193847656250));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 10810.79465902316587744;
        actual = find_distance_between_two_points(LatLon(43.87495422363281250, -79.56262969970703125), LatLon(43.85248184204101562, -79.43147277832031250));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 14257.15447348564339336;
        actual = find_distance_between_two_points(LatLon(43.61819458007812500, -79.22166442871093750), LatLon(43.72824478149414062, -79.31253051757812500));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 14466.27648594317543029;
        actual = find_distance_between_two_points(LatLon(43.77970123291015625, -79.60785675048828125), LatLon(43.71545028686523438, -79.45131683349609375));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 16151.57112973360381147;
        actual = find_distance_between_two_points(LatLon(43.73253250122070312, -79.09117126464843750), LatLon(43.58732604980468750, -79.09316253662109375));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 17244.03077702475638944;
        actual = find_distance_between_two_points(LatLon(43.56804275512695312, -79.77233123779296875), LatLon(43.69831466674804688, -79.65619659423828125));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 23088.60558041352851433;
        actual = find_distance_between_two_points(LatLon(43.84590911865234375, -79.17972564697265625), LatLon(43.66649627685546875, -79.32427978515625000));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 26696.83790673241310287;
        actual = find_distance_between_two_points(LatLon(43.77907943725585938, -79.08944702148437500), LatLon(43.88920211791992188, -79.38510131835937500));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 28346.57113680840484449;
        actual = find_distance_between_two_points(LatLon(43.57031250000000000, -79.68901062011718750), LatLon(43.77373886108398438, -79.47676086425781250));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 29221.49002234368890640;
        actual = find_distance_between_two_points(LatLon(43.65841293334960938, -79.78363037109375000), LatLon(43.66227722167968750, -79.42051696777343750));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 30851.35060581533980439;
        actual = find_distance_between_two_points(LatLon(43.77691268920898438, -79.58845520019531250), LatLon(43.56536483764648438, -79.34043121337890625));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 31805.81615727611278999;
        actual = find_distance_between_two_points(LatLon(43.71936035156250000, -79.52051544189453125), LatLon(43.89463424682617188, -79.20743560791015625));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 34408.83086704561719671;
        actual = find_distance_between_two_points(LatLon(43.74295425415039062, -79.03385925292968750), LatLon(43.64625930786132812, -79.44028472900390625));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 36762.76484817622986156;
        actual = find_distance_between_two_points(LatLon(43.89926147460937500, -79.00795745849609375), LatLon(43.57030868530273438, -79.05248260498046875));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 38140.16821192804491147;
        actual = find_distance_between_two_points(LatLon(43.87306213378906250, -79.04129791259765625), LatLon(43.88061904907226562, -79.51689147949218750));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 38384.24630290480854455;
        actual = find_distance_between_two_points(LatLon(43.89250564575195312, -79.44643402099609375), LatLon(43.57640457153320312, -79.25479125976562500));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 40275.23822157075483119;
        actual = find_distance_between_two_points(LatLon(43.83801269531250000, -79.22557830810546875), LatLon(43.91736602783203125, -79.71571350097656250));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 41625.44069501305057202;
        actual = find_distance_between_two_points(LatLon(43.56082534790039062, -79.01221466064453125), LatLon(43.72190093994140625, -79.47900390625000000));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 44869.30046041508467169;
        actual = find_distance_between_two_points(LatLon(43.87590789794921875, -79.01165771484375000), LatLon(43.90193939208984375, -79.57024383544921875));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 45646.59117179606255377;
        actual = find_distance_between_two_points(LatLon(43.64857482910156250, -79.20729827880859375), LatLon(43.71325683593750000, -79.76767730712890625));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 50284.11633087694644928;
        actual = find_distance_between_two_points(LatLon(43.90982437133789062, -79.60358428955078125), LatLon(43.48199844360351562, -79.40149688720703125));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 51318.54446459973405581;
        actual = find_distance_between_two_points(LatLon(43.65852355957031250, -79.65676879882812500), LatLon(43.54739761352539062, -79.03836822509765625));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 60998.15420726743468549;
        actual = find_distance_between_two_points(LatLon(43.74922943115234375, -79.02179718017578125), LatLon(43.81575393676757812, -79.77579498291015625));
        CHECK(relative_error(expected, actual) < 0.001000000);
        
    } //distance_between_two_points

    TEST_FIXTURE(MapFixture, street_segment_length) {
        double expected;
        double actual;
        
        expected = 9.09774878682763521;
        actual = find_street_segment_length(140688);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 9.24207725918289391;
        actual = find_street_segment_length(151372);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 17.09496565388253231;
        actual = find_street_segment_length(28196);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 23.48891492243527068;
        actual = find_street_segment_length(124495);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 24.15866891885319490;
        actual = find_street_segment_length(90597);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 25.82816268275384530;
        actual = find_street_segment_length(36389);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 27.41978422292287121;
        actual = find_street_segment_length(142034);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 29.72096698861328434;
        actual = find_street_segment_length(113732);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 32.14021668534559240;
        actual = find_street_segment_length(155517);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 44.83770116140563999;
        actual = find_street_segment_length(117861);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 49.78241523119176293;
        actual = find_street_segment_length(126916);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 50.86959588221137096;
        actual = find_street_segment_length(77997);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 54.44416223401825050;
        actual = find_street_segment_length(110253);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 55.41091787626419318;
        actual = find_street_segment_length(84468);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 57.29100715801643418;
        actual = find_street_segment_length(122449);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 58.21534292233229735;
        actual = find_street_segment_length(142671);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 62.77545881717573195;
        actual = find_street_segment_length(43905);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 66.43213964050811171;
        actual = find_street_segment_length(30625);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 81.76324865924725316;
        actual = find_street_segment_length(139629);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 88.29971916585265035;
        actual = find_street_segment_length(86014);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 88.64990923313939675;
        actual = find_street_segment_length(139231);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 125.91658222530665512;
        actual = find_street_segment_length(67668);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 158.35320150200374201;
        actual = find_street_segment_length(107519);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 165.05292411409178044;
        actual = find_street_segment_length(42404);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 187.72345269100949849;
        actual = find_street_segment_length(106519);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 222.14266565772751960;
        actual = find_street_segment_length(40265);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 304.82387440614292018;
        actual = find_street_segment_length(111218);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 313.42368622788245602;
        actual = find_street_segment_length(99426);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 530.64198183238954698;
        actual = find_street_segment_length(89824);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 660.70680896110570757;
        actual = find_street_segment_length(10);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
    } //street_segment_length

    TEST_FIXTURE(MapFixture, street_length) {
        double expected;
        double actual;
        
        expected = 68.94498340320835439;
        actual = find_street_length(9098);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 83.59169625867053810;
        actual = find_street_length(10487);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 88.39675553219957749;
        actual = find_street_length(20909);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 91.52527038753788702;
        actual = find_street_length(12181);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 143.61104414393872730;
        actual = find_street_length(11357);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 186.37679273336874530;
        actual = find_street_length(18773);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 192.41808057775466523;
        actual = find_street_length(19096);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 199.21033255849334864;
        actual = find_street_length(5903);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 229.30745138354708956;
        actual = find_street_length(14456);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 272.98694841705327008;
        actual = find_street_length(5701);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 325.71336610022336799;
        actual = find_street_length(16463);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 331.54461504482202372;
        actual = find_street_length(4118);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 374.18997794636300114;
        actual = find_street_length(15846);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 403.56821987986796785;
        actual = find_street_length(17063);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 422.42757786293145728;
        actual = find_street_length(3791);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 434.95506486046878081;
        actual = find_street_length(14321);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 455.12257706863618978;
        actual = find_street_length(12077);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 460.37021757360480478;
        actual = find_street_length(14953);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 520.60634217883693964;
        actual = find_street_length(16738);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 538.28395173888827685;
        actual = find_street_length(11564);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 541.75094288859770586;
        actual = find_street_length(4893);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 580.87837195423321646;
        actual = find_street_length(18915);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 582.75535934102310875;
        actual = find_street_length(18719);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 594.61681327446206069;
        actual = find_street_length(19181);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 702.92987682766056423;
        actual = find_street_length(20351);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 728.87313627090520640;
        actual = find_street_length(5414);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 985.62795764283407607;
        actual = find_street_length(13368);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 997.22009402007211065;
        actual = find_street_length(14823);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1118.82766310318356773;
        actual = find_street_length(15291);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 2408.61874267665962179;
        actual = find_street_length(2);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
    } //street_length

    TEST_FIXTURE(MapFixture, street_segment_travel_time) {
        double expected;
        double actual;
        
        expected = 0.65503791265158984;
        actual = find_street_segment_travel_time(140688);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 0.66542956266116826;
        actual = find_street_segment_travel_time(151372);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1.23083752707954219;
        actual = find_street_segment_travel_time(28196);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1.61415724181060294;
        actual = find_street_segment_travel_time(117861);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1.69120187441533942;
        actual = find_street_segment_travel_time(124495);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1.73942416215743023;
        actual = find_street_segment_travel_time(90597);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1.85962771315827680;
        actual = find_street_segment_travel_time(36389);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 1.97422446405044671;
        actual = find_street_segment_travel_time(142034);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 2.13990962318015621;
        actual = find_street_segment_travel_time(113732);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 2.31409560134488235;
        actual = find_street_segment_travel_time(155517);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 3.43746042948098607;
        actual = find_street_segment_travel_time(122449);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 3.58433389664580737;
        actual = find_street_segment_travel_time(126916);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 3.91997968084931436;
        actual = find_street_segment_travel_time(110253);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 4.19150469040792473;
        actual = find_street_segment_travel_time(142671);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 4.51983303483665289;
        actual = find_street_segment_travel_time(43905);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 4.57826362939902332;
        actual = find_street_segment_travel_time(77997);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 4.78311405411658352;
        actual = find_street_segment_travel_time(30625);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 4.98698260886377653;
        actual = find_street_segment_travel_time(84468);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 6.38279346478603671;
        actual = find_street_segment_travel_time(139231);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 7.35869237933225229;
        actual = find_street_segment_travel_time(139629);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 7.94697472492673818;
        actual = find_street_segment_travel_time(86014);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 9.99641995459773902;
        actual = find_street_segment_travel_time(40265);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 11.33249240027759974;
        actual = find_street_segment_travel_time(67668);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 14.25178813518033571;
        actual = find_street_segment_travel_time(107519);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 14.85476317026826010;
        actual = find_street_segment_travel_time(42404);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 16.89511074219085529;
        actual = find_street_segment_travel_time(106519);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 21.94731895724229176;
        actual = find_street_segment_travel_time(111218);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 22.56650540840753649;
        actual = find_street_segment_travel_time(99426);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 38.20622269193204090;
        actual = find_street_segment_travel_time(89824);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
        expected = 47.57089024519960674;
        actual = find_street_segment_travel_time(10);
        CHECK(relative_error(expected, actual) < 0.001000000);
        
    } //street_segment_travel_time

} //distance_time_queries_public_toronto_canada

