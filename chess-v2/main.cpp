#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

struct Point {
    int x, y;
};

bool operator==(const Point& l, const Point& r) {
    return l.x == r.x && l.y == r.y;
}

bool operator==(const pair<Point, Point>& l, const pair<Point, Point>& r) {
    return (l.first == r.first && l.second == r.second) ||
            (l.first == r.second && l.second == r.first);
}

bool operator<(const Point& l, const Point& r) {
    return l.x < r.x || l.y < r.y;
}

bool read_point(istream& in, Point* p) {
    char x, y;
    in >> x >> y;
    if(in.fail()) {
        return false;
    }
    if(x < 'A' || x > 'H') {
        return false;
    }
    if(y < '1' || y > '9') {
        return false;
    }
    p->x = x - 'A' + 1;
    p->y = y - '1' + 1;
    return true;
}

bool threaten_x(const Point& p1, const Point& p2) {
    return p1.x == p2.x;
}

bool threaten_y(const Point& p1, const Point& p2) {
    return p1.y == p2.y;
}

bool threaten_diag(const Point& p1, const Point& p2) {
    return abs(p1.x - p2.x) == abs(p1.y - p2.y);
}

bool threatens(const Point& p1, const Point& p2) {
    return threaten_x(p1, p2) ||
        threaten_y(p1, p2) ||
        threaten_diag(p1, p2);
}

bool blocks_x(const Point& p1, const Point& p2, const Point& p3) {
    return threaten_x(p1, p2) && threaten_x(p1, p3) && p3.y > min(p1.y, p2.y) && p3.y < max(p2.y, p1.y);
}

bool blocks_y(const Point& p1, const Point& p2, const Point& p3) {
    return threaten_y(p1, p2) && threaten_y(p1, p3) && p3.x > min(p1.x, p2.x) && p3.x < max(p2.x, p1.x);
}

bool blocks_diag(const Point& p1, const Point& p2, const Point& p3) {
    return threaten_diag(p1, p2) && threaten_diag(p1, p3) && p3.x > min(p1.x, p2.x) && p3.x < max(p2.x, p1.x);
}

bool blocks_threaten(const Point& p1, const Point& p2, const Point& p3) {
    return blocks_x(p1, p2, p3) || blocks_y(p1, p2, p3) || blocks_diag(p1, p2, p3);
}

map<Point, vector<Point>> collect_threatening(vector<Point>& points) {
    map<Point, vector<Point>> threatening;
    for(int i = 0; i < points.size(); i++) {
        for(int j = 0; j < points.size(); j++) {
            if(i == j) continue;
            if(threatens(points[i], points[j]))
                threatening[points[i]].push_back(points[j]);
        }
    }
    return threatening;
}

vector<Point> find_overlaps(const vector<Point>& v1, const vector<Point>& v2) {
    vector<Point> overlaps;
    for(auto p1 : v1) {
        for (auto p2 : v2) {
            if (p1 == p2) {
                overlaps.push_back(p1);
                break;
            }
        }
    }
    return overlaps;
}

bool contains(const vector<pair<Point, Point>>& written, const pair<Point, Point>& p) {
    return any_of(written.cbegin(), written.cend(), [&p](const pair<Point, Point>& v) { return v == p; });
}

bool check_and_write(ostream& out, pair<Point, Point> p, vector<pair<Point, Point>>& written) {
    if(!contains(written, p)) {
        out << (char)(p.first.x - 1 + 'A') <<
            (char)(p.first.y - 1 + '1') <<
            "-" <<
            (char)(p.second.x - 1 + 'A') <<
            (char)(p.second.y - 1 + '1') << endl;
        written.push_back(p);
        return out.fail();
    }
    return true;
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
    vector<Point> points;
    Point p{};
    while(read_point(in, &p)) {
        points.push_back(p);
    }
    if(points.empty()) {
        cout << "No valid points were found" << endl;
        return 2;
    }
    
    auto threatening = collect_threatening(points);
    vector<pair<Point, Point>> written;
    for(auto& threatening_pairs : threatening) {
        for(auto& point : threatening_pairs.second) {
            if(contains(written, pair<Point, Point>(threatening_pairs.first, point)))
                continue;

            auto overlaps = find_overlaps(threatening_pairs.second, threatening[point]);
            if(!any_of(overlaps.cbegin(), overlaps.cend(), [&threatening_pairs, &point](const Point& p) {
                return blocks_threaten(threatening_pairs.first, point, p);
            })) {
                check_and_write(out, pair<Point, Point>(threatening_pairs.first, point), written);
            }
        }
    }
}
