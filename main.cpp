/*
 * Siftkey: a sifteo smart key proof of concept.
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

// These enums are used to identify cubes:
enum Tasks {TRASH,        // tilted => horiz = emptied
         // WINDOW,       // vertical = closed
         // DISHWASHER,   // checked with open time ?
            TASK_NUM};
enum Users {USER_BASE = TASK_NUM-1,
            LESLIE,
            CEDRIC,
            ID_NUM,
            USER_NUM = ID_NUM - USER_BASE};
//const auto users[] = {"Leslie", "Cedric", "Other"};

static Metadata M = Metadata()
    .title("siftkey")
    .package("eu.honnet.siftkey", "0.1")
    .icon(Icon)
    .cubeRange(2, ID_NUM);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];

class SensorListener {
public:
    struct Counter {
        unsigned touch;
        unsigned neighborAdd;
    } counters[CUBE_ALLOCATION]; // useful ?

    void install()
    {
        Events::neighborAdd.set(&SensorListener::onNeighborAdd, this);
        Events::neighborRemove.set(&SensorListener::onNeighborRemove, this);
        Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        Events::cubeTouch.set(&SensorListener::onTouch, this);
        Events::cubeConnect.set(&SensorListener::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

private:
    void onConnect(unsigned id)
    {
        bzero(counters[id]); // useful ?
        vid[id].initMode(BG0_ROM);
        vid[id].attach(id);
        motion[id].attach(id);

        drawScreen(id);
    }

    void drawScreen(unsigned id)
    {
        CubeID cube(id);

        // Draw the cube's identity
        String<128> str;
        if (id < TASK_NUM) {
            str << "    Task #" << id << "\n\n";
            //str << "    Task #" << id+1 << "\n\n";
        } else
            str << "    User #" << id << "\n\n";
            //str << "    User #" << id+1 - TASK_NUM << "\n\n";
        vid[cube].bg0rom.text(vec(1,2), str);

        // Draw initial state for all sensors
        onAccelChange(cube);
        onTouch(cube);
        drawNeighbors(cube);
    }

    void onTouch(unsigned id)
    {
        /* TODO: debounce !
        static bool alreadyRunning = false;
        CubeID cube(id);
        if (cube.isTouching() && !alreadyRunning) // don't update when release
        {
            alreadyRunning = true;
            LOG("+ video mode = %d\n", vid[id].mode());
            if (vid[id].mode() != POWERDOWN_MODE)
                vid[id].setMode(POWERDOWN_MODE);
            else {
                vid[id].initMode(BG0_ROM);
                drawScreen(id);
            }
            LOG("- video mode = %d\n\n", vid[id].mode());
            alreadyRunning = false;
        }
        //*/
    }

    void onAccelChange(unsigned id)
    {
        /*
        CubeID cube(id);
        auto accel = cube.accel();

        String<64> str;
        str << "acc: "
            << Fixed(accel.x, 3)
            << Fixed(accel.y, 3)
            << Fixed(accel.z, 3) << "\n";

        unsigned changeFlags = motion[id].update();
        if (changeFlags) {
            // Tilt/shake changed

            LOG("Tilt/shake changed, flags=%08x\n", changeFlags);

            auto tilt = motion[id].tilt;
            str << "tilt:"
                << Fixed(tilt.x, 3)
                << Fixed(tilt.y, 3)
                << Fixed(tilt.z, 3) << "\n";

            str << "shake: " << motion[id].shake;
        }

        vid[cube].bg0rom.text(vec(1,10), str);
        */
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        if (firstID < arraysize(counters)) {
            drawNeighbors(firstID);
        }
        if (secondID < arraysize(counters)) {
            drawNeighbors(secondID);
        }
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {

        if (firstID < arraysize(counters)) {
            counters[firstID].neighborAdd++;
            drawNeighbors(firstID);
        }
        if (secondID < arraysize(counters)) {
            counters[secondID].neighborAdd++;
            drawNeighbors(secondID);
        }

        if (firstID < TASK_NUM && secondID > USER_BASE)
            LOG("%d, %d, %d\n", secondID, firstID, counters[secondID].neighborAdd);

        if (secondID < TASK_NUM && firstID > USER_BASE)
            LOG("%d, %d, %d\n", firstID, secondID, counters[firstID].neighborAdd);
    }

    void drawNeighbors(CubeID cube)
    {
        Neighborhood nb(cube);

        String<64> str;
        if (int(cube) > USER_BASE)
            str << "   total:" << counters[cube].neighborAdd << "\n\n";

        BG0ROMDrawable &draw = vid[cube].bg0rom;
        draw.text(vec(1,6), str);

        drawSideIndicator(draw, nb, vec( 1,  0), vec(14,  1), TOP);
        drawSideIndicator(draw, nb, vec( 0,  1), vec( 1, 14), LEFT);
        drawSideIndicator(draw, nb, vec( 1, 15), vec(14,  1), BOTTOM);
        drawSideIndicator(draw, nb, vec(15,  1), vec( 1, 14), RIGHT);
    }

    static void drawSideIndicator(BG0ROMDrawable &draw, Neighborhood &nb,
        Int2 topLeft, Int2 size, Side s)
    {
        unsigned nbColor = draw.ORANGE;
        draw.fill(topLeft, size,
            nbColor | (nb.hasNeighborAt(s) ? draw.SOLID_FG : draw.SOLID_BG));
    }
};


void main()
{
    static SensorListener sensors;

    sensors.install();

    // We're entirely event-driven. Everything is
    // updated by SensorListener's event callbacks.
    while (1)
        System::paint();
}
