/*
    Wifi settings and IP address
*/

// select Static IP Mode, uncomment below to enable
#ifdef Fixed_IP
#define STATIC_IP
#endif
IPAddress ip(192, 168, 1, 199);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//See Secrets.h for SSIDs and passwords