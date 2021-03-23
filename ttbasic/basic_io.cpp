#include "basic.h"
#include "c_io.h"

#include <Wire.h>

void SMALL basic_init_io() {
#ifdef ESP8266
  Wire.begin(2, 0);
  // ESP8266 Wire code assumes that SCL and SDA pins are set low, instead
  // of taking care of that itself. WTF?!?
  digitalWrite(0, LOW);
  digitalWrite(2, LOW);
#endif
}

/***bc io GPOUT
Sets the state of a general-purpose I/O pin.
\usage
GPOUT pin, value (ESP8266)

GPOUT port, pin, value (H3)
\args
@port	port number [`0` to `7`]
@pin	pin number [`0` to `15` (ESP8266) or `31` (H3)]
@value	pin state [`0` for "low", anything else for "high"]
\note
* This command is only implemented on the ESP8266 and H3 platforms.
* On the ESP8266 platform, `GPOUT` allows access to pins on the I2C I/O
  extender only.
\ref GPIN() GPMODE
***/
void Basic::idwrite() {
  uint32_t portno = 0;
  uint32_t pinno, data;

  if (
#ifndef ESP8266
      getParam(portno, I_COMMA) ||
#endif
      getParam(pinno, I_COMMA) ||
      getParam(data, I_NONE))
    return;

  c_gpio_set_pin(portno, pinno, !!data);
}

/***bf io I2CW
Sends data to an I2C device.
\usage res = I2CW(i2c_addr, out_data)
\args
@i2c_addr	I2C address [`0` to `$7F`]
@out_data	data to be transmitted
\ret
Status code of the transmission.
\ref I2CR()
***/
num_t BASIC_INT Basic::ni2cw() {
  int32_t i2cAdr;
  BString out;

  if (checkOpen())
    return 0;
  if (getParam(i2cAdr, 0, 0x7f, I_COMMA))
    return 0;
  out = istrexp();
  if (checkClose())
    return 0;

  // SDA is multiplexed with MVBLK0, so we wait for block move to finish
  // to avoid interference.
  while (!blockFinished()) {}

  // I2Cデータ送信
  Wire.beginTransmission(i2cAdr);
  if (out.length()) {
    for (uint32_t i = 0; i < out.length(); i++)
      Wire.write(out[i]);
  }
  return Wire.endTransmission();
}

/***bf io I2CR
Request data from I2C device.
\usage in$ = I2CR(i2c_addr, out_data, read_length)
\args
@i2c_addr	I2C address [`0` to `$7F`]
@out_data	data to be transmitted
@read_length	number of bytes to be received
\ret
Returns the received data as the value of the function call.

Also returns the status code of the outward transmission in `RET(0)`.
\note
If `out_data` is an empty string, no data is sent before the read request.
\ref I2CW()
***/
BString Basic::si2cr() {
  int32_t i2cAdr, rdlen;
  BString in, out;

  if (checkOpen())
    goto out;
  if (getParam(i2cAdr, 0, 0x7f, I_COMMA))
    goto out;
  out = istrexp();
  if (*cip++ != I_COMMA) {
    E_SYNTAX(I_COMMA);
    goto out;
  }
  if (getParam(rdlen, 0, INT32_MAX, I_CLOSE))
    goto out;

  // SDA is multiplexed with MVBLK0, so we wait for block move to finish
  // to avoid interference.
  while (!blockFinished()) {}

  // I2Cデータ送受信
  Wire.beginTransmission(i2cAdr);

  // 送信
  if (out.length()) {
    Wire.write((const uint8_t *)out.c_str(), out.length());
  }
  if ((retval[0] = Wire.endTransmission()))
    goto out;
  Wire.requestFrom(i2cAdr, rdlen);
  while (Wire.available()) {
    in += (char)Wire.read();
  }
out:
  return in;
}

/***bc io SWRITE
Writes a byte to the serial port.

WARNING: This command may be renamed in the future to reduce namespace
pollution.
\usage SWRITE c
\args
@c	byte to be written
\ref SMODE
***/
void Basic::iswrite() {
  int32_t c;
  if (getParam(c, I_NONE))
    return;
  Serial.write(c);
}

/***bc io SMODE
Changes the serial port configuration.

WARNING: This command is likely to be renamed in the future to reduce
namespace pollution.
\usage SMODE baud[, flags]
\args
@baud	baud rate [`0` to `921600`]
@flags	serial port flags
\bugs
The meaning of `flags` is determined by `enum` values in the Arduino core
and cannot be relied on to remain stable.
\ref SWRITE
***/
void SMALL Basic::ismode() {
  int32_t baud, flags = SERIAL_8N1;
  if (getParam(baud, 0, 921600, I_NONE))
    return;
  if (*cip == I_COMMA) {
    ++cip;
    if (getParam(flags, 0, 0x3f, I_NONE))
      return;
  }
  Serial.begin(baud,
#ifdef ESP8266
    (SerialConfig)
#endif
    flags);
}

/***bf io GPIN
Reads the state of a general-purpose I/O pin.
\usage s = GPIN(pin)
\args
@portno	port number [`0` to `7`]
@pin	pin number [`0` to `15`]
\ret State of pin: `0` for low, `1` for high.
\note
* This function is only implemented on the ESP8266 and H3 platforms.
* On the ESP8266 platform, `GPIN()` allows access to pins on the I2C I/O
  extender only.
\ref GPOUT GPMODE
***/
num_t BASIC_INT Basic::ngpin() {
  uint32_t portno = 0;
  uint32_t pinno;
  if (checkOpen())
    return 0;
#ifndef ESP8266
  if (getParam(portno, I_COMMA)) return 0;
#endif
  if (getParam(pinno, 0, 15, I_NONE)) return 0;
  if (checkClose()) return 0;

  return c_gpio_get_pin(portno, pinno);
}

/***bf io ANA
Reads value from the analog input pin.
\usage v = ANA()
\ret Analog value read.
***/
num_t BASIC_FP Basic::nana() {
#if !(defined(ESP8266) || defined(ESP32)) || defined(ESP8266_NOWIFI)
  err = ERR_NOT_SUPPORTED;
  return 0;
#else
  if (checkOpen())
    return 0;
  if (checkClose())
    return 0;
  return analogRead(A0);  // 入力値取得
#endif
}

/***bf io SREAD
Reads bytes from the serial port.

WARNING: This function is currently useless because the serial port
receive pin is used for sound output in the BASIC Engine. It may be
removed in the future if it turns out that there is no viable way
to support serial input.
\usage b = SREAD()
\ret Byte of data read, or `-1` if there was no data available.
\ref SREADY()
***/
num_t BASIC_INT Basic::nsread() {
  if (checkOpen() || checkClose())
    return 0;
  return Serial.read();
}

/***bf io SREADY
Checks for available bytes on the serial port.

WARNING: This function is currently useless because the serial port
receive pin is used for sound output in the BASIC Engine. It may be
removed in the future if it turns out that there is no viable way
to support serial input.
\usage n = SREADY()
\ret Number of bytes available to read.
\ref SREAD()
***/
num_t BASIC_INT Basic::nsready() {
  if (checkOpen() || checkClose())
    return 0;
  return Serial.available();
}

void Basic::isprint() {
  iprint(1);
}
