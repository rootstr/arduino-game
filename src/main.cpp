#include <Servo.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Keypad.h>
#include <Key.h>

/*
 *  Available digital (PWM~) I/O ports.
 */
namespace ArduinoDigitalPort
{
  enum DigitalPort : byte
  {
    PORT_0 = 0x000000,
    PORT_1 = 0x000001,
    PORT_2 = 0x000002,
    PORT_3 = 0x000003,
    PORT_4 = 0x000004,
    PORT_5 = 0x000005,
    PORT_6 = 0x000006,
    PORT_7 = 0x000007,
    PORT_8 = 0x000008,
    PORT_9 = 0x000009,
    PORT_10 = 0x00000A,
    PORT_11 = 0x00000B,
    PORT_12 = 0x00000C,
    PORT_13 = 0x00000D,
  };
}

/*
 *  Available analog ports.
 */
namespace ArduinoAnalogPort
{
  enum AnalogPorts : byte
  {
    APORT0 = 0x00000E,
    APORT1 = 0x00000F,
    APORT2 = 0x000010,
    APORT3 = 0x000011,
    APORT4 = 0x000012,
    APORT5 = 0x000013,
    APORT6 = 0x000014,
    APORT7 = 0x000015,
  };
}

/*
 *  Used ports
 */
namespace ArduinoGamePort
{
  enum GamePort : byte
  {
    SERVO = ArduinoDigitalPort::PORT_3,
    KEYPAD_3 = ArduinoDigitalPort::PORT_2,
    KEYPAD_2 = ArduinoDigitalPort::PORT_4,
    KEYPAD_1 = ArduinoDigitalPort::PORT_5,
    KEYPAD_R1 = ArduinoDigitalPort::PORT_6,
    LCD_RS = ArduinoDigitalPort::PORT_7,
    LCD_E = ArduinoDigitalPort::PORT_8,
    LCD_D4 = ArduinoDigitalPort::PORT_9,
    LCD_D5 = ArduinoDigitalPort::PORT_10,
    LCD_D6 = ArduinoDigitalPort::PORT_11,
    LCD_D7 = ArduinoDigitalPort::PORT_12,
    LED_RED = ArduinoAnalogPort::APORT0,
    LED_GREEN = ArduinoAnalogPort::APORT1,
    LED_YELLOW = ArduinoAnalogPort::APORT2,
    ON_RED = ArduinoAnalogPort::APORT3,
    ON_GREEN = ArduinoAnalogPort::APORT4,
    ON_YELLOW = ArduinoAnalogPort::APORT5,
    NONE_PORT = 0x00000FF
  };
}

/* Constants related to the peripheral LCD. */
namespace Display
{
  static LiquidCrystal lcd(
      ArduinoGamePort::LCD_RS,
      ArduinoGamePort::LCD_E,
      ArduinoGamePort::LCD_D4,
      ArduinoGamePort::LCD_D5,
      ArduinoGamePort::LCD_D6,
      ArduinoGamePort::LCD_D7); // Connection (RS, ENABLE, D4, D5, D6, D7)

  static const uint8_t LCD_ROWS{2};
  static const uint8_t LCD_COLUMNS{16};

  static void set_default_conf()
  {
    Display::lcd.begin(Display::LCD_COLUMNS, Display::LCD_ROWS);
    Display::lcd.setCursor(0, 0);
    Display::lcd.clear();
  }

  /*
    Prints a sequence of characters on the LCD, as long as it does not exceed
    the LCD's capacity.
  */
  void print(const char *serial_data)
  {
    set_default_conf();
    uint8_t row = 0;
    uint8_t col = 0;

    if (serial_data != nullptr)
    {
      for (size_t i = 0; serial_data[i] != '\0'; i++)
      {
        if (serial_data[i] == '\n')
        {
          row++;
          col = 0;

          if (row >= LCD_ROWS)
            break;

          lcd.setCursor(0, row);
        }
        else
        {
          lcd.print(serial_data[i]);
          col++;

          if (col > LCD_COLUMNS)
          {
            col = 0;
            row++;

            if (row > LCD_ROWS)
              break;

            lcd.setCursor(0, row);
          }
        }
      }
    }
  }
}

namespace ServoMotor
{
  enum class Angle
  {
    RIGHT = 0,
    LEFT = 180,
    CENTER = 90
  };

  static Servo servo;
  uint8_t degrees = 0;
}

namespace Keyboard
{
  enum class KeyPressed
  {
    ONE = '1',
    TWO = '2',
    THREE = '3',
    None = '0'
  };

  const byte ROWS = 1;
  const byte COLUMNS = 3;
  byte ROW_PORTS[ROWS]{ArduinoGamePort::KEYPAD_R1};
  byte COLUMN_PORTS[COLUMNS]{
      ArduinoGamePort::KEYPAD_1,
      ArduinoGamePort::KEYPAD_2,
      ArduinoGamePort::KEYPAD_3};
  char keys[ROWS][COLUMNS]{{'1', '2', '3'}};

  static Keypad keyBoard(makeKeymap(keys), ROW_PORTS, COLUMN_PORTS, ROWS, COLUMNS);
}

namespace ArduinoUtils
{
  static void turn_off_port(ArduinoGamePort::GamePort port)
  {
    digitalWrite(port, LOW);
  }

  static void turn_on_port(ArduinoGamePort::GamePort port)
  {
    digitalWrite(port, HIGH);
  }

  // Convert a character to a pressable key option.
  static Keyboard::KeyPressed on_key_pressed(char key)
  {
    if (key >= 0x000031 && key <= 0x000033)
      return (Keyboard::KeyPressed)key;
    return Keyboard::KeyPressed::None;
  }

  static void servo_set_angle(ServoMotor::Angle angle)
  {
    ServoMotor::servo.write(static_cast<int>(angle));
  }

  // Convert a button-related port as a byte to the corresponding port of the LED it should activate.
  static ArduinoGamePort::GamePort on_push_pressed(byte push_event)
  {
    if (push_event == static_cast<byte>(ArduinoGamePort::GamePort::ON_GREEN))
      return ArduinoGamePort::GamePort::LED_GREEN;

    if (push_event == static_cast<byte>(ArduinoGamePort::GamePort::ON_RED))
      return ArduinoGamePort::GamePort::LED_RED;

    if (push_event == static_cast<byte>(ArduinoGamePort::GamePort::ON_YELLOW))
      return ArduinoGamePort::GamePort::LED_YELLOW;
    return ArduinoGamePort::NONE_PORT;
  }

  // Welcome Message
  static void greeting()
  {
    Display::print("Bienvenido!");
    delay(1500);
    Display::print("1) Memoria\n2) Preguntas");
  }

  // Set the initial state of the hardware.
  static void set_default_values()
  {
    turn_off_port(ArduinoGamePort::LED_GREEN);
    turn_off_port(ArduinoGamePort::LED_RED);
    turn_off_port(ArduinoGamePort::LED_YELLOW);
    ArduinoUtils::servo_set_angle(ServoMotor::Angle::CENTER);
    Display::set_default_conf();
  }
}

/* Implementations of available games */
namespace ArduinoGames
{
  // Represents the initial state of the game.
  enum class GameState
  {
    RUNNING,
    OVER
  };

  /*
    The MemoryGame structure represents an abstraction of the game (2).
    The implementation uses the singleton pattern to ensure that only one instance can be created.
  */
  struct MemoryGame
  {
    static MemoryGame &instance()
    {
      static MemoryGame _instance;
      return _instance;
    }

    // Main function with the game logic.
    void initialize()
    {
      GameState game_state = GameState::RUNNING; // Current game state, running.

      // Main loop only ends if the player loses.
      while (game_state != GameState::OVER)
      {
        this->difficulty += 1; // Increase difficulty for each correct guess.
        char match_advice[16];
        (String("Round #") + String(this->difficulty)).toCharArray(match_advice, 16);
        Display::print(match_advice);
        random_color_sequence();            // Create a new random sequence.
        turn_on_color_sequence(1000, 3000); // Show the sequence with a delay of 2(1/2) sec.

        // Check if the user correctly enters the sequence.
        Display::print("Your turn!\nWaiting...");
        byte tries = 0; // User's attempts.

        // Secondary loop only ends if the user makes a mistake or enters the sequence correctly.
        while (tries < this->difficulty && game_state != GameState::OVER)
        {
          // Iterate through the button ports to check which one was pressed by the user.
          for (byte port{ArduinoGamePort::GamePort::ON_RED}; port <= ArduinoGamePort::GamePort::ON_YELLOW; port++)
          {
            // If any button is pressed, check if the user made a mistake or not.
            if (digitalRead(port) > 0)
            {
              ArduinoGamePort::GamePort current_led_port = ArduinoUtils::on_push_pressed(port);
              ArduinoGamePort::GamePort led_port = this->color_sequence[tries];
              if (current_led_port != led_port) // Only if the user made a mistake.
              {
                game_state = GameState::OVER;
              }
              else
              {
                tries++; // Increment only if the correct button was pressed.
              }
              ArduinoUtils::turn_on_port(ArduinoUtils::on_push_pressed(port));
              delay(1500); // Time the LED stays on (or the electric pulse)
              ArduinoUtils::turn_off_port(ArduinoUtils::on_push_pressed(port));
              delay(1500); // Delay after turning off the LED (this is done due to the simulation delay)
            }
          }
        }
      }
      // Free memory and apply default values.
      char game_over_advice[32];
      (String("GAME OVER\nBest round #") + String(this->difficulty)).toCharArray(game_over_advice, 32);
      Display::print(game_over_advice);

      if (this->difficulty < 5)
        ArduinoUtils::turn_on_port(ArduinoGamePort::LED_RED);

      if (this->difficulty >= 6 && this->difficulty <= 8)
        ArduinoUtils::turn_on_port(ArduinoGamePort::LED_YELLOW);

      if (this->difficulty > 8)
        ArduinoUtils::turn_on_port(ArduinoGamePort::LED_GREEN);

      delay(1500); // Delay for the game over message.
      this->difficulty = 0;
      drop_color_sequence();
    }

  private:
    void turn_on_color_sequence(uint16_t flash_delay, uint16_t after_flash_delay)
    {
      for (byte i{}; i < this->difficulty; i++)
      {
        ArduinoGamePort::GamePort port = this->color_sequence[i];
        ArduinoUtils::turn_on_port(port);
        delay(flash_delay); // Delay after turning on an LED.
        ArduinoUtils::turn_off_port(port);
        delay(after_flash_delay); // Delay after turning off the LED (This is done due to the simulation delay)
      }
    }

    void random_color_sequence()
    {
      // Allocate enough space for the new LED sequence.
      allocate_color_sequence();

      // Randomly choose the ports of the LEDs to turn on.
      for (byte i{}; i < this->difficulty; i++)
      {
        this->color_sequence[i] = (ArduinoGamePort::GamePort) static_cast<byte>(random(ArduinoGamePort::LED_RED, ArduinoGamePort::LED_YELLOW + 1));
      }
    }

    // Free memory, random color sequence.
    void drop_color_sequence()
    {
      if (is_valid_color_sequence())
      {
        delete[] this->color_sequence;
        this->color_sequence = nullptr;
      }
    }

    // Memory validation.
    bool is_valid_color_sequence()
    {
      return this->color_sequence != nullptr;
    }

    void allocate_color_sequence()
    {
      // Free memory occupied by the previous sequence.
      drop_color_sequence();

      // Allocate memory for the new sequence.
      this->color_sequence = new ArduinoGamePort::GamePort[this->difficulty];
    }

  private:
    MemoryGame() : difficulty(0), color_sequence(nullptr){};
    MemoryGame(const MemoryGame &) = delete;
    MemoryGame &operator=(const MemoryGame &) = delete;

    // Free the memory occupied by the game sequence.
    ~MemoryGame()
    {
      drop_color_sequence();
    }

  private:
    uint8_t difficulty;
    ArduinoGamePort::GamePort *color_sequence;
  };

// Structure Question to store the questions
typedef struct
{
  const char quiz[17]; // Question
  const char options[3][17];
  char answer; // Index of the correct answer.
} Question;

// Array of questions stored in flash memory
const Question allQuestions[] PROGMEM = {
    // Movie Questions
    {"Pulp Fic. dir?", {"Scorsese", "Tarantino", "Spielberg"}, '2'},
    {"Godfather actor?", {"Al Pacino", "Marlon Brando", "Robert De Niro"}, '2'},
    {"Titanic release?", {"1995", "1997", "1999"}, '2'},
    {"LaLaLand act?", {"Emma Stone", "J. Lawrence", "Natalie Port"}, '1'},
    {"Matrix genre?", {"Action", "Sci-Fi", "Comedy"}, '2'},
    // Sports Questions
    {"Racket sport?", {"Tennis", "Soccer", "Basketball"}, '1'},
    {"2018 World Cup?", {"France", "Brazil", "Germany"}, '1'},
    {"Famous player?", {"M. Jordan", "LeBron James", "Kobe Bryant"}, '1'},
    {"Super Bowl 2021?", {"KC Chiefs", "SF 49ers", "TB Buccaneers"}, '3'},
    {"Ball-less sport?", {"Athletics", "Volleyball", "Hockey"}, '1'},
    // Art Questions
    {"Famous painting?", {"Last Supper", "Starry Night", "The Scream"}, '1'},
    {"Scul in Vatican?", {"Pieta", "David", "Moses"}, '1'},
    {"Sgt Pepper's?", {"The Beatles", "Rolling Stones", "Led Zeppelin"}, '1'},
    {"Impress painter?", {"C. Monet", "P. Picasso", "V. van Gogh"}, '1'},
    {"Verdi's opera?", {"La Traviata", "Carmen", "Rigoletto"}, '1'},
    // Science Questions
    {"Fst man on moon?", {"Neil A.", "Buzz A.", "Yuri G."}, '3'},
    {"Subatomic part?", {"Proton", "Electron", "Neutron"}, '1'},
    {"Celestial body?", {"Planet", "Star", "Moon"}, '1'},
    {"Unit of force?", {"Volt", "Watt", "Newton"}, '3'},
    {"Vision center?", {"Eye", "Brain", "Heart"}, '2'},
    // History Questions
    {"WW2 end year?", {"1939", "1945", "1951"}, '2'},
    {"Fst US presid?", {"George W.", "Abraham L.", "Thomas J."}, '2'},
    {"French Rev?", {"18th", "19th", "20th"}, '2'},
    {"Treaty city?", {"Paris", "Berlin", "London"}, '1'},
    {"First Roman emp?", {"Julius Caesar", "Augustus", "Nero"}, '2'},
    // Geography Questions
    {"Capital of Aust?", {"Canberra", "Sydney", "Melbourne"}, '1'},
    {"Egypt continent?", {"Africa", "Asia", "Europe"}, '1'},
    {"H-hest mountain?", {"Mount Everest", "Kilimanjaro", "Mount Aconcagua"}, '1'},
    {"Renaissance?", {"Italy", "France", "Spain"}, '1'},
    {"Most popu. cont?", {"Asia", "Africa", "Europe"}, '2'}
};

const char category_catalog[6][14] = {"1) Movie", "2) Sports", "1) Art", "2) Science", "1) History", "2) Geography"};

  struct QuizGame
  {
    static QuizGame &instance()
    {
      static QuizGame _instance;
      return _instance;
    }

    void initialize()
    {
      GameState game_state = GameState::RUNNING;
      byte page_index = 0;                                                  // Index of the category page, each page contains 2 categories.
      Keyboard::KeyPressed key_category_event = Keyboard::KeyPressed::None; // Selected category.

      Display::print("More categories\nPress #3");
      delay(2000); // Instructions usage time.

      // Only ends if a category is chosen.
      while (key_category_event != Keyboard::KeyPressed::ONE && key_category_event != Keyboard::KeyPressed::TWO)
      {
        key_category_event = Keyboard::KeyPressed::None;

        // Display the categories.
        display_categories(page_index * 2, page_index * 2 + 1);

        // Only ends if a category is chosen or page is changed.
        while (key_category_event == Keyboard::KeyPressed::None)
        {
          // Get keyboard events to validate which category is selected.
          key_category_event = ArduinoUtils::on_key_pressed(Keyboard::keyBoard.getKey());
          if (key_category_event == Keyboard::KeyPressed::THREE)
          {
            page_index++;
            if (page_index > 2)
              page_index = 0;
          }
        }
      }

      byte next_quiz = 0;       // Next question.
      byte correct_answers = 0; // User's correct answers.
      // Convert the selected category to the equivalent byte value.
      byte category_selected = static_cast<byte>(key_category_event) % 3;
      Keyboard::KeyPressed key_answer_event = Keyboard::KeyPressed::None; // User's answer.

      while (game_state != GameState::OVER)
      {
        // Read the content from the Arduino's flash memory.
        const Question *quiz = &allQuestions[next_quiz + (page_index * 10) + (category_selected - 1) * 5];

        Display::print(read_mem_str(quiz->quiz));
        delay(2000);

        // Display the options.
        for (byte option{}; option < 3; option++)
        {
          Display::print(read_mem_str(quiz->options[option]));
          delay(2000);
        }

        Display::print("Which option?\nWaiting...");

        key_answer_event = Keyboard::KeyPressed::None;
        Keyboard::KeyPressed answer = ArduinoUtils::on_key_pressed(
            *read_mem_str(static_cast<const char *>(&quiz->answer)));

        while (key_answer_event == Keyboard::KeyPressed::None)
        {
          // Get keyboard events.
          key_answer_event = ArduinoUtils::on_key_pressed(Keyboard::keyBoard.getKey());
          if (key_answer_event != Keyboard::KeyPressed::None)
          {
            next_quiz++;
            if (key_answer_event == answer)
            {
              correct_answers++;
            }

            // If the maximum number of questions is reached, the game ends.
            if (next_quiz > 4)
            {
              game_state = GameState::OVER;
            }
          }
        }
      }
      char game_over_advice[32];
      (String("GAME OVER\nBest round #") + String(correct_answers)).toCharArray(game_over_advice, 32);
      Display::print(game_over_advice);

      if (correct_answers <= 2)
      {
        ArduinoUtils::turn_on_port(ArduinoGamePort::LED_RED);
        ArduinoUtils::servo_set_angle(ServoMotor::Angle::LEFT);
      }

      if (correct_answers == 3)
      {
        ArduinoUtils::turn_on_port(ArduinoGamePort::LED_YELLOW);
        ArduinoUtils::servo_set_angle(ServoMotor::Angle::RIGHT);
      }

      if (correct_answers >= 4 && correct_answers <= 5)
      {
        ArduinoUtils::turn_on_port(ArduinoGamePort::LED_GREEN);
        ArduinoUtils::servo_set_angle(ServoMotor::Angle::RIGHT);
      }
      delay(2500); // Delay before ending the game.
    }

  private:
    const char *read_mem_str(const char *str)
    {
      static char buffer[256];                          // Temporary buffer to store the text string
      strcpy_P(buffer, static_cast<const char *>(str)); // Copy the text string from flash memory to the buffer
      return buffer;                                    // Return the constant pointer to the buffer
    }

    void display_categories(byte at_index, byte end_index)
    {
      String categories = ""; // Temporary string for easier concatenation.
      char buff[24];          // Temporary buffer to visualize the resulting string.
      for (byte option{at_index}; option <= end_index; option++)
      {
        categories += category_catalog[option];
        categories += '\n';
      }
      categories.toCharArray(buff, 24);
      Display::print(buff);
    }

  private:
    QuizGame() {}
    QuizGame(const MemoryGame &) = delete;
    QuizGame &operator=(const MemoryGame &) = delete;
    ~QuizGame()
    {
    }
  };

  static void start_game(Keyboard::KeyPressed event)
  {
    if (event == Keyboard::KeyPressed::ONE)
      MemoryGame::instance().initialize();

    if (event == Keyboard::KeyPressed::TWO)
      QuizGame::instance().initialize();

    if (event == Keyboard::KeyPressed::ONE || event == Keyboard::KeyPressed::TWO)
    {
      ArduinoUtils::set_default_values();
      ArduinoUtils::greeting();
    }
  }
}

/*
 *  Configuration of I/O peripherals.
 */
void setup()
{

  Serial.begin(9600);
  Display::set_default_conf();

  // LEDs on analog ports.
  pinMode(ArduinoGamePort::LED_RED, OUTPUT);
  pinMode(ArduinoGamePort::LED_GREEN, OUTPUT);
  pinMode(ArduinoGamePort::LED_YELLOW, OUTPUT);

  // Buttons on analog ports.
  pinMode(ArduinoGamePort::ON_RED, INPUT);
  pinMode(ArduinoGamePort::ON_GREEN, INPUT);
  pinMode(ArduinoGamePort::ON_YELLOW, INPUT);

  // Servo motor
  ServoMotor::servo.attach(ArduinoGamePort::SERVO);
  ArduinoUtils::greeting();
}

void loop()
{
  Keyboard::KeyPressed key_event =
      ArduinoUtils::on_key_pressed(Keyboard::keyBoard.getKey()); // Get keyboard event.
  ArduinoGames::start_game(key_event);                           // Validate keyboard event.
}