# Arduino Game
**Circuit Control using Arduino**

University Project - Programming applied to Electronics and Telecommunications:

This project focuses on the implementation of two interactive games using an Arduino circuit. The games included are "MemoryGame" and "QuizGame". The main goal is to provide users with an entertaining and educational gaming experience through the use of Arduino controlled input and output components.

The circuit will consist of several input and output components, including an LCD display, a 4x4 keyboard, an RGB LED and a servo motor. These components will be interfaced and controlled using the TinkerCAD simulator to facilitate the development and simulation of the project.

## MemoryGame

1. In this game, the user will try to memorize a sequence of lights to reproduce it later.

2. The circuit will have 3 LEDs (red, yellow and green) and a push button associated to each one.

3. The game will start with a random sequence by lighting a single LED. Then at each level, the difficulty will increase and more LEDs will light up in a random sequence.

4. The user must play the sequence using the push buttons corresponding to each LED.

5. The game will end when the user makes a mistake in the sequence.

6. At the end, the length of the last sequence played by the user will be displayed on the LCD.

7. In addition, the RGB LED will light up according to the length of the sequence:

    * If the sequence length is less than 5, the LED will light red.
    * If the sequence length is between 6 and 8, the LED will light yellow.
    * If the sequence length is greater than 8, the LED will light green.

## QuizGame

The game consists of a list of 30 questions grouped by topic (e.g., sports, movies, geography, etc.).

1. When selecting this game, the user will be asked to indicate the desired topic.

2. Five questions on the selected topic will be chosen and presented one by one along with 3 possible answers.

3. The user will be asked to select an answer and a record will be kept of his correct answers.

4. At the end of the 5 questions, the program will activate the RGB LED and the servomotor according to the results obtained:

    * If the user answers 2 questions or less correctly, the LED will light red.
    * If the user answers 3 questions correctly, the LED will light yellow.
    * If the user answers 4 or 5 questions correctly, the LED will light green.

5. The servomotor will also be activated depending on the number of questions answered correctly:

    * If the user correctly answers 2 questions or less, the servomotor will move counterclockwise.
    * If the user answers 3 or more questions correctly, the servomotor will move clockwise.

### More Details

Detailed instructions will be provided for circuit configuration and game implementation in [Tinkercad-ArduinoGame](https://www.tinkercad.com/things/eHE3SfiOjiF).