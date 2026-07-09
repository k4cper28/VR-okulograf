
# Projekt i wykonanie prostego okulografu dla VR (Simple Oculograph for VR)

Kompletna aplikacja służąca do śledzenia ruchu gałek ocznych (eye-tracking) użytkownika w środowisku wirtualnej rzeczywistości (VR) oraz dedykowany serwer analityczny do przetwarzania i wizualizacji zebranych danych.

## 📝 Opis projektu
Głównym zadaniem systemu jest precyzyjna analiza punktów skupienia wzroku osoby badanej podczas interakcji z wybranymi obrazami w przestrzeni trójwymiarowej. Projekt stanowi niezależną, nisko kosztową i łatwo modyfikowalną alternatywę dla drogich, komercyjnych rozwiązań okulograficznych (takich jak Tobii Pro Lab czy iMotions).

System składa się z dwóch integralnych części:
1. **Aplikacji VR (Klient):** Odpowiedzialnej za interfejs użytkownika, kalibrację, wyświetlanie bodźców wizualnych oraz rejestrację współrzędnych spojrzenia w czasie rzeczywistym.
2. **Serwera analitycznego:** Przetwarzającego surowe dane tekstowe przesłane z gogli na graficzne mapy wynikowe.

---

## 📸 Prezentacja systemu i wyniki badań

### Środowisko VR i Stanowisko Badawcze
Aplikacja przenosi użytkownika do wirtualnego laboratorium (zaprojektowanego w programie Blender), gdzie zostaje on ustawiony naprzeciwko punktu badawczego z wyświetlanym obrazem.

|  Widok mapy z poziomu gogli VR |
| :---: |
| <img width="618" height="348" alt="obraz" src="https://github.com/user-attachments/assets/fa15a1ee-94d0-4655-9868-b1d1dec04867" /> |

### Wygenerowane Mapy Wynikowe (Analiza danych)
Po zakończeniu sesji badawczej serwer Flask automatycznie generuje dwa rodzaje map analitycznych na podstawie zebranych współrzędnych trafień wzroku:

1. **Mapa punktowa (Point Map):** Prezentuje surowe punkty fiksacji wzroku naniesione bezpośrednio na badany obraz.
2. **Ścieżka spojrzenia (Scanpath):** Wizualizuje dokładną sekwencję (kolejność) spojrzeń za pomocą ponumerowanych punktów połączonych liniami. Rozmiar każdego punktu zależy od czasu trwania fiksacji w danym miejscu.

| 1. Mapa punktowa (ImagePoint) | 2. Ścieżka spojrzenia (ScanPath) |
| :---: | :---: |
| <img width="456" height="639" alt="obraz" src="https://github.com/user-attachments/assets/b283d3fc-145a-4bcf-83b2-4bdc19d92abc" /> | <img width="462" height="646" alt="obraz" src="https://github.com/user-attachments/assets/2489b7fe-b2c3-4870-9b67-9190136588db" /> |

---

## 🛠️ Architektura i technologie

### Aplikacja VR (Klient)
- **Silnik:** Unreal Engine 5.4 (C++ oraz system skryptów wizualnych Blueprints)
- **Standard VR:** OpenXR (zapewniający niezależność platformową)
- **Modelowanie 3D:** Blender (budynek laboratorium, detale środowiskowe, modele oświetlenia)
- **Docelowy sprzęt:** Oculus Quest Pro + dedykowane kontrolery z technologią TruTouch

### Serwer analityczny
- **Język programowania:** Python 3.12
- **Framework sieciowy:** Flask
- **Analiza danych i grafika:** Pandas, Matplotlib, Pillow (PIL)

---

## 📂 Struktura danych wynikowych
Po pomyślnym przesłaniu danych przez gogle VR metodą HTTP POST (kodowanie Base64), serwer zapisuje wyniki w dedykowanym folderze sesji:
- `img.png` – badany obraz źródłowy.
- `wyniki-[Imię]-[Nazwisko]-[Data].txt` – surowy plik zawierający współrzędne X, Y, Z, nazwę obiektu oraz sygnaturę czasową dla każdego zapisanego spojrzenia.
- `info.txt` – metadane sesji (dane demograficzne autora, czas trwania, całkowita liczba zarejestrowanych punktów).
- `ImagePoint.png` – wygenerowana mapa punktowa.
- `scanPath.png` – wygenerowany wykres ścieżki i sekwencji spojrzeń.

---

## 🚀 Uruchomienie i konfiguracja

### Wymagania wstępne (Komputer PC):
- Windows 10/11
- Java SE Development Kit (JDK) 17.0.10
- Android Studio Flamingo (Android API 34, Android 12L)
- Visual Studio 2022 (z pakietami: Programowanie gier w C++, Programowanie aplikacji klasycznych w C++, Narzędzia HLSL)
- Aplikacja Oculus Desktop App (z włączoną usługą Oculus Link)

### Krok 1: Konfiguracja Serwera (Python)
1. Przejdź do katalogu serwera:
   ```bash
   cd serwer
   ```
2. wymagane pakiety:
    ```bash
   pip install flask pandas matplotlib pillow
    ```
3. Uruchom serwer aplikacji:
   ```bash
   python app.py
   ```
