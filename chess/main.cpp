#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

struct Point {
    int row, column;
};

struct Pair {
    Point* p1;
    Point* p2;
};

bool operator==(const Point& p1, const Point& p2) {
    return (p1.row == p2.row && p1.column == p2.column);
}

bool operator==(const Pair& p1, const Pair& p2) {
    return (*p1.p1 == *p2.p1 && *p1.p2 == *p2.p2) ||
            (*p1.p2 == *p2.p1 && *p1.p1 == *p2.p2);
}

Point* read_point(istream& f) {
    char col, row;
    f >> col >> row;
    if(f.fail()) {
        return nullptr;
    }
    auto* p = new Point;
    p->column = col - 'A';
    p->row = row - '1';
    return p;
}

bool is_overlapping(int col1, int row1, int col2, int row2) {
    return (col1 == col2) ||
            (row1 == row2) ||
            (col1-col2 == row1-row2) ||
            (col2-col1 == row1-row2);
}

bool is_valid_point(const Point* p) {
    return p->column >= 0 && p->column < 8 && p->row >= 0 && p->row < 8;
}

bool write_pair(ostream& f, Pair p) {
    f << (char)(p.p1->column + 'A') << p.p1->row+1 << "-" << (char)(p.p2->column + 'A') << p.p2->row+1 << endl;
    return f.fail();
}

vector<Point*> find_overlaps(const vector<Point*>& v1, const vector<Point*>& v2) {
    vector<Point*> r;
    for(auto point : v1) {
        for(auto point2 : v2) {
            if(point == point2) {
                r.push_back(point);
                break;
            }
        }
    }
    return r;
}

bool blocks(Point* p1, Point *p2, Point *p3) {
    if(p1->column == p2->column && p1->column == p3->column) {
        if(p1->row < p2->row) {
            return p3->row > p1->row && p3->row < p2->row;
        } else {
            return p3->row < p1->row && p3->row > p2->row;
        }
    } else if(p1->row == p2->row && p1->row == p3->row) {
        if(p1->column < p2->column) {
            return p3->column > p1->column && p3->column < p2->column;
        } else {
            return p3->column < p1->column && p3->column > p2->column;
        }
    } else if((p1->column - p2->column == p1->row - p2->row || p2->column - p1->column == p1->row - p2->row) &&
            (p1->column - p3->column == p1->row - p3->row || p3->column - p1->column == p1->row - p3->row)) {
        if(p1->column < p2->column) {
            return p3->column > p1->column && p3->column < p2->column;
        } else {
            return p3->column < p1->column && p3->column > p2->column;
        }
    }
    return false;
}

bool contains(const vector<Pair>& p, Pair pr) {
    for(auto v : p) {
        if(v == pr) {
            return true;
        }
    }
    return false;
}

vector<Point> read_points(istream &in) {
    vector<Point> points;
    Point *p;
    for (p = read_point(in); p != nullptr; p = read_point(in)) {
        if(!is_valid_point(p)) {
            cout << "Invalid point at position " << points.size() + 1 << endl;
            exit(3);
        }
        points.push_back(*p);
        delete p;
    }
    return points;
}

map<Point*, vector<Point*>> find_overlaps(vector<Point> &points) {
    map<Point*, vector<Point*>> overlaps;
    for(int i = 0; i < points.size(); i++) {
        for(int j = 0; j < points.size(); j++) {
            if(i == j)
                continue;
            if(is_overlapping(points[i].column, points[i].row, points[j].column, points[j].row)) {
                overlaps[&points[i]].push_back(&points[j]);
                overlaps[&points[j]].push_back(&points[i]);
            }
        }
    }
    return overlaps;
}

bool check_blocks(Point* p1, Point* p2, vector<Point*>& o) {
    for(auto p : o) {
        if(blocks(p1, p2, p)) {
            return false;
        }
    }
    return true;
}

void check_and_write_pair(ostream& out, Point* p1, Point* p2, vector<Pair> &written) {
    auto p = Pair{p1, p2};
    bool r = write_pair(out, p);
    if(r) {
        cout << "Output error" << endl;
        exit(3);
    }
    written.push_back(p);
}

int main() {
    ifstream in("input.txt");
    if(!in.is_open()) {
        cout << "Cannot open input.txt" << endl;
        return 1;
    }
    ofstream out("output.txt");
    if(!out.is_open()) {
        cout << "Cannot open output.txt" << endl;
        return 1;
    }

    vector<Point> points = read_points(in);
    if(points.empty()) {
        cout << "No points were found" << endl;
        return 2;
    }

    map<Point*, vector<Point*>> overlaps = find_overlaps(points);
    vector<Pair> written;
    for(const auto& kv : overlaps) {
        for(auto v : kv.second) {
            if(contains(written, Pair{kv.first, v})) {
                continue;
            }
            auto o = find_overlaps(kv.second, overlaps[v]);
            if(o.empty()) {
                check_and_write_pair(out, kv.first, v, written);
                continue;
            }
            bool ok = check_blocks(kv.first, v, o);
            if(!ok) {
                continue;
            }
            check_and_write_pair(out, kv.first, v, written);
        }
    }

    return 0;
}
