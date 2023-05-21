#include "utility.h"
#include "imageToBitmap.h"
#include <iostream>
#include <fstream>
#include <tuple>

using namespace std;

int main() {
    // Read csv file.
    ifstream fin("mnist_test.csv");
    string str((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    cout << "The file mnist_test.csv was read. It contains " << str.size() << " characters." << endl;
    fin.close();

    // Convert csv to images.
    auto images = imagesFromCsv(str);
    cout << "The csv was converted to " << images.size() << " images. The first image has label " <<
        get<0>(images.front()) << " and " << get<1>(images.front()).size() << " rows of pixels." << endl;

    // Save the n-th image to "img-n-test-label-x.bmp".
    int n = 7;
    int label = get<0>(images.at(n));
    auto bmp_bytes = imageToBmp(grayscaleToColorImage(get<1>(images.at(n))));
    string fn = "img-" + to_string(n) + "-test-label-" + to_string(label) + ".bmp";

    ofstream fout(fn);
    fout.write(bmp_bytes.data(), (int)bmp_bytes.size());
    fout.close();

    cout << "Image " + to_string(n) + " was saved into '" + fn + "'." << endl;
    system(fn.c_str());

    return 0;
}
