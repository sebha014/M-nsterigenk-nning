/*
 * fast < input.txt
 *
 * Compute and plot all line segments involving 4 points in input.txt
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <cmath>
#include <utility>
#include "point.h"
#include "window.h"

using namespace std;

int main(int argc, const char* argv[]) {
    WindowPtr window = create_window(argc, argv);
    if (!window)
        return 1;

    // The array of points
    vector<Point> points;

    // Read tolerance from cin
    double tolerance{};
    cin >> tolerance;

    // Read points from cin
    int N{};
    cin >> N;

    for (int i{0}; i < N; ++i) {
        double x{}, y{};
        cin >> x >> y;
        points.push_back(Point{x, y});
    }

    // draw points to screen all at once
    window->draw_points(points);

    auto begin = chrono::high_resolution_clock::now();

    //////////////////////////////////////////////////////////////////////////////
    // Draw any lines that you find in 'points' using the function 'window->draw_line'.
    //////////////////////////////////////////////////////////////////////////////

    ///// NY KOD: Vinkelkonstant för att behandla samma linje oberoende av riktning.
    const double pi = acos(-1.0);

    ///// NY KOD: Gör om vinkeln till intervallet [0, pi).
    auto lineAngle = [pi](double angle) {
        angle = fmod(angle, pi);
        if (angle < 0) {
            angle += pi;
        }
        return angle;
    };

    ///// NY KOD: Jämför vinklar med tolerans, även nära gränsen 0 och pi.
    auto sameLineAngle = [pi, tolerance](double a, double b) {
        double difference = abs(a - b);

        return sameSlope(a, b, tolerance) ||
               abs(difference - pi) <= tolerance;
    };

    ///// NY KOD: Välj varje punkt som utgångspunkt.
    for (int o{0}; o < N; ++o) {
        Point origin = points[o];

        ///// NY KOD: Sortera de andra punkterna efter linjevinkeln från origin.
        vector<pair<double, Point>> sorted_points;

        for (int p{0}; p < N; ++p) {
            if (p != o) {
                double angle = lineAngle(origin.slopeTo(points[p]));
                sorted_points.push_back({angle, points[p]});
            }
        }

        sort(sorted_points.begin(), sorted_points.end(),
             [](const auto& a, const auto& b) {
                 return a.first < b.first;
             });

        ///// NY KOD: Hitta grupper med liknande vinklar.
        vector<vector<Point>> groups;
        int i{0};
        int size = static_cast<int>(sorted_points.size());

        while (i < size) {
            int j{i + 1};

            while (j < size &&
                   sameLineAngle(sorted_points[i].first,
                                 sorted_points[j].first)) {
                ++j;
            }

            vector<Point> group;

            for (int k{i}; k < j; ++k) {
                group.push_back(sorted_points[k].second);
            }

            groups.push_back(group);
            i = j;
        }

        ///// NY KOD: Slå ihop grupper som ligger nära gränsen 0 och pi.
        if (groups.size() > 1 &&
            sameLineAngle(sorted_points.front().first,
                          sorted_points.back().first)) {
            groups.front().insert(groups.front().end(),
                                  groups.back().begin(),
                                  groups.back().end());
            groups.pop_back();
        }

        ///// NY KOD: Minst tre andra punkter krävs för ett linjesegment.
        for (const auto& group : groups) {
            if (group.size() >= 3) {
                vector<Point> line_points = group;
                line_points.push_back(origin);

                ///// NY KOD: Sortera punkterna för att hitta den minsta.
                sort(line_points.begin(), line_points.end());

                ///// NY KOD: Rita bara när origin är linjens minsta punkt.
                if (line_points.front().x == origin.x &&
                    line_points.front().y == origin.y) {
                    window->draw_line(line_points);
                }
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    cout << "Computing line segments took "
         << std::chrono::duration_cast<chrono::milliseconds>(end - begin).count()
         << " milliseconds." << endl;

    // wait for user to terminate program
    window->run();

    return 0;
}

