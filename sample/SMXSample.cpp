#include <stdio.h>
#include <windows.h>
#include "SMX.h"
#include <memory>
#include <string>
using namespace std;

struct RGB {
    uint8_t r, g, b;
};

class InputSample
{
public:
    InputSample()
    {
        // Set a logging callback.  This can be called before SMX_Start.
        // SMX_SetLogCallback( SMXLogCallback );

        // Start scanning.  The update callback will be called when devices connect or
        // disconnect or panels are pressed or released.  This callback will be called
        // from a thread.
        SMX_Start( SMXStateChangedCallback, this );
    }

    static void SMXStateChangedCallback(int pad, SMXUpdateCallbackReason reason, void *pUser)
    {
        InputSample *pSelf = (InputSample *) pUser;
        pSelf->SMXStateChanged( pad, reason );
    }

    static void SMXLogCallback(const char *log)
    {
        printf("-> %s\n", log);
    }

    void SMXStateChanged(int pad, SMXUpdateCallbackReason reason)
    {
        printf("Device %i state changed: %04x\n", pad, SMX_GetInputState(pad));

    }

    void addColor(string* sLightsData, uint8_t r, uint8_t g, uint8_t b)
    {
        sLightsData->append(1, r);
        sLightsData->append(1, g);
        sLightsData->append(1, b);
    }

    void SetCabinetLights(int step, RGB colors[3])
    {
        for (int i = 0; i < 5; i++) {
            string sLightsData;

            // Determine how many lights to send based on the device index
            int numLights = 0;
            if (i == 0) {
                numLights = 24;
            } else if (i == 1 || i == 3) {
                numLights = 28;
            } else if (i == 2 || i == 4) {
                numLights = 8;
            }

            // Construct some lights data for each device
            RGB color = colors[step % 3];
            for (int c = 0; c < numLights; c++) {
                addColor(&sLightsData, color.r, color.g, color.b);
            }

            SMX_SetDedicatedCabinetLights(static_cast<SMXDedicatedCabinetLights>(i), sLightsData.data(), sLightsData.size());
        }
    }

    int iPanelToLight = 0;
    void SetStageLights()
    {
        string sLightsData;
        for( int iPad = 0; iPad < 2; ++iPad )
        {
            for( int iPanel = 0; iPanel < 9; ++iPanel )
            {
                bool bLight = iPanel == iPanelToLight && iPad == 0;
                if( !bLight )
                {
                    // We're not lighting this panel, so append black for the 4x4 and 3x3 lights.
                    for( int iLED = 0; iLED < 25; ++iLED )
                        addColor( &sLightsData, 0, 0, 0 );
                    continue;
                }

                // Append light data for the outer 4x4 grid of lights.
                addColor( &sLightsData, 0xFF, 0, 0 );
                addColor( &sLightsData, 0xFF, 0, 0 );
                addColor( &sLightsData, 0xFF, 0, 0 );
                addColor( &sLightsData, 0xFF, 0, 0 );
                addColor( &sLightsData, 0, 0xFF, 0 );
                addColor( &sLightsData, 0, 0xFF, 0 );
                addColor( &sLightsData, 0, 0xFF, 0 );
                addColor( &sLightsData, 0, 0xFF, 0 );
                addColor( &sLightsData, 0, 0, 0xFF );
                addColor( &sLightsData, 0, 0, 0xFF );
                addColor( &sLightsData, 0, 0, 0xFF );
                addColor( &sLightsData, 0, 0, 0xFF );
                addColor( &sLightsData, 0xFF, 0xFF, 0 );
                addColor( &sLightsData, 0xFF, 0xFF, 0 );
                addColor( &sLightsData, 0xFF, 0xFF, 0 );
                addColor( &sLightsData, 0xFF, 0xFF, 0 );

                // Append light data for the inner 3x3 grid of lights, if present.  These
                // are ignored if the platform doesn't have them.
                addColor( &sLightsData, 0xFF, 0, 0 );
                addColor( &sLightsData, 0xFF, 0, 0 );
                addColor( &sLightsData, 0xFF, 0, 0 );
                addColor( &sLightsData, 0, 0xFF, 0 );
                addColor( &sLightsData, 0, 0xFF, 0 );
                addColor( &sLightsData, 0, 0xFF, 0 );
                addColor( &sLightsData, 0, 0, 0xFF );
                addColor( &sLightsData, 0, 0, 0xFF );
                addColor( &sLightsData, 0, 0, 0xFF );
            }
        }

        SMX_SetLights2( sLightsData.data(), sLightsData.size() );
    }
};

int main()
{
    InputSample demo;
    RGB colors[3];

    colors[0].r = 0xff;
    colors[0].g = 0x01;
    colors[0].b = 0x01;

    colors[1].r = 0x01;
    colors[1].g = 0xff;
    colors[1].b = 0x01;

    colors[2].r = 0x01;
    colors[2].g = 0x01;
    colors[2].b = 0xff;

    // Loop forever for this sample.
    int step = 0;
    while(1)
    {
        Sleep(500);
        demo.SetStageLights();
        demo.SetCabinetLights(step, colors);
        step++;
    }

    return 0;
}

