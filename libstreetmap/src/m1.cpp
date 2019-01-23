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
