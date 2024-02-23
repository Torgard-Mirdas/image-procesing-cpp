#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstring>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"


using namespace std;
/*
zastosowuje progowanie dla każdego kanału obrazu.

ta funkcja przetwarza dane obrazu, ustawiając wartości pikseli na 255,
jeśli są większe niż określony próg, a w przeciwnym razie na 0, dla każdego kanału.
imageData Wskaźnik do tablicy danych obrazu (założenie: kanały są ułożone naprzemiennie).
threshold Wartość progu do zastosowania dla każdego kanału.
width Szerokość obrazu.
height Wysokość obrazu.
channels Liczba kanałów w obrazie 
*/
void thresholdImage(unsigned char* imageData, int threshold, int width, int height, int channels) {
    // iteracja przez każdy piksel
    for (int i = 0; i < width * height * channels; i += channels) {
        // iteracja przez każdy kanał aktualnego piksela
        for (int j = 0; j < channels; ++j) {
            // zastosowanie progu dla każdego kanału niezależnie
            imageData[i + j] = (imageData[i + j] > threshold) ? 255 : 0;
        }
    }
}

/*
generuje obraz histogramu na podstawie danych histogramu i zapisuje go do pliku.

obraz zawiera słupki reprezentujące częstość występowania poszczególnych wartości pikseli (tzn naprzykład w kanale green ile razy pojawiła sie wartość 0 ile razy wartosc 1 itp)
histogram to wektor zawierający dane histogramu.
outputPath Ścieżka do pliku, do którego zostanie zapisany obraz histogramu.
*/
void generateHistogramImage(const vector<int>& histogram, const string& outputPath) {
    // znalezienie maksymalnej wartości w histogramie
    int maxCount = *max_element(histogram.begin(), histogram.end());

    // ustalenie wymiarów obrazu histogramu
    int histWidth = 256 * 2;
    int histHeight = 256; // domyślna wysokość obrazu histogramu

    // alokacja pamięci dla danych obrazu histogramu
    unsigned char* histImageData = new unsigned char[histWidth * histHeight * 3];

    // ustawienie wszystkich pikseli na biały kolor (tło)
    for (int i = 0; i < histWidth * histHeight * 3; ++i) {
        histImageData[i] = 255;
    }

    // rysowanie słupków histogramu
    // pętla iterująca przez poziomy intensywności pikseli w zakresie od 0 do 255 (wartości histogramu)
    for (int i = 0; i < 256; ++i) {
    // Oblicz wartość 'histValue' dla aktualnej intensywności pikseli
    int histValue = static_cast<int>((static_cast<float>(histogram[i]) / maxCount) * histHeight);
    // pętla iterująca po wysokości kolumny histogramu
    // rozpoczyna się od dolnej krawędzi i sięga do wartości 'histValue'
        for (int j = 0; j <= histValue; ++j) {
            // ustawia czerwoną składową piksela na 0 (czyli czarny, faktycznie rysuje śłupki)
            histImageData[((histHeight - 1 - j) * histWidth + i * 2) * 3] = 0;
            histImageData[((histHeight - 1 - j) * histWidth + i * 2) * 3 + 1] = 0;
            histImageData[((histHeight - 1 - j) * histWidth + i * 2) * 3 + 2] = 0;
        }
    }
    // zapisanie obrazu histogramu do pliku png
    if (!stbi_write_png(outputPath.c_str(), histWidth, histHeight, 3, histImageData, histWidth * 3)) {
        cerr << "błąd zapisu obrazu histogramu" << endl;
    }

    // zwolnienie pamięci zaalokowanej dla danych obrazu histogramu
    delete[] histImageData;

}

/*
funkcja oblicza histogram dla danego obrazu o podanych parametrach (szerokość, wysokość, liczba kanałów) i zapisuje go do pliku png

image Wskaźnik do danych obrazu.
width Szerokość obrazu.
height Wysokość obrazu.
channels Liczba kanałów obrazu.
histogram wektor przechowujący wartości histogramu.
channelName Nazwa kanału, dla którego jest tworzony histogram.
 */
void calculateHistogram(const unsigned char* image, int width, int height, int channels, vector<int>& histogram, const string& channelName) {
    int histSize = 256;  // liczba przedziałów histogramu

    // inicjalizacja histogramu
    histogram.resize(histSize, 0);

    // obliczanie histogramu
    for (int i = 0; i < width * height * channels; i += channels) {
        histogram[image[i]]++;
    }

    // generowanie osobnego obrazu histogramu dla każdego kanału
    generateHistogramImage(histogram, "histogram_" + channelName + ".png");
}

/*
funkcja skaluje wejściowy obraz na podstawie podanych wymiarów wyjściowych i liczby kanałów, a następnie zapisuje przeskalowany obraz do pliku o podanej ścieżce.
inputData Wskaźnik do danych wejściowego obrazu.
inputWidth Szerokość wejściowego obrazu.
inputHeight Wysokość wejściowego obrazu.
channels Liczba kanałów wejściowego obrazu.
outputWidth Szerokość docelowego obrazu (po skalowaniu).
outputHeight Wysokość docelowego obrazu (po skalowaniu).
outputFilePath Ścieżka do pliku, do którego zostanie zapisany przeskalowany obraz.
*/
void scaleImage(const unsigned char* inputData, int inputWidth, int inputHeight, int channels,int outputWidth, int outputHeight, const string& outputFilePath) {
    // alokacja pamięci dla danych przeskalowanego obrazu
    unsigned char* scaledImageData = new unsigned char[outputWidth * outputHeight * channels];

    // skalowanie obrazu za pomocą biblioteki STB Image Resize
    stbir_resize_uint8(inputData, inputWidth, inputHeight, 0, scaledImageData, outputWidth, outputHeight, 0, channels);
//0 to są flagi wejściowe oraz wjściowe które nie są tu używane
    // zapisanie przeskalowanego obrazu do pliku PNG
    if (!stbi_write_png(outputFilePath.c_str(), outputWidth, outputHeight, channels, scaledImageData, outputWidth * channels)) {
        cerr << "Błąd zapisu przeskalowanego obrazu" << endl;
    }

    // zwolnienie zaalokowanej pamięci dla danych przeskalowanego obrazu
    delete[] scaledImageData;
}


void applyRedFilter(unsigned char* imageData, int width, int height, int channels, const string& outputFileName) {
    for (int i = 0; i < width * height * channels; i += channels) {
        // kanał czerwny ma index 0, dodanie wartości do kanału czerwonego
        imageData[i] = min(255, imageData[i] + 50);
    }

    if (!stbi_write_png(outputFileName.c_str(), width, height, channels, imageData, width * channels)) {
        cerr << "błąd zapisu zdjęcia" << endl;
    }
}


void applyGreenFilter(unsigned char* imageData, int width, int height, int channels, const string& outputFileName) {
    for (int i = 0; i < width * height * channels; i += channels) {
        //kanał zielony ma index 1
        imageData[i + 1] = min(255, imageData[i + 1] + 50);
    }
 
    if (!stbi_write_png(outputFileName.c_str(), width, height, channels, imageData, width * channels)) {
        cerr << "błąd zapisu zdjęcia" << endl;
    }
}

void applyBlueFilter(unsigned char* imageData, int width, int height, int channels, const string& outputFileName) {
    for (int i = 0; i < width * height * channels; i += channels) {
        // kanał niebieski ma index 2
        imageData[i + 2] = min(255, imageData[i + 2] + 50);
    }

    if (!stbi_write_png(outputFileName.c_str(), width, height, channels, imageData, width * channels)) {
        cerr << "błąd zapisu zdjęcia" << endl;
    }
}


int main() {
    string inputFilePath;
    string outputFilePath;
    int choice;
    int width, height, channels;
    cout << "wprowadź nazwe zdjęcia, które chcesz poddać obróbce\nwpisz je razem z rozszerzeniem naprzukład obrazek.jpg :";
    cin >> inputFilePath;
    //wczytanie danych zdjęcia    
    unsigned char* originalImageData = stbi_load(inputFilePath.c_str(), &width, &height, &channels, 0);

   if (originalImageData == nullptr) {
    cerr << "błąd poczas wczytania zdjęcia, podaj prawidłową nazwe" << endl;
    do {
        cout << "wprowadź nazwe zdjęcia, które chcesz poddać obróbce\nwpisz je razem z rozszerzeniem na przukład obrazek.jpg :";
        cin >> inputFilePath;

        originalImageData = stbi_load(inputFilePath.c_str(), &width, &height, &channels, 0);

        if (originalImageData == nullptr) {
            cerr << "błąd poczas wczytania zdjęcia, podaj prawidłową nazwe" << endl;
        } 
    } while (originalImageData == NULL);
    
}
cout << "wprowadź nazwe pliku wyjściowego (wraz z rozszerzeniem): ";
cin >> outputFilePath;
// kopia danych zdjęcia
unsigned char* imageData = new unsigned char[width * height * channels];
memcpy(imageData, originalImageData, width * height * channels);
//ważna informacja: dane obrazka przechowywane są w 1 wymiarowej tabeli z ułożonymi po kolei wartościami rgb kolejnych pikseli

    do {
        cout << "Menu:\n";
        cout << "1. zmień rozmiar\n";
        cout << "2. histogram\n";
        cout << "3. progowanie\n";
        cout << "4. filtr czerwonego\n";
        cout << "5. filtr zielonego\n";
        cout << "6. filtr niebieskiego\n";
        cout << "7. zmień nazwe pliku wejściowego\n";
        cout << "8. zmień nazwe pliku wyjściowego\n";
        cout << "0. wyjdź z programu\n";
        cout << "wprowadź swój wybór (liczba od 0-9 po czym wciśnij enter): ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int outputWidth, outputHeight;
                //pobierz dane od użytkownika
                cout << "wprowadź szerokość zdjęcia: ";
                cin >> outputWidth;
                cout << "wprowadź wysokość zdjęcia: ";
                cin >> outputHeight;
                // wyślij dane do funkcji która przeskaluje obrazek
                scaleImage(originalImageData, width, height, channels, outputWidth, outputHeight, inputFilePath+"-scaled_image.png");

                cout << "skalowanie zakończone pomyślnie" << endl;
                cout<< "nazwa pliku wyjściowego: " + inputFilePath+"- scaled_image.png";
                break;
            }
            case 2: {
                vector<int> histogramRed, histogramGreen, histogramBlue;
                calculateHistogram(originalImageData, width, height, channels, histogramRed, "red");
                calculateHistogram(originalImageData + 1, width, height, channels, histogramGreen, "green");
                calculateHistogram(originalImageData + 2, width, height, channels, histogramBlue, "blue");
                break;
            }
            case 3: {
                //wartosć graniczna progowania
                int threshold = 128;
                thresholdImage(originalImageData, threshold, width, height, channels);

                if (!stbi_write_png(outputFilePath.c_str(), width, height, channels, originalImageData, width * channels)) {
                    cerr << "błąd w zapiscie obrazu" << endl;
                    return 1;
                }
                cout << "Progowanie zakończone sukcesem" << endl;
                cout << "nazwa pliku wyjściowego: " + outputFilePath + "\n";
                break;
            }
            case 4: {
                string outputFileName = outputFilePath + "-red-filter.png";
                applyRedFilter(imageData, width, height, channels, outputFileName);
                cout << "zdjęcie z czerwonym filtrem zapisane jako: " << outputFileName << endl;
                break;
            }
            case 5: {
                string outputFileName = outputFilePath + "-green-filter.png";
                applyGreenFilter(imageData, width, height, channels, outputFileName);
                cout << "zdjęcie z czerwonym filtrem zapisane jako: " << outputFileName << endl;
                break;
            }
            case 6: {
                string outputFileName = outputFilePath + "-blue-filter.png";
                applyBlueFilter(imageData, width, height, channels, outputFileName);
                cout << "zdjęcie z czerwonym filtrem zapisane jako: " << outputFileName << endl;
                break;
            }
            case 7: { 
                cout << "wprowadź nazwe zdjęcia, które chcesz poddać obróbce\nwpisz je razem z rozszerzeniem na przukład obrazek.jpg :";
                cin >> inputFilePath;

                // sprawdź poprawność pliku wejściowego
                stbi_image_free(originalImageData); // Zwolnij pamięć przed ponownym wczytaniem
                originalImageData = stbi_load(inputFilePath.c_str(), &width, &height, &channels, 0);

                if (originalImageData == nullptr) {
                    cerr << "Nastąpił błąd, wprowadź poprawną nazwe pliku" << endl;
                    // jeżeli wczytanie nie powiodło się, powróć do menu
                    continue;
                }

                // Utwórz kopię dla nowego pliku wejściowego
                delete[] imageData;
                imageData = new unsigned char[width * height * channels];
                memcpy(imageData, originalImageData, width * height * channels);

                cout << "zmiana nazwy pliku wejściowego zmieniona pomyślnie" << endl;
                break;
            }

            case 8: { 
                cout << "wprowadź nową nazwe pliku wyjściowego (wraz z rozszerzeniem): ";
                cin >> outputFilePath;

                // sprawdź poprawność pliku wyjściowego
                if (outputFilePath.empty()) {
                    cerr << "Invalid output file name." << endl;
                    // jeżeli nazwa pliku wyjściowego jest pusta, powróć do menu
                    continue;
                }

                cout << "zmiana nazwy pliku wyjsciowego została zastosowana pomyślnie" << endl;
                break;
            }
            case 0:
                cout << "wyjście z programu" << endl;
                break;
            default:
                cerr << "błąd" << endl;
        }

    } while (choice != 0);

    stbi_image_free(originalImageData);
    delete[] imageData;

    return 0;
}
