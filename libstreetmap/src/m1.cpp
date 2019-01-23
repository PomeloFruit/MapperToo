/* 
 * Copyright 2018 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "m1.h"
#include "StreetsDatabaseAPI.h"

bool load_map(std::string /*map_path*/) {
    bool load_successful = false; //Indicates whether the map has loaded 
                                  //successfully

    //
    //Load your map related data structures here
    //

    

    load_successful = true; //Make sure this is updated to reflect whether
                            //loading the map succeeded or failed

    return load_successful;
}

void close_map() {
    //Clean-up your map related data structures here
    
}



std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){
    std::vector<unsigned> ids;
    int numOfSegs=getIntersectionStreetSegmentCount(intersection_id);
    for(int i=0;i<numOfSegs;i++){
        ids.push_back(getIntersectionStreetSegment(i, intersection_id));
    }
    return ids;
    //so as of now this or any of the other things I write won't pass the performance test
    //but don't worry I have it all under control just like these versions
    //to fix this one and probably all the other ones I'm about to write 
    //create a global variable and make a nested for loops for nested vectors
            
}
std::vector<std::string> find_intersection_street_names(unsigned intersection_id){
    //this is supposed to return duplicate names here also so don't worry about
    std::vector<std::string> names;
    int numOfSegs=getIntersectionStreetSegmentCount(intersection_id);
    for(int i=0;i<numOfSegs;i++){
        names.push_back(getStreetName(getIntersectionStreetSegment(i, intersection_id))); //It's gross but it works I think
    }
    return names;
    //this will suffer from the same problems as the prev function
}

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){
    //so the corner case here is if it has curvepoints or not I think??
    //but basically do they share a road and if so is the road one way
    //is an intersection always connected to itself? or does a street seg need to curve back into itself?
    //**read above unless I get answers later
    //if an intersection is always connected to itself I can just return true if they are =
    //and finally this would be easier if I had made the global variable first but oh well
    //bool returniee=false;
    std::vector<unsigned> segsInt1=find_intersection_street_segments(intersection_id1);
    std::vector<unsigned> segsInt2=find_intersection_street_segments(intersection_id2);
    //so I have the two lists
    //literally O(n^2) incoming
    //if you have a better idea please implement it
    for(int i=0;i<segsInt1.size();i++){
        for(int c=0;c<segsInt2.size();c++){
            if(segsInt1[i]==segsInt2[c]){
                if(getInfoStreetSegment(segsInt1[i]).oneWay){
                    return true;
                }
            }
        }
    }
    
    return false;
    //this is really not going to pass the speed test
}