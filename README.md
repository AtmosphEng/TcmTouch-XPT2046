A successful workaround for (this) issue https://github.com/TcMenu/tcMenu/issues/471 is in (this) Github repo https://github.com/AtmosphEng/TcmTouch-XPT2046.git. tcMenuDesigner is used to generate the XPT2046 touchscreen as per normal and the workaround code is found by searching for the tag: WORKAROUND_TCMENU_WITH_ESP32_CYD. 

Basically the ESP32_CYD uses a different SPI for the resistive touchscreen on SPI2 (VSPI). The io pin set for VSPI is put in *_main.cpp and is used to create mySPI. The tcMenuDesigner generated file tcMenuAdaTouchDriver.h is modded to access mySPI via an extern. In its init(), theTouchDevice(); is then modded to theTouchDevice(mySpi);.

Thanks to Brian Lough for the initial ESP32_CYD resistive touch code sample at: https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/tree/main/Examples/Basics/2-TouchTest.
