#include <hidboot.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <SPI.h>
#include "Keyboard.h"

#define KEY_1 0x1e // Keyboard 1 and !
#define KEY_2 0x1f // Keyboard 2 and @
#define KEY_3 0x20 // Keyboard 3 and #
#define KEY_4 0x21 // Keyboard 4 and $
#define KEY_5 0x22 // Keyboard 5 and %
#define KEY_6 0x23 // Keyboard 6 and ^
#define KEY_7 0x24 // Keyboard 7 and &
#define KEY_8 0x25 // Keyboard 8 and *
#define KEY_9 0x26 // Keyboard 9 and (
#define KEY_0 0x27 // Keyboard 0 and )
#define KEY_tilda 0x32 // Keyboard ` and ~

#define KEY_MINUS 0x2d // Keyboard - and _
#define KEY_EQUAL 0x2e // Keyboard = and +
#define KEY_LEFTBRACE 0x2f // Keyboard [ and {
#define KEY_RIGHTBRACE 0x30 // Keyboard ] and }
#define KEY_BACKSLASH 0x31 // Keyboard \ and |
#define KEY_HASHTILDE 0x32 // Keyboard Non-US # and ~
#define KEY_SEMICOLON 0x33 // Keyboard ; and :
#define KEY_APOSTROPHE 0x34 // Keyboard ' and "
#define KEY_GRAVE 0x35 // Keyboard ` and ~
#define KEY_COMMA 0x36 // Keyboard , and <
#define KEY_DOT 0x37 // Keyboard . and >
#define KEY_SLASH 0x38 // Keyboard / and ?
#define KEY_CAPSLOCK 0x39 // Keyboard Caps Lock

class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
  if (buf[2] == 1 || buf[2] == 0) return;

  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(buf[0]==2, buf, buf[i]);
    }

    return;
  }
}

uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters
  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    if (upper) return (key - 4 + 'A');
    else return (key - 4 + 'a');
  }
    
  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    if (upper)
    {
      switch(key){
        case KEY_1: return  '!';
        case KEY_2: return  '@';
        case KEY_3: return  '#';
        case KEY_4: return  '$';
        case KEY_5: return  '%';
        case KEY_6: return  '^';
        case KEY_7: return  '&';
        case KEY_8: return  '*';
        case KEY_9: return  '(';
        case KEY_0: return  ')';
      }
    }
    else return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
  }

  else if(VALUE_WITHIN(key, 0x2d, 0x38)) {
      switch(key){
        case KEY_MINUS: return  (upper ? '_' : '-');
        case KEY_EQUAL: return  (upper ? '+' : '=');
        case KEY_LEFTBRACE: return  (upper ? '{' : '[');
        case KEY_RIGHTBRACE: return  (upper ? ']' : '}');
        case KEY_BACKSLASH: return  (upper ? '|' : '\\');
        case KEY_HASHTILDE: return  (upper ? '~' : '#');
        case KEY_SEMICOLON: return  (upper ? ':' : ';');
        case KEY_APOSTROPHE: return  (upper ? '\"' : '\'');
        case KEY_GRAVE: return  (upper ? '~' : '`');
        case KEY_COMMA: return  (upper ? '<' : ',');
        case KEY_DOT: return  (upper ? '>' : '.');
        case KEY_SLASH: return  (upper ? '?' : '/');
      }

    if (upper) return (key - 0x2d + '_');
    else return (key - 0x2d + '-');
  }
  return 0;

}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  uint8_t ascii = KeyToAscii(upper, mod, key);
  Serial.print((char)ascii);
  Keyboard.print((char)ascii);
}

void MyParser::OnScanFinished() {
  Serial.println(" - Finished");
  Keyboard.println();
}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;

void setup() {
  Serial.begin( 115200 );
  Serial.println("Start");

  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }

  delay( 200 );

  Hid.SetReportParser(0, &Parser);
}

void loop() {
  Usb.Task();
}