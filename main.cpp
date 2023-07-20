#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <sstream>
#include <cmath>

class Graph_M {
public:
    struct Vertex {
        std::unordered_map<std::string, int> nbrs;
    };

    std::unordered_map<std::string, Vertex> vtces;

    Graph_M() {}

    int numVetex() {
        return static_cast<int>(vtces.size());
    }

    bool containsVertex(const std::string& vname) {
        return vtces.find(vname) != vtces.end();
    }

    void addVertex(const std::string& vname) {
        Vertex vtx;
        vtces[vname] = vtx;
    }

    void removeVertex(const std::string& vname) {
        Vertex vtx = vtces[vname];
        std::vector<std::string> keys;
        for (const auto& entry : vtx.nbrs) {
            keys.push_back(entry.first);
        }

        for (const auto& key : keys) {
            Vertex& nbrVtx = vtces[key];
            nbrVtx.nbrs.erase(vname);
        }

        vtces.erase(vname);
    }

    int numEdges() {
        int count = 0;

        for (const auto& entry : vtces) {
            const Vertex& vtx = entry.second;
            count += static_cast<int>(vtx.nbrs.size());
        }

        return count / 2;
    }

    bool containsEdge(const std::string& vname1, const std::string& vname2) {
        auto it1 = vtces.find(vname1);
        auto it2 = vtces.find(vname2);

        if (it1 == vtces.end() || it2 == vtces.end()) {
            return false;
        }

        const Vertex& vtx1 = it1->second;
        return vtx1.nbrs.find(vname2) != vtx1.nbrs.end();
    }

    void addEdge(const std::string& vname1, const std::string& vname2, int value) {
        auto it1 = vtces.find(vname1);
        auto it2 = vtces.find(vname2);

        if (it1 == vtces.end() || it2 == vtces.end() || containsEdge(vname1, vname2)) {
            return;
        }

        Vertex& vtx1 = it1->second;
        Vertex& vtx2 = it2->second;

        vtx1.nbrs[vname2] = value;
        vtx2.nbrs[vname1] = value;
    }

    void removeEdge(const std::string& vname1, const std::string& vname2) {
        auto it1 = vtces.find(vname1);
        auto it2 = vtces.find(vname2);

        if (it1 == vtces.end() || it2 == vtces.end() || !containsEdge(vname1, vname2)) {
            return;
        }

        Vertex& vtx1 = it1->second;
        Vertex& vtx2 = it2->second;

        vtx1.nbrs.erase(vname2);
        vtx2.nbrs.erase(vname1);
    }

    void display_Map() {
        std::cout << "\t Delhi Metro Map\n";
        std::cout << "\t------------------\n";
        for (const auto& entry : vtces) {
            const std::string& key = entry.first;
            const Vertex& vtx = entry.second;

            std::cout << key << " =>\n";
            for (const auto& nbrEntry : vtx.nbrs) {
                const std::string& nbr = nbrEntry.first;
                int value = nbrEntry.second;
                std::cout << "\t" << nbr;
                if (nbr.length() < 16) std::cout << "\t";
                if (nbr.length() < 8) std::cout << "\t";
                std::cout << "\t" << value << "\n";
            }
        }
        std::cout << "\t------------------\n";
    }

    void display_Stations() {
        std::cout << "\n***********************************************************************\n";
        int i = 1;
        for (const auto& entry : vtces) {
            const std::string& key = entry.first;
            std::cout << i << ". " << key << "\n";
            i++;
        }
        std::cout << "\n***********************************************************************\n";
    }

    bool hasPath(const std::string& vname1, const std::string& vname2, std::unordered_map<std::string, bool>& processed) {
        if (containsEdge(vname1, vname2)) {
            return true;
        }

        processed[vname1] = true;

        const Vertex& vtx = vtces[vname1];
        for (const auto& nbrEntry : vtx.nbrs) {
            const std::string& nbr = nbrEntry.first;
            if (!processed[nbr] && hasPath(nbr, vname2, processed)) {
                return true;
            }
        }

        return false;
    }

private:
    struct DijkstraPair {
        std::string vname;
        std::string psf;
        int cost;

        bool operator<(const DijkstraPair& other) const {
            return cost > other.cost; // Using greater than for min-heap
        }
    };

public:
    int dijkstra(const std::string& src, const std::string& des, bool nan) {
        int val = 0;
        std::vector<std::string> ans;
        std::unordered_map<std::string, DijkstraPair> map;
        std::priority_queue<DijkstraPair> heap;

        for (const auto& entry : vtces) {
            const std::string& key = entry.first;
            DijkstraPair np;
            np.vname = key;
            np.cost = std::numeric_limits<int>::max();

            if (key == src) {
                np.cost = 0;
                np.psf = key;
            }

            heap.push(np);
            map[key] = np;
        }

        while (!heap.empty()) {
            DijkstraPair rp = heap.top();
            heap.pop();

            if (rp.vname == des) {
                val = rp.cost;
                break;
            }

            map.erase(rp.vname);
            ans.push_back(rp.vname);

            const Vertex& v = vtces[rp.vname];
            for (const auto& nbrEntry : v.nbrs) {
                const std::string& nbr = nbrEntry.first;
                if (map.find(nbr) != map.end()) {
                    int oc = map[nbr].cost;
                    const Vertex& k = vtces[rp.vname];
                    int nc;
                    if (nan) {
                        nc = rp.cost + 120 + 40 * k.nbrs.at(nbr);
                    } else {
                        nc = rp.cost + k.nbrs.at(nbr);
                    }

                    if (nc < oc) {
                        DijkstraPair& gp = map[nbr];
                        gp.psf = rp.psf + nbr;
                        gp.cost = nc;

                        heap.push(gp);
                    }
                }
            }
        }
        return val;
    }

private:
    struct Pair {
        std::string vname;
        std::string psf;
        int min_dis;
        int min_time;
    };

public:
    std::string Get_Minimum_Distance(const std::string& src, const std::string& dst) {
        int min = std::numeric_limits<int>::max();
        std::string ans;
        std::unordered_map<std::string, bool> processed;
        std::queue<Pair> queue;

        Pair sp;
        sp.vname = src;
        sp.psf = src + "  ";
        sp.min_dis = 0;
        sp.min_time = 0;

        queue.push(sp);

        while (!queue.empty()) {
            Pair rp = queue.front();
            queue.pop();

            if (processed.find(rp.vname) != processed.end()) {
                continue;
            }

            processed[rp.vname] = true;

            if (rp.vname == dst) {
                int temp = rp.min_dis;
                if (temp < min) {
                    ans = rp.psf;
                    min = temp;
                }
                continue;
            }

            const Vertex& rpvtx = vtces[rp.vname];
            for (const auto& nbrEntry : rpvtx.nbrs) {
                const std::string& nbr = nbrEntry.first;
                if (processed.find(nbr) == processed.end()) {
                    Pair np;
                    np.vname = nbr;
                    np.psf = rp.psf + nbr + "  ";
                    np.min_dis = rp.min_dis + rpvtx.nbrs.at(nbr);
                    queue.push(np);
                }
            }
        }

        ans += std::to_string(min);
        return ans;
    }

    std::string Get_Minimum_Time(const std::string& src, const std::string& dst) {
        int min = std::numeric_limits<int>::max();
        std::string ans;
        std::unordered_map<std::string, bool> processed;
        std::queue<Pair> queue;

        Pair sp;
        sp.vname = src;
        sp.psf = src + "  ";
        sp.min_dis = 0;
        sp.min_time = 0;

        queue.push(sp);

        while (!queue.empty()) {
            Pair rp = queue.front();
            queue.pop();

            if (processed.find(rp.vname) != processed.end()) {
                continue;
            }

            processed[rp.vname] = true;

            if (rp.vname == dst) {
                int temp = rp.min_time;
                if (temp < min) {
                    ans = rp.psf;
                    min = temp;
                }
                continue;
            }

            const Vertex& rpvtx = vtces[rp.vname];
            for (const auto& nbrEntry : rpvtx.nbrs) {
                const std::string& nbr = nbrEntry.first;
                if (processed.find(nbr) == processed.end()) {
                    Pair np;
                    np.vname = nbr;
                    np.psf = rp.psf + nbr + "  ";
                    np.min_dis = rp.min_dis + rpvtx.nbrs.at(nbr);
                    np.min_time = rp.min_time + 120 + 40 * rpvtx.nbrs.at(nbr);
                    queue.push(np);
                }
            }
        }

        double minutes = std::ceil(static_cast<double>(min) / 60);
        ans += std::to_string(minutes);
        return ans;
    }

    std::vector<std::string> get_Interchanges(const std::string& str) {
        std::vector<std::string> arr;
        std::stringstream ss(str);
        std::string token;
        while (getline(ss, token, ' ')) {
            arr.push_back(token);
        }

        std::vector<std::string> res;
        for (size_t i = 1; i < arr.size() - 1; ++i) {
            std::string station = arr[i];
            size_t index = station.find('~');
            std::string s = station.substr(index + 1);

            if (s.length() == 2) {
                std::string prev = arr[i - 1].substr(arr[i - 1].find('~') + 1);
                std::string next = arr[i + 1].substr(arr[i + 1].find('~') + 1);

                if (prev == next) {
                    res.push_back(station);
                } else {
                    res.push_back(station + " ==> " + arr[i + 1]);
                    i++;
                }
            } else {
                res.push_back(station);
            }
        }

        res.push_back(arr.back());
        res.push_back(std::to_string(res.size() - 2));
        res.push_back(arr.front());

        return res;
    }

    static Graph_M Create_Metro_Map() {
        Graph_M g;
        g.addVertex("Noida Sector 62~B");
        g.addVertex("Botanical Garden~B");
        g.addVertex("Yamuna Bank~B");
        g.addVertex("Rajiv Chowk~BY");
        g.addVertex("Vaishali~B");
        g.addVertex("Moti Nagar~B");
        g.addVertex("Janak Puri West~BO");
        g.addVertex("Dwarka Sector 21~B");
        g.addVertex("Huda City Center~Y");
        g.addVertex("Saket~Y");
        g.addVertex("Vishwavidyalaya~Y");
        g.addVertex("Chandni Chowk~Y");
        g.addVertex("New Delhi~YO");
        g.addVertex("AIIMS~Y");
        g.addVertex("Shivaji Stadium~O");
        g.addVertex("DDS Campus~O");
        g.addVertex("IGI Airport~O");
        g.addVertex("Rajouri Garden~BP");
        g.addVertex("Netaji Subhash Place~PR");
        g.addVertex("Punjabi Bagh West~P");

        g.addEdge("Noida Sector 62~B", "Botanical Garden~B", 8);
        g.addEdge("Botanical Garden~B", "Yamuna Bank~B", 10);
        g.addEdge("Yamuna Bank~B", "Vaishali~B", 8);
        g.addEdge("Yamuna Bank~B", "Rajiv Chowk~BY", 6);
        g.addEdge("Rajiv Chowk~BY", "Moti Nagar~B", 9);
        g.addEdge("Moti Nagar~B", "Janak Puri West~BO", 7);
        g.addEdge("Janak Puri West~BO", "Dwarka Sector 21~B", 6);
        g.addEdge("Huda City Center~Y", "Saket~Y", 15);
        g.addEdge("Saket~Y", "AIIMS~Y", 6);
        g.addEdge("AIIMS~Y", "Rajiv Chowk~BY", 7);
        g.addEdge("Rajiv Chowk~BY", "New Delhi~YO", 1);
        g.addEdge("New Delhi~YO", "Chandni Chowk~Y", 2);
        g.addEdge("Chandni Chowk~Y", "Vishwavidyalaya~Y", 5);
        g.addEdge("New Delhi~YO", "Shivaji Stadium~O", 2);
        g.addEdge("Shivaji Stadium~O", "DDS Campus~O", 7);
        g.addEdge("DDS Campus~O", "IGI Airport~O", 8);
        g.addEdge("Moti Nagar~B", "Rajouri Garden~BP", 2);
        g.addEdge("Punjabi Bagh West~P", "Rajouri Garden~BP", 2);
        g.addEdge("Punjabi Bagh West~P", "Netaji Subhash Place~PR", 3);

        return g;
    }
};

int main() {
    Graph_M g = Graph_M::Create_Metro_Map();

    std::cout << "\n\t\t\t****WELCOME TO THE METRO APP*****\n";

    while (true) {
        std::cout << "\n\t\t\t\t~~LIST OF ACTIONS~~\n\n";
        std::cout << "1. LIST ALL THE STATIONS IN THE MAP\n";
        std::cout << "2. SHOW THE METRO MAP\n";
        std::cout << "3. GET SHORTEST DISTANCE FROM A 'SOURCE' STATION TO 'DESTINATION' STATION\n";
        std::cout << "4. GET SHORTEST TIME TO REACH FROM A 'SOURCE' STATION TO 'DESTINATION' STATION\n";
        std::cout << "5. GET INTERCHANGES OF A ROUTE\n";
        std::cout << "6. EXIT\n\n";
        std::cout << "ENTER THE NUMBER OF ACTION YOU WANT TO PERFORM: ";

        int action;
        std::cin >> action;

        switch (action) {
            case 1: {
                g.display_Stations();
                break;
            }
            case 2: {
                g.display_Map();
                break;
            }
            case 3: {
                std::cout << "\n\t\t****FIND THE SHORTEST DISTANCE****\n";
                std::string source, destination;
                std::cout << "Enter the 'SOURCE' Station: ";
                std::cin.ignore();
                std::getline(std::cin, source);
                std::cout << "Enter the 'DESTINATION' Station: ";
                std::getline(std::cin, destination);

                if (g.containsVertex(source) && g.containsVertex(destination)) {
                    std::unordered_map<std::string, bool> processed;
                    bool val = g.hasPath(source, destination, processed);
                    if (val) {
                        int distance = g.dijkstra(source, destination, false);
                        std::cout << "\n\tThe Shortest Distance between " << source << " and " << destination << " is " << distance << " km\n";
                    } else {
                        std::cout << "\n\tSorry! No Direct Route Found between " << source << " and " << destination << "\n";
                    }
                } else {
                    std::cout << "\n\t\tPlease Enter Valid Source and Destination Stations\n";
                }
                break;
            }
            case 4: {
                std::cout << "\n\t\t****FIND THE SHORTEST TIME****\n";
                std::string source, destination;
                std::cout << "Enter the 'SOURCE' Station: ";
                std::cin.ignore();
                std::getline(std::cin, source);
                std::cout << "Enter the 'DESTINATION' Station: ";
                std::getline(std::cin, destination);

                if (g.containsVertex(source) && g.containsVertex(destination)) {
                    std::unordered_map<std::string, bool> processed;
                    bool val = g.hasPath(source, destination, processed);
                    if (val) {
                        std::string time = g.Get_Minimum_Time(source, destination);
                        std::cout << "\n\tThe Minimum Time to reach " << source << " to " << destination << " is " << time << " minutes\n";
                    } else {
                        std::cout << "\n\tSorry! No Direct Route Found between " << source << " and " << destination << "\n";
                    }
                } else {
                    std::cout << "\n\t\tPlease Enter Valid Source and Destination Stations\n";
                }
                break;
            }
            case 5: {
                std::cout << "\n\t\t****GET INTERCHANGES OF A ROUTE****\n";
                std::string route;
                std::cout << "Enter the Stations separated by space: ";
                std::cin.ignore();
                std::getline(std::cin, route);

                std::vector<std::string> interchanges = g.get_Interchanges(route);
                std::cout << "\n\tInterchanges:\n\n";
                for (const std::string& str : interchanges) {
                    std::cout << str << "\n";
                }
                break;
            }
            case 6: {
                std::cout << "\n\t\t\t****THANK YOU FOR USING THE METRO APP*****\n";
                return 0;
            }
            default: {
                std::cout << "\n\t\tPlease Enter a Valid Action Number\n";
                break;
            }
        }
    }

    return 0;
}
