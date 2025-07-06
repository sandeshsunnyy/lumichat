#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);  // Initialize LCD with corresponding pins
#define ldr A0                          // Define the LDR pin (using A0 for analog readings)

String binaryString = "";              // To store the received binary sequence
const int ldrThreshold = 350;          // Threshold value to distinguish between room light and flashlight
const int bitDuration = 150;           // Duration for which each bit is transmitted. If detection is great, consider increasing this value. Both here and in the app's code. Look out for the varibale flashDuration in the app's code.

bool detecting = false;                 // State variable to track if we are detecting bits

const String startSequence = "1111";   // start sequence
const String endSequence = "11110000"; // end sequence  why?? to prevent infinite detection loops, this particular start and end sequences are implemented in the app, while transmitting.

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Waiting for data...");
}

void loop() {
  static unsigned long lastTime = 0;
  static bool lastState = HIGH;
  bool currentState = analogRead(ldr) < ldrThreshold ? LOW : HIGH;

  if (currentState != lastState) {
    lastTime = millis();                // Record the time of state change, millis records time from the start of execution.
    lastState = currentState;           // Update last state
  }
    
  // LOW means that the analog reading is lower than LDR threshold. That means, there is an excess of light; the flashlight is ON, or a 1 is being transmitted.

  // Check if enough time has passed to determine bit value
  if (millis() - lastTime >= bitDuration) {
    if (currentState == LOW) {          // Light detected (flashlight ON)
      if (!detecting) {
        detecting = true;               // Start detecting on first '1'
      }
      binaryString += "1";              // Append '1' to the string
      Serial.println("Detected: 1");
    } else if (detecting) {              // Only log '0' if we are in detecting mode
      binaryString += "0";              // Append '0' to the string
      Serial.println("Detected: 0");
    }
    
    // Check for end sequence "11110000"
    if (binaryString.endsWith(endSequence)) {
      Serial.println("End sequence detected. Stopping detection.");
      
      String cleanedString = removeSequences(binaryString, startSequence, endSequence);
      
      lcd.clear();
      String message = binaryToString(cleanedString); // Convert binary to string
      
      scrollMessage(message);          // Call scroll function
      
      binaryString = "";                 // Clear the string for next transmission
      detecting = false;                 // Stop detecting
    }

    lastTime = millis();                // Reset timer after reading a bit
  }
}

// Function to remove start and end sequences from the received binary string
String removeSequences(String input, const String& startSeq, const String& endSeq) {
    int startIndex = input.indexOf(startSeq);
    int endIndex = input.indexOf(endSeq);

    if (startIndex != -1) {
        input.remove(0, startIndex + startSeq.length()); // Remove start sequence
    }
    
    if (endIndex != -1) {
        input.remove(endIndex - startIndex - startSeq.length(), endSeq.length()); // Remove end sequence
    }

    return input;
}

String binaryToString(const String& binaryInput) {
    String output = "";

    for (int i = 0; i < binaryInput.length(); i += 8) { 
        String byteString = binaryInput.substring(i, i + 8); // Get each byte (8 bits)
        char character = (char) strtol(byteString.c_str(), nullptr, 2); // Convert byte to char
        output += character;
    }

    return output;                        
}

// Function to scroll message across the LCD display
void scrollMessage(const String& message) {
    int messageLength = message.length();
    
    for (int position = 0; position < messageLength + 16; position++) { 
        lcd.clear();
        
        if (position < messageLength) {
            lcd.print(message.substring(position));
        } else {
            lcd.print(message.substring(position - messageLength));
        }
        
        delay(300);                        // Adjust delay for speed of scrolling
    }
}
