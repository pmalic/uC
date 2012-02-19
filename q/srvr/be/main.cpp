#include "mbed.h"
#include "EthernetNetIf.h"
#include "HTTPServer.h"
#include "SDFileSystem.h"

#include "xbee/XBee.h"
#include "wsan/OfferMsg.h"
#include "wsan/DiscoverMsg.h"

#include "QHTTPHandler.h"

DigitalOut led1(LED1, "led1");

#define PORT 80

SDFileSystem sd(p5, p6, p7, p8, "sd");
//LocalFileSystem fs("local");

EthernetNetIf eth;
HTTPServer srv;

using namespace wsan;

int main ()
{
    printf("\r\nSetting up...\r\n");
    EthernetErr ethErr = eth.setup();

    if (ethErr)
		{
        printf("Error %d in setup.\r\n", ethErr);
        return -1;
    }

    FILE *fp = fopen("/sd/htpasswd", "rt");
    
    char user[32], pass[33], code[64];
    
    fscanf(fp, "%s:%s:%s\n", user, pass, code);
    
    printf("read %s %s %s\r\n", user, pass, code);
    
    fclose(fp);
    
    printf("\r\nSetup OK\r\n");

    FSHandler::mount("/sd/fe", "/");
//    FSHandler::mount("/local", "/");

    srv.addHandler<FSHandler>("/");

    srv.addHandler<QHTTPHandler>("/be");

    srv.bind(PORT);

    printf("Listening...\r\n");

    srand(time(NULL));

    Timer tm;
    tm.start();

    XBee xbee = XBee();
    xbee.begin(115200);

    printf("Sending discovery msg...\r\n");

    XBeeAddress64 addr = XBeeAddress64(0x0, 0xffff);

    DiscoverMsg msg;

    ZBTxRequest tx = ZBTxRequest(addr, msg.getFrame(), msg.getFrameLen());

    xbee.send(tx);

    while (true) {
    
        Net::poll();

        xbee.readPacket();

        XBeeResponse& res = xbee.getResponse();

        if (!res.isAvailable() || res.getApiId() != ZB_RX_RESPONSE)
            continue;

        ZBRxResponse rx = ZBRxResponse();
        res.getZBRxResponse(rx);

        uint8_t len = rx.getDataLength();

        if (!len || rx.getData(0) != Msg::PREAMBLE || rx.getData(1) != OfferMsg::MSG_TYPE)
            continue;

        OfferMsg msg(rx.getData());

        unsigned int decimals = msg.header.desc >> 1;
        unsigned int factor = 1;

        while (decimals--)
            factor *= 10;

        //sprintf(ext, "%s: %c%d.%d C", msg.header.node_name, (msg.header.desc & 0x1 ? '-' : '+'), (msg.header.val / factor), (msg.header.val % factor));

        if (tm.read() > .5) {
            led1 =! led1;
            tm.start();
        }
    }

}
