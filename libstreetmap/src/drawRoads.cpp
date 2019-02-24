#include "drawRoads.h"

#include "globals.h"
#include "latLonToXY.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"



void roadDrawing::setRoadColourSize(int type, bool highlight, ezgl::renderer &g, double startArea, double currentArea){
    const int HIGHLIGHTFACT = 5;
    int width = ROADWIDTH;
    g.set_color(255,255,255,255); //Ddefault white colour
    //1/(cur/start))
    double inputAdjust=1/(currentArea/startArea);
    double adjustingAdd=(280930.9-(2586289/1221110)) + (2.117982 - 280930.9)/(1 + (pow(inputAdjust/17546410000, 0.7331144)));
    switch(type){
        case HIGHWAY: // yellowish
            width =HIGHWAYWIDTH+adjustingAdd;
            g.set_color(250,215,56,200);
            break;
        case TRUNK:
            width = PRIMWIDTH+adjustingAdd;
            g.set_color(255,255,255,255);
            break;
        case PRIMARY: // white and thick
            width = PRIMWIDTH+adjustingAdd-1;
            g.set_color(255,255,255,255);
            break;
        case SECONDARY: //===========================================================================
            width = ROADWIDTH+adjustingAdd-1;
            g.set_color(255,255,255,255);
            break;
        case RESIDENTIAL: //===========================================================================
            width = ROADWIDTH+adjustingAdd-2;
            g.set_color(255,255,255,255);
            break;
        case SERVICE: // light gray
            width = ROADWIDTH+adjustingAdd-2;
            g.set_color(244,244,244,255);
            break;
        default: //=====================================================================================
            break;
    }
    
    if(highlight){
        width = width + HIGHLIGHTFACT;
        g.set_color(255,102,255,175);
    }
    
    g.set_line_width(width);
    g.set_line_cap (ezgl::line_cap::round);
}
//y = (280930.9-(2586289/1221110)) + (2.117982 - 280930.9)/(1 + (x/17546410000)^0.7331144)
 void roadDrawing::drawStreetRoads(int numSegs, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g, double startArea, double currentArea, ezgl::rectangle currentRectangle){
    LatLon from, to;
    bool sufficentlyZoomed=false;
    bool sufficentlyBig=false;
    bool sufficentlyZoomedLevel1=false;
    bool sufficentlyZoomedLevel2=false;
    //this should determine how what roads I should show off the bat
    //it's based on nothing but testing
    //make sure it's in bounds
    
    bool draw=false;
    
    for(int i=0 ; i<numSegs ; i++){
        setRoadColourSize(info.StreetSegInfo[i].type, info.StreetSegInfo[i].clicked, g, startArea, currentArea);
        
        from = info.IntersectionInfo[info.StreetSegInfo[i].fromIntersection].position;
        //info.numStreetType
        //determines which roads we are going to show for each city at default zoom
        //will likely need to introduce leveled zooming for the largest classification of cities
        if((100<info.numStreetType[5])&&(info.numStreetType[5]<1000)){
            sufficentlyBig=(info.StreetSegInfo[i].type==0||info.StreetSegInfo[i].type==5||info.StreetSegInfo[i].type==1);
            sufficentlyZoomedLevel1=(info.StreetSegInfo[i].type==2)&&((currentArea/startArea)<0.25);
            sufficentlyZoomed=(currentArea/startArea)<0.10;//randomly chosen
        }
        else if(info.numStreetType[5]<100){
            sufficentlyBig=(info.StreetSegInfo[i].type==0||info.StreetSegInfo[i].type==5||info.StreetSegInfo[i].type==1||info.StreetSegInfo[i].type==2);
            sufficentlyZoomed=(currentArea/startArea)<0.25;//randomly chosen
        }
        else{
            sufficentlyBig=(info.StreetSegInfo[i].type==0||info.StreetSegInfo[i].type==5);
            sufficentlyZoomedLevel1=(info.StreetSegInfo[i].type==1)&&((currentArea/startArea)<0.25);
            sufficentlyZoomed=(currentArea/startArea)<0.05;//randomly chosen
            sufficentlyZoomedLevel2=(info.StreetSegInfo[i].type==2)&&(currentArea/startArea)<0.1;//I'll prob have to change this 
            
        }
        
        
        
        if(sufficentlyZoomed||sufficentlyBig||sufficentlyZoomedLevel1||sufficentlyZoomedLevel2){
            for(int c=0;c<info.StreetSegInfo[i].numCurvePoints;c++){
                to = getStreetSegmentCurvePoint(c,i);
                draw=(inBounds(xy, currentRectangle, to))||(inBounds(xy, currentRectangle, from));
                if(draw){
                    drawStraightStreet(from, to, xy, g);
                }
                from = to;
             //   std::cout<<"I AM DRAWING "<<i<<'\n';
            }
            to = info.IntersectionInfo[info.StreetSegInfo[i].toIntersection].position;
            draw=(inBounds(xy, currentRectangle, to))||(inBounds(xy, currentRectangle, from));
            if(draw){
                    drawStraightStreet(from, to, xy, g);
            }
        }
    }
}

void roadDrawing::drawStraightStreet(LatLon &pt1, LatLon &pt2, mapBoundary &xy, ezgl::renderer &g){
    float xInitial, yInitial, xFinal, yFinal;
    
    xInitial = xy.xFromLon(pt1.lon());
    yInitial = xy.yFromLat(pt1.lat());
    xFinal = xy.xFromLon(pt2.lon());
    yFinal = xy.yFromLat(pt2.lat());
   
    g.draw_line({xInitial, yInitial},{xFinal, yFinal});
}

void roadDrawing::drawIntersections(int numInter, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    //make sure it's in bounds
    ezgl::rectangle currentRectangle=g.get_visible_world();
    for(int i = 0 ; i < numInter ; i++){
        if(inBounds(xy, currentRectangle, info.IntersectionInfo[i].position)){
            drawOneIntersection(i, xy, info, g);
        }
    }
    drawSpecialIntersections(xy, info, g);
}

void roadDrawing::drawSpecialIntersections(mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    for(unsigned i = 0 ; i < info.lastIntersection.size() ; i++){
        drawOneIntersection(info.lastIntersection[i], xy, info, g);
    }
}

void roadDrawing::drawOneIntersection(int id, mapBoundary &xy, infoStrucs &info, ezgl::renderer &g){
    //const double NORMALRAD = 0.00015;
    const float RADIUSNORM = 0.00003;
    const double radius = g.get_visible_world().width()*0.009;
    float x, y;
       
    x = xy.xFromLon(info.IntersectionInfo[id].position.lon());
    y = xy.yFromLat(info.IntersectionInfo[id].position.lat());

    if(info.IntersectionInfo[id].clicked) {
        g.draw_surface(g.load_png("/homes/d/dujia3/ece297/work/mapper/libstreetmap/resources/intersection.png"), ezgl::point2d(x-2*radius, y+2*radius));
//        //outer circle (turquoise)
//        g.set_color(110,236,209,125);
//        g.fill_elliptic_arc(ezgl::point2d(x,y),RADIUSHIGH,RADIUSHIGH,0,360);
//        
//        //inner circle (dark navy)
//        g.set_color(0,119,119,255);
//        g.fill_elliptic_arc(ezgl::point2d(x,y),RADIUSHIGH/5,RADIUSHIGH/5,0,360);
        
    } else {
        //regular intersection (white)
        g.set_color(255,255,255,255);
        g.fill_elliptic_arc(ezgl::point2d(x,y),RADIUSNORM,RADIUSNORM,0,360);
    }
}
bool roadDrawing::inBounds(mapBoundary &xy, ezgl::rectangle& curBounds, LatLon& position){
    return (xy.yFromLat(position.lat())<curBounds.top())&&(xy.yFromLat(position.lat())>curBounds.bottom())&&(xy.xFromLon(position.lon())>curBounds.left())&&(xy.xFromLon(position.lon())<curBounds.right());
}



/*SHOWING TRUNKS AND HIGHWAYS ONLY==>try trunks, highways, primary for those biggers
 * beijing:seems a bit empty in terms of feats, roads seem nice
 * cairo: needs more roads shown, again feats, seems a bit barren
 * cape town:needs more roads shown
 * golden-horseshoe:make it the same as toronto tbh
 * hamilton:toronto
 * hong kong:more features otherwise probably fine
 * iceland:toronto
 * interlaken:toronto
 * london:roads acc look pretty good but try with primary later after making categorization
 * moscow:toronto?
 * new delhi:toronto?
 * new york: show primaries but thin
 * rio:show primaries/secondaries?(maybe toronto this)
 * singapore:toronto this?
 * sydney:show primaries?
 * tehran:primaries but thin?
 * tokyo:fine but gate showing all roads (needs like 2 levels to sufficently zoomed)
 * 
 * PLAN:make a vector that contains the road levels that should be shown for each classification, determine the classification, fill vector with the roads it should show
 * to determine the classification I'm going to try just looking at how many roads there are
 * I think I'm going to try and show 3 types of road for each thing as to not speed/slowdown the time it takes to draw
 */
//make them all show more features
//only draw intersections in bounds
//same with roads (draw them if the start node is in bounds)
//^^doing both of those should speed up the map a shitload
//ONE WAYS <__NAME__< 
/*NUM OF ROADS(maybe I'd have to show based on the number of some types of roads)
 * toronto:21222
 * beijing:5879
 * cairo:6426
 * Cape:21738
 * golden-horseshoe:35837
 * hamilton:3411
 * hong kong:12108
 * iceland:4772
 * interlaken:338
 * london:108546
 * moscow:5508
 * new-delhi:1299
 * new-york:41458
 * rio:28244
 * singapore:13285
 * sydney:34080
 * tehran:23763
 * tokyo:16965
 * 
 * so as we can see the size of the city does not correspond to how many roads it has
 * so I think I should be basing what I show based on the number of trunks, higher than x number of trunks but lower than y show highways, trunks and primaries
 * higher than y trunks show only highways and trunks->this is the only problem one but I think I can fix it with one if statement
 * lower than x number of trunks show highways, primaries, trunks and secondaries -> prob need to check in drawroads
 * trying x=100, y=1000
 * 
 * LEVEL ZOOMING
 * beijing:yes->current implementation works well
 * cairo:maybe a little->it's better now
 * cape-town:seems fine to me
 * golden-horseshoe:fine
 * hamilton: fine
 * hong-kong:yes->maybe try making secondary roads a bit thinner
 * iceland:fine
 * interlaken:fine
 * london:yes->same as hk->thinner secondaries worked
 * moscow:fine
 * new-delhi:probably->doesn't look amazing but there isn't much I can do without doing a massive amount of work
 * new-york:honestly it lokos fine to me
 * rio:fine
 * singapore:maybe->works well
 * sydney:fine
 * tehran:probably->yeah I guess
 * tokyo:yes->try thinner secondaries->better
 * 
 * 
 * 
 * maybe super big cities need like 3 levels of zoom
 * maybe primaries smaller than trunks
 * maybe show residential roads a bit later
 * 
 * so determining which cities need zoom levels isn't as straightforward, for example, moscow falls within the same category as beijing but it does not need levels
 * but very big cities (london, tokyo) need more than one level of zoom, probably one that shows primaries and another that shows secondaries (the the rest after)
 * larger cities need one level of zoom it reveals secondaries (the the rest after)
 */