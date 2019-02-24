#include <RH_RF95.h>

//DO NOT CHANGE THESE REGARDLESS OF WHAT THE INTERNET SAYS <3 -Aaron
/*
#define RFM95_CS A0
#define RFM95_RST A5
#define RFM95_INT A3 */
#define RF95_FREQ 915.0


namespace nustars {
    class Radio {
    public:
        Radio(int cs, int rst, int interrupt);
        void transmit(uint8_t* x, uint8_t length);

    private:
        int rstPin;
        int csPin;
        int intPin;
        RH_RF95* rf95;
    };
}
