#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using std::vector;
using std::cout;
using std::endl;

const int MAX_LEN = 17;

struct Point;
vector<Point> translate(Point P);
bool is_inside_triangle (Point pt, Point v1, Point v2, Point v3);

struct Point {
    float x;
    float y;
    Point(float X, float Y) : x(X), y(Y) {};
};

/*
 *  Important info in struct Point:
 *  1. Triangle dimensions expressed as ordered pair (base, height)
 *  2. All the possible offsets this (base, height) ordered pair can achieve while keeping the square (in the grid) 
 *     inside it.
 */

struct valid_translations {
    // Dimensions (variable: _dimensions) are represented as a point (x,y). 
    // Actually represents dimensions as a (base, height) ordered pair.
    Point _dimensions; 
    vector<Point> _translations;
    valid_translations(Point point, vector<Point> translations) : _dimensions(point), _translations(translations) {}; 
};


/*
 *      _x and _y are the X and Y coordinates of the right angle triangle.
 *      _area is the area of the triangle.
 *      
 *      _combinations holds all the possible dimensions of the triangle, as well as the respective
 *      valid offsets for each dimension
 *
 *      all_triangles holds all the possible valid triangle combinations (for each shape base/height combination) and each 
 *      triangle orientation. Given that a triangle has three vertices, they represent 1 triangle.
 *      For instance, a vector may contain 27 points. This means it contains 9 valid triangles. 
 */

class Triangle {
    private:

        int _x;
        int _y;
        int _area;
    
    public:

        vector<valid_translations> _combinations;
        vector<Point> all_triangles; 

        Triangle(int area, int x, int y) : _x(x), _y(y), _area(area) {
            create_dimensions(_area, _combinations);
            make_combinations(_x,_y,_combinations,all_triangles);
        };

        void create_dimensions(int area, vector<valid_translations>& combinations);
        void make_combinations(int x, int y, vector<valid_translations>& combinations, vector<Point>& allTriangles);

        vector<Point> translate(Point dimensions);

        int inline getXC() const {return _x;};
        int inline getYC() const {return _y;};

        void print_dimensions() const;
        void print_triangles() const;
        
        vector<valid_translations> getCombinations() {return _combinations;};
};

/* Calculate all valid integer base/height combinations given
 * the triangle's area. Do so by imagining the triangle is a square.
 *
 * A base must be at least 2 squares wide, otherwise, the 1 by 1 square
 * representing that triangle's area cannot fit in it.
 */

void Triangle::create_dimensions(int area, vector<valid_translations>& combinations) {
    int effectiveArea = 2 * area;

    for (int base{2}; base < effectiveArea; ++base) {
        int height;
        if (effectiveArea % base == 0) {
            height = effectiveArea / base;
            combinations.push_back(valid_translations(Point(base, height), translate(Point(base, height))));
        }
    }
    return;
}

vector<Point> Triangle::translate(Point dimensions) {
    
    vector<Point> results;

    int shiftY{}, shiftX{};
    
    /*
     * The box that our triangle contains is size 1 by 1
     */

    Point q1(1,1); 

    // The three vertices of our triangle.
    Point v1(shiftX,shiftY); 
    Point v2(0, dimensions.y);
    Point v3(dimensions.x, 0);

    int maxHeight = dimensions.y;
    while (is_inside_triangle(q1,v1,v2,v3)) {
        shiftY = 0;
        v1.x = shiftX;
        v1.y = shiftY;
        while (is_inside_triangle(q1,v1,v2,v3)) {
            results.push_back(Point(shiftX, shiftY));
            shiftY--;
            v2.y--;
            v3.y--;
        }
        v2.y = maxHeight;
        v2.x--;
        v3.y = 0;
        v3.x--;
        shiftX--;
    }
   return results;
}

/* Having a vector that contains all the possible dimensions and translations (offsets) to these
 * dimensions, we can print out each base/height and shift combination.
 *
 */

void Triangle::print_dimensions() const {
    for (const auto& j : _combinations) {
        cout << j._dimensions.x << " " << j._dimensions.y << " = ";
        for (int i{}; i < j._translations.size(); i++) {
            cout << j._translations[i].x << ' ' << j._translations[i].y << ' ';
        }
        cout << "\n";
    }
    return;
}

/*
 *  This is a very meaty function. All our base/height and shift combinations
 *  apply to a triangle that is upright. But what if our triangle needs to point rightwards, 
 *  downwards, or even to the left? Well, we can take our potential translations (offsets) for each dimension
 *  and create a corresponding shift/offset for a triangle in the other directions. 
 *
 *  (aX, aY), (bX, bY), (cX, cY) will each represent valid triangle vertices for our triangle.
 *  The vector all_triangles will contain three vertices for each valid triangle placement on our board.
 *  
 *
 *
 */

void Triangle::make_combinations(int X, int Y, vector<valid_translations>& combinations, vector<Point>& allTriangles) {
    int aX, aY, bX, bY, cX, cY;
    
    // All valid triangle combinations in the upward direction.

    for (int i{}; i < combinations.size();i++) { 
        for (int j{}; j < combinations[i]._translations.size();j++) {
        
            aX = X + combinations[i]._translations[j].x;
            aY = Y + combinations[i]._translations[j].y;
            bX = aX + combinations[i]._dimensions.x;
            bY = aY;
            cX = aX;
            cY = aY + combinations[i]._dimensions.y;

            // The points must lie on the board.

            if ( aX < 0 || aX > MAX_LEN || bX < 0 || bX > MAX_LEN ||cX < 0 || cX > MAX_LEN || 
                 aY < 0 || aY > MAX_LEN || bY < 0 || bY > MAX_LEN ||cY < 0 || cY > MAX_LEN) {
                continue;
            }

            // If points are valid, push them back into our allTriangles vector for this triangle.

            allTriangles.push_back(Point(aX,aY));
            allTriangles.push_back(Point(bX,bY));
            allTriangles.push_back(Point(cX,cY));
        }
    }

    // All valid triangle combinations in the right direction.

    for (int i{}; i < combinations.size();i++) {
        for (int j{}; j < combinations[i]._translations.size();j++) {
    
            aX = X + combinations[i]._translations[j].y;
            aY = Y + std::abs(combinations[i]._translations[j].x) + 1;
            bX = aX;
            bY = aY - combinations[i]._dimensions.x;
            cX = aX + combinations[i]._dimensions.y;
            cY = aY;

            if ( aX < 0 || aX > MAX_LEN || bX < 0 || bX > MAX_LEN ||cX < 0 || cX > MAX_LEN ||
                 aY < 0 || aY > MAX_LEN || bY < 0 || bY > MAX_LEN ||cY < 0 || cY > MAX_LEN) {
                continue;
            }

            allTriangles.push_back(Point(aX,aY));
            allTriangles.push_back(Point(bX,bY));
            allTriangles.push_back(Point(cX,cY));
        }
    }

    // All valid triangle combinations in the downward direction.

    for (int i{}; i < combinations.size(); ++i) { 
        for (int j{}; j < combinations[i]._translations.size(); ++j) {

            aX = X + std::abs(combinations[i]._translations[j].x) + 1;
            aY = Y + std::abs(combinations[i]._translations[j].y) + 1;
            bX = aX - combinations[i]._dimensions.x;
            bY = aY;
            cX = aX;
            cY = aY - combinations[i]._dimensions.y;

            if ( aX < 0 || aX > MAX_LEN || bX < 0 || bX > MAX_LEN ||cX < 0 || cX > MAX_LEN || 
                 aY < 0 || aY > MAX_LEN || bY < 0 || bY > MAX_LEN ||cY < 0 || cY > MAX_LEN) {
                continue;
            }

            allTriangles.push_back(Point(aX,aY));
            allTriangles.push_back(Point(bX,bY));
            allTriangles.push_back(Point(cX,cY));
        }
    }

    // All valid triangle combinations in the left direction.

    for (int i{}; i < combinations.size(); i++) { 
        for (int j{}; j < combinations[i]._translations.size(); j++) {
        
            aX = X + std::abs(combinations[i]._translations[j].y) + 1;
            aY = Y + combinations[i]._translations[j].x;
            bX = aX;
            bY = aY + combinations[i]._dimensions.x;
            cX = aX - combinations[i]._dimensions.y;
            cY = aY;

            if ( aX < 0 || aX > MAX_LEN || bX < 0 || bX > MAX_LEN ||cX < 0 || cX > MAX_LEN ||
                 aY < 0 || aY > MAX_LEN || bY < 0 || bY > MAX_LEN ||cY < 0 || cY > MAX_LEN) {
                continue;
            }

            allTriangles.push_back(Point(aX,aY));
            allTriangles.push_back(Point(bX,bY));
            allTriangles.push_back(Point(cX,cY));
        }
    }

    return;
}

/*
 * Print the contents of the all_triangles vector, remaining cognesant that
 * each three points is one triangle.
 */


void Triangle::print_triangles() const {
    for (int i{}; i < all_triangles.size();i++) {
        cout << "( ";
           cout << all_triangles[i].x << " " << all_triangles[i].y << ' ';
        cout << ") |";
        if ((i + 1) % 3 == 0) {
            cout << "\n";
        }
    }
    cout << "\n";
    return;
}

////////////////////////////////////
//   Point Comparison Functions   //
////////////////////////////////////

// Determines whether a point lies on another line.

bool is_on_same_line(Point p, Point q, Point r) { 
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && 
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y)) {
            return true; 
        }
    return false; 
} 
  


int orientation(Point p, Point q, Point r) { 

    int val = (q.y - p.y) * (r.x - q.x) - 
              (q.x - p.x) * (r.y - q.y); 
    
    // Co-linear
    if (val == 0) return 0;  
    // Non-colinear (1 = clockwise, 2 = counterclock wise) 
    return (val > 0) ? 1 : 2; 
} 

bool do_intersect(Point p1, Point q1, Point p2, Point q2) { 
    // Find the four orientations needed for general and special cases 
    int o1 = orientation(p1, q1, p2); 
    int o2 = orientation(p1, q1, q2); 
    int o3 = orientation(p2, q2, p1); 
    int o4 = orientation(p2, q2, q1); 

    // If the tips (vertices) of a triangle touch, we consider it as NOT
    // crossing. 
    if (o1 == 0 || o2 == 0 || o3 == 0 || o4 == 0) return false;

    // General case 
    if (o1 != o2 && o3 != o4) 
        return true; 
  
    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && is_on_same_line(p1, p2, q1)) return false;  
  
    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && is_on_same_line(p1, q2, q1)) return false; 
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && is_on_same_line(p2, p1, q2)) return false; 
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && is_on_same_line(p2, q1, q2)) return false; 
  
    return false; // Doesn't fall in any of the above cases 
} 

float sign (Point p1, Point p2, Point p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

/*
 * Given a point of interest, pt, find out whether or not it lies in 
 * a triangle with vertices v1, v2, and v3.
 */

bool is_inside_triangle (Point pt, Point v1, Point v2, Point v3) {
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}
/*
 * Given a vector of valid triangles, figure out whether another triangle is contained within it
 */


bool is_triangle_contained_in_another_triangle(vector<Point>& triangle_vertices, Point p, Point q, Point r) { 
    for (int i{}; i < triangle_vertices.size(); i += 3) {
        if (is_inside_triangle(p,triangle_vertices[i],triangle_vertices[i+1],triangle_vertices[i+2]) &&
            is_inside_triangle(q,triangle_vertices[i],triangle_vertices[i+1],triangle_vertices[i+2]) && 
            is_inside_triangle(r,triangle_vertices[i],triangle_vertices[i+1],triangle_vertices[i+2])) return true;
    }
    return false;
}

/*
 * Given a vector of valid triangles and three points, find out whether a point from the vector lies within these three points.
 */

bool triangle_contains_another_triangle(vector<Point>& triangle_vertices, Point p, Point q, Point r) {
    for (int i{}; i < triangle_vertices.size(); i += 3) {
        Point vertex_1(triangle_vertices[i].x,triangle_vertices[i].y);
        Point vertex_2(triangle_vertices[i+1].x, triangle_vertices[i+1].y);
        Point vertex_3(triangle_vertices[i+2].x, triangle_vertices[i+2].y);

        if (is_inside_triangle(vertex_1, p, q, r) &&
            is_inside_triangle(vertex_2, p, q, r) && 
            is_inside_triangle(vertex_3, p, q, r)) return true;
            
    }
    return false;
}


////////////////////////////////////
//   Preprocessing and Solution   //
////////////////////////////////////

/*
     *  Preprocess all the triangles. Get rid of all vertices of valid triangles that overlap.
     *  For instance, it's possible that Triangle #1 has points P1, P2, and P3, which are
     *  a valid set of vertices (valid as in they are on the 17 by 17 board, and contain the
     *  1 by 1 box representing the triangles area within them); but, these vertices cross
     *  with Triangle #2, which has a valid set of vertices that cross Triangle #1s
     *  
     *  THIS CUTS RUNTIME IN HALF.
     */

void pre_process_valid_triangles(vector<Triangle>& board) {
    for (int i{}; i < board.size(); i++) {
        for (int j{}; j < board[i].all_triangles.size(); j += 3) {
            Point p(board[i].all_triangles[j].x, board[i].all_triangles[j].y);
            Point q(board[i].all_triangles[j+1].x, board[i].all_triangles[j+1].y);
            Point r(board[i].all_triangles[j+2].x, board[i].all_triangles[j+2].y);
            for (int k{}; k < board.size(); k++) {
                if (k == i) continue;
                Point a(board[k].getXC(), board[k].getYC());
                Point b(board[k].getXC(), board[k].getYC() + 1);
                Point c(board[k].getXC() + 1, board[k].getYC() + 1);
                Point d(board[k].getXC() + 1, board[k].getYC());

                if (do_intersect(p, q, a, b) || do_intersect(p, q, a, c) || do_intersect(p, q, a, d) ||
                    do_intersect(p, q, b, c) || do_intersect(p, q, b, d) || do_intersect(p, q, c, d) ||
                    do_intersect(p, r, a, b) || do_intersect(p, r, a, c) || do_intersect(p, q, a, d) ||
                    do_intersect(p, r, b, c) || do_intersect(p, r, b, d) || do_intersect(p, r, c, d) ||
                    do_intersect(q, r, a, b) || do_intersect(q, r, a, c) || do_intersect(q, q, a, d) ||
                    do_intersect(q, r, b, c) || do_intersect(q, r, b, d) || do_intersect(q, r, c, d)) {
                    board[i].all_triangles.erase(board[i].all_triangles.begin() + j, board[i].all_triangles.begin() + j + 3);
                    break;
                }
            }
        }
    }
}



void print_solution(const vector<Point>& board) {
    for (int j{}; j < board.size(); j += 3) {
        cout << "Printing Triangle Coordinates: ";
        cout << "(" << board[j].x << "," << board[j].y <<  ") | (";
        cout << board[j+1].x << "," << board[j+1].y << ") | (" ;
        cout << board[j+2].x << "," << board[j+2].y << ") ";
        cout << "\n";
    }
    cout << "\n";
}

void solution(vector<Triangle>& board, int index, vector<Point> solution_vector) {

    // Print the index/triangle I'm operating on for clarity as the program cracks the puzzle.
    cout << (std::string(index, '-')) << index << endl;

    if (index == board.size()) {

        print_solution(solution_vector);
        exit(0);

    }

    for (int i{}; i < board[index].all_triangles.size(); i += 3) {
        Point p = board[index].all_triangles[i];
        Point q = board[index].all_triangles[i+1];
        Point r = board[index].all_triangles[i+2];
        
        bool is_triangle_valid = true;
        for (int j{}; j < solution_vector.size(); j += 3) {
            if (do_intersect(p,q,solution_vector[j],solution_vector[j+1])) {
                    is_triangle_valid = false;
                    break;
            }

            if (do_intersect(p,q,solution_vector[j],solution_vector[j+2])) {
                    is_triangle_valid = false;
                    break;
            }

            if (do_intersect(p,q,solution_vector[j+2],solution_vector[j+1])) {
                    is_triangle_valid = false; 
                    break;
            }

            if (do_intersect(p,r,solution_vector[j],solution_vector[j+1])) {
                    is_triangle_valid = false;
                    break;
            }

            if (do_intersect(p,r,solution_vector[j],solution_vector[j+2])) {
                    is_triangle_valid = false;
                    break;
            }

            if (do_intersect(p,r,solution_vector[j+2],solution_vector[j+1])) {
                    is_triangle_valid = false;
                    break;
            }

            if (do_intersect(q,r,solution_vector[j],solution_vector[j+1])) {
                    is_triangle_valid = false;
                    break;
            }

            if (do_intersect(q,r,solution_vector[j],solution_vector[j+2])) {
                    is_triangle_valid = false;
                    break;
            }

            if (do_intersect(q,r,solution_vector[j+2],solution_vector[j+1])) {
                    is_triangle_valid = false; 
                    break;
            }

        }

        if (is_triangle_valid) {
            
            if(is_triangle_contained_in_another_triangle(solution_vector,p,q,r)) {
                is_triangle_valid = false; // If this triangle is in others
            }

            if(triangle_contains_another_triangle(solution_vector,p,q,r)) {
                is_triangle_valid = false;
            }
        }

        if (is_triangle_valid) {
                solution_vector.insert(solution_vector.end(), {q, p, r});
                solution(board, index + 1, solution_vector);
                solution_vector.erase(solution_vector.end() - 3, solution_vector.end());
        }
    }
    return;  
}

int main() {

    // This is the initial board, as provided in the puzzle.
    // The board contains 29 triangles. 

    vector<Triangle> init_board = { {2,3,0}, {18,7,0}, {12,2,1}, {4,13,1}, {3,4,2}, {7,11,2},
                                     {6,16,2}, {6,0,3}, {9,3,4}, {11,9,4}, {8,14,5}, {4,0,6},
                                     {14,5,6}, {18,15,6}, {20,8,8}, {7,1,10}, {3,11,10},
                                     {3,16,10}, {3,2,11}, {7,7,12}, {10,13,12}, {5,16,13},
                                     {4,0,14}, {10,5,14}, {3,12,14},  {12,3,15}, {7,14,15},
                                     {8,9,16}, {2,13,16} };

    // This vector will hold the answer.
    // Every three points will be the correct vertices of an individual triangle
    vector<Point> acceptable_triangle_vertices;
    
    // Get rid of any valid triangle orientations that intersect with valid orientations
    // before searching for the solution. This cuts runtime in half, as it doesn't need checking over
    // 400 triangles that are valid when viewed in isolation, but intersect with other existing triangles.
    pre_process_valid_triangles(init_board);

    // Run the recursive solution. 
    solution(init_board, 0, acceptable_triangle_vertices);

    return 0;
}