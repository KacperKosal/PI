/**
 * @file main.cpp
 * @brief Program oblicza przybliżoną wartość liczby PI metodą całkowania numerycznego
 *        z wykorzystaniem wielowątkowości.
 *
 * Program dzieli obliczenia na zadaną liczbę wątków i porównuje czas wykonywania
 * dla różnych ilości wątków, wypisując wyniki w terminalu.
 */

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <chrono>
#include <mutex>

 /// Mutex do bezpiecznego wypisywania w konsoli
std::mutex console_mutex;

/**
 * @brief Funkcja obliczająca sumę częściową całki w danym zakresie.
 *
 * @param poczatek Początek zakresu całkowania.
 * @param koniec Koniec zakresu całkowania.
 * @param krok Długość kroku całkowania.
 * @param wynik Zmienna, do której zapisany zostanie wynik obliczeń.
 */
void oblicz_sume_czesciowa(double poczatek, double koniec, double krok, double& wynik) {
    double suma_czesciowa = 0.0;
    for (double x = poczatek; x < koniec; x += krok) {
        suma_czesciowa += 4.0 / (1.0 + x * x) * krok;
    }
    wynik = suma_czesciowa;
}

/**
 * @brief Główna funkcja programu.
 *
 * Pobiera od użytkownika liczbę przedziałów całkowania, a następnie wykonuje obliczenia
 * przybliżonej wartości liczby PI dla różnych ilości wątków (od 1 do 50).
 * Wyniki oraz czas obliczeń dla każdego testu są wypisywane w terminalu.
 *
 * @return Kod zakończenia programu (0 oznacza sukces).
 */
int main() {
    // Zmienne dla granic całkowania i liczby przedziałów
    const double dolna_granica = 0.0; ///< Dolna granica całkowania
    const double gorna_granica = 1.0; ///< Górna granica całkowania

    size_t liczba_przedzialow; ///< Liczba przedziałów całkowania

    // Pobieranie liczby przedziałów od użytkownika
    std::cout << "Podaj liczbę przedziałów: ";
    std::cin >> liczba_przedzialow;

    // Obliczanie długości kroku
    double krok = (gorna_granica - dolna_granica) / liczba_przedzialow;

    for (size_t liczba_watkow = 1; liczba_watkow <= 50; ++liczba_watkow) {
        // Wektor do przechowywania wyników cząstkowych z wątków
        std::vector<double> wyniki_czesciowe(liczba_watkow, 0.0);

        // Podział pracy między wątki
        std::vector<std::thread> watki;
        double zakres_na_watek = (gorna_granica - dolna_granica) / liczba_watkow;

        auto czas_start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < liczba_watkow; ++i) {
            double start_watku = dolna_granica + i * zakres_na_watek;
            double koniec_watku = start_watku + zakres_na_watek;

            watki.emplace_back(oblicz_sume_czesciowa, start_watku, koniec_watku, krok, std::ref(wyniki_czesciowe[i]));
        }

        // Czekanie na zakończenie pracy wszystkich wątków
        for (auto& w : watki) {
            w.join();
        }

        // Agregacja wyników
        double przyblizona_pi = 0.0;
        for (const auto& wynik : wyniki_czesciowe) {
            przyblizona_pi += wynik;
        }

        auto czas_koniec = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> czas_trwania = czas_koniec - czas_start;

        // Wypisanie wyniku i czasu obliczeń
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << "Liczba wątków: " << liczba_watkow << std::endl;
        std::cout << "Przybliżona wartość liczby PI: " << przyblizona_pi << std::endl;
        std::cout << "Czas obliczeń: " << czas_trwania.count() << " sekund" << std::endl;
        std::cout << "-----------------------------" << std::endl;
    }
    return 0;
}
