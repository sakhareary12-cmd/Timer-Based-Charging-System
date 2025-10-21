#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#define outputA 13 // clk pin of rotary encoder
#define outputB 6 // dt pin of rotary encoder
#define button 7   // sw pin of rotary encoder
#define led 10     // status led
#define relay 14   // in1 of relay

int count = 1;
int current_state;
int previous_state;
int hh = 0;
int mm = 0;
int ss = 0;
int h = 0;
int m = 0;
int s = 0;
int ledToggle;
int previousState = HIGH;
unsigned int previousPress;
volatile int buttonFlag;
int buttonDebounce = 20;

void setup()
{
    lcd.begin(16, 2);
    pinMode(outputA, INPUT);
    pinMode(outputB, INPUT);
    pinMode(button, INPUT_PULLUP);
    pinMode(led, OUTPUT);
    pinMode(relay, OUTPUT);
    digitalWrite(relay, HIGH);
    attachInterrupt(digitalPinToInterrupt(2), button_ISR, CHANGE);

    Serial.begin(9600);

    previous_state = digitalRead(outputA);

    if (EEPROM.read(0) != 0 || EEPROM.read(1) != 0 || EEPROM.read(2) != 0)
    {
        lcd.setCursor(0, 0);
        lcd.print("Load Preset ?");
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(" <NO> ");
        int temp = 5;

        while (temp > 0 && ledToggle == 0)
        {
            lcd.setCursor(14, 0);
            lcd.print(temp);
            temp--;
            delay(1000);
        }

        if (temp == 0 && ledToggle == 0)
        {
            hh = EEPROM.read(0);
            mm = EEPROM.read(1);
            ss = EEPROM.read(2);
            timer();
        }
        else
        {
            EEPROM.write(0, 0);
            EEPROM.write(1, 0);
            EEPROM.write(2, 0);
            ledToggle = 0;
        }
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" HH MM SS OK ");
    lcd.setCursor(0, 1);
    lcd.print(" 0 0 0 ");
}

void loop()
{
    encoder();

    if (count == 1)
    {
        lcd.setCursor(0, 0);
        lcd.print("<HH> MM SS OK ");

        while (count == 1)
        {
            encoder();

            if (ledToggle)
            {
                h = 1;
                m = 0;
                s = 0;

                lcd.setCursor(0, 0);
                lcd.print(" HH MM SS OK ");
                lcd.setCursor(1, 1);
                lcd.print(hh);
                lcd.setCursor(5, 1);
                lcd.print(mm);
                lcd.setCursor(9, 1);
                lcd.print(ss);
                lcd.setCursor(0, 1);
                lcd.print('<');
                lcd.setCursor(3, 1);
                lcd.print('>');

                while (ledToggle)
                {
                    encoder();
                    lcd.setCursor(1, 1);
                    lcd.print(hh);
                }
            }

            EEPROM.write(0, hh);

            h = 0;
            m = 0;
            s = 0;

            lcd.setCursor(0, 0);
            lcd.print("<HH> MM SS OK ");
            lcd.setCursor(0, 1);
            lcd.print(' ');
            lcd.setCursor(3, 1);
            lcd.print(' ');
        }
    }
    else if (count == 2)
    {
        lcd.setCursor(0, 0);
        lcd.print(" HH <MM> SS OK ");

        while (count == 2)
        {
            encoder();

            if (ledToggle)
            {
                h = 0;
                m = 1;
                s = 0;

                lcd.setCursor(0, 0);
                lcd.print(" HH MM SS OK ");
                lcd.setCursor(1, 1);
                lcd.print(hh);
                lcd.setCursor(5, 1);
                lcd.print(mm);
                lcd.setCursor(9, 1);
                lcd.print(ss);
                lcd.setCursor(4, 1);
                lcd.print('<');
                lcd.setCursor(7, 1);
                lcd.print('>');

                while (ledToggle)
                {
                    encoder();
                    lcd.setCursor(5, 1);
                    lcd.print(mm);
                }
            }

            EEPROM.write(1, mm);

            h = 0;
            m = 0;
            s = 0;

            lcd.setCursor(0, 0);
            lcd.print(" HH <MM> SS OK ");
            lcd.setCursor(4, 1);
            lcd.print(' ');
            lcd.setCursor(7, 1);
            lcd.print(' ');
        }
    }
    else if (count == 3)
    {
        lcd.setCursor(0, 0);
        lcd.print(" HH MM <SS> OK ");

        while (count == 3)
        {
            encoder();

            if (ledToggle)
            {
                h = 0;
                m = 0;
                s = 1;

                lcd.setCursor(0, 0);
                lcd.print(" HH MM SS OK ");
                lcd.setCursor(1, 1);
                lcd.print(hh);
                lcd.setCursor(5, 1);
                lcd.print(mm);
                lcd.setCursor(9, 1);
                lcd.print(ss);
                lcd.setCursor(8, 1);
                lcd.print('<');
                lcd.setCursor(11, 1);
                lcd.print('>');

                while (ledToggle)
                {
                    encoder();
                    lcd.setCursor(9, 1);
                    lcd.print(ss);
                }
            }

            EEPROM.write(2, ss);

            h = 0;
            m = 0;
            s = 0;

            lcd.setCursor(0, 0);
            lcd.print(" HH MM <SS> OK ");
            lcd.print(' ');
            lcd.setCursor(11, 1);
            lcd.print(' ');
        }
    }
    else if (count == 4)
    {
        lcd.setCursor(0, 0);
        lcd.print(" HH MM SS <OK>");

        while (count == 4)
        {
            encoder();

            if (ledToggle)
            {
                timer();
            }
        }
    }
}

void encoder()
{
    current_state = digitalRead(outputA);

    if (current_state != previous_state)
    {
        if (digitalRead(outputB) != current_state)
        {
            if (count < 4 && ledToggle == 0)
                count++;
            else
            {
                if (h == 1)
                {
                    if (hh < 23)
                        hh = hh + 1;
                }
                else if (m == 1)
                {
                    if (mm < 59)
                        mm = mm + 1;
                }
                else if (s == 1)
                {
                    if (ss < 59)
                        ss = ss + 1;
                }
            }
        }
        else
        {
            if (count > 1 && ledToggle == 0)
                count--;
            else
            {
                if (h == 1)
                {
                    if (hh > 0)
                        hh = hh - 1;
                }
                else if (m == 1)
                {
                    if (mm > 0)
                        mm = mm - 1;
                }
                else if (s == 1)
                {
                    if (ss > 0)
                        ss = ss - 1;
                }
            }
        }

        Serial.print("Position: ");
        Serial.println(count);
    }

    previous_state = current_state;
}

void button_ISR()
{
    buttonFlag = 1;

    if ((millis() - previousPress) > buttonDebounce && buttonFlag)
    {
        previousPress = millis();

        if (digitalRead(button) == LOW && previousState == HIGH)
        {
            ledToggle = !ledToggle;
            digitalWrite(led, ledToggle);
            previousState = LOW;
        }
        else if (digitalRead(button) == HIGH && previousState == LOW)
        {
            previousState = HIGH;
        }

        buttonFlag = 0;
    }
}

void timer()
{
    digitalWrite(relay, LOW);

    while (1)
    {
        lcd.setCursor(0, 0);
        lcd.print("Charging... ");
        lcd.setCursor(0, 1);

        if (hh < 10)
        {
            lcd.print('0');
            lcd.print(hh);
            lcd.print(':');
        }
        else
        {
            lcd.print(hh);
            lcd.print(':');
        }

        if (mm < 10)
        {
            lcd.print('0');
            lcd.print(mm);
            lcd.print(':');
        }
        else
        {
            lcd.print(mm);
            lcd.print(':');
        }

        if (ss < 10)
        {
            lcd.print('0');
            lcd.print(ss);
            lcd.print(" ");
        }
        else
        {
            lcd.print(ss);
            lcd.print(" ");
        }

        Serial.println(ss);

        for (ss; ss > 0; ss--)
        {
            Serial.println(ss);

            if (ss < 10)
            {
                lcd.setCursor(6, 1);
                lcd.print('0');
                lcd.print(ss);
                EEPROM.write(2, ss);
            }
            else
            {
                lcd.setCursor(6, 1);
                lcd.print(ss);
                EEPROM.write(2, ss);
            }

            delay(1000);
        }

        if (ss == 0 && mm == 0 && hh == 0)
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Charged");
            digitalWrite(relay, HIGH);

            EEPROM.write(0, 0);
            EEPROM.write(1, 0);
            EEPROM.write(2, 0);

            while (1)
                ;
        }

        ss = 59;

        if (mm != 0)
            mm = mm - 1;

        if (hh != 0)
            hh = hh - 1;

        EEPROM.write(0, hh);
        EEPROM.write(1, mm);
        EEPROM.write(2, ss);
    }
}
